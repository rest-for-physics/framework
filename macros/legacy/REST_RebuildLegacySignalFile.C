// Stage 2 of 2 — rebuild a legacy REST file with current TRestDetectorSignalEvent.
//
// Takes the original legacy file plus the intermediate file produced by stage 1
// (macros/legacy/recoverLegacySignalData.C, run with plain root) and writes a new
// file in which:
//   - the TRestDetectorSignalEventBranch is rebuilt with the current
//     vector<Double_t>-based classes,
//   - all other EventTree branches and the AnalysisTree are copied unchanged,
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
// Arguments:
//   originalFile   - the legacy REST file
//   signalDataFile - intermediate from stage 1; default: <original>_LegacySignalData.root
//   outputFile     - default: <original>_Fixed.root (ignored when overwrite=true)
//   overwrite      - replace originalFile in place, keeping a .bak copy

#include <TBranchElement.h>
#include <TFile.h>
#include <TKey.h>
#include <TNamed.h>
#include <TRestDetectorSignalEvent.h>
#include <TString.h>
#include <TSystem.h>
#include <TTree.h>

#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace REST_Rebuild_Internal {

Int_t GetOnDiskSignalVersion(TBranch* branch) {
    if (branch == nullptr) return -1;

    auto branchElement = dynamic_cast<TBranchElement*>(branch);
    if (branchElement != nullptr) {
        std::string name = branch->GetName();
        if (name == "fSignal.fSignalTime" || name == "fSignal.fSignalCharge")
            return branchElement->GetClassVersion();
    }

    auto subs = branch->GetListOfBranches();
    for (int i = 0; i <= subs->GetLast(); i++) {
        const Int_t version = GetOnDiskSignalVersion((TBranch*)subs->At(i));
        if (version > 0) return version;
    }
    return -1;
}

void SetBranchStatusRecursive(TBranch* branch, Bool_t status) {
    if (branch == nullptr) return;
    branch->SetStatus(status);
    auto subs = branch->GetListOfBranches();
    for (int i = 0; i <= subs->GetLast(); i++)
        SetBranchStatusRecursive((TBranch*)subs->At(i), status);
}

}  // namespace REST_Rebuild_Internal

