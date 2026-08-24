/******************** REST disclaimer ***********************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
/// TRestTools is a class that defines static methods that might be handy
/// when accessing files or system utilities, or other basic methods that
/// do not fit in a specialized class, and are generic enought to be
/// considered a REST tool.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-December: First concept.
///				   Javier Galan
///
/// \class      TRestTools
/// \author     Javier Galan <javier.galan@unizar.es>
/// \author     Kaixiang Ni
///
/// <hr>
///
#include "TRestTools.h"

#include <TArrayC.h>
#include <TClass.h>
#include <TClassEdit.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TFileMerger.h>
#include <TKey.h>
#include <TList.h>
#include <TObjString.h>
#include <TSchemaRule.h>
#include <TSchemaRuleSet.h>
#include <TStreamerInfo.h>
#include <TSystem.h>
#include <TTree.h>
#include <TUUID.h>
#include <TUrl.h>

#include <regex>

#ifdef USE_Curl
#include <curl/curl.h>
#endif

#ifdef WIN32
#include <io.h>
#include <windows.h>
#else
#include <sys/stat.h>

#include "unistd.h"
#endif  // !WIN32

#ifdef __APPLE__
#include <array>
#endif

#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <thread>
#include <tuple>

#include "TRestStringHelper.h"
#include "TRestStringOutput.h"

using namespace std;

namespace {
#ifdef REST_TESTING_ENABLED
std::atomic_bool gForceNextRootUpdatePreflightFailure{false};
std::atomic_bool gForceNextMergeValidationFailure{false};
#endif

struct StreamerInfoIdentity {
    std::string name;
    Int_t version;
    UInt_t checksum;

    bool operator<(const StreamerInfoIdentity& other) const {
        return std::tie(name, version, checksum) < std::tie(other.name, other.version, other.checksum);
    }
};

struct RootSchemaSnapshot {
    std::set<StreamerInfoIdentity> streamerInfos;
    std::set<std::string> schemaRules;
};

struct RootContentEntry {
    std::string className;
    std::optional<Long64_t> treeEntries;
};

using RootContentManifest = std::map<std::string, RootContentEntry>;

struct LocalFileIdentity {
    std::uintmax_t size;
    std::filesystem::file_time_type modificationTime;
#ifndef WIN32
    std::uintmax_t device;
    std::uintmax_t inode;
#endif

    bool operator==(const LocalFileIdentity& other) const {
        return size == other.size && modificationTime == other.modificationTime
#ifndef WIN32
               && device == other.device && inode == other.inode
#endif
            ;
    }
};

std::optional<std::filesystem::path> LocalRootPath(const std::string& filename) {
    TUrl url(filename.c_str(), kTRUE);
    if (!url.IsValid() || std::string(url.GetProtocol()) != "file") return std::nullopt;
    return std::filesystem::path(url.GetFile());
}

std::optional<LocalFileIdentity> CaptureLocalFileIdentity(const std::filesystem::path& path,
                                                          std::string& error) {
    std::error_code ec;
    LocalFileIdentity identity;
    identity.size = std::filesystem::file_size(path, ec);
    if (ec) {
        error = "Cannot inspect size of " + path.string() + ": " + ec.message();
        return std::nullopt;
    }
    identity.modificationTime = std::filesystem::last_write_time(path, ec);
    if (ec) {
        error = "Cannot inspect modification time of " + path.string() + ": " + ec.message();
        return std::nullopt;
    }
#ifndef WIN32
    struct stat status {};
    if (::stat(path.c_str(), &status) != 0) {
        error = "Cannot inspect filesystem identity of " + path.string() + ": " + std::strerror(errno);
        return std::nullopt;
    }
    identity.device = static_cast<std::uintmax_t>(status.st_dev);
    identity.inode = static_cast<std::uintmax_t>(status.st_ino);
#endif
    return identity;
}

std::string NormalizeSchemaRule(const std::string& rule, std::string* targetClass = nullptr) {
    ROOT::TSchemaRule parsed;
    if (!parsed.SetFromRule(rule.c_str())) return "";
    TString normalized;
    parsed.AsString(normalized);
    if (targetClass != nullptr) *targetClass = parsed.GetTargetClass();
    return normalized.Data();
}

bool ReadSchemaSnapshot(TFile& file, RootSchemaSnapshot& snapshot, std::string& error) {
    if (!file.IsOpen() || file.IsZombie()) {
        error = "Cannot inspect StreamerInfos in a closed or invalid ROOT file";
        return false;
    }
    if (file.GetSeekInfo() == 0) return true;

    std::unique_ptr<TList> infos(file.GetStreamerInfoList());
    if (infos == nullptr) {
        error = "ROOT could not read the existing StreamerInfo record";
        return false;
    }
    infos->SetOwner(kTRUE);

    TIter next(infos.get());
    while (TObject* object = next()) {
        if (auto* info = dynamic_cast<TStreamerInfo*>(object)) {
            snapshot.streamerInfos.insert({info->GetName(), info->GetClassVersion(), info->GetCheckSum()});
            continue;
        }

        auto* rules = dynamic_cast<TList*>(object);
        if (rules == nullptr || std::string(rules->GetName()) != "listOfRules") continue;
        TIter nextRule(rules);
        while (TObject* ruleObject = nextRule()) {
            auto* rule = dynamic_cast<TObjString*>(ruleObject);
            if (rule == nullptr) continue;
            const std::string normalized = NormalizeSchemaRule(rule->GetString().Data());
            if (normalized.empty()) {
                error = "The on-disk StreamerInfo contains an invalid schema rule";
                return false;
            }
            snapshot.schemaRules.insert(normalized);
        }
    }
    return true;
}

bool IsSubset(const RootSchemaSnapshot& expected, const RootSchemaSnapshot& actual, std::string& error) {
    for (const auto& info : expected.streamerInfos) {
        if (actual.streamerInfos.count(info) == 0) {
            // ROOT deliberately normalizes or omits implementation metadata for
            // standard-library types when it rewrites StreamerInfo. These are
            // not user schemas; exact historical identities remain mandatory
            // for every other class.
            if (TClassEdit::IsStdClass(info.name.c_str()) || TClassEdit::IsStdPairBase(info.name.c_str()))
                continue;
            error = "Merged file lost StreamerInfo " + info.name + " version " +
                    std::to_string(info.version) + " checksum " + std::to_string(info.checksum);
            return false;
        }
    }
    for (const auto& rule : expected.schemaRules) {
        if (actual.schemaRules.count(rule) == 0) {
            error = "Merged file lost an on-disk schema-evolution rule";
            return false;
        }
    }
    return true;
}

bool ReadContentManifest(TDirectory& directory, const std::string& prefix, RootContentManifest& manifest,
                         std::string& error) {
    TDirectory::TContext directoryContext(&directory);
    std::map<std::string, TKey*> newestKeys;
    TIter nextKey(directory.GetListOfKeys());
    while (auto* key = dynamic_cast<TKey*>(nextKey())) {
        auto existing = newestKeys.find(key->GetName());
        if (existing == newestKeys.end() || key->GetCycle() > existing->second->GetCycle())
            newestKeys[key->GetName()] = key;
    }

    for (const auto& [name, key] : newestKeys) {
        const std::string path = prefix.empty() ? name : prefix + "/" + name;
        RootContentEntry entry{key->GetClassName(), std::nullopt};
        TClass* keyClass = TClass::GetClass(key->GetClassName(), kFALSE);
        const bool isDirectory = keyClass != nullptr && keyClass->InheritsFrom(TDirectory::Class());
        const bool isTree = keyClass != nullptr && keyClass->InheritsFrom(TTree::Class());

        if (isDirectory || isTree) {
            std::unique_ptr<TObject> object(key->ReadObj());
            if (object == nullptr) {
                error =
                    "Cannot read " + std::string(isDirectory ? "directory" : "tree") + " key '" + path + "'";
                return false;
            }
            if (isDirectory) {
                auto* child = dynamic_cast<TDirectory*>(object.get());
                if (child == nullptr) {
                    error = "Key '" + path + "' claims directory class '" + entry.className +
                            "' but ROOT read a different object";
                    return false;
                }
                manifest[path] = entry;
                if (!ReadContentManifest(*child, path, manifest, error)) return false;
                continue;
            }

            auto* tree = dynamic_cast<TTree*>(object.get());
            if (tree == nullptr) {
                error = "Key '" + path + "' claims tree class '" + entry.className +
                        "' but ROOT read a different object";
                return false;
            }
            entry.treeEntries = tree->GetEntries();
        }
        manifest[path] = entry;
    }
    return true;
}

bool AddContentManifest(const RootContentManifest& source, RootContentManifest& expected,
                        std::string& error) {
    for (const auto& [path, sourceEntry] : source) {
        const auto existing = expected.find(path);
        if (existing == expected.end()) {
            expected[path] = sourceEntry;
            continue;
        }
        if (existing->second.className != sourceEntry.className) {
            error = "ROOT merge input key '" + path + "' has incompatible classes '" +
                    existing->second.className + "' and '" + sourceEntry.className + "'";
            return false;
        }
        if (existing->second.treeEntries.has_value() != sourceEntry.treeEntries.has_value()) {
            error = "ROOT merge input key '" + path + "' is inconsistently identified as a TTree";
            return false;
        }
        if (sourceEntry.treeEntries.has_value()) {
            if (*sourceEntry.treeEntries >
                std::numeric_limits<Long64_t>::max() - *existing->second.treeEntries) {
                error = "Merged TTree entry count overflows for key '" + path + "'";
                return false;
            }
            *existing->second.treeEntries += *sourceEntry.treeEntries;
        }
    }
    return true;
}

bool ValidateContentManifest(const RootContentManifest& expected, const RootContentManifest& actual,
                             std::string& error) {
    for (const auto& [path, expectedEntry] : expected) {
        const auto found = actual.find(path);
        if (found == actual.end()) {
            error = "Merged file lost key '" + path + "'";
            return false;
        }
        if (found->second.className != expectedEntry.className) {
            error = "Merged key '" + path + "' changed class from '" + expectedEntry.className + "' to '" +
                    found->second.className + "'";
            return false;
        }
        if (expectedEntry.treeEntries.has_value()) {
            if (!found->second.treeEntries.has_value()) {
                error = "Merged key '" + path + "' is no longer readable as a TTree";
                return false;
            }
            if (*found->second.treeEntries != *expectedEntry.treeEntries) {
                error = "Merged TTree '" + path + "' has " + std::to_string(*found->second.treeEntries) +
                        " entries; expected " + std::to_string(*expectedEntry.treeEntries);
                return false;
            }
        }
    }
    return true;
}

bool LoadOnDiskSchemaRules(const RootSchemaSnapshot& snapshot, std::string& error) {
    std::map<std::string, std::set<std::string>> rulesByTarget;
    for (const auto& rule : snapshot.schemaRules) {
        std::string targetClass;
        const std::string normalized = NormalizeSchemaRule(rule, &targetClass);
        if (normalized.empty() || targetClass.empty()) {
            error = "Cannot parse an on-disk schema-evolution rule";
            return false;
        }
        rulesByTarget[targetClass].insert(normalized);
    }

    for (const auto& [targetClass, expectedRules] : rulesByTarget) {
        TClass* cl = TClass::GetClass(targetClass.c_str(), kFALSE);
        if (cl == nullptr) {
            error = "Cannot register schema rules for unloaded class " + targetClass;
            return false;
        }

        auto registeredRules = [&cl]() {
            std::set<std::string> result;
            if (cl->GetSchemaRules() == nullptr) return result;
            TObjArrayIter nextRule(cl->GetSchemaRules()->GetRules());
            while (auto* registered = dynamic_cast<ROOT::TSchemaRule*>(nextRule())) {
                TString serialized;
                registered->AsString(serialized);
                result.insert(serialized.Data());
            }
            return result;
        };

        std::set<std::string> actualRules = registeredRules();
        for (const auto& expected : expectedRules) {
            if (actualRules.count(expected) != 0) continue;

            bool added = false;
            if (cl->TestBit(TClass::kIsEmulation)) {
                auto parsed = std::make_unique<ROOT::TSchemaRule>();
                if (parsed->SetFromRule(expected.c_str())) {
                    auto* rules = cl->GetSchemaRules(kTRUE);
                    added = rules != nullptr &&
                            rules->AddRule(parsed.get(), ROOT::Detail::TSchemaRuleSet::kNoCheck);
                    if (added) parsed.release();
                }
            } else {
                added = TClass::AddRule(expected.c_str());
            }
            if (!added) {
                error = "Cannot register an on-disk schema rule for class " + targetClass;
                return false;
            }
            actualRules = registeredRules();
        }
        for (const auto& expected : expectedRules) {
            if (actualRules.count(expected) == 0) {
                error = "Cannot preserve an on-disk schema rule for class " + targetClass;
                return false;
            }
        }
    }
    return true;
}

bool ResolveOnDiskStreamerInfos(TFile& file, std::vector<Int_t>& numbers, Int_t& maximumNumber,
                                std::string& error) {
    numbers.clear();
    maximumNumber = 0;
    if (file.GetSeekInfo() == 0) return true;

    std::unique_ptr<TList> infos(file.GetStreamerInfoList());
    if (infos == nullptr) {
        error = "ROOT could not resolve the existing StreamerInfo record";
        return false;
    }

    // This mirrors TFile::ReadStreamerInfo. BuildCheck may transfer an
    // on-disk TStreamerInfo into ROOT's global registry, while aliases and
    // ROOT-managed implementation descriptors are marked kCanDelete. An
    // owning TList would destroy the globally registered objects.
    infos->SetOwner(kFALSE);
    std::set<Int_t> resolvedNumbers;
    std::string unresolvedError;
    TIter next(infos.get());
    while (TObject* object = next()) {
        auto* info = dynamic_cast<TStreamerInfo*>(object);
        if (info == nullptr) {
            object->SetBit(TObject::kCanDelete);
            continue;
        }

        info->BuildCheck(&file);
        const Int_t number = info->GetNumber();
        if (number > 0) {
            resolvedNumbers.insert(number);
            maximumNumber = std::max(maximumNumber, number);
        } else if (!info->TestBit(TObject::kCanDelete) && unresolvedError.empty()) {
            unresolvedError = "Cannot preserve StreamerInfo " + std::string(info->GetName()) + " version " +
                              std::to_string(info->GetClassVersion()) + " checksum " +
                              std::to_string(info->GetCheckSum());
        }
    }
    infos->Clear();

    if (!unresolvedError.empty()) {
        error = unresolvedError;
        return false;
    }
    numbers.assign(resolvedNumbers.begin(), resolvedNumbers.end());
    return true;
}

bool ResolveSchemaUpdate(TFile& file, std::vector<Int_t>& numbers, Int_t& maximumNumber, std::string& error) {
    RootSchemaSnapshot snapshot;
    if (!ReadSchemaSnapshot(file, snapshot, error) ||
        !ResolveOnDiskStreamerInfos(file, numbers, maximumNumber, error) ||
        !LoadOnDiskSchemaRules(snapshot, error))
        return false;
    return true;
}

bool ApplySchemaUpdate(TFile& file, const std::vector<Int_t>& numbers, Int_t maximumNumber,
                       std::string& error) {
    TArrayC* classIndex = file.GetClassIndex();
    if (classIndex == nullptr) {
        error = "ROOT file has no StreamerInfo class index";
        return false;
    }
    if (maximumNumber >= classIndex->GetSize()) classIndex->Set(maximumNumber + 1);
    for (const Int_t number : numbers) classIndex->fArray[number] = 1;
    classIndex->fArray[0] = 1;
    return true;
}

std::filesystem::path UniqueSiblingPath(const std::filesystem::path& target, const std::string& label) {
    for (int attempt = 0; attempt < 20; ++attempt) {
        TString uuid = TUUID().AsString();
        uuid.ReplaceAll("-", "");
        auto candidate =
            target.parent_path() / ("." + target.filename().string() + ".rest-" + label + "-" + uuid.Data());
        if (!std::filesystem::exists(candidate)) return candidate;
    }
    return {};
}

bool ReplaceLocalFile(const std::filesystem::path& temporary, const std::filesystem::path& target,
                      const std::filesystem::path& backup, std::string& error) {
    const bool targetExists = std::filesystem::exists(target);
#ifdef WIN32
    if (targetExists) {
        if (!ReplaceFileW(target.wstring().c_str(), temporary.wstring().c_str(), backup.wstring().c_str(),
                          REPLACEFILE_WRITE_THROUGH, nullptr, nullptr)) {
            error = "Windows ReplaceFile failed with error " + std::to_string(GetLastError());
            return false;
        }
    } else if (!MoveFileExW(temporary.wstring().c_str(), target.wstring().c_str(), MOVEFILE_WRITE_THROUGH)) {
        error = "Windows MoveFileEx failed with error " + std::to_string(GetLastError());
        return false;
    }
#else
    if (targetExists) {
        std::error_code ec;
        std::filesystem::create_hard_link(target, backup, ec);
        if (ec) {
            ec.clear();
            std::filesystem::copy_file(target, backup, std::filesystem::copy_options::none, ec);
            if (ec) {
                error = "Cannot create rollback backup " + backup.string() + ": " + ec.message();
                return false;
            }
        }
    }
    if (::rename(temporary.c_str(), target.c_str()) != 0) {
        error = "Atomic rename to " + target.string() + " failed: " + std::strerror(errno);
        if (targetExists) {
            std::error_code cleanupError;
            std::filesystem::remove(backup, cleanupError);
            if (cleanupError)
                error += ". REST also could not remove rollback backup '" + backup.string() +
                         "': " + cleanupError.message();
        }
        return false;
    }
#endif
    return true;
}

bool RollBackLocalFile(const std::filesystem::path& target, const std::filesystem::path& backup,
                       bool targetPreviouslyExisted, std::string& error) {
    if (!targetPreviouslyExisted) {
        std::error_code ec;
        std::filesystem::remove(target, ec);
        if (ec) error = "Cannot remove invalid replacement " + target.string() + ": " + ec.message();
        return !ec;
    }
#ifdef WIN32
    if (!ReplaceFileW(target.wstring().c_str(), backup.wstring().c_str(), nullptr, REPLACEFILE_WRITE_THROUGH,
                      nullptr, nullptr)) {
        error = "Cannot roll back " + target.string() + "; Windows error " + std::to_string(GetLastError());
        return false;
    }
#else
    if (::rename(backup.c_str(), target.c_str()) != 0) {
        error = "Cannot roll back " + target.string() + ": " + std::strerror(errno);
        return false;
    }
#endif
    return true;
}
}  // namespace

TRestRootFileHandle TRestRootFileHandle::Open(const std::string& filename, TRestRootFileMode mode) {
    TRestRootFileHandle result;
    if (mode != TRestRootFileMode::Read && TRestTools::IsRemoteRootPath(filename)) {
        result.fError = "REST refuses to mutate remote ROOT destination '" + filename +
                        "'. Choose an explicit local output file.";
        return result;
    }

    const char* option = mode == TRestRootFileMode::Read
                             ? "READ"
                             : (mode == TRestRootFileMode::Recreate ? "RECREATE" : "READ");
    result.fFile.reset(TFile::Open(filename.c_str(), option));
    if (result.fFile == nullptr || !result.fFile->IsOpen() || result.fFile->IsZombie()) {
        result.fError = "Cannot open ROOT file '" + filename +
                        (mode == TRestRootFileMode::Update ? "' for read-only UPDATE preflight"
                                                           : "' in mode " + std::string(option));
        result.fFile.reset();
        return result;
    }
    if (mode == TRestRootFileMode::Recreate && !result.fFile->IsWritable()) {
        result.fError = "ROOT file '" + filename + "' is not writable";
        result.fFile.reset();
        return result;
    }

    if (mode == TRestRootFileMode::Update && !PrepareBorrowedUpdate(*result.fFile, &result.fError)) {
        result.fFile.reset();
    }
    return result;
}

TRestRootFileHandle::TRestRootFileHandle(TRestRootFileHandle&& other) noexcept
    : fFile(std::move(other.fFile)), fError(std::move(other.fError)) {}

TRestRootFileHandle& TRestRootFileHandle::operator=(TRestRootFileHandle&& other) noexcept {
    if (this == &other) return *this;
    Close();
    fFile = std::move(other.fFile);
    fError = std::move(other.fError);
    return *this;
}

TRestRootFileHandle::~TRestRootFileHandle() { Close(); }

bool TRestRootFileHandle::Close() noexcept {
    if (fFile == nullptr) return fError.empty();
    if (fFile->IsOpen()) fFile->Close();
    const bool success = !fFile->IsZombie() && !fFile->TestBit(TFile::kWriteError);
    if (!success && fError.empty())
        fError = "ROOT reported a write error while closing " + std::string(fFile->GetName());
    fFile.reset();
    return success;
}

bool TRestRootFileHandle::PrepareBorrowedUpdate(TFile& file, std::string* error) {
    std::string localError;
    if (!file.IsOpen() || file.IsZombie() || file.IsWritable()) {
        localError = "UPDATE preparation requires a valid TFile that is currently open in READ mode";
        if (error != nullptr) *error = localError;
        return false;
    }

    const std::string filename = file.GetName();
    if (TRestTools::IsRemoteRootPath(filename)) {
        localError = "REST refuses to mutate remote ROOT destination '" + filename +
                     "'. Choose an explicit local output file.";
        if (error != nullptr) *error = localError;
        return false;
    }

    std::vector<Int_t> numbers;
    Int_t maximumNumber = 0;
    if (!ResolveSchemaUpdate(file, numbers, maximumNumber, localError)) {
        if (error != nullptr) *error = localError;
        return false;
    }

    const auto localPath = LocalRootPath(filename);
    if (!localPath) {
        localError = "Cannot resolve local ROOT path '" + filename + "'";
        if (error != nullptr) *error = localError;
        return false;
    }
    const std::string uuidBefore = file.GetUUID().AsString();
    const auto identityBefore = CaptureLocalFileIdentity(*localPath, localError);
    if (!identityBefore) {
        if (error != nullptr) *error = localError;
        return false;
    }

#ifdef REST_TESTING_ENABLED
    if (gForceNextRootUpdatePreflightFailure.exchange(false)) {
        localError = "Forced writable ROOT preflight failure for testing";
        if (error != nullptr) *error = localError;
        return false;
    }
#endif

    if (file.ReOpen("UPDATE") != 0 || !file.IsWritable()) {
        localError = "ROOT could not transition '" + filename + "' from the validated READ handle to UPDATE";
        if (file.IsWritable()) file.SetWritable(kFALSE);
        if (error != nullptr) *error = localError;
        return false;
    }

    const auto identityAfter = CaptureLocalFileIdentity(*localPath, localError);
    const std::string uuidAfter = file.GetUUID().AsString();
    if (!identityAfter || !(*identityBefore == *identityAfter) || uuidBefore != uuidAfter) {
        if (localError.empty())
            localError = "ROOT file '" + filename + "' changed during the READ-to-UPDATE transition";
        file.SetWritable(kFALSE);
        if (error != nullptr) *error = localError;
        return false;
    }
    if (!ApplySchemaUpdate(file, numbers, maximumNumber, localError)) {
        file.SetWritable(kFALSE);
        if (error != nullptr) *error = localError;
        return false;
    }
    return true;
}

///////////////////////////////////////////////
/// \brief Returns all the options in an option string
///
/// This method gives string to the method GetFirstOption().
/// And then adds the result to the list.
///
std::vector<string> TRestTools::GetOptions(string optionsStr) { return Split(optionsStr, ":"); }

///////////////////////////////////////////////
/// \brief Calls gSystem to load REST library.
///
/// REST library lies in $REST_PATH/lib and $HOME/.rest/userlib/
/// After this we can use reflection methods TClass::GetClass() and
/// TRestMetadata::GetDataMemberRef()
///
void TRestTools::LoadRESTLibrary(bool silent) {
    const set<string> libraryExtension{".so", ".dylib", ".dll"};
    const set<string> excludedLibraries{
        "RestG4"};  // Ignoring package libraries if they exist. TODO: do not hardcode this

    vector<string> ldPaths;
#ifdef WIN32
    ldPaths.push_back(REST_PATH + "/bin/");
#elif __APPLE__
    ldPaths.push_back(REST_PATH + "/bin/");
#else
    ldPaths.push_back(REST_PATH + "/lib/");
#endif  // WIN32
    ldPaths.push_back(REST_USER_PATH + "/userlib/");

    vector<string> fileList;
    for (const std::filesystem::path path : ldPaths) {
        if (!exists(path)) {
            // RESTWarning << "Directory " << string(path) << " for library loading not exist" << RESTendl;
            continue;
        }
        for (const auto& it : std::filesystem::directory_iterator(path)) {
            if (!it.is_regular_file()) {
                continue;
            }
            if (libraryExtension.count(it.path().extension().string()) == 0) {
                // needs correct extension e.g. ".so"
                continue;
            }
            const TString pathRootString = it.path().string();
            TString libName = TRestTools::SeparatePathAndName((std::string)pathRootString).second;
            if (!libName.Contains("Rest", TString::kExact)) {
                // e.g. "libRestFramework.so"
                continue;
            }
            // Check if library is excluded from loading e.g. is from a package
            bool excluded = false;
            for (const TString excludedLibrary : excludedLibraries) {
                if (libName.Contains(excludedLibrary, TString::kExact)) {
                    excluded = true;
                    // RESTWarning << "Library '" << pathRootString << "' excluded from loading" << RESTendl;
                    break;
                }
            }
            if (excluded) {
                continue;
            }
            fileList.emplace_back(it.path().string());
        }
    }

    // load the found REST libraries
    if (!silent) cout << "= Loading libraries ..." << endl;
    for (const auto& library : fileList) {
        if (!silent) cout << " - " << library << endl;
        gSystem->Load(library.c_str());
    }
    if (!silent) cout << endl;
}

///////////////////////////////////////////////
/// \brief Prints the contents of the vector table given as argument in screen.
/// Allowed types are Int_t, Float_t and Double_t.
///
/// The printed out data can be restricted to the row lines between `start`
/// and `end` parameters given by argument.
///
template <typename T>
int TRestTools::PrintTable(std::vector<std::vector<T>> data, Int_t start, Int_t end) {
    Int_t size = data.size();
    if (end > 0 && size > end) size = end;
    for (int n = start; n < size; n++) {
        for (unsigned int m = 0; m < data[n].size(); m++) cout << data[n][m] << "\t";
        cout << endl;
    }
    return 0;
}

template int TRestTools::PrintTable<Int_t>(std::vector<std::vector<Int_t>> data, Int_t start, Int_t end);
template int TRestTools::PrintTable<Float_t>(std::vector<std::vector<Float_t>> data, Int_t start, Int_t end);
template int TRestTools::PrintTable<Double_t>(std::vector<std::vector<Double_t>> data, Int_t start,
                                              Int_t end);
template int TRestTools::PrintTable<std::string>(std::vector<std::vector<std::string>> data, Int_t start,
                                                 Int_t end);

///////////////////////////////////////////////
/// \brief Writes the contents of the vector table given as argument to `fname`.
/// Allowed types are Int_t, Float_t and Double_t.
///
template <typename T>
int TRestTools::ExportASCIITable(std::string fname, std::vector<std::vector<T>>& data) {
    ofstream file(fname);
    if (!file.is_open()) {
        RESTError << "Unable to open file for writing : " << fname << RESTendl;
        return 1;
    }

    for (unsigned int n = 0; n < data.size(); n++)
        for (unsigned int m = 0; m < data[n].size(); m++) {
            file << data[n][m];
            if (m + 1 < data[n].size()) file << "\t";
            if (m + 1 == data[n].size()) file << "\n";
        }
    file.close();

    return 0;
}

template int TRestTools::ExportASCIITable<Int_t>(std::string fname, std::vector<std::vector<Int_t>>& data);
template int TRestTools::ExportASCIITable<Float_t>(std::string fname,
                                                   std::vector<std::vector<Float_t>>& data);
template int TRestTools::ExportASCIITable<Double_t>(std::string fname,
                                                    std::vector<std::vector<Double_t>>& data);

///////////////////////////////////////////////
/// \brief Writes the contents of the vector table given as argument to `fname` as a binary file.
/// Allowed types are Int_t, Float_t and Double_t.
///
template <typename T>
int TRestTools::ExportBinaryTable(std::string fname, std::vector<std::vector<T>>& data) {
    ofstream file(fname, ios::out | ios::binary);
    if (!file.is_open()) {
        RESTError << "Unable to open file for writing : " << fname << RESTendl;
        return 1;
    }

    for (unsigned int n = 0; n < data.size(); n++)
        for (unsigned int m = 0; m < data[n].size(); m++) {
            file.write((char*)&data[n][m], sizeof(T));
        }
    file.close();

    return 0;
}

template int TRestTools::ExportBinaryTable<Int_t>(std::string fname, std::vector<std::vector<Int_t>>& data);
template int TRestTools::ExportBinaryTable<Float_t>(std::string fname,
                                                    std::vector<std::vector<Float_t>>& data);
template int TRestTools::ExportBinaryTable<Double_t>(std::string fname,
                                                     std::vector<std::vector<Double_t>>& data);

///////////////////////////////////////////////
/// \brief Reads a binary file containing a fixed-columns table with values
///
/// This method will open the file fName. This file should contain a
/// table with numeric values of the type specified inside the syntax < >.
///
/// For example, a float number table with 6-columns would be read as follows:
///
/// \code
/// std::vector<std::vector <Float_t> > fvec;
/// ReadBinaryTable( "myfile.bin", fvec, 6);
/// \endcode
///
/// The values on the table will be loaded in the matrix provided through the
/// argument `data`. The content of `data` will be cleared in this method.
///
template <typename T>
int TRestTools::ReadBinaryTable(string fName, std::vector<std::vector<T>>& data, Int_t columns) {
    if (!TRestTools::isValidFile((string)fName)) {
        RESTError << "TRestTools::ReadBinaryTable. Error." << RESTendl;
        RESTError << "Cannot open file : " << fName << RESTendl;
        return 0;
    }

    if (columns == -1) {
        columns = GetBinaryFileColumns(fName);
        if (columns == -1) {
            RESTError << "TRestTools::ReadBinaryTable. Format extension error." << RESTendl;
            RESTError << "Please, specify the number of columns at the method 3rd argument" << RESTendl;
            return 0;
        }
    }

    std::ifstream fin(fName, std::ios::binary);
    fin.seekg(0, std::ios::end);
    const size_t num_elements = fin.tellg() / sizeof(T);
    fin.seekg(0, std::ios::beg);

    if (num_elements % columns != 0) {
        cout << "TRestTools::ReadBinaryTable. Error." << endl;
        cout << "Number of elements : " << num_elements
             << " is not compatible with the number of columns : " << columns << endl;
        fin.close();
        return 0;
    }

    std::vector<T> dataArray(columns);
    fin.read(reinterpret_cast<char*>(&dataArray[0]), columns * sizeof(T));
    while (fin.good()) {
        data.push_back(dataArray);
        fin.read(reinterpret_cast<char*>(&dataArray[0]), columns * sizeof(T));
    }
    return 1;
}

template int TRestTools::ReadBinaryTable<Int_t>(string fName, std::vector<std::vector<Int_t>>& data,
                                                Int_t columns);
template int TRestTools::ReadBinaryTable<Float_t>(string fName, std::vector<std::vector<Float_t>>& data,
                                                  Int_t columns);
template int TRestTools::ReadBinaryTable<Double_t>(string fName, std::vector<std::vector<Double_t>>& data,
                                                   Int_t columns);

///////////////////////////////////////////////
/// \brief It identifies if the filename extension follows the formatting ".Nxyzf", where the
/// number of columns is `xyz`, and the last character is the type of data (f/d/i), float,
/// double and integer respectively.
///
Bool_t TRestTools::IsBinaryFile(string fname) {
    if (GetBinaryFileColumns(fname) > 0) return true;
    return false;
}

///////////////////////////////////////////////
/// \brief It extracts the number of columns from the filename extension given by argument.
/// The file should contain a binary formatted table with a fixed number of rows and columns.
///
/// The filename extension will be : ".Nxyzf", where the number of columns is `xyz`, and the
/// last character is the type of data (f/d/i), float, double and integer respectively.
///
int TRestTools::GetBinaryFileColumns(string fname) {
    string extension = GetFileNameExtension(fname);
    if (extension.find("N") != 0) {
        return -1;
    }

    size_t pos = extension.find("i");
    if (pos != string::npos) {
        return StringToInteger(extension.substr(1, pos - 1));
    }

    pos = extension.find("f");
    if (pos != string::npos) {
        return StringToInteger(extension.substr(1, pos - 1));
    }

    pos = extension.find("d");
    if (pos != string::npos) {
        return StringToInteger(extension.substr(1, pos - 1));
    }

    RESTError << "Format " << ToUpper(extension) << " not recognized" << RESTendl;
    return -1;
}

///////////////////////////////////////////////
/// \brief It transposes the std::vector<std::vector> table given in the argument.
/// It will transform rows in columns.
///
template <typename T>
void TRestTools::TransposeTable(std::vector<std::vector<T>>& data) {
    if (data.size() == 0) return;

    std::vector<std::vector<T>> trans_vec(data[0].size(), std::vector<T>());

    for (unsigned int i = 0; i < data.size(); i++)
        for (unsigned int j = 0; j < data[i].size(); j++) trans_vec[j].push_back(data[i][j]);

    data = trans_vec;
}

template void TRestTools::TransposeTable<Double_t>(std::vector<std::vector<Double_t>>& data);

template void TRestTools::TransposeTable<Float_t>(std::vector<std::vector<Float_t>>& data);

template void TRestTools::TransposeTable<Int_t>(std::vector<std::vector<Int_t>>& data);

///////////////////////////////////////////////
/// \brief It returns the maximum value for a particular `column` from the table given by
/// argument. If no column is specified in the arguments, then it gets the maximum from the
/// full table.
///
/// This method is available for tables of type Float_t, Double_t and Int_t.
///
template <typename T>
T TRestTools::GetMaxValueFromTable(const std::vector<std::vector<T>>& data, Int_t column) {
    if (data.size() == 0) return 0;
    if (column > -1 && data[0].size() <= (unsigned int)column) return 0;

    T maxValue = data[0][0];
    if (column == -1) {
        for (unsigned int n = 0; n < data.size(); n++)
            for (unsigned int c = 0; c < data[n].size(); c++)
                if (maxValue < data[n][c]) maxValue = data[n][c];
    } else {
        maxValue = data[0][column];
        for (unsigned int n = 0; n < data.size(); n++)
            if (maxValue < data[n][column]) maxValue = data[n][column];
    }

    return maxValue;
}

template Int_t TRestTools::GetMaxValueFromTable<Int_t>(const std::vector<std::vector<Int_t>>& data,
                                                       Int_t column);

template Float_t TRestTools::GetMaxValueFromTable<Float_t>(const std::vector<std::vector<Float_t>>& data,
                                                           Int_t column);

template Double_t TRestTools::GetMaxValueFromTable<Double_t>(const std::vector<std::vector<Double_t>>& data,
                                                             Int_t column);

///////////////////////////////////////////////
/// \brief It returns the minimum value for a particular `column` from the table given by
/// argument. If no column is specified in the arguments, then it gets the minimum from the
/// full table.
///
/// This method is available for tables of type Float_t, Double_t and Int_t.
///
template <typename T>
T TRestTools::GetMinValueFromTable(const std::vector<std::vector<T>>& data, Int_t column) {
    if (data.empty()) return 0;
    if (column != -1 && data[0].size() <= (unsigned int)column) return 0;

    T minValue = data[0][0];
    if (column == -1) {
        for (unsigned int n = 0; n < data.size(); n++)
            for (unsigned int c = 0; c < data[n].size(); c++)
                if (minValue > data[n][c]) minValue = data[n][c];
    } else {
        minValue = data[0][column];
        for (unsigned int n = 0; n < data.size(); n++)
            if (minValue > data[n][column]) minValue = data[n][column];
    }

    return minValue;
}

template Int_t TRestTools::GetMinValueFromTable<Int_t>(const std::vector<std::vector<Int_t>>& data,
                                                       Int_t column);

template Float_t TRestTools::GetMinValueFromTable<Float_t>(const std::vector<std::vector<Float_t>>& data,
                                                           Int_t column);

template Double_t TRestTools::GetMinValueFromTable<Double_t>(const std::vector<std::vector<Double_t>>& data,
                                                             Int_t column);

///////////////////////////////////////////////
/// \brief It returns the lowest increase, different from zero, between the elements of a
/// particular `column` from the table given by argument.
///
/// This method is available for tables of type Float_t, Double_t and Int_t.
///
/// \warning This method will not check every possible column element difference. It will only
/// look for consecutive elements steps.
///
template <typename T>
T TRestTools::GetLowestIncreaseFromTable(std::vector<std::vector<T>> data, Int_t column) {
    if (data.size() == 0 || data[0].size() <= (unsigned int)column) return 0;
    T lowestIncrease = abs(data[0][column] - data[1][column]);
    for (unsigned int n = 1; n < data.size(); n++) {
        T value = abs(data[n - 1][column] - data[n][column]);
        if (lowestIncrease == 0) lowestIncrease = value;
        if (value > 0 && value < lowestIncrease) lowestIncrease = value;
    }
    return lowestIncrease;
}

template Int_t TRestTools::GetLowestIncreaseFromTable<Int_t>(std::vector<std::vector<Int_t>> data,
                                                             Int_t column);

template Float_t TRestTools::GetLowestIncreaseFromTable<Float_t>(std::vector<std::vector<Float_t>> data,
                                                                 Int_t column);

template Double_t TRestTools::GetLowestIncreaseFromTable<Double_t>(std::vector<std::vector<Double_t>> data,
                                                                   Int_t column);

///////////////////////////////////////////////
/// \brief It returns the lowest increase, different from zero, between the elements of a
/// particular `column` from the table given by argument.
///
/// This method is available for tables of type Float_t, Double_t and Int_t.
///
/// \warning This method will not check every possible column element difference. It will only
/// look for consecutive elements steps.
///
template <typename T>
T TRestTools::GetIntegralFromTable(const std::vector<std::vector<T>>& data) {
    if (data.size() == 0) return 0;
    T sum = 0;
    for (unsigned int n = 0; n < data.size(); n++) {
        for (unsigned int m = 0; m < data[n].size(); m++) sum += data[n][m];
    }
    return sum;
}

template Int_t TRestTools::GetIntegralFromTable<Int_t>(const std::vector<std::vector<Int_t>>& data);

template Float_t TRestTools::GetIntegralFromTable<Float_t>(const std::vector<std::vector<Float_t>>& data);

template Double_t TRestTools::GetIntegralFromTable<Double_t>(const std::vector<std::vector<Double_t>>& data);

///////////////////////////////////////////////
/// \brief It returns a vector with the values extracted from the particular `column` inside the `data` table
/// given by argument.
///
/// This method is available for tables of type Float_t, Double_t and Int_t.
///
template <typename T>
std::vector<T> TRestTools::GetColumnFromTable(const std::vector<std::vector<T>>& data, unsigned int column) {
    std::vector<T> columnData;
    if (data.size() == 0 || data[0].size() <= column) return columnData;

    for (unsigned int n = 0; n < data.size(); n++) columnData.push_back(data[n][column]);

    return columnData;
}

template std::vector<Int_t> TRestTools::GetColumnFromTable<Int_t>(const std::vector<std::vector<Int_t>>& data,
                                                                  unsigned int column);

template std::vector<Float_t> TRestTools::GetColumnFromTable<Float_t>(
    const std::vector<std::vector<Float_t>>& data, unsigned int column);

template std::vector<Double_t> TRestTools::GetColumnFromTable<Double_t>(
    const std::vector<std::vector<Double_t>>& data, unsigned int column);

template std::vector<std::string> TRestTools::GetColumnFromTable<std::string>(
    const std::vector<std::vector<std::string>>& data, unsigned int column);

///////////////////////////////////////////////
/// \brief Reads an ASCII file containing a table with values
///
/// This method will open the file fName. This file should contain a tabulated
/// ASCII table containing any format values. The values on the table will be
/// loaded in the matrix provided through the argument `data`. The content of
/// `data` will be cleared in this method.
///
/// If any header in the file is present, it should be skipped using the argument
///`skipLines` or preceding any line inside the header using `#`.
///
/// This table will just split the ASCII elements inside a std::string matrix
///
int TRestTools::ReadASCIITable(string fName, std::vector<std::vector<std::string>>& data, Int_t skipLines,
                               std::string separator) {
    if (!TRestTools::isValidFile((string)fName)) {
        cout << "TRestTools::ReadASCIITable. Error" << endl;
        cout << "Cannot open file : " << fName << endl;
        return 0;
    }

    data.clear();

    std::ifstream fin(fName);

    // First we create a table with string values
    std::vector<std::vector<string>> values;

    for (string line; std::getline(fin, line);) {
        if (skipLines > 0) {
            skipLines--;
            continue;
        }

        if (line.find("#") == string::npos) {
            std::istringstream in(line);

            std::string token;
            std::vector<std::string> tokens;
            while (std::getline(in, token, (char)separator[0])) {
                tokens.push_back(token);
            }
            data.push_back(tokens);
        }
    }

    return 1;
}

///////////////////////////////////////////////
/// \brief Reads an ASCII file containing a table with values
///
/// This method will open the file fName. This file should contain a tabulated
/// ASCII table containing numeric values. The values on the table will be
/// loaded in the matrix provided through the argument `data`. The content of
/// `data` will be cleared in this method.
///
/// If any header in the file is present, it should be skipped using the argument `skipLines`
/// or preceding any line inside the header using `#`.
///
/// Only works with Double_t vector since we use StringToDouble method.
///
int TRestTools::ReadASCIITable(string fName, std::vector<std::vector<Double_t>>& data, Int_t skipLines,
                               std::string separator) {
    if (!TRestTools::isValidFile((string)fName)) {
        cout << "TRestTools::ReadASCIITable. Error" << endl;
        cout << "Cannot open file : " << fName << endl;
        return 0;
    }

    data.clear();

    std::ifstream fin(fName);

    // First we create a table with string values
    std::vector<std::vector<string>> values;

    for (string line; std::getline(fin, line);) {
        if (skipLines > 0) {
            skipLines--;
            continue;
        }

        if (line.find("#") == string::npos) {
            std::istringstream in(line);

            std::string token;
            std::vector<std::string> tokens;
            while (std::getline(in, token, (char)separator[0])) {
                tokens.push_back(token);
            }
            values.push_back(tokens);
        }
    }

    // Filling the double values table (TODO error handling in case ToDouble conversion fails)
    for (unsigned int n = 0; n < values.size(); n++) {
        std::vector<Double_t> dblTmp;
        dblTmp.clear();

        for (unsigned int m = 0; m < values[n].size(); m++) dblTmp.push_back(StringToDouble(values[n][m]));

        data.push_back(dblTmp);
    }

    return 1;
}

///////////////////////////////////////////////
/// \brief Reads an ASCII file containing a table with values
///
/// This method will open the file fName. This file should contain a tabulated
/// ASCII table containing numeric values. The values on the table will be
/// loaded in the matrix provided through the argument `data`. The content of
/// `data` will be cleared in this method.
///
/// If any header in the file is present, it should be skipped using the argument `skipLines`
/// or preceding any line inside the header using `#`.
///
/// This version works with Float_t vector since we use StringToFloat method.
///
int TRestTools::ReadASCIITable(string fName, std::vector<std::vector<Float_t>>& data, Int_t skipLines,
                               std::string separator) {
    if (!TRestTools::isValidFile((string)fName)) {
        cout << "TRestTools::ReadASCIITable. Error" << endl;
        cout << "Cannot open file : " << fName << endl;
        return 0;
    }

    data.clear();

    std::ifstream fin(fName);

    // First we create a table with string values
    std::vector<std::vector<string>> values;

    for (string line; std::getline(fin, line);) {
        if (skipLines > 0) {
            skipLines--;
            continue;
        }

        if (line.find("#") == string::npos) {
            std::istringstream in(line);

            std::string token;
            std::vector<std::string> tokens;
            while (std::getline(in, token, (char)separator[0])) {
                tokens.push_back(token);
            }
            values.push_back(tokens);
        }
    }

    // Filling the float values table (TODO error handling in case ToFloat conversion fails)
    for (unsigned int n = 0; n < values.size(); n++) {
        std::vector<Float_t> dblTmp;
        dblTmp.clear();

        for (unsigned int m = 0; m < values[n].size(); m++) dblTmp.push_back(StringToFloat(values[n][m]));

        data.push_back(dblTmp);
    }

    return 1;
}

///////////////////////////////////////////////
/// \brief Reads a CSV file containing a table with comma separated values
///
/// This method will open the file fName. This file should contain a comma
/// separated table containing numeric values. The values on the table will be
/// loaded in the matrix provided through the argument `data`. The content of
/// `data` will be cleared in this method.
///
/// If any header in the file is present, it should be skipped using the argument `skipLines`
/// or preceding any line inside the header using `#`.
///
/// Only works with Double_t vector since we use StringToDouble method.
///
int TRestTools::ReadCSVFile(std::string fName, std::vector<std::vector<Double_t>>& data, Int_t skipLines) {
    return ReadASCIITable(fName, data, skipLines, ",");
}

///////////////////////////////////////////////
/// \brief Reads a CSV file containing a table with comma separated values
///
/// This method will open the file fName. This file should contain a comma
/// separated table containing numeric values. The values on the table will be
/// loaded in the matrix provided through the argument `data`. The content of
/// `data` will be cleared in this method.
///
/// If any header in the file is present, it should be skipped using the argument `skipLines`
/// or preceding any line inside the header using `#`.
///
/// Only works with Float_t vector since we use StringToFloat method.
///
int TRestTools::ReadCSVFile(std::string fName, std::vector<std::vector<Float_t>>& data, Int_t skipLines) {
    return ReadASCIITable(fName, data, skipLines, ",");
}

///////////////////////////////////////////////
/// \brief Returns true if the file with path filename exists.
///
Int_t TRestTools::isValidFile(const string& path) { return std::filesystem::is_regular_file(path); }

///////////////////////////////////////////////
/// \brief Returns true if the file (or directory) with path filename exists.
///
/// This method will return true even if it is a pure path.
/// We should call `isValidFile` to check if we will be able to open it without
/// problems.
///
bool TRestTools::fileExists(const string& filename) { return std::filesystem::exists(filename); }

///////////////////////////////////////////////
/// \brief Returns true if the **filename** has *.root* extension.
///
bool TRestTools::isRootFile(const string& filename) { return GetFileNameExtension(filename) == "root"; }

///////////////////////////////////////////////
/// \brief It checks if the file has been processed using a REST event processing chain
///
bool TRestTools::isRunFile(const std::string& filename) {
    if (!isRootFile(filename)) return false;

    TFile* f = TFile::Open((TString)filename);

    TIter nextkey(f->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)nextkey())) {
        if ((std::string)key->GetClassName() == "TRestRun") return true;
    }
    return false;
}

///////////////////////////////////////////////
/// \brief It checks if the file contains a dataset object
///
bool TRestTools::isDataSet(const std::string& filename) {
    if (!isRootFile(filename)) return false;

    if (!TRestTools::fileExists(filename)) return false;

    TFile* f = TFile::Open((TString)filename);

    TIter nextkey(f->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)nextkey())) {
        if ((std::string)key->GetClassName() == "TRestDataSet") return true;
    }
    return false;
}

///////////////////////////////////////////////
/// \brief Returns true if **filename** is an *http* address.
///
bool TRestTools::isURL(const string& s) {
    std::regex pattern("^https?://(.+)");
    return std::regex_match(s, pattern);
}

///////////////////////////////////////////////
/// \brief Returns true when ROOT resolves **filename** to a non-local protocol.
///
bool TRestTools::IsRemoteRootPath(const std::string& filename) {
    TUrl url(filename.c_str(), kTRUE);
    return url.IsValid() && std::string(url.GetProtocol()) != "file";
}

#ifdef REST_TESTING_ENABLED
///////////////////////////////////////////////
/// \brief Forces one writable ROOT preflight failure for no-mutation tests.
///
void TRestTools::ForceNextRootUpdatePreflightFailureForTesting() {
    gForceNextRootUpdatePreflightFailure.store(true);
}

///////////////////////////////////////////////
/// \brief Forces one post-replacement validation failure for rollback tests.
///
void TRestTools::ForceNextTransactionalMergeValidationFailureForTesting() {
    gForceNextMergeValidationFailure.store(true);
}
#endif

///////////////////////////////////////////////
/// \brief Transactionally merges ROOT files into a local destination.
///
bool TRestTools::MergeRootFilesTransactionally(const std::string& outputFile,
                                               const std::vector<std::string>& inputFiles,
                                               const std::string& existingTarget, bool removeInputsOnSuccess,
                                               std::string* error) {
    std::string localError;
    if (IsRemoteRootPath(outputFile)) {
        localError = "REST refuses to replace remote ROOT destination '" + outputFile +
                     "'. Choose an explicit local output file.";
        if (error != nullptr) *error = localError;
        return false;
    }

    const auto localOutput = LocalRootPath(outputFile);
    if (!localOutput) {
        localError = "Cannot resolve local ROOT output path '" + outputFile + "'";
        if (error != nullptr) *error = localError;
        return false;
    }
    std::error_code ec;
    const std::filesystem::path target = std::filesystem::absolute(*localOutput, ec);
    if (ec) {
        localError = "Cannot resolve absolute output path '" + outputFile + "': " + ec.message();
        if (error != nullptr) *error = localError;
        return false;
    }
    if (target.parent_path().empty() || !std::filesystem::exists(target.parent_path())) {
        localError = "Output directory does not exist for '" + target.string() + "'";
        if (error != nullptr) *error = localError;
        return false;
    }

    std::vector<std::string> sources;
    if (!existingTarget.empty()) sources.push_back(existingTarget);
    sources.insert(sources.end(), inputFiles.begin(), inputFiles.end());
    if (sources.empty()) {
        localError = "No ROOT input files were provided for transactional merge";
        if (error != nullptr) *error = localError;
        return false;
    }

    RootSchemaSnapshot expectedSchema;
    RootContentManifest expectedContent;
    RootContentManifest expectedInputContent;
    std::set<Int_t> resolvedSchemaNumbers;
    Int_t maximumSchemaNumber = 0;
    for (std::size_t sourceIndex = 0; sourceIndex < sources.size(); ++sourceIndex) {
        const auto& sourceName = sources[sourceIndex];
        std::unique_ptr<TFile> source(TFile::Open(sourceName.c_str(), "READ"));
        RootSchemaSnapshot sourceSchema;
        RootContentManifest sourceContent;
        std::vector<Int_t> sourceSchemaNumbers;
        Int_t sourceMaximumNumber = 0;
        if (source == nullptr || !source->IsOpen() || source->IsZombie()) {
            localError = "Cannot open ROOT merge input '" + sourceName + "'";
            if (error != nullptr) *error = localError;
            return false;
        }
        if (!ReadSchemaSnapshot(*source, sourceSchema, localError) ||
            !ResolveOnDiskStreamerInfos(*source, sourceSchemaNumbers, sourceMaximumNumber, localError) ||
            !LoadOnDiskSchemaRules(sourceSchema, localError) ||
            !ReadContentManifest(*source, "", sourceContent, localError)) {
            localError = "Cannot inspect ROOT merge input '" + sourceName + "': " + localError;
            if (error != nullptr) *error = localError;
            return false;
        }
        RootContentManifest& accumulatedContent =
            !existingTarget.empty() && sourceIndex == 0 ? expectedContent : expectedInputContent;
        if (!AddContentManifest(sourceContent, accumulatedContent, localError)) {
            localError = "Cannot merge ROOT input '" + sourceName + "': " + localError;
            if (error != nullptr) *error = localError;
            return false;
        }
        expectedSchema.streamerInfos.insert(sourceSchema.streamerInfos.begin(),
                                            sourceSchema.streamerInfos.end());
        expectedSchema.schemaRules.insert(sourceSchema.schemaRules.begin(), sourceSchema.schemaRules.end());
        resolvedSchemaNumbers.insert(sourceSchemaNumbers.begin(), sourceSchemaNumbers.end());
        maximumSchemaNumber = std::max(maximumSchemaNumber, sourceMaximumNumber);
    }

    // Match TFileMerger's historical UPDATE contract: worker inputs merge
    // with one another, then replace same-named target objects. Target-only
    // keys remain opaque. Treating the target as another additive input is
    // what caused #567 to rewrite existing TRestAnalysisTree objects.
    for (const auto& [path, inputEntry] : expectedInputContent) {
        const auto targetEntry = expectedContent.find(path);
        if (targetEntry != expectedContent.end() && targetEntry->second.className != inputEntry.className) {
            localError = "ROOT merge input key '" + path + "' has incompatible classes '" +
                         targetEntry->second.className + "' and '" + inputEntry.className + "'";
            if (error != nullptr) *error = localError;
            return false;
        }
        expectedContent[path] = inputEntry;
    }

    const std::filesystem::path temporary = UniqueSiblingPath(target, "merge");
    const std::filesystem::path backup = UniqueSiblingPath(target, "backup");
    if (temporary.empty() || backup.empty()) {
        localError = "Cannot allocate temporary paths next to '" + target.string() + "'";
        if (error != nullptr) *error = localError;
        return false;
    }

    auto removeTemporary = [&temporary](std::string& primaryError) {
        std::error_code cleanupError;
        std::filesystem::remove(temporary, cleanupError);
        if (cleanupError)
            primaryError += ". REST also could not remove temporary file '" + temporary.string() +
                            "': " + cleanupError.message();
    };

    if (!existingTarget.empty()) {
        const auto localExisting = LocalRootPath(existingTarget);
        if (localExisting) {
            std::filesystem::copy_file(*localExisting, temporary, std::filesystem::copy_options::none, ec);
            if (ec)
                localError =
                    "Cannot seed temporary merge output from '" + existingTarget + "': " + ec.message();
        } else if (!TFile::Cp(existingTarget.c_str(), temporary.c_str(), kFALSE)) {
            localError = "ROOT could not seed temporary merge output from '" + existingTarget + "'";
        }
    }

    TRestRootFileHandle outputHandle;
    if (localError.empty()) {
        outputHandle =
            TRestRootFileHandle::Open(temporary.string(), existingTarget.empty() ? TRestRootFileMode::Recreate
                                                                                 : TRestRootFileMode::Update);
        if (!outputHandle)
            localError =
                "Cannot prepare temporary merge output '" + temporary.string() + "': " + outputHandle.Error();
    }
    const std::vector<Int_t> allSchemaNumbers(resolvedSchemaNumbers.begin(), resolvedSchemaNumbers.end());
    if (localError.empty() &&
        !ApplySchemaUpdate(*outputHandle, allSchemaNumbers, maximumSchemaNumber, localError)) {
        localError = "Cannot preserve input schemas in temporary merge output '" + temporary.string() +
                     "': " + localError;
    }

    if (localError.empty() && !inputFiles.empty()) {
        TFileMerger merger(kFALSE);
        merger.SetPrintLevel(0);
        if (!merger.OutputFile(std::move(outputHandle.fFile))) {
            localError = "ROOT could not adopt temporary merge output '" + temporary.string() + "'";
        } else {
            // An existing destination seeds the temporary file byte-for-byte.
            // Only new worker inputs are merged, preserving the historical
            // TFileMerger UPDATE semantics for target-only objects.
            for (const auto& source : inputFiles) {
                if (!merger.AddFile(source.c_str(), kFALSE)) {
                    localError = "ROOT could not add merge input '" + source + "'";
                    break;
                }
            }
        }
        if (localError.empty() && !merger.Merge())
            localError = "ROOT failed while merging into temporary output '" + temporary.string() + "'";
    } else if (localError.empty() && !outputHandle.Close()) {
        localError =
            "ROOT reported a write error while preparing temporary output '" + temporary.string() + "'";
    }
    if (!localError.empty()) {
        removeTemporary(localError);
        if (error != nullptr) *error = localError;
        return false;
    }

    bool temporaryValid = false;
    {
        std::unique_ptr<TFile> merged(TFile::Open(temporary.c_str(), "READ"));
        RootSchemaSnapshot actualSchema;
        RootContentManifest actualContent;
        temporaryValid = merged != nullptr && merged->IsOpen() && !merged->IsZombie() &&
                         ReadSchemaSnapshot(*merged, actualSchema, localError) &&
                         IsSubset(expectedSchema, actualSchema, localError) &&
                         ReadContentManifest(*merged, "", actualContent, localError) &&
                         ValidateContentManifest(expectedContent, actualContent, localError);
    }
    if (!temporaryValid) {
        if (localError.empty()) localError = "Cannot validate temporary ROOT merge output";
        removeTemporary(localError);
        if (error != nullptr) *error = localError;
        return false;
    }

    const bool targetExisted = std::filesystem::exists(target);
    if (targetExisted) {
        const auto permissions = std::filesystem::status(target, ec).permissions();
        if (ec) {
            localError = "Cannot inspect permissions of '" + target.string() + "': " + ec.message();
            removeTemporary(localError);
            if (error != nullptr) *error = localError;
            return false;
        }
        std::filesystem::permissions(temporary, permissions, ec);
        if (ec) {
            localError = "Cannot preserve permissions on temporary ROOT output: " + ec.message();
            removeTemporary(localError);
            if (error != nullptr) *error = localError;
            return false;
        }
    }

    if (!ReplaceLocalFile(temporary, target, backup, localError)) {
        removeTemporary(localError);
        if (error != nullptr) *error = localError;
        return false;
    }

    bool replacementValid = false;
    {
        std::unique_ptr<TFile> merged(TFile::Open(target.c_str(), "READ"));
        RootSchemaSnapshot actualSchema;
        RootContentManifest actualContent;
        replacementValid = merged != nullptr && merged->IsOpen() && !merged->IsZombie() &&
                           ReadSchemaSnapshot(*merged, actualSchema, localError) &&
                           IsSubset(expectedSchema, actualSchema, localError) &&
                           ReadContentManifest(*merged, "", actualContent, localError) &&
                           ValidateContentManifest(expectedContent, actualContent, localError);
        if (!replacementValid && localError.empty())
            localError = "Cannot validate replaced ROOT output '" + target.string() + "'";
    }
#ifdef REST_TESTING_ENABLED
    if (gForceNextMergeValidationFailure.exchange(false)) {
        replacementValid = false;
        localError = "Forced post-replacement ROOT validation failure for testing";
    }
#endif
    if (!replacementValid) {
        std::string rollbackError;
        if (!RollBackLocalFile(target, backup, targetExisted, rollbackError))
            localError +=
                ". Rollback also failed: " + rollbackError + ". The backup is at '" + backup.string() + "'";
        if (error != nullptr) *error = localError;
        return false;
    }

    if (targetExisted) {
        ec.clear();
        std::filesystem::remove(backup, ec);
        if (ec) {
            localError = "Merged output is valid, but REST could not remove rollback backup '" +
                         backup.string() + "': " + ec.message();
            if (error != nullptr) *error = localError;
            return false;
        }
    }

    if (removeInputsOnSuccess) {
        std::vector<std::string> removalFailures;
        for (const auto& input : inputFiles) {
            const auto localInput = LocalRootPath(input);
            if (!localInput) continue;
            ec.clear();
            const auto absoluteInput = std::filesystem::absolute(*localInput, ec);
            if (ec) {
                removalFailures.push_back("'" + localInput->string() +
                                          "': cannot resolve absolute path: " + ec.message());
                continue;
            }
            if (absoluteInput == target) continue;
            const bool removed = std::filesystem::remove(absoluteInput, ec);
            std::error_code existsError;
            const bool stillExists = std::filesystem::exists(absoluteInput, existsError);
            if (ec || existsError || (!removed && stillExists))
                removalFailures.push_back(
                    "'" + absoluteInput.string() + "': " +
                    (ec ? ec.message() : (existsError ? existsError.message() : "file was not removed")));
        }
        if (!removalFailures.empty()) {
            localError = "Merged output is valid, but REST could not remove input file(s): ";
            for (std::size_t index = 0; index < removalFailures.size(); ++index) {
                if (index != 0) localError += "; ";
                localError += removalFailures[index];
            }
            if (error != nullptr) *error = localError;
            return false;
        }
    }
    if (error != nullptr) error->clear();
    return true;
}

///////////////////////////////////////////////
/// \brief Returns true if the **path** given by argument is writable
///
bool TRestTools::isPathWritable(const string& path) {
    int result = 0;
#ifdef WIN32
    result = _access(path.c_str(), 2);
#else
    result = access(path.c_str(), 2);
#endif

    if (result == 0)
        return true;
    else
        return false;
}

///////////////////////////////////////////////
/// \brief Check if the path is absolute path or not
///
bool TRestTools::isAbsolutePath(const string& path) {
    if (path[0] == '/' || path[0] == '~' || path.find(':') != string::npos) {
        return true;
    }
    return false;
}

///////////////////////////////////////////////
/// \brief Separate path and filename in a full path+filename string, returns a
/// pair of string
///
/// if input file name contains no directory, the returned directory is set to
/// "." if input file name contains no file, the returned filename is set to ""
/// e.g.
/// Input: "/home/nkx/abc.txt" and ":def", Output: { "/home/nkx/", "abc.txt" }
/// Input: "abc.txt" and ":", Output: { ".", "abc.txt" }
/// Input: "/home/nkx/" and ":", Output: { "/home/nkx/", "" }
///
std::pair<string, string> TRestTools::SeparatePathAndName(const string& fullname) {
    filesystem::path path(fullname);
    return {path.parent_path().string(), path.filename().string()};
}

///////////////////////////////////////////////
/// \brief Gets the file extension as the substring found after the latest "."
///
/// Input: "/home/jgalan/abc.txt" Output: "txt"
///
string TRestTools::GetFileNameExtension(const string& fullname) {
    string extension = filesystem::path(fullname).extension().string();
    if (extension.size() > 1) return extension.substr(1);
    return extension;
}

///////////////////////////////////////////////
/// \brief Gets the filename root as the substring found before the latest "."
///
/// Input: "/home/jgalan/abc.txt" Output: "abc"
///
string TRestTools::GetFileNameRoot(const string& fullname) {
    return filesystem::path(fullname).stem().string();
}

//////////////////////////////////////////////////
/// \brief Returns a vector with the observables names found in the input string.
/// The observables names must contain the character "_" to be identified as such.
/// e.g. Input: "x1_x2 + x3 - x4*y_z". Output: {"x1_x2", "y_z"}
/// Input: "hitsAna_xMean*hitsAna_xMean+hitsAna_yMean*hitsAna_yMean<100" and true,
// Output: {"hitsAna_xMean", "hitsAna_yMean"}.
/// Input: "hitsAna_xMean*hitsAna_xMean+hitsAna_yMean*hitsAna_yMean<100" and false,
/// Output: {"hitsAna_xMean", "hitsAna_xMean", "hitsAna_yMean", "hitsAna_yMean"}
///
std::vector<std::string> TRestTools::GetObservablesInString(const std::string& observablesStr,
                                                            bool removeDuplicates) {
    std::vector<std::string> obsList;
    std::string text = observablesStr;
    while (text.find("_") != std::string::npos) {
        size_t pos_ = text.find("_");
        size_t beginning = text.find_last_of(" -+*/)(^%<>", pos_) + 1;
        size_t end = text.find_first_of(" -+*/)(^%<>", pos_);
        std::string obs = text.substr(beginning, end - beginning);
        text = Replace(text, obs, "1", 0, removeDuplicates ? 0 : 1);
        obsList.push_back(obs);
    }
    return obsList;
}

