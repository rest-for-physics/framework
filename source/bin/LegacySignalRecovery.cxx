#include "LegacySignalRecovery.h"

#include <TBranch.h>
#include <TFile.h>
#include <TKey.h>
#include <TRestDetectorSignalEvent.h>
#include <TRestTools.h>
#include <TString.h>
#include <TSystem.h>
#include <TTree.h>

#include <cerrno>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <set>
#include <string>
#include <system_error>
#include <vector>

#ifndef _WIN32
#include <sys/wait.h>
#include <unistd.h>
#endif

#include "../../macros/legacy/LegacySignalRecoveryFormat.h"

namespace {
namespace fs = std::filesystem;
using namespace REST_LegacySignalRecovery;

struct IntermediateEvent {
    Int_t runOrigin = 0, subRunOrigin = 0, eventID = 0, subEventID = 0, timeNanoSec = 0;
    Long64_t timeSec = 0;
    Bool_t ok = false;
    TString* subEventTag = nullptr;
    std::vector<Int_t>* signalIDs = nullptr;
    std::vector<Int_t>* pointCounts = nullptr;
    std::vector<Float_t>* times = nullptr;
    std::vector<Float_t>* charges = nullptr;
    std::vector<std::string>* names = nullptr;
    std::vector<std::string>* types = nullptr;

    bool Bind(TTree& tree) {
        return tree.SetBranchAddress("runOrigin", &runOrigin) >= TTree::kMatch &&
               tree.SetBranchAddress("subRunOrigin", &subRunOrigin) >= TTree::kMatch &&
               tree.SetBranchAddress("eventID", &eventID) >= TTree::kMatch &&
               tree.SetBranchAddress("subEventID", &subEventID) >= TTree::kMatch &&
               tree.SetBranchAddress("timeSec", &timeSec) >= TTree::kMatch &&
               tree.SetBranchAddress("timeNanoSec", &timeNanoSec) >= TTree::kMatch &&
               tree.SetBranchAddress("ok", &ok) >= TTree::kMatch &&
               tree.SetBranchAddress("subEventTag", &subEventTag) >= TTree::kMatch &&
               tree.SetBranchAddress("signalIDs", &signalIDs) >= TTree::kMatch &&
               tree.SetBranchAddress("pointCounts", &pointCounts) >= TTree::kMatch &&
               tree.SetBranchAddress("times", &times) >= TTree::kMatch &&
               tree.SetBranchAddress("charges", &charges) >= TTree::kMatch &&
               tree.SetBranchAddress("names", &names) >= TTree::kMatch &&
               tree.SetBranchAddress("types", &types) >= TTree::kMatch;
    }

