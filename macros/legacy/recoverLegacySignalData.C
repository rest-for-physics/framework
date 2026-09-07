// Extract the one unsafe legacy schema in an isolated plain-ROOT process.
// The user-facing restRoot command launches this macro; do not run it in a
// REST session because these replica class names intentionally match REST.

#include <TFile.h>
#include <TString.h>
#include <TSystem.h>
#include <TTimeStamp.h>
#include <TTree.h>

#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "LegacySignalRecoveryFormat.h"

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

namespace {

bool Extract(const char* inputName, const char* outputName) {
    using namespace REST_LegacySignalRecovery;
    const TString libraries = gSystem->GetLibraries();
    if (libraries.Contains("libRestFramework") || libraries.Contains("libRestDetector")) {
        std::cerr << "ERROR: legacy extraction must run without REST libraries loaded.\n";
        return false;
    }

    TFile input(inputName, "READ");
    auto* tree = dynamic_cast<TTree*>(input.Get(kEventTree));
    auto* branch = tree == nullptr ? nullptr : tree->GetBranch(kSignalBranch);
    const int version = DetectSignalVersion(branch);
    if (input.IsZombie() || tree == nullptr || branch == nullptr ||
        std::string(branch->GetClassName()) != "TRestDetectorSignalEvent" || version < 1 || version > 3) {
        std::cerr << "ERROR: input is not a supported TRestDetectorSignal v1-v3 file"
                  << " (detected version " << version << ").\n";
        return false;
    }

    auto* event = new TRestDetectorSignalEvent;
    if (tree->SetBranchAddress(kSignalBranch, &event) < TTree::kMatch) {
        std::cerr << "ERROR: cannot bind the legacy signal branch.\n";
        return false;
    }

    TFile output(outputName, "CREATE");
    if (output.IsZombie()) {
        std::cerr << "ERROR: cannot create the private recovery intermediate.\n";
        return false;
    }
    TTree data(kDataTree, "Legacy detector signal data");
    Int_t runOrigin, subRunOrigin, eventID, subEventID, timeNanoSec;
    Long64_t timeSec;
    Bool_t ok;
    TString subEventTag;
    std::vector<Int_t> signalIDs, pointCounts;
    std::vector<Float_t> times, charges;
    std::vector<std::string> names, types;
    data.Branch("runOrigin", &runOrigin);
    data.Branch("subRunOrigin", &subRunOrigin);
    data.Branch("eventID", &eventID);
    data.Branch("subEventID", &subEventID);
    data.Branch("timeSec", &timeSec);
    data.Branch("timeNanoSec", &timeNanoSec);
    data.Branch("ok", &ok);
    data.Branch("subEventTag", &subEventTag);
    data.Branch("signalIDs", &signalIDs);
    data.Branch("pointCounts", &pointCounts);
    data.Branch("times", &times);
    data.Branch("charges", &charges);
    data.Branch("names", &names);
    data.Branch("types", &types);

    Long64_t signalCount = 0;
    Long64_t pointCount = 0;
    for (Long64_t entry = 0; entry < tree->GetEntries(); ++entry) {
        if (branch->GetEntry(entry) <= 0) {
            std::cerr << "ERROR: cannot read legacy signal entry " << entry << ".\n";
            return false;
        }
        runOrigin = event->fRunOrigin;
        subRunOrigin = event->fSubRunOrigin;
        eventID = event->fEventID;
        subEventID = event->fSubEventID;
        timeSec = event->fEventTime.GetSec();
        timeNanoSec = event->fEventTime.GetNanoSec();
        ok = event->fOk;
        subEventTag = event->fSubEventTag;
        signalIDs.clear();
        pointCounts.clear();
        times.clear();
        charges.clear();
        names.clear();
        types.clear();
        for (const auto& signal : event->fSignal) {
            if (signal.fSignalTime.size() != signal.fSignalCharge.size() ||
                signal.fSignalTime.size() > static_cast<std::size_t>(std::numeric_limits<Int_t>::max())) {
                std::cerr << "ERROR: invalid legacy signal at entry " << entry << ".\n";
                return false;
            }
            signalIDs.push_back(signal.fSignalID);
            pointCounts.push_back(static_cast<Int_t>(signal.fSignalTime.size()));
            names.push_back(signal.fName);
            types.push_back(signal.fType);
            for (std::size_t point = 0; point < signal.fSignalTime.size(); ++point) {
                times.push_back(signal.fSignalTime[point]);
                charges.push_back(signal.fSignalCharge[point]);
            }
        }
        signalCount += signalIDs.size();
        pointCount += times.size();
        if (data.Fill() < 0) return false;
    }

    const Long64_t entries = tree->GetEntries();
    const Long64_t sourceSize = input.GetSize();
    const std::string sourceUuid = input.GetUUID().AsString();
    output.cd();
    const bool written = data.Write() > 0 && WriteText(output, "sourceUuid", sourceUuid) &&
                         WriteNumber(output, "sourceSize", sourceSize) &&
                         WriteNumber(output, "sourceEntries", entries) &&
                         WriteNumber(output, "sourceSignalVersion", version) &&
                         WriteNumber(output, "recoveredSignals", signalCount) &&
                         WriteNumber(output, "recoveredPoints", pointCount);
    output.Close();
    if (!written || output.TestBit(TFile::kWriteError)) {
        std::cerr << "ERROR: failed to write the recovery intermediate.\n";
        return false;
    }
    std::cout << "Extracted " << entries << " events, " << signalCount << " signals and " << pointCount
              << " points.\n";
    return true;
}

}  // namespace

void recoverLegacySignalData() {
    const char* input = gSystem->Getenv("REST_LEGACY_RECOVERY_INPUT");
    const char* output = gSystem->Getenv("REST_LEGACY_RECOVERY_INTERMEDIATE");
    if (input == nullptr || output == nullptr) {
        std::cerr << "ERROR: incomplete private recovery environment.\n";
        gSystem->Exit(64);
    } else {
        gSystem->Exit(Extract(input, output) ? 0 : 1);
    }
}