////////////////////////////////////////////////////////////
/// \brief Returns a set of strings that match the wanted strings from the stack.
/// The wanted strings can contain wildcards "*" and "?".
/// \param stack: vector of strings to be searched
/// \param wantedStrings: vector of strings with the wanted strings to be matched.
/// \return a set of strings that match the wanted strings
/// e.g.
/// Input: stack = {"x1", "x2", "x11", "y1", "y2", "y11", "z1", "z2"},
/// wantedStrings = {"x*", "y?", "z1"},
/// Output: {"x1", "x11", "x2", "y1", "y2", "z1"}
///
std::set<std::string> TRestTools::GetMatchingStrings(const std::vector<std::string>& stack,
                                                     const std::vector<std::string>& wantedStrings) {
    std::set<std::string> result;
    for (auto& ws : wantedStrings) {
        if (ws.find("*") != std::string::npos || ws.find("?") != std::string::npos) {
            for (auto& c : stack)
                if (MatchString(c, ws)) result.insert(c);
        } else if (std::find(stack.begin(), stack.end(), ws) != stack.end())
            result.insert(ws);
    }
    // return std::vector<std::string>(result.begin(), result.end()); // convert to vector
    return result;
}

///////////////////////////////////////////////
/// \brief Returns the input string but without multiple slashes ("/")
///
/// \param str: input path string (e.g. "///home///test/")
/// \return path string without multiple slashes (e.g. "/home/test/")
///
string TRestTools::RemoveMultipleSlash(string str) {
    // we replace multiple appearances of "/" (slash) by a single "/"
    string to_replace = "//";
    size_t start_pos = str.find(to_replace);
    while (start_pos != string::npos) {
        str.replace(start_pos, to_replace.length(), "/");
        start_pos = str.find(to_replace);
    }
    return str;
}

