// Stage 2 of 2 — rebuild a legacy REST file with current TRestDetectorSignalEvent.
//
// Takes the original legacy file plus the intermediate file produced by stage 1
// (macros/legacy/recoverLegacySignalData.C, run with plain root) and writes a new
// file in which:
//   - the TRestDetectorSignalEventBranch is rebuilt with the current
//     vector<Double_t>-based classes,
//   - all other EventTree branches, the AnalysisTree, and every additional
//     top-level TTree (highest key cycle) are copied unchanged,
//   - all readable metadata keys (TRestRun, readout, processes, ...) are copied,
//   - the event-class StreamerInfos ARE stored (they are missing from legacy
//     restManager output, which is what made these files unreadable in the
//     first place — see rest-for-physics/detectorlib#125).
//
// Usage (restRoot):
//   REST_RebuildLegacySignalFile("R00236_...V2.4.0.root")
//       writes R00236_...V2.4.0_Fixed.root
//   REST_RebuildLegacySignalFile("input.root", "", "", true)
//       overwrites input.root in place (the original is kept as input.root.bak)
//
// The macro never overwrites an existing output or backup. In-place recovery
// refuses to start while input.root.bak or its temporary fixed file exists.
// Event branches without a loaded dictionary and metadata objects unreadable
// with the current libraries cannot be copied. They are reported explicitly;
// if any are encountered, in-place recovery is refused and the fixed file is
// left at <input>_FixedTmp.root for inspection.
// The intermediate is accepted only when its ROOT UUID and persisted source
// identity match the requested source. This prevents accidental mix-ups but is
// not a cryptographic signature against a maliciously edited intermediate.
// Before an in-place replacement, the completed candidate is closed, reopened,
// and read back in full; any validation failure leaves the original and any
// pre-existing backup untouched.
//
// Arguments:
//   originalFile   - the legacy REST file
//   signalDataFile - intermediate from stage 1; default: <original>_LegacySignalData.root
//   outputFile     - default: <original>_Fixed.root, or <original>_FixedTmp.root
//                    when overwrite=true; an explicit path is always honored
//   overwrite      - replace originalFile in place, keeping a .bak copy

#include <TBranchElement.h>
#include <TFile.h>
#include <TKey.h>
#include <TNamed.h>
#include <TRestDetectorSignalEvent.h>
#include <TString.h>
#include <TTree.h>

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "LegacyRecoveryCandidateValidation.h"
#include "LegacyRecoveryDataUtils.h"
#include "LegacyRecoveryFileUtils.h"
#include "LegacyRecoveryProvenance.h"

namespace REST_Rebuild_Internal {

int gLegacySignalRebuildStatus = 1;
bool gRequireCompleteRecovery = false;

void SetBranchStatusRecursive(TBranch* branch, Bool_t status) {
    if (branch == nullptr) return;
    branch->SetStatus(status);
    auto subs = branch->GetListOfBranches();
    for (int i = 0; i <= subs->GetLast(); i++) SetBranchStatusRecursive((TBranch*)subs->At(i), status);
}

template <typename Address>
bool BindRequiredBranch(TTree* tree, const char* name, Address address, std::ostream& errors) {
    if (tree->GetBranch(name) == nullptr) {
        errors << "ERROR: required intermediate branch '" << name << "' is missing.\n";
        return false;
    }
    const int status = tree->SetBranchAddress(name, address);
    if (status >= TTree::kMatch) return true;

    errors << "ERROR: cannot bind required intermediate branch '" << name << "' (status " << status << ").\n";
    return false;
}

struct IntermediateData {
    Int_t runOrigin = 0;
    Int_t subRunOrigin = 0;
    Int_t eventID = 0;
    Int_t subEventID = 0;
    Int_t timeSec = 0;
    Int_t timeNanoSec = 0;
    Bool_t ok = false;
    TString* subEventTag = nullptr;
    std::vector<Int_t>* signalID = nullptr;
    std::vector<Int_t>* nPoints = nullptr;
    std::vector<Float_t>* times = nullptr;
    std::vector<Float_t>* charges = nullptr;