    bool Validate(Long64_t entry) const {
        if (subEventTag == nullptr || signalIDs == nullptr || pointCounts == nullptr || times == nullptr ||
            charges == nullptr || names == nullptr || types == nullptr ||
            signalIDs->size() != pointCounts->size() || signalIDs->size() != names->size() ||
            signalIDs->size() != types->size() || times->size() != charges->size()) {
            std::cerr << "ERROR: malformed recovery intermediate at entry " << entry << ".\n";
            return false;
        }
        std::size_t points = 0;
        for (const Int_t count : *pointCounts) {
            if (count < 0 || static_cast<std::size_t>(count) > times->size() - points) return false;
            points += count;
        }
        return points == times->size();
    }
};

std::set<std::string> BranchInventory(TTree& tree) {
    std::set<std::string> result;
    const auto visit = [&](auto&& self, TBranch* branch, const std::string& parent) -> void {
        const std::string path = parent.empty() ? branch->GetName() : parent + "/" + branch->GetName();
        result.insert(path);
        auto* children = branch->GetListOfBranches();
        for (int index = 0; index <= children->GetLast(); ++index)
            self(self, static_cast<TBranch*>(children->At(index)), path);
    };
    auto* branches = tree.GetListOfBranches();
    for (int index = 0; index <= branches->GetLast(); ++index) {
        auto* branch = static_cast<TBranch*>(branches->At(index));
        if (std::string(branch->GetName()) != kSignalBranch) visit(visit, branch, "");
    }
    return result;
}

std::multiset<std::string> OpaqueKeyInventory(TFile& file) {
    std::multiset<std::string> result;
    TIter next(file.GetListOfKeys());
    while (auto* key = dynamic_cast<TKey*>(next())) {
        const std::string name = key->GetName();
        if (name != kEventTree && name != "REST_LegacySignalRecovery")
            result.insert(name + ":" + key->GetClassName() + ":" + std::to_string(key->GetCycle()));
    }
    return result;
}

bool FillModernEvent(const IntermediateEvent& input, TRestDetectorSignalEvent& output) {
    output.Initialize();
    output.SetRunOrigin(input.runOrigin);
    output.SetSubRunOrigin(input.subRunOrigin);
    output.SetID(input.eventID);
    output.SetSubID(input.subEventID);
    output.SetTime(input.timeSec, input.timeNanoSec);
    output.SetOK(input.ok);
    output.SetSubEventTag(*input.subEventTag);
    std::size_t point = 0;
    for (std::size_t index = 0; index < input.signalIDs->size(); ++index) {
        TRestDetectorSignal signal;
        signal.SetSignalID(input.signalIDs->at(index));
        signal.SetSignalName(input.names->at(index));
        signal.SetSignalType(input.types->at(index));
        for (Int_t offset = 0; offset < input.pointCounts->at(index); ++offset, ++point)
            signal.NewPoint(input.times->at(point), input.charges->at(point));
        output.AddSignal(signal);
    }
    return point == input.times->size();
}

bool SameModernEvent(const IntermediateEvent& input, TRestDetectorSignalEvent& output) {
    const auto same = [](Float_t oldValue, Double_t newValue) {
        return static_cast<Double_t>(oldValue) == newValue || (std::isnan(oldValue) && std::isnan(newValue));
    };
    if (output.GetRunOrigin() != input.runOrigin || output.GetSubRunOrigin() != input.subRunOrigin ||
        output.GetID() != input.eventID || output.GetSubID() != input.subEventID ||
        output.GetTimeStamp().GetSec() != input.timeSec ||
        output.GetTimeStamp().GetNanoSec() != input.timeNanoSec || output.isOk() != input.ok ||
        output.GetSubEventTag() != *input.subEventTag ||
        output.GetNumberOfSignals() != static_cast<Int_t>(input.signalIDs->size()))
        return false;
    std::size_t point = 0;
    for (std::size_t index = 0; index < input.signalIDs->size(); ++index) {
        auto* signal = output.GetSignal(index);
        if (signal->GetSignalID() != input.signalIDs->at(index) ||
            signal->GetSignalName() != input.names->at(index) ||
            signal->GetSignalType() != input.types->at(index) ||
            signal->GetNumberOfPoints() != input.pointCounts->at(index))
            return false;
        for (Int_t offset = 0; offset < input.pointCounts->at(index); ++offset, ++point) {
            if (!same(input.times->at(point), signal->GetTime(offset)) ||
                !same(input.charges->at(point), signal->GetData(offset)))
                return false;
        }
    }
    return point == input.times->size();
}

bool ValidateOutput(const fs::path& outputName, const fs::path& intermediateName,
                    const std::set<std::string>& branches, const std::multiset<std::string>& keys) {
    TFile output(outputName.c_str(), "READ");
    TFile intermediate(intermediateName.c_str(), "READ");
    auto* tree = dynamic_cast<TTree*>(output.Get(kEventTree));
    auto* data = dynamic_cast<TTree*>(intermediate.Get(kDataTree));
    auto* signalBranch = tree == nullptr ? nullptr : tree->GetBranch(kSignalBranch);
    if (output.IsZombie() || intermediate.IsZombie() || tree == nullptr || data == nullptr ||
        signalBranch == nullptr || tree->GetEntries() != data->GetEntries() ||
        DetectSignalVersion(signalBranch) < 4 || BranchInventory(*tree) != branches ||
        OpaqueKeyInventory(output) != keys) {
        std::cerr << "ERROR: recovered file structure failed validation.\n";
        return false;
    }

    IntermediateEvent input;
    auto* event = new TRestDetectorSignalEvent;
    if (!input.Bind(*data) || tree->SetBranchAddress(kSignalBranch, &event) < TTree::kMatch) return false;
    for (Long64_t entry = 0; entry < tree->GetEntries(); ++entry) {
        if (data->GetEntry(entry) <= 0 || signalBranch->GetEntry(entry) <= 0 || !input.Validate(entry) ||
            !SameModernEvent(input, *event)) {
            std::cerr << "ERROR: recovered signal values differ at entry " << entry << ".\n";
            return false;
        }
    }
    return true;
}

bool Rebuild(const fs::path& sourceName, const fs::path& intermediateName, const fs::path& candidateName) {
    TFile source(sourceName.c_str(), "READ");
    TFile intermediate(intermediateName.c_str(), "READ");
    auto* sourceTree = dynamic_cast<TTree*>(source.Get(kEventTree));
    auto* data = dynamic_cast<TTree*>(intermediate.Get(kDataTree));
    auto* sourceSignal = sourceTree == nullptr ? nullptr : sourceTree->GetBranch(kSignalBranch);
    Long64_t sourceSize = -1, sourceEntries = -1, sourceVersion = -1, recoveredSignals = -1,
             recoveredPoints = -1;
    std::string sourceUuid;
    if (source.IsZombie() || intermediate.IsZombie() || sourceTree == nullptr || data == nullptr ||
        sourceSignal == nullptr || !ReadText(intermediate, "sourceUuid", sourceUuid) ||
        !ReadNumber(intermediate, "sourceSize", sourceSize) ||
        !ReadNumber(intermediate, "sourceEntries", sourceEntries) ||
        !ReadNumber(intermediate, "sourceSignalVersion", sourceVersion) ||
        !ReadNumber(intermediate, "recoveredSignals", recoveredSignals) ||
        !ReadNumber(intermediate, "recoveredPoints", recoveredPoints) ||
        sourceUuid != source.GetUUID().AsString() || sourceSize != source.GetSize() ||
        sourceEntries != sourceTree->GetEntries() || sourceVersion != DetectSignalVersion(sourceSignal) ||
        sourceVersion < 1 || sourceVersion > 3 || data->GetEntries() != sourceEntries) {
        std::cerr << "ERROR: recovery intermediate does not match the source file.\n";
        return false;
    }
    const auto branches = BranchInventory(*sourceTree);
    const auto keys = OpaqueKeyInventory(source);
    source.Close();

    std::error_code error;
    if (!fs::copy_file(sourceName, candidateName, fs::copy_options::none, error)) {
        std::cerr << "ERROR: cannot copy source to recovery candidate: " << error.message() << ".\n";
        return false;
    }
    fs::permissions(candidateName, fs::perms::owner_write, fs::perm_options::add, error);
    if (error) {
        std::cerr << "ERROR: cannot make recovery candidate writable: " << error.message() << ".\n";
        return false;
    }

    auto candidate = TRestRootFileHandle::Open(candidateName.string(), TRestRootFileMode::Update);
    if (!candidate) {
        std::cerr << "ERROR: cannot safely update recovery candidate: " << candidate.Error() << ".\n";
        return false;
    }
    auto* oldTree = dynamic_cast<TTree*>(candidate->Get(kEventTree));
    auto* oldSignal = oldTree == nullptr ? nullptr : oldTree->GetBranch(kSignalBranch);
    if (oldTree == nullptr || oldSignal == nullptr || oldTree->GetEntries() != sourceEntries ||
        candidate->GetUUID().AsString() != sourceUuid)
        return false;

    // Clone every unaffected branch as raw baskets. Disabling the top-level
    // signal branch makes ROOT omit its complete split subtree.
    oldSignal->SetStatus(false);
    oldTree->SetName("__RESTLegacySourceEventTree");
    candidate->cd();
    std::cerr << "Recovery: fast-cloning unaffected EventTree branches...\n";
    auto* newTree = oldTree->CloneTree(-1, "fast");
    if (newTree == nullptr || newTree->GetEntries() != sourceEntries) return false;
    candidate->Delete("EventTree;*");
    std::cerr << "Recovery: rebuilding detector signal branch...\n";
    newTree->SetName(kEventTree);

    IntermediateEvent input;
    auto* event = new TRestDetectorSignalEvent;
    auto* newSignal = newTree->Branch(kSignalBranch, &event);
    if (!input.Bind(*data) || newSignal == nullptr) return false;
    Long64_t signalCount = 0, pointCount = 0;
    for (Long64_t entry = 0; entry < sourceEntries; ++entry) {
        if (data->GetEntry(entry) <= 0 || !input.Validate(entry) || !FillModernEvent(input, *event) ||
            newSignal->Fill() < 0)
            return false;
        signalCount += input.signalIDs->size();
        pointCount += input.times->size();
    }
    if (signalCount != recoveredSignals || pointCount != recoveredPoints) return false;

    candidate->cd();
    const std::string provenance =
        "source=" + sourceUuid + ";signalVersion=" + std::to_string(sourceVersion) +
        ";entries=" + std::to_string(sourceEntries) + ";signals=" + std::to_string(signalCount) +
        ";points=" + std::to_string(pointCount);
    std::cerr << "Recovery: writing rebuilt EventTree...\n";
    if (newTree->Write(kEventTree, TObject::kOverwrite) <= 0 ||
        !WriteText(*candidate, "REST_LegacySignalRecovery", provenance) || !candidate.Close())
        return false;
    return ValidateOutput(candidateName, intermediateName, branches, keys);
}

fs::path DefaultOutput(fs::path input) {
    if (input.extension() == ".root")
        input.replace_filename(input.stem().string() + "_Fixed.root");
    else
        input += "_Fixed.root";
    return input;
}

#ifndef _WIN32
int RunExtractor(const fs::path& input, const fs::path& intermediate, const fs::path& directory) {
    fs::path macro = fs::path(REST_PATH) / "macros/legacy/recoverLegacySignalData.C";
#ifdef REST_TESTING_ENABLED
    if (const char* overridePath = std::getenv("REST_LEGACY_RECOVERY_MACRO")) macro = overridePath;
#endif
    const pid_t child = fork();
    if (child == 0) {
        setenv("REST_LEGACY_RECOVERY_INPUT", input.c_str(), 1);
        setenv("REST_LEGACY_RECOVERY_INTERMEDIATE", intermediate.c_str(), 1);
        setenv("REST_LEGACY_RECOVERY_DIR", directory.c_str(), 1);
        chdir(directory.c_str());
        const std::string build = "gSystem->SetBuildDir(gSystem->Getenv(\"REST_LEGACY_RECOVERY_DIR\"),kTRUE)";
        const std::string compiledMacro = macro.string() + "+";
        execlp("root", "root", "-l", "-b", "-n", "-x", "-q", "-e", build.c_str(), compiledMacro.c_str(),
               static_cast<char*>(nullptr));
        _exit(127);
    }
    if (child < 0) return -1;
    int status = 0;
    while (waitpid(child, &status, 0) < 0 && errno == EINTR) {
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : 128;
}
#endif

}  // namespace

int RunLegacySignalRecoveryCommand(int argc, char* argv[]) {
    int flag = -1;
    for (int index = 1; index < argc; ++index)
        if (std::string(argv[index]) == "--recover-legacy-signals") flag = index;
    if (flag < 0) return -1;

#ifdef _WIN32
    std::cerr << "ERROR: legacy recovery is currently supported on POSIX systems only.\n";
    return 2;
#else
    fs::path input, output;
    for (int index = flag + 1; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--output" && index + 1 < argc)
            output = argv[++index];
        else if (argument == "--help" || argument == "-h") {
            std::cout << "Usage: restRoot --recover-legacy-signals INPUT [--output OUTPUT]\n";
            return 0;
        } else if (!argument.empty() && argument.front() == '-') {
            std::cerr << "ERROR: unknown recovery option '" << argument << "'.\n";
            return 2;
        } else if (input.empty())
            input = argument;
        else {
            std::cerr << "ERROR: recovery accepts exactly one input file.\n";
            return 2;
        }
    }
    if (input.empty() || TRestTools::IsRemoteRootPath(input.string())) {
        std::cerr << "ERROR: recovery requires one local input ROOT file.\n";
        return 2;
    }
    std::error_code error;
    input = fs::canonical(input, error);
    if (error) {
        std::cerr << "ERROR: cannot resolve input file: " << error.message() << ".\n";
        return 2;
    }
    if (!output.empty() && TRestTools::IsRemoteRootPath(output.string())) {
        std::cerr << "ERROR: recovery output must be a local path.\n";
        return 2;
    }
    output = output.empty() ? DefaultOutput(input) : fs::absolute(output, error);
    if (error) {
        std::cerr << "ERROR: cannot resolve output path: " << error.message() << ".\n";
        return 2;
    }
    const bool outputExists = fs::exists(output, error);
    if (error || TRestTools::IsRemoteRootPath(output.string()) || outputExists || output == input) {
        std::cerr << "ERROR: output must be a new local file distinct from the input.\n";
        return 2;
    }

    const fs::path work =
        output.parent_path() / (".rest-legacy-signals-" + std::to_string(static_cast<long long>(getpid())));
    const fs::path intermediate = work / "signals.root";
    const fs::path candidate = work / "candidate.root";
    if (!fs::create_directory(work, error)) {
        std::cerr << "ERROR: cannot create recovery work directory"
                  << (error ? ": " + error.message() : " because it already exists") << ".\n";
        return 3;
    }

    const int extraction = RunExtractor(input, intermediate, work);
    if (extraction == 0) TRestTools::LoadRESTLibrary(true);
    const bool rebuilt = extraction == 0 && Rebuild(input, intermediate, candidate);
    // The candidate shares the output filesystem. A hard link publishes it
    // atomically and, unlike rename on POSIX, cannot replace a racing writer.
    if (rebuilt) fs::create_hard_link(candidate, output, error);
    const bool installed = rebuilt && !error;
    fs::remove_all(work, error);
    if (!installed) {
        std::cerr << "ERROR: recovery failed; the original file was not modified.\n";
        return extraction == 0 ? 4 : extraction;
    }
    std::cout << "Recovered file written to " << output << "\n";
    return 0;
#endif
}