///////////////////////////////////////////////
/// \brief Removes all directories in the full path filename description
/// given in the argument.
///
/// e.g.
/// Input: "/home/nkx/abc.txt", Returns: "abc.txt"
/// Input: "/home/nkx/", Output: ""
///
string TRestTools::GetPureFileName(const string& path) { return filesystem::path(path).filename().string(); }

///////////////////////////////////////////////
/// \brief It takes a path and returns its absolute path
///
string TRestTools::ToAbsoluteName(const string& filename) {
    filesystem::path path;
    for (const auto& directory : filesystem::path(filename)) {
        if (path.empty() && directory == "~") {
            // path starts with ~
            const auto envVariableHome = getenv("HOME");
            if (envVariableHome == nullptr) {
                cout << "TRestTools::ToAbsoluteName - ERROR - "
                        "cannot resolve ~ because 'HOME' env variable does not exist"
                     << endl;
                exit(1);
            }
            const auto userHomePath = filesystem::path(envVariableHome);
            if (userHomePath.empty()) {
                cout << "TRestTools::ToAbsoluteName - ERROR - "
                        "cannot resolve ~ because 'HOME' env variable is not set to a valid value"
                     << endl;
                exit(1);
            }
            path /= userHomePath;
        } else {
            path /= directory;
        }
    }
    return filesystem::weakly_canonical(path).string();
}