    bool Bind(TTree* tree, std::ostream& errors) {
        return BindRequiredBranch(tree, "runOrigin", &runOrigin, errors) &&
               BindRequiredBranch(tree, "subRunOrigin", &subRunOrigin, errors) &&
               BindRequiredBranch(tree, "eventID", &eventID, errors) &&
               BindRequiredBranch(tree, "subEventID", &subEventID, errors) &&
               BindRequiredBranch(tree, "timeSec", &timeSec, errors) &&
               BindRequiredBranch(tree, "timeNanoSec", &timeNanoSec, errors) &&
               BindRequiredBranch(tree, "ok", &ok, errors) &&
               BindRequiredBranch(tree, "subEventTag", &subEventTag, errors) &&
               BindRequiredBranch(tree, "signalID", &signalID, errors) &&
               BindRequiredBranch(tree, "nPoints", &nPoints, errors) &&
               BindRequiredBranch(tree, "times", &times, errors) &&
               BindRequiredBranch(tree, "charges", &charges, errors);
    }

    bool Validate(Long64_t entry, std::uint64_t& pointCount, std::ostream& errors) const {
        if (subEventTag == nullptr) {
            errors << "ERROR: intermediate entry " << entry << " has a null subEventTag pointer.\n";
            return false;
        }
        return REST_LegacyRecovery::ValidateFlattenedSignalData(
            signalID, nPoints, times, charges, pointCount, errors,
            "intermediate entry " + std::to_string(entry));
    }
};

bool ScanIntermediateTree(TTree* tree, IntermediateData& values, REST_LegacyRecovery::RecoveryCounts& counts,
                          std::ostream& errors) {
    counts = {};
    counts.entries = static_cast<std::uint64_t>(tree->GetEntries());
    for (Long64_t entry = 0; entry < tree->GetEntries(); ++entry) {
        if (tree->GetEntry(entry) <= 0) {
            errors << "ERROR: cannot read intermediate entry " << entry << ".\n";
            return false;
        }
        std::uint64_t points = 0;
        if (!values.Validate(entry, points, errors) ||
            !REST_LegacyRecovery::CheckedAdd(counts.signals, values.signalID->size(), errors,
                                             "intermediate signals") ||
            !REST_LegacyRecovery::CheckedAdd(counts.points, points, errors, "intermediate points")) {
            return false;
        }
    }
    return true;
}

}  // namespace REST_Rebuild_Internal

