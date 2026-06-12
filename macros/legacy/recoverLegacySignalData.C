// Stage 1 of 2 — recover TRestDetectorSignalEvent data from legacy REST files.
//
// REST files produced with detectorlib < v4 of TRestDetectorSignal (REST <= v2.4.2)
// store fSignalTime/fSignalCharge as vector<Float_t>. The current class uses
// vector<Double_t>, and since those files do not contain the StreamerInfo of the
// event classes, ROOT cannot convert on read: it misinterprets the float payload
// as doubles, leading to huge bogus allocations (rest-for-physics/detectorlib#125).
//
// This macro reads the signal data with replica classes that match the legacy
// on-disk layout exactly (so no conversion is needed) and extracts it to an
// intermediate file. Stage 2 (REST_RebuildLegacySignalFile.C, run with restRoot)
// rebuilds a fixed file with the current classes.
//
// IMPORTANT: run this macro with PLAIN root, NOT restRoot — the replica classes
// would clash with the real REST classes:
//
//     root -l -b -q 'recoverLegacySignalData.C+("R00236_...V2.4.0.root")'
//
// (note the '+': the macro must be compiled with ACLiC so that the replica
//  classes get a dictionary)
//
// The intermediate file is written next to the input as
// <input>_LegacySignalData.root unless an explicit output path is given.
//
// This file is deliberately NOT named REST_*.C so that restRoot's --m macro
// loading does not interpret it (the replica class definitions below would
// conflict with the compiled REST classes).

#include <TBranchElement.h>
#include <TFile.h>
#include <TNamed.h>
#include <TObject.h>
#include <TString.h>
#include <TSystem.h>
#include <TTimeStamp.h>
#include <TTree.h>

#include <iostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Replica classes matching the legacy on-disk layout (REST <= v2.4.2).
// TRestDetectorSignal uses the v3 layout (fName/fType added Aug 2023); files
// written with v1/v2 simply lack those sub-branches and reading them is a no-op.
//------------------------------------------------------------------------------
class TRestEvent : public TObject {
   public:
    Int_t fRunOrigin = 0;
    Int_t fSubRunOrigin = 0;
    Int_t fEventID = 0;
    Int_t fSubEventID = 0;
    TString fSubEventTag;
    TTimeStamp fEventTime;
    Bool_t fOk = true;
    ClassDef(TRestEvent, 1)
};

class TRestDetectorSignal {
   public:
    Int_t fSignalID = -1;
    std::vector<Float_t> fSignalTime;
    std::vector<Float_t> fSignalCharge;
    std::string fName;
    std::string fType;
    ClassDef(TRestDetectorSignal, 3)
};

class TRestDetectorSignalEvent : public TRestEvent {
   public:
    std::vector<TRestDetectorSignal> fSignal;
    ClassDef(TRestDetectorSignalEvent, 1)
};