///////////////////////////////////////////////
/// \brief It lists all the subdirectories inside path and adds
/// them to the result vector.
///
/// If recursion is 0, then list only the subdirectory of this directory
/// If recursion is < 0, then list subdirectories recursively
///
/// Otherwise recurse only certain times.
///
vector<string> TRestTools::GetSubdirectories(const string& _path, int recursion) {
    vector<string> result;

    std::filesystem::path path(_path);
    if (exists(path)) {
        std::filesystem::directory_iterator iter(path);
        for (auto& it : iter) {
            if (it.is_directory()) {
                result.push_back(it.path().string());

                if (recursion != 0) {
                    vector<string> subD = GetSubdirectories(it.path().string(), recursion - 1);
                    result.insert(result.begin(), subD.begin(), subD.end());
                }
            }
        }
    }

    return result;
}

///////////////////////////////////////////////
/// \brief Search file in the given vector of path strings, return a full name
/// if found, return "" if not
///
string TRestTools::SearchFileInPath(vector<string> paths, string filename) {
    if (fileExists(filename)) {
        return filename;
    } else {
        for (unsigned int i = 0; i < paths.size(); i++) {
            string path = paths[i];
            if (path[path.size() - 1] != '/') {
                path = path + "/";
            }

            if (fileExists(path + filename)) {
                return path + filename;
            }

            // search also in subdirectory, but only 5 times of recursion
            vector<string> pathsExpanded = GetSubdirectories(paths[i], 5);
            for (unsigned int j = 0; j < pathsExpanded.size(); j++)
                if (fileExists(pathsExpanded[j] + "/" + filename)) return pathsExpanded[j] + "/" + filename;
        }
    }
    return "";
}