void REST_RebuildLegacySignalFile(const char* originalFile, const char* signalDataFile = "",
                                  const char* outputFile = "", bool overwrite = false) {
    using namespace REST_Rebuild_Internal;
    gLegacySignalRebuildStatus = 1;

    std::string dataName = signalDataFile;
    if (dataName.empty())
        dataName = REST_LegacyRecovery::BuildSiblingRootPath(originalFile, "_LegacySignalData").string();

    std::string outName = outputFile;
    if (outName.empty())
        outName = REST_LegacyRecovery::BuildSiblingRootPath(originalFile, overwrite ? "_FixedTmp" : "_Fixed")
                      .string();

    if (!REST_LegacyRecovery::ValidateNewOutputPath(originalFile, outName, "fixed output", std::cout)) {
        return;
    }
    if (!REST_LegacyRecovery::ValidateNewOutputPath(dataName, outName, "fixed output", std::cout)) {
        return;
    }

    const std::string backupName = std::string(originalFile) + ".bak";
    if (overwrite && !REST_LegacyRecovery::ValidateUnusedPath(backupName, "backup path", std::cout)) {
        return;
    }

    // --- open inputs ---
    std::unique_ptr<TFile> original(TFile::Open(originalFile));
    if (original == nullptr || original->IsZombie()) {
        std::cout << "ERROR: cannot open original file: " << originalFile << std::endl;
        return;
    }
    std::unique_ptr<TFile> data(TFile::Open(dataName.c_str()));
    if (data == nullptr || data->IsZombie()) {
        std::cout << "ERROR: cannot open signal data file: " << dataName << std::endl;
        std::cout << "Run recoverLegacySignalData.C first with plain root (NOT restRoot)." << std::endl;
        std::cout << "  original:     " << originalFile << std::endl;
        std::cout << "  intermediate: " << dataName << std::endl;
        return;
    }

    auto oldTree = dynamic_cast<TTree*>(original->Get("EventTree"));
    auto dataTree = dynamic_cast<TTree*>(data->Get("LegacySignalData"));
    if (oldTree == nullptr || dataTree == nullptr) {
        std::cout << "ERROR: EventTree or LegacySignalData tree not found." << std::endl;
        return;
    }

    TBranch* signalBranch = oldTree->GetBranch("TRestDetectorSignalEventBranch");
    if (signalBranch == nullptr) {
        std::cout << "ERROR: no TRestDetectorSignalEventBranch in " << originalFile << std::endl;
        return;
    }
    const auto onDiskVersions = REST_LegacyRecovery::DetectSignalSchemaVersions(signalBranch);
    if (!REST_LegacyRecovery::ValidateSupportedLegacySchema(onDiskVersions, std::cout, "source file")) {
        return;
    }
    const Int_t onDiskVersion = onDiskVersions.time;

    const Long64_t nEntries = oldTree->GetEntries();
    if (nEntries < 0 || original->GetSize() < 0) {
        std::cout << "ERROR: source file reports an invalid entry count or file size." << std::endl;
        return;
    }
    if (dataTree->GetEntries() != nEntries) {
        std::cout << "ERROR: entry mismatch — EventTree has " << nEntries << " entries, signal data has "
                  << dataTree->GetEntries() << ". Wrong intermediate file?" << std::endl;
        return;
    }

    std::string sourcePath;
    std::string pathError;
    if (!REST_LegacyRecovery::ResolvePathIdentity(originalFile, sourcePath, pathError)) {
        std::cout << "ERROR: " << pathError << std::endl;
        return;
    }
    std::string intermediatePath;
    if (!REST_LegacyRecovery::ResolvePathIdentity(dataName, intermediatePath, pathError)) {
        std::cout << "ERROR: " << pathError << std::endl;
        return;
    }

    REST_LegacyRecovery::SourceIdentity sourceIdentity;
    sourceIdentity.uuid = original->GetUUID().AsString();
    sourceIdentity.normalizedPath = sourcePath;
    sourceIdentity.fileSize = static_cast<std::uint64_t>(original->GetSize());
    sourceIdentity.entries = static_cast<std::uint64_t>(nEntries);
    sourceIdentity.signalVersion = onDiskVersion;

    REST_LegacyRecovery::RecoveryProvenance intermediateProvenance;
    if (!REST_LegacyRecovery::ReadRecoveryProvenance(*data, intermediateProvenance, std::cout) ||
        !REST_LegacyRecovery::ValidateIntermediateSource(intermediateProvenance, sourceIdentity,
                                                         data->GetUUID().AsString(), std::cout)) {
        std::cout << "ERROR: refusing to combine an unauthenticated intermediate with the source."
                  << std::endl;
        return;
    }

    IntermediateData intermediateValues;
    if (!intermediateValues.Bind(dataTree, std::cout)) return;
    REST_LegacyRecovery::RecoveryCounts scannedCounts;
    if (!ScanIntermediateTree(dataTree, intermediateValues, scannedCounts, std::cout) ||
        !REST_LegacyRecovery::ValidateRecoveredCounts(intermediateProvenance, scannedCounts, std::cout,
                                                      "intermediate")) {
        return;
    }

    // --- output file (single write session: StreamerInfos are preserved) ---
    std::unique_ptr<TFile> out(TFile::Open(outName.c_str(), "CREATE"));
    if (out == nullptr || out->IsZombie()) {
        std::cout << "ERROR: cannot create output file: " << outName << std::endl;
        return;
    }

    // --- copy metadata keys (highest cycle only, skip trees) ---
    std::set<std::string> seen;
    std::vector<std::string> skipped;
    std::vector<std::string> copiedMetadataKeys;
    TIter nextKey(original->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)nextKey())) {
        const std::string keyName = key->GetName();
        if (seen.count(keyName)) continue;  // keys are ordered newest cycle first
        seen.insert(keyName);

        TClass* cl = TClass::GetClass(key->GetClassName());
        if (cl != nullptr && cl->InheritsFrom("TTree")) continue;  // trees handled below

        // A non-null TClass is not enough: for a class whose compiled dictionary
        // is gone (renamed/removed, e.g. obsolete legacy processes), ROOT builds
        // an *emulated* TClass from the file's StreamerInfo. Reading such an
        // object can abort inside ROOT when an abstract compiled base in its
        // inheritance chain cannot be instantiated (TClass::New failure), so the
        // "obj == nullptr" guard below never gets the chance to skip it. Treat a
        // missing compiled dictionary the same as a missing class.
        if (cl == nullptr || !cl->HasDictionary()) {
            skipped.push_back(keyName + " (" + key->GetClassName() + ")");
            continue;
        }

        TObject* obj = key->ReadObj();
        if (obj == nullptr) {
            skipped.push_back(keyName + " (" + key->GetClassName() + ")");
            continue;
        }
        out->cd();
        if (obj->Write(keyName.c_str(), TObject::kOverwrite) <= 0) {
            std::cout << "ERROR: failed to write metadata key '" << keyName << "'." << std::endl;
            REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
            return;
        }
        copiedMetadataKeys.push_back(keyName);
    }

    // --- rebuild the EventTree ---
    // A brand-new tree is created with branches built from the CURRENT classes.
    // Fast-cloning the old tree is not possible: without StreamerInfos in the
    // file, ROOT cannot map old split-branch structures onto the current
    // classes (e.g. the track branch aborts in InitializeOffsets). Reading the
    // other event branches with the current classes works, so they are copied
    // by deserialize-and-fill.
    SetBranchStatusRecursive(signalBranch, 0);

    std::vector<std::string> otherBranchNames;
    std::vector<TBranch*> otherBranches;
    std::vector<std::string> skippedEventBranches;
    TIter nextBranch(oldTree->GetListOfBranches());
    TBranch* br;
    while ((br = (TBranch*)nextBranch())) {
        const std::string name = br->GetName();
        if (name == "TRestDetectorSignalEventBranch") continue;
        const size_t suffix = name.rfind("Branch");
        const std::string className = (suffix != std::string::npos) ? name.substr(0, suffix) : name;
        const TClass* eventClass = TClass::GetClass(className.c_str());
        if (eventClass == nullptr || !eventClass->HasDictionary()) {
            std::cout << "WARNING: no dictionary for event class '" << className << "'; branch '" << name
                      << "' will NOT be copied!" << std::endl;
            skippedEventBranches.push_back(name + " (" + className + ")");
            SetBranchStatusRecursive(br, 0);
            continue;
        }
        otherBranchNames.push_back(name);
        otherBranches.push_back(br);
    }

    out->cd();
    auto newTree = new TTree("EventTree", "EventTree");

    // stable storage for the object pointers shared between both trees
    std::vector<TRestEvent*> otherEvents(otherBranchNames.size(), nullptr);
    for (size_t b = 0; b < otherBranchNames.size(); b++) {
        const std::string& name = otherBranchNames[b];
        const std::string className = name.substr(0, name.rfind("Branch"));
        TClass* cl = TClass::GetClass(className.c_str());
        otherEvents[b] = (TRestEvent*)cl->New();
        if (otherEvents[b] == nullptr) {
            std::cout << "ERROR: cannot construct event class '" << className << "'." << std::endl;
            REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
            return;
        }
        const int bindStatus = oldTree->SetBranchAddress(name.c_str(), &otherEvents[b]);
        if (bindStatus < TTree::kMatch) {
            std::cout << "ERROR: cannot bind source event branch '" << name << "' (status " << bindStatus
                      << ")." << std::endl;
            REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
            return;
        }
        if (newTree->Branch(name.c_str(), className.c_str(), &otherEvents[b]) == nullptr) {
            std::cout << "ERROR: cannot create rebuilt event branch '" << name << "'." << std::endl;
            REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
            return;
        }
    }

    auto event = new TRestDetectorSignalEvent();
    if (newTree->Branch("TRestDetectorSignalEventBranch", &event) == nullptr) {
        std::cout << "ERROR: cannot create rebuilt detector signal branch." << std::endl;
        REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
        return;
    }

    REST_LegacyRecovery::RecoveryCounts writtenCounts;
    writtenCounts.entries = scannedCounts.entries;
    for (Long64_t i = 0; i < nEntries; i++) {
        for (size_t branchIndex = 0; branchIndex < otherBranches.size(); ++branchIndex) {
            if (otherBranches[branchIndex]->GetEntry(i) <= 0) {
                std::cout << "ERROR: cannot read source event branch '" << otherBranchNames[branchIndex]
                          << "' at entry " << i << "." << std::endl;
                REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
                return;
            }
        }
        if (dataTree->GetEntry(i) <= 0) {
            std::cout << "ERROR: cannot reread intermediate entry " << i << "." << std::endl;
            REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
            return;
        }
        std::uint64_t entryPoints = 0;
        if (!intermediateValues.Validate(i, entryPoints, std::cout)) {
            REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
            return;
        }

        event->Initialize();
        event->SetRunOrigin(intermediateValues.runOrigin);
        event->SetSubRunOrigin(intermediateValues.subRunOrigin);
        event->SetID(intermediateValues.eventID);
        event->SetSubID(intermediateValues.subEventID);
        event->SetSubEventTag(*intermediateValues.subEventTag);
        event->SetTime((Double_t)intermediateValues.timeSec, (Double_t)intermediateValues.timeNanoSec);
        event->SetOK(intermediateValues.ok);

        size_t offset = 0;
        for (size_t s = 0; s < intermediateValues.signalID->size(); s++) {
            TRestDetectorSignal signal;
            signal.SetSignalID(intermediateValues.signalID->at(s));
            const size_t n = static_cast<size_t>(intermediateValues.nPoints->at(s));
            if (offset > intermediateValues.times->size() || n > intermediateValues.times->size() - offset) {
                std::cout << "ERROR: intermediate entry " << i
                          << " exceeds flattened array bounds while rebuilding signal " << s << "."
                          << std::endl;
                REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
                return;
            }
            for (size_t p = 0; p < n; p++) {
                signal.NewPoint(intermediateValues.times->at(offset + p),
                                intermediateValues.charges->at(offset + p));
            }
            offset += n;
            event->AddSignal(signal);
        }
        if (offset != entryPoints ||
            !REST_LegacyRecovery::CheckedAdd(writtenCounts.signals, intermediateValues.signalID->size(),
                                             std::cout, "rebuilt signals") ||
            !REST_LegacyRecovery::CheckedAdd(writtenCounts.points, offset, std::cout, "rebuilt points")) {
            std::cout << "ERROR: rebuilt entry " << i << " failed count validation." << std::endl;
            REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
            return;
        }

        if (newTree->Fill() < 0) {
            std::cout << "ERROR: failed to write rebuilt EventTree entry " << i << "." << std::endl;
            REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
            return;
        }
    }
    if (writtenCounts.entries != scannedCounts.entries || writtenCounts.signals != scannedCounts.signals ||
        writtenCounts.points != scannedCounts.points) {
        std::cout << "ERROR: rebuilt counts changed while writing the candidate." << std::endl;
        REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
        return;
    }
    if (newTree->Write("", TObject::kOverwrite) <= 0) {
        std::cout << "ERROR: failed to write the rebuilt EventTree." << std::endl;
        REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
        return;
    }

    // --- copy the AnalysisTree unchanged ---
    auto anaTree = dynamic_cast<TTree*>(original->Get("AnalysisTree"));
    Long64_t analysisEntries = -1;
    std::vector<std::string> analysisBranchNames;
    if (anaTree != nullptr) {
        analysisEntries = anaTree->GetEntries();
        TIter nextAnalysisBranch(anaTree->GetListOfBranches());
        TBranch* analysisBranch;
        while ((analysisBranch = static_cast<TBranch*>(nextAnalysisBranch()))) {
            analysisBranchNames.emplace_back(analysisBranch->GetName());
        }
        out->cd();
        TTree* anaClone = anaTree->CloneTree(-1, "fast");
        if (anaClone == nullptr || anaClone->GetEntries() != analysisEntries ||
            anaClone->Write("", TObject::kOverwrite) <= 0) {
            std::cout << "ERROR: failed to clone and write the AnalysisTree." << std::endl;
            REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
            return;
        }
    } else {
        std::cout << "WARNING: no AnalysisTree found; skipping." << std::endl;
    }

    // --- copy every other top-level tree, using the highest key cycle ---
    std::vector<REST_LegacyRecovery::AdditionalTreeExpectation> additionalTrees;
    if (!REST_LegacyRecovery::CopyAdditionalTopLevelTrees(*original, *out, additionalTrees, std::cout)) {
        REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
        return;
    }

    REST_LegacyRecovery::RecoveryProvenance resultProvenance = intermediateProvenance;
    resultProvenance.kind = REST_LegacyRecovery::kResultProvenanceKind;
    resultProvenance.recovered = scannedCounts;
    resultProvenance.intermediatePath = intermediatePath;
    resultProvenance.resultUuid = out->GetUUID().AsString();
    if (!REST_LegacyRecovery::WriteRecoveryProvenance(*out, resultProvenance, std::cout)) {
        std::cout << "ERROR: failed to persist rebuilt-file recovery provenance." << std::endl;
        REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout);
        return;
    }
    if (!REST_LegacyRecovery::CheckAndCloseOutputFile(*out, std::cout)) {
        std::cout << "ERROR: rebuilt candidate is incomplete; the original was not touched." << std::endl;
        return;
    }

    original->Close();
    data->Close();

    if (!skipped.empty()) {
        std::cout << "WARNING: " << skipped.size()
                  << " metadata key(s) could not be read with the current libraries and were NOT copied:"
                  << std::endl;
        for (const auto& s : skipped) std::cout << "   - " << s << std::endl;
    }
    if (!skippedEventBranches.empty()) {
        std::cout << "WARNING: " << skippedEventBranches.size()
                  << " event branch(es) had no loaded dictionary and were NOT copied:" << std::endl;
        for (const auto& s : skippedEventBranches) std::cout << "   - " << s << std::endl;
    }
    if (gRequireCompleteRecovery && (!skipped.empty() || !skippedEventBranches.empty())) {
        std::cout
            << "ERROR: strict recovery refuses a rebuilt file that omitted metadata or event branches.\n"
            << "The original is unchanged. Inspect the candidate file at: " << outName << std::endl;
        return;
    }

    // --- overwrite handling ---
    std::string finalName = outName;
    REST_LegacyRecovery::CandidateExpectations candidateExpectations;
    candidateExpectations.provenance = resultProvenance;
    candidateExpectations.analysisEntries = analysisEntries;
    candidateExpectations.metadataKeys = copiedMetadataKeys;
    candidateExpectations.otherEventBranches = otherBranchNames;
    candidateExpectations.analysisBranches = analysisBranchNames;
    candidateExpectations.additionalTrees = additionalTrees;
    const auto validateCandidate = [&candidateExpectations](const std::filesystem::path& path,
                                                            std::ostream& errors) {
        return REST_LegacyRecovery::ValidateRecoveryCandidate(path, candidateExpectations, errors);
    };

    if (overwrite) {
        if (!skipped.empty() || !skippedEventBranches.empty()) {
            std::cout << "ERROR: refusing in-place replacement because the rebuilt file has omitted "
                         "content.\n"
                      << "The original is unchanged. Inspect the candidate file at: " << outName << std::endl;
            return;
        }
        std::error_code permissionError;
        const auto originalPermissions = std::filesystem::status(originalFile, permissionError).permissions();
        if (permissionError) {
            std::cout << "ERROR: cannot read original file permissions: " << permissionError.message()
                      << ". The original is unchanged." << std::endl;
            return;
        }
        std::filesystem::permissions(outName, originalPermissions, std::filesystem::perm_options::replace,
                                     permissionError);
        if (permissionError) {
            std::cout << "ERROR: cannot preserve original file permissions on the rebuilt candidate: "
                      << permissionError.message() << ". The original is unchanged." << std::endl;
            return;
        }
        if (!REST_LegacyRecovery::ValidateAndReplaceFileWithBackup(outName, originalFile, backupName,
                                                                   std::cout, validateCandidate)) {
            return;
        }
        finalName = originalFile;
        std::cout << "Original file kept as: " << backupName << std::endl;
    } else if (!validateCandidate(outName, std::cout)) {
        std::cout << "ERROR: rebuilt sibling candidate failed readback validation and must not be "
                     "used."
                  << std::endl;
        return;
    }

    std::cout << std::endl;
    std::cout << "Rebuilt " << nEntries << " entries: " << scannedCounts.signals << " signals, "
              << scannedCounts.points << " points." << std::endl;
    std::cout << "Fixed file written to: " << finalName << std::endl;
    gLegacySignalRebuildStatus = 0;
}