//------------------------------------------------------------------------------
// Return the ClassDef version of TRestDetectorSignal recorded in the file's
// fSignal sub-branches, or -1 if it cannot be determined.
//------------------------------------------------------------------------------
static Int_t GetOnDiskSignalVersion(TBranch* branch) {
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

void recoverLegacySignalData(const char* inputFile, const char* outputFile = "") {
    // Refuse to run if the real REST libraries are loaded (restRoot session):
    // the replica classes above would clash with the compiled ones.
    TString loadedLibraries = gSystem->GetLibraries();
    if (loadedLibraries.Contains("libRestFramework") || loadedLibraries.Contains("libRestDetector")) {
        std::cout << "ERROR: REST libraries are loaded in this session." << std::endl;
        std::cout << "Run this macro with plain root, not restRoot:" << std::endl;
        std::cout << "    root -l -b -q 'recoverLegacySignalData.C+(\"" << inputFile << "\")'" << std::endl;
        return;
    }

    TFile* f = TFile::Open(inputFile);
    if (f == nullptr || f->IsZombie()) {
        std::cout << "ERROR: cannot open input file: " << inputFile << std::endl;
        return;
    }

    auto eventTree = dynamic_cast<TTree*>(f->Get("EventTree"));
    if (eventTree == nullptr) {
        std::cout << "ERROR: no EventTree found in " << inputFile << std::endl;
        return;
    }

    TBranch* signalBranch = eventTree->GetBranch("TRestDetectorSignalEventBranch");
    if (signalBranch == nullptr) {
        std::cout << "ERROR: no TRestDetectorSignalEventBranch in EventTree. Nothing to recover."
                  << std::endl;
        return;
    }

    const Int_t onDiskVersion = GetOnDiskSignalVersion(signalBranch);
    if (onDiskVersion < 0) {
        std::cout << "ERROR: could not determine the on-disk TRestDetectorSignal version." << std::endl;
        return;
    }
    if (onDiskVersion >= 4) {
        std::cout << "This file already uses the vector<Double_t> layout (TRestDetectorSignal v"
                  << onDiskVersion << "). Nothing to recover." << std::endl;
        return;
    }
    std::cout << "On-disk TRestDetectorSignal version: " << onDiskVersion << " (legacy float layout)"
              << std::endl;

    auto event = new TRestDetectorSignalEvent();
    eventTree->SetBranchAddress("TRestDetectorSignalEventBranch", &event);

    // Output file
    std::string outName = outputFile;
    if (outName.empty()) {
        outName = inputFile;
        const size_t pos = outName.rfind(".root");
        if (pos != std::string::npos) outName = outName.substr(0, pos);
        outName += "_LegacySignalData.root";
    }

    TFile out(outName.c_str(), "RECREATE");
    TTree dataTree("LegacySignalData", "TRestDetectorSignalEvent data recovered from legacy file");

    // Event header
    Int_t runOrigin, subRunOrigin, eventID, subEventID, timeSec, timeNanoSec;
    Bool_t ok;
    TString* subEventTag = new TString();
    // Signal data, flattened: per signal an entry in signalID/nPoints; the
    // time/charge values of all signals concatenated in order.
    std::vector<Int_t> signalID, nPoints;
    std::vector<Float_t> times, charges;

    dataTree.Branch("runOrigin", &runOrigin);
    dataTree.Branch("subRunOrigin", &subRunOrigin);
    dataTree.Branch("eventID", &eventID);
    dataTree.Branch("subEventID", &subEventID);
    dataTree.Branch("timeSec", &timeSec);
    dataTree.Branch("timeNanoSec", &timeNanoSec);
    dataTree.Branch("ok", &ok);
    dataTree.Branch("subEventTag", &subEventTag);
    dataTree.Branch("signalID", &signalID);
    dataTree.Branch("nPoints", &nPoints);
    dataTree.Branch("times", &times);
    dataTree.Branch("charges", &charges);

    const Long64_t nEntries = eventTree->GetEntries();
    Long64_t totalSignals = 0, totalPoints = 0, suspectValues = 0;

    for (Long64_t i = 0; i < nEntries; i++) {
        // Read only the signal branch: the other event branches have no
        // dictionary in a plain root session.
        signalBranch->GetEntry(i);

        runOrigin = event->fRunOrigin;
        subRunOrigin = event->fSubRunOrigin;
        eventID = event->fEventID;
        subEventID = event->fSubEventID;
        timeSec = event->fEventTime.GetSec();
        timeNanoSec = event->fEventTime.GetNanoSec();
        ok = event->fOk;
        *subEventTag = event->fSubEventTag;

        signalID.clear();
        nPoints.clear();
        times.clear();
        charges.clear();

        for (const auto& signal : event->fSignal) {
            signalID.push_back(signal.fSignalID);
            nPoints.push_back((Int_t)signal.fSignalTime.size());
            times.insert(times.end(), signal.fSignalTime.begin(), signal.fSignalTime.end());
            charges.insert(charges.end(), signal.fSignalCharge.begin(), signal.fSignalCharge.end());
            for (const auto v : signal.fSignalTime)
                if (std::abs(v) > 1e12) suspectValues++;
        }
        totalSignals += signalID.size();
        totalPoints += times.size();

        dataTree.Fill();
    }

    dataTree.Write();

    // Provenance
    TNamed("sourceFile", inputFile).Write();
    TNamed("onDiskSignalVersion", std::to_string(onDiskVersion).c_str()).Write();
    out.Close();

    std::cout << std::endl;
    std::cout << "Recovered " << nEntries << " entries: " << totalSignals << " signals, " << totalPoints
              << " points." << std::endl;
    if (suspectValues > 0)
        std::cout << "WARNING: " << suspectValues
                  << " suspicious values (|v| > 1e12) found — the recovered data may be corrupted!"
                  << std::endl;
    std::cout << "Signal data written to: " << outName << std::endl;
    std::cout << std::endl;
    std::cout << "Next step — rebuild the fixed file with restRoot:" << std::endl;
    std::cout << "    restRoot -b -q 'REST_RebuildLegacySignalFile.C(\"" << inputFile << "\")'" << std::endl;
}