///////////////////////////////////////////////
/// \brief Checks if the config file can be opened (and thus exists).
/// It returns true in case of success, false otherwise.
///
bool TRestTools::CheckFileIsAccessible(const std::string& filename) {
    ifstream ifs;
    ifs.open(filename.c_str());

    if (!ifs) {
        return false;
    } else {
        ifs.close();
    }
    return true;
}

///////////////////////////////////////////////
/// \brief Returns a list of files whose name match the pattern string. Key word
/// is "*". e.g. abc00*.root
///
/// Argument unlimited will fix an issue with the number of files being to high.
/// However, it causes issues when searching/listing the macros.
/// The default value for unlimited is `false`.
///
vector<string> TRestTools::GetFilesMatchingPattern(string pattern, bool unlimited) {
    std::vector<string> outputFileNames;
    if (pattern != "") {
        vector<string> items = Split(pattern, "\n");
        for (auto item : items) {
            if (item.find_first_of("*?") != string::npos) {
#ifdef WIN32
                item = Replace(item, "/", "\\");
                string item_trim =
                    item.substr(0, item.find_first_of("*?"));  // trim string to before wildcard character
                auto path_name = SeparatePathAndName(item_trim);
                string _path = path_name.first;
                if (!std::filesystem::exists(_path)) {
                    RESTError << "TRestTools::GetFilesMatchingPattern(): path " << _path << " does not exist!"
                              << RESTendl;
                    return outputFileNames;
                }

                std::filesystem::path path(_path);
                std::filesystem::recursive_directory_iterator iter(path);
                for (auto& it : iter) {
                    if (it.is_regular_file()) {
                        string filename = it.path().string();
                        if (MatchString(filename, item)) {
                            outputFileNames.push_back(it.path().string());
                        }
                    }
                }
#else
                auto path_name = SeparatePathAndName(item);
                if (unlimited) {
                    std::string currentDir = filesystem::current_path();
                    ChangeDirectory(path_name.first);
                    string a = Execute("find -type f -name \'" + path_name.second + "\'");
                    ChangeDirectory(currentDir);
                    auto b = Split(a, "\n");

                    for (unsigned int i = 0; i < b.size(); i++) {
                        outputFileNames.push_back(path_name.first + "/" + b[i]);
                    }

                } else {
                    string a = Execute("find " + item);
                    auto b = Split(a, "\n");

                    for (unsigned int i = 0; i < b.size(); i++) {
                        outputFileNames.push_back(b[i]);
                    }
                }
#endif

            } else {
                if (fileExists(item)) outputFileNames.push_back(item);
            }
        }
    }
    return outputFileNames;
}