int REST_RebuildLegacySignalFileWithStatus(const char* originalFile, const char* signalDataFile = "",
                                           const char* outputFile = "", bool overwrite = false,
                                           bool requireComplete = false) {
    REST_Rebuild_Internal::gRequireCompleteRecovery = requireComplete;
    REST_RebuildLegacySignalFile(originalFile, signalDataFile, outputFile, overwrite);
    REST_Rebuild_Internal::gRequireCompleteRecovery = false;
    return REST_Rebuild_Internal::gLegacySignalRebuildStatus;
}

// No-argument entry point used only by the restRoot one-command orchestrator.
// Paths come from the child environment and never enter ROOT's command parser.
void REST_RebuildLegacySignalFile() {
    const char* input = gSystem->Getenv("REST_LEGACY_RECOVERY_INPUT");
    const char* intermediate = gSystem->Getenv("REST_LEGACY_RECOVERY_INTERMEDIATE");
    const char* output = gSystem->Getenv("REST_LEGACY_RECOVERY_OUTPUT");
    const char* inPlaceValue = gSystem->Getenv("REST_LEGACY_RECOVERY_IN_PLACE");
    const char* requireCompleteValue = gSystem->Getenv("REST_LEGACY_RECOVERY_REQUIRE_COMPLETE");
    if (input == nullptr || intermediate == nullptr || output == nullptr || inPlaceValue == nullptr ||
        requireCompleteValue == nullptr) {
        std::cerr << "ERROR: incomplete stage-2 recovery environment." << std::endl;
        gSystem->Exit(64);
        return;
    }
    const std::string inPlaceText(inPlaceValue);
    const std::string requireCompleteText(requireCompleteValue);
    if ((inPlaceText != "0" && inPlaceText != "1") ||
        (requireCompleteText != "0" && requireCompleteText != "1")) {
        std::cerr << "ERROR: invalid boolean value in the stage-2 recovery environment." << std::endl;
        gSystem->Exit(64);
        return;
    }

    gSystem->Exit(REST_RebuildLegacySignalFileWithStatus(input, intermediate, output, inPlaceText == "1",
                                                         requireCompleteText == "1"));
}