void REST_RebuildLegacySignalFile(const char* originalFile, const char* signalDataFile = "",
                                  const char* outputFile = "", bool overwrite = false) {
    using namespace REST_Rebuild_Internal;

    std::string base = originalFile;
    const size_t pos = base.rfind(".root");
    if (pos != std::string::npos) base = base.substr(0, pos);

    std::string dataName = signalDataFile;
    if (dataName.empty()) dataName = base + "_LegacySignalData.root";

    std::string outName = outputFile;
    if (outName.empty()) outName = base + "_Fixed.root";
    if (overwrite) outName = base + "_FixedTmp.root";

    // --- open inputs ---
    TFile* original = TFile::Open(originalFile);
    if (original == nullptr || original->IsZombie()) {
        std::cout << "ERROR: cannot open original file: " << originalFile << std::endl;
        return;
    }
    TFile* data = TFile::Open(dataName.c_str());
    if (data == nullptr || data->IsZombie()) {
        std::cout << "ERROR: cannot open signal data file: " << dataName << std::endl;
        std::cout << "Run stage 1 first (with plain root, NOT restRoot):" << std::endl;
        std::cout << "    root -l -b -q 'recoverLegacySignalData.C+(\"" << originalFile << "\")'"
                  << std::endl;
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
    const Int_t onDiskVersion = GetOnDiskSignalVersion(signalBranch);
    if (onDiskVersion >= 4) {
        std::cout << "This file already uses the current layout (TRestDetectorSignal v" << onDiskVersion
                  << "). Nothing to rebuild." << std::endl;
        return;
    }

    const Long64_t nEntries = oldTree->GetEntries();
    if (dataTree->GetEntries() != nEntries) {
        std::cout << "ERROR: entry mismatch — EventTree has " << nEntries << " entries, signal data has "
                  << dataTree->GetEntries() << ". Wrong intermediate file?" << std::endl;
        return;
    }

    // --- bind the intermediate tree ---
    Int_t runOrigin, subRunOrigin, eventID, subEventID, timeSec, timeNanoSec;
    Bool_t ok;
    TString* subEventTag = nullptr;
    std::vector<Int_t>*signalID = nullptr, *nPoints = nullptr;
    std::vector<Float_t>*times = nullptr, *charges = nullptr;

    dataTree->SetBranchAddress("runOrigin", &runOrigin);
    dataTree->SetBranchAddress("subRunOrigin", &subRunOrigin);
    dataTree->SetBranchAddress("eventID", &eventID);
    dataTree->SetBranchAddress("subEventID", &subEventID);
    dataTree->SetBranchAddress("timeSec", &timeSec);
    dataTree->SetBranchAddress("timeNanoSec", &timeNanoSec);
    dataTree->SetBranchAddress("ok", &ok);
    dataTree->SetBranchAddress("subEventTag", &subEventTag);
    dataTree->SetBranchAddress("signalID", &signalID);
    dataTree->SetBranchAddress("nPoints", &nPoints);
    dataTree->SetBranchAddress("times", &times);
    dataTree->SetBranchAddress("charges", &charges);

    // --- output file (single write session: StreamerInfos are preserved) ---
    TFile* out = TFile::Open(outName.c_str(), "RECREATE");
    if (out == nullptr || out->IsZombie()) {
        std::cout << "ERROR: cannot create output file: " << outName << std::endl;
        return;
    }

    // --- copy metadata keys (highest cycle only, skip trees) ---
    std::set<std::string> seen;
    std::vector<std::string> skipped;
    TIter nextKey(original->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)nextKey())) {
        const std::string keyName = key->GetName();
        if (seen.count(keyName)) continue;  // keys are ordered newest cycle first
        seen.insert(keyName);

        TClass* cl = TClass::GetClass(key->GetClassName());
        if (cl != nullptr && cl->InheritsFrom("TTree")) continue;  // trees handled below

        TObject* obj = (cl != nullptr) ? key->ReadObj() : nullptr;
        if (obj == nullptr) {
            skipped.push_back(keyName + " (" + key->GetClassName() + ")");
            continue;
        }
        out->cd();
        obj->Write(keyName.c_str());
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
    TIter nextBranch(oldTree->GetListOfBranches());
    TBranch* br;
    while ((br = (TBranch*)nextBranch())) {
        const std::string name = br->GetName();
        if (name == "TRestDetectorSignalEventBranch") continue;
        const size_t suffix = name.rfind("Branch");
        const std::string className = (suffix != std::string::npos) ? name.substr(0, suffix) : name;
        if (TClass::GetClass(className.c_str()) == nullptr) {
            std::cout << "WARNING: no dictionary for event class '" << className << "'; branch '" << name
                      << "' will NOT be copied!" << std::endl;
            SetBranchStatusRecursive(br, 0);
            continue;
        }
        otherBranchNames.push_back(name);
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
        oldTree->SetBranchAddress(name.c_str(), &otherEvents[b]);
        newTree->Branch(name.c_str(), className.c_str(), &otherEvents[b]);
    }

    auto event = new TRestDetectorSignalEvent();
    newTree->Branch("TRestDetectorSignalEventBranch", &event);

    Long64_t totalSignals = 0, totalPoints = 0;
    for (Long64_t i = 0; i < nEntries; i++) {
        oldTree->GetEntry(i);  // reads the other event branches (signal branch disabled)
        dataTree->GetEntry(i);

        event->Initialize();
        event->SetRunOrigin(runOrigin);
        event->SetSubRunOrigin(subRunOrigin);
        event->SetID(eventID);
        event->SetSubID(subEventID);
        event->SetSubEventTag(*subEventTag);
        event->SetTime((Double_t)timeSec, (Double_t)timeNanoSec);
        event->SetOK(ok);

        size_t offset = 0;
        for (size_t s = 0; s < signalID->size(); s++) {
            TRestDetectorSignal signal;
            signal.SetSignalID(signalID->at(s));
            const size_t n = nPoints->at(s);
            for (size_t p = 0; p < n; p++)
                signal.NewPoint(times->at(offset + p), charges->at(offset + p));
            offset += n;
            event->AddSignal(signal);
        }
        totalSignals += signalID->size();
        totalPoints += offset;

        newTree->Fill();
    }
    newTree->Write("", TObject::kOverwrite);

    // --- copy the AnalysisTree unchanged ---
    auto anaTree = dynamic_cast<TTree*>(original->Get("AnalysisTree"));
    if (anaTree != nullptr) {
        out->cd();
        TTree* anaClone = anaTree->CloneTree(-1, "fast");
        anaClone->Write("", TObject::kOverwrite);
    } else {
        std::cout << "WARNING: no AnalysisTree found; skipping." << std::endl;
    }

    out->Close();
    original->Close();
    data->Close();

    // --- overwrite handling ---
    std::string finalName = outName;
    if (overwrite) {
        const std::string backup = std::string(originalFile) + ".bak";
        if (gSystem->Rename(originalFile, backup.c_str()) != 0) {
            std::cout << "ERROR: could not move original to " << backup << "; fixed file left at "
                      << outName << std::endl;
            return;
        }
        gSystem->Rename(outName.c_str(), originalFile);
        finalName = originalFile;
        std::cout << "Original file kept as: " << backup << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Rebuilt " << nEntries << " entries: " << totalSignals << " signals, " << totalPoints
              << " points." << std::endl;
    if (!skipped.empty()) {
        std::cout << "WARNING: " << skipped.size()
                  << " metadata key(s) could not be read with the current libraries and were NOT copied:"
                  << std::endl;
        for (const auto& s : skipped) std::cout << "   - " << s << std::endl;
    }
    std::cout << "Fixed file written to: " << finalName << std::endl;
}