///////////////////////////////////////////////
/// \brief Convert version to a unique string
///
int TRestTools::ConvertVersionCode(string in) {
#ifndef REST_Version
#define REST_VERSION(a, b, c) (((a) << 16) + ((b) << 8) + (c))
#endif
    vector<string> ver = Split(in, ".");
    if (ver.size() == 3) {
        vector<int> verint;
        for (auto v : ver) {
            int n = StringToInteger(v.substr(0, v.find_first_not_of("0123456789")));
            if (n != -1) {
                verint.push_back(n);
            } else {
                return -1;
            }
        }
        return REST_VERSION(verint[0], verint[1], verint[2]);
    }
    return -1;
}

///////////////////////////////////////////////
/// \brief Executes a shell command and returns its output in a string
///
string TRestTools::Execute(string cmd) {
    std::array<char, 128> buffer;
    string result;
#ifdef WIN32
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(("powershell.exe " + cmd).c_str(), "r"), _pclose);
#else
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
#endif  // WIN32

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if (result.size() > 0 && result[result.size() - 1] == '\n')
        result = result.substr(0, result.size() - 1);  // remove last "\n"

    return result;
}

///////////////////////////////////////////////
/// \brief It reads the next line from the incoming istream and puts it
/// in the string argument `t`. The remaining istream is returned.
///
std::istream& TRestTools::GetLine(std::istream& is, std::string& t) {
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for (;;) {
        int c = sb->sbumpc();
        switch (c) {
            case '\n':
                return is;
            case '\r':
                if (sb->sgetc() == '\n') sb->sbumpc();
                return is;
            case std::streambuf::traits_type::eof():
                // Also handle the case when the last line has no line ending
                if (t.empty()) is.setstate(std::ios::eofbit);
                return is;
            default:
                t += (char)c;
        }
    }
}

///////////////////////////////////////////////
/// \brief download the remote file automatically, returns the downloaded file name.
///
/// The file name is given in url format, and is parsed by TUrl. Various methods
/// will be used, including scp, wget. Downloads to REST_USER_PATH + "/download/" + filename
/// by default.
///
std::string TRestTools::DownloadRemoteFile(const string& url, bool pidPrefix) {
    string pureName = TRestTools::GetPureFileName(url);
    if (pureName.empty()) {
        RESTWarning << "error! (TRestTools::DownloadRemoteFile): url is not a file!" << RESTendl;
        RESTWarning << "please specify a concrete file name in this url" << RESTendl;
        RESTWarning << "url: " << url << RESTendl;
        return "";
    }

    if (url.find("local:") == 0) {
        return Replace(url, "local:", "");
    } else {
        string fullpath =
            REST_USER_PATH + "/download/" + (pidPrefix ? "PID_" + ToString(getpid()) + "_" : "") + pureName;
        int out;
        int attempts = 10;
        do {
            out = TRestTools::DownloadRemoteFile(url, fullpath);
            if (out == 1024) {
                RESTWarning << "Retrying download in 5 seconds" << RESTendl;
                std::this_thread::sleep_for(std::chrono::seconds(5));
            } else if (attempts < 10) {
                RESTSuccess << "Download succeeded after " << 10 - attempts << " attempts" << RESTendl;
            }
            attempts--;
        } while (out == 1024 && attempts > 0);

        if (out == 0 || TRestTools::fileExists(fullpath)) {
            return fullpath;
        } else {
            return "";
        }
    }
}

///////////////////////////////////////////////
/// \brief download the remote file to the given local address.
///
/// The file name is given in url format, and is parsed by TUrl. Various methods
/// will be used, including scp, wget. returns 0 if succeed.
///
int TRestTools::DownloadRemoteFile(string remoteFile, string localFile) {
    TUrl url(remoteFile.c_str());

    RESTInfo << "Downloading remote file : " << remoteFile << RESTendl;
    RESTInfo << "To local file : " << localFile << RESTendl;

    string localFiletmp = localFile + ".restdownload";
    if ((string)url.GetProtocol() == "https" || (string)url.GetProtocol() == "http") {
        string path = TRestTools::SeparatePathAndName(localFiletmp).first;
        if (!TRestTools::fileExists(path)) {
            if (!filesystem::create_directories(path)) {
                std::cerr << "mkdir failed to create directory: " << path << std::endl;
                return -1;
            }
        }

        string cmd = "wget --no-check-certificate " + EscapeSpecialLetters(remoteFile) + " -O " +
                     EscapeSpecialLetters(localFiletmp) + " -q";
        RESTDebug << cmd << RESTendl;
        int a = system(cmd.c_str());

        if (a == 0) {
            rename(localFiletmp.c_str(), localFile.c_str());
            return 0;
        } else {
            RESTError << "download failed! (" << remoteFile << ")" << RESTendl;
            if (a == 1024) {
                RESTError << "Network connection problem?" << RESTendl;
                return 1024;
            }
            if (a == 2048) {
                RESTError << "File does NOT exist in remotely?" << RESTendl;
                return 2048;
            }
        }
    } else if ((string)url.GetProtocol() == "ssh") {
        string cmd = "scp -P " + ToString(url.GetPort() == 0 ? 22 : url.GetPort()) + " " + url.GetUser() +
                     "@" + url.GetHost() + ":" + EscapeSpecialLetters(url.GetFile()) + " " + localFiletmp;
        cout << cmd << endl;
        int a = system(cmd.c_str());
        if (a == 0) {
            rename(localFiletmp.c_str(), localFile.c_str());
            return 0;
        }
    } else {
        if (!TRestTools::fileExists(remoteFile)) {
            RESTWarning << "Trying to download: " << remoteFile << RESTendl;
            RESTWarning << "Unknown protocol!" << RESTendl;
        }
    }

    return -1;
}

///////////////////////////////////////////////
/// \brief It performs a POST web protocol request using a set of keys and values given
/// by argument, and returns the result as a string.
///
std::string TRestTools::POSTRequest(const std::string& url, const std::map<std::string, std::string>& keys) {
    std::string file_content = "";
#ifdef USE_Curl
    CURL* curl;
    CURLcode res;

    string filename = REST_USER_PATH + "/download/curl.out";

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    FILE* f = fopen(filename.c_str(), "wt");

    std::string request = "";
    int n = 0;
    for (auto const& x : keys) {
        if (n > 0) request += "&";
        request += x.first + "=" + x.second;
        n++;
    }
    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)f);
        /* Now specify the POST data */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.c_str());

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            RESTError << "curl_easy_perform() failed: " << curl_easy_strerror(res) << RESTendl;

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    fclose(f);
    curl_global_cleanup();

    std::getline(std::ifstream(filename), file_content, '\0');
#else
    RESTError << "TRestTools::POSTRequest. REST framework was compiled without CURL support" << RESTendl;
    RESTError << "Please recompile REST after installing curl development libraries." << RESTendl;
    RESTError << "Depending on your system this might be: curl-dev, curl-devel or libcurl-openssl-dev. "
              << RESTendl;
    RESTError << "No file will be downloaded" << RESTendl;
#endif

    return file_content;
}

///////////////////////////////////////////////
/// Upload the local file to remote file, method url will overwrite the login information
/// inside remotefile.
///
/// Example: UploadToServer("/home/nkx/abc.txt", "https://sultan.unizar.es/gasFiles/gases.rml",
/// "ssh://nkx:M123456@:8322") Then, the local file abc.txt will be uploaded to the server,
/// renamed to gases.rml and overwrite it
int TRestTools::UploadToServer(string localFile, string remoteFile, string methodUrl) {
    if (!TRestTools::fileExists(localFile)) {
        cout << "error! local file not exist!" << endl;
        return -1;
    }
    // construct path
    // [proto://][user[:passwd]@]host[:port]/file.ext[#anchor][?options]
    TUrl url(remoteFile.c_str());
    TUrl method(methodUrl.c_str());
    if (method.GetProtocol() != (string) "") url.SetProtocol(method.GetProtocol());
    if (method.GetPort() != 0) url.SetPort(method.GetPort());
    if (method.GetUser() != (string) "") url.SetUser(method.GetUser());
    if (method.GetPasswd() != (string) "") url.SetPasswd(method.GetPasswd());

    if ((string)url.GetProtocol() == "https" || (string)url.GetProtocol() == "http") {
        // maybe we use curl to upload to http in future
    } else if ((string)url.GetProtocol() == "ssh") {
        string cmd = "scp -P " + ToString(url.GetPort() == 0 ? 22 : url.GetPort()) + " " +
                     EscapeSpecialLetters(localFile) + " " + url.GetUser() + "@" + url.GetHost() + ":" +
                     EscapeSpecialLetters(url.GetFile());
        cout << cmd << endl;
        int a = system(cmd.c_str());

        if (a != 0) {
            RESTError << __PRETTY_FUNCTION__ << RESTendl;
            RESTError << "problem copying gases definitions to remote server" << RESTendl;
            RESTError << "Please report this problem at "
                         "http://gifna.unizar.es/rest-forum/"
                      << RESTendl;
            return -1;
        }

        return 0;
    }
    return 0;
}

void TRestTools::ChangeDirectory(const string& toDirectory) { filesystem::current_path(toDirectory); }

///////////////////////////////////////////////
/// \brief It returns a vector with 2 components {a,b}, the components satisfy that `a x b = n`,
/// being the ratio a/b as close to 1 as possible.
///
/// This method can be used to help dividing a canvas that will contain a number `n` of plots.
///
/// If `n` is a prime number, then the pair generated will be `n x 1`.
///
std::vector<int> TRestTools::CanvasDivisions(int n) {
    std::vector<int> r;
    for (int i = 2; i * i <= n; i += 1 + (i > 2)) {
        while ((n % i) == 0) {
            r.push_back(i);
            n /= i;
        }
    }
    if (n != 1) r.push_back(n);

    while (r.size() > 2) {
        // We multiply the 2 lowest elements and
        // replace the elements in the vector by the result
        auto min1 = std::min_element(r.begin(), r.end());
        int low1 = *min1;

        // Remove the first element equal to min1 (efficient way)
        auto it = std::find(r.begin(), r.end(), low1);
        if (it != r.end()) {
            std::iter_swap(it, r.end() - 1);
            r.erase(r.end() - 1);
        }

        auto min2 = std::min_element(r.begin(), r.end());
        int low2 = *min2;

        // Remove the first element equal to min2 (efficient way)
        it = std::find(r.begin(), r.end(), low2);
        if (it != r.end()) {
            std::iter_swap(it, r.end() - 1);
            r.erase(r.end() - 1);
        }

        int resultado = low1 * low2;
        r.push_back(resultado);
    }

    std::sort(r.begin(), r.end());

    if (r.size() == 1) r.push_back(1);

    return r;
}

string ValueWithQuantity::ToString() const {
    string unit;
    auto value = fValue;
    if (fQuantity == ENERGY) {
        unit = "eV";
        value *= 1E3;  // since we store energy in keV, not in eV
    } else if (fQuantity == TIME) {
        unit = "s";  // time is stored in microseconds
        value *= 1E-6;
    } else if (fQuantity == LENGTH) {
        unit = "m";
        value *= 1E-3;  // since we store length in mm, not in m
    } else {
        return "";
    }

    const auto abs = TMath::Abs(value);
    if (abs == 0) {
        return TString::Format("%d", 0).Data();
    } else if (abs < 1E-6) {
        return TString::Format("%.2f %s%s", value * 1E9, "n", unit.c_str()).Data();
    } else if (abs < 1E-3) {
        return TString::Format("%.2f %s%s", value * 1E6, "u", unit.c_str()).Data();
    } else if (abs < 1E0) {
        return TString::Format("%.2f %s%s", value * 1E3, "m", unit.c_str()).Data();
    } else if (abs < 1E3) {
        return TString::Format("%.2f %s%s", value * 1E0, "", unit.c_str()).Data();
    } else if (abs < 1E6) {
        return TString::Format("%.2f %s%s", value * 1E-3, "k", unit.c_str()).Data();
    } else if (abs < 1E9) {
        return TString::Format("%.2f %s%s", value * 1E-6, "M", unit.c_str()).Data();
    } else if (abs < 1E12) {
        return TString::Format("%.2f %s%s", value * 1E-9, "G", unit.c_str()).Data();
    } else {
        return TString::Format("%.2f %s%s", value * 1E-12, "T", unit.c_str()).Data();
    }
}

string ToTimeStringLong(double seconds) {
    const auto abs = TMath::Abs(seconds);
    if (abs < 60) {
        return TString::Format("%.2f %s", seconds, "seconds").Data();
    } else if (abs < 60 * 60) {
        return TString::Format("%.2f %s", seconds / 60.0, "minutes").Data();
    } else if (abs < 60 * 60 * 24) {
        return TString::Format("%.2f %s", seconds / (60.0 * 60.0), "hours").Data();
    } else {
        return TString::Format("%.2f %s", seconds / (60.0 * 60.0 * 24.0), "days").Data();
    }
}
