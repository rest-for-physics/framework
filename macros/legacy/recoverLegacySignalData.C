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
// Existing output files are never overwritten.
// It records the source ROOT UUID, canonicalized source path, file size,
// legacy schema version, and recovered counts. These facts authenticate the
// intended source against accidental mix-ups; they are not a cryptographic
// signature and do not make a maliciously edited intermediate trustworthy.
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

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "LegacyRecoveryDataUtils.h"
#include "LegacyRecoveryFileUtils.h"
#include "LegacyRecoveryProvenance.h"

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
// Return the ClassDef versions recorded in the time and charge sub-branches.
//------------------------------------------------------------------------------
static void CollectOnDiskSignalVersions(TBranch* branch, REST_LegacyRecovery::SignalSchemaVersions& versions,
                                        bool& timeSeen, bool& chargeSeen) {
    if (branch == nullptr) return;

    auto branchElement = dynamic_cast<TBranchElement*>(branch);
    if (branchElement != nullptr) {
        const std::string name = branch->GetName();
        const int version = branchElement->GetClassVersion();
        if (name == "fSignal.fSignalTime") {
            versions.time = timeSeen && versions.time != version ? -2 : version;
            timeSeen = true;
        } else if (name == "fSignal.fSignalCharge") {
            versions.charge = chargeSeen && versions.charge != version ? -2 : version;
            chargeSeen = true;
        }
    }

    auto children = branch->GetListOfBranches();
    for (int index = 0; index <= children->GetLast(); ++index) {
        CollectOnDiskSignalVersions(static_cast<TBranch*>(children->At(index)), versions, timeSeen,
                                    chargeSeen);
    }
}

static REST_LegacyRecovery::SignalSchemaVersions GetOnDiskSignalVersions(TBranch* branch) {
    REST_LegacyRecovery::SignalSchemaVersions versions;
    bool timeSeen = false;
    bool chargeSeen = false;
    CollectOnDiskSignalVersions(branch, versions, timeSeen, chargeSeen);
    if (!timeSeen) versions.time = -1;
    if (!chargeSeen) versions.charge = -1;
    return versions;
}

namespace {
int gLegacySignalExtractionStatus = 1;
}

static bool ValidateLegacyEvent(const TRestDetectorSignalEvent* event, Long64_t entry,
                                std::uint64_t& signalCount, std::uint64_t& pointCount,
                                Long64_t* suspectValues, std::ostream& errors) {
    signalCount = 0;
    pointCount = 0;
    if (event == nullptr) {
        errors << "ERROR: legacy signal event pointer is null at entry " << entry << ".\n";
        return false;
    }

    signalCount = event->fSignal.size();
    for (std::size_t index = 0; index < event->fSignal.size(); ++index) {
        const auto& signal = event->fSignal[index];
        if (signal.fSignalTime.size() != signal.fSignalCharge.size()) {
            errors << "ERROR: legacy signal event " << entry << ", signal " << index << " has "
                   << signal.fSignalTime.size() << " time values but " << signal.fSignalCharge.size()
                   << " charge values.\n";
            return false;
        }
        if (signal.fSignalTime.size() > static_cast<std::size_t>(std::numeric_limits<Int_t>::max())) {
            errors << "ERROR: legacy signal event " << entry << ", signal " << index
                   << " has too many points to encode in the intermediate format.\n";
            return false;
        }
        if (!REST_LegacyRecovery::CheckedAdd(pointCount, signal.fSignalTime.size(), errors,
                                             "legacy signal points")) {
            return false;
        }
        for (std::size_t point = 0; point < signal.fSignalTime.size(); ++point) {
            const auto time = signal.fSignalTime[point];
            const auto charge = signal.fSignalCharge[point];
            if (!std::isfinite(time) || !std::isfinite(charge)) {
                errors << "ERROR: legacy signal event " << entry << ", signal " << index
                       << " has a non-finite time or charge at point " << point << ".\n";
                return false;
            }
            if (suspectValues != nullptr) {
                if (std::abs(time) > 1e12) ++(*suspectValues);
                if (std::abs(charge) > 1e12) ++(*suspectValues);
            }
        }
    }
    return true;
}

void recoverLegacySignalData(const char* inputFile, const char* outputFile = "") {
    gLegacySignalExtractionStatus = 1;
    // Refuse to run if the real REST libraries are loaded (restRoot session):
    // the replica classes above would clash with the compiled ones.
    TString loadedLibraries = gSystem->GetLibraries();
    if (loadedLibraries.Contains("libRestFramework") || loadedLibraries.Contains("libRestDetector")) {
        std::cout << "ERROR: REST libraries are loaded in this session." << std::endl;
        std::cout << "Exit this session and use the isolated one-command workflow:" << std::endl;
        std::cout << "    restRoot --recover-legacy-signals INPUT" << std::endl;
        std::cout << "  input: " << inputFile << std::endl;
        return;
    }

    std::string outName = outputFile;
    if (outName.empty())
        outName = REST_LegacyRecovery::BuildSiblingRootPath(inputFile, "_LegacySignalData").string();
    if (!REST_LegacyRecovery::ValidateNewOutputPath(inputFile, outName, "legacy signal data output",
                                                    std::cout)) {
        return;
    }

    std::unique_ptr<TFile> f(TFile::Open(inputFile));
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

    const auto onDiskVersions = GetOnDiskSignalVersions(signalBranch);
    if (!REST_LegacyRecovery::ValidateSupportedLegacySchema(onDiskVersions, std::cout, "source file")) {
        return;
    }
    const Int_t onDiskVersion = onDiskVersions.time;
    std::cout << "On-disk TRestDetectorSignal version: " << onDiskVersion << " (legacy float layout)"
              << std::endl;

    auto event = new TRestDetectorSignalEvent();
    const int signalBindStatus = eventTree->SetBranchAddress("TRestDetectorSignalEventBranch", &event);
    if (signalBindStatus < TTree::kMatch) {
        std::cout << "ERROR: cannot bind the legacy detector signal branch (status " << signalBindStatus
                  << ")." << std::endl;
        return;
    }

    const Long64_t nEntries = eventTree->GetEntries();
    const Long64_t sourceSize = f->GetSize();
    if (nEntries < 0 || sourceSize < 0) {
        std::cout << "ERROR: source file reports an invalid entry count or file size." << std::endl;
        return;
    }
    REST_LegacyRecovery::RecoveryCounts recoveredCounts;
    recoveredCounts.entries = static_cast<std::uint64_t>(nEntries);
    Long64_t suspectValues = 0;
    for (Long64_t entry = 0; entry < nEntries; ++entry) {
        if (signalBranch->GetEntry(entry) <= 0) {
            std::cout << "ERROR: cannot read the legacy detector signal branch at entry " << entry << "."
                      << std::endl;
            return;
        }
        std::uint64_t eventSignals = 0;
        std::uint64_t eventPoints = 0;
        if (!ValidateLegacyEvent(event, entry, eventSignals, eventPoints, &suspectValues, std::cout) ||
            !REST_LegacyRecovery::CheckedAdd(recoveredCounts.signals, eventSignals, std::cout,
                                             "legacy signals") ||
            !REST_LegacyRecovery::CheckedAdd(recoveredCounts.points, eventPoints, std::cout,
                                             "legacy points")) {
            return;
        }
    }

    std::string sourcePath;
    std::string pathError;
    if (!REST_LegacyRecovery::ResolvePathIdentity(inputFile, sourcePath, pathError)) {
        std::cout << "ERROR: " << pathError << std::endl;
        return;
    }

    REST_LegacyRecovery::SourceIdentity sourceIdentity;
    sourceIdentity.uuid = f->GetUUID().AsString();
    sourceIdentity.normalizedPath = sourcePath;
    sourceIdentity.fileSize = static_cast<std::uint64_t>(sourceSize);
    sourceIdentity.entries = recoveredCounts.entries;
    sourceIdentity.signalVersion = onDiskVersion;

    TFile out(outName.c_str(), "CREATE");
    if (out.IsZombie()) {
        std::cout << "ERROR: cannot create output file: " << outName << std::endl;
        return;
    }
    TTree dataTree("LegacySignalData", "TRestDetectorSignalEvent data recovered from legacy file");

    // Event header
    Int_t runOrigin, subRunOrigin, eventID, subEventID, timeSec, timeNanoSec;
    Bool_t ok;
    TString* subEventTag = new TString();
    // Signal data, flattened: per signal an entry in signalID/nPoints; the
    // time/charge values of all signals concatenated in order.
    std::vector<Int_t> signalID, nPoints;
    std::vector<Float_t> times, charges;

    if (dataTree.Branch("runOrigin", &runOrigin) == nullptr ||
        dataTree.Branch("subRunOrigin", &subRunOrigin) == nullptr ||
        dataTree.Branch("eventID", &eventID) == nullptr ||
        dataTree.Branch("subEventID", &subEventID) == nullptr ||
        dataTree.Branch("timeSec", &timeSec) == nullptr ||
        dataTree.Branch("timeNanoSec", &timeNanoSec) == nullptr || dataTree.Branch("ok", &ok) == nullptr ||
        dataTree.Branch("subEventTag", &subEventTag) == nullptr ||
        dataTree.Branch("signalID", &signalID) == nullptr ||
        dataTree.Branch("nPoints", &nPoints) == nullptr || dataTree.Branch("times", &times) == nullptr ||
        dataTree.Branch("charges", &charges) == nullptr) {
        std::cout << "ERROR: failed to create one or more intermediate branches." << std::endl;
        REST_LegacyRecovery::CheckAndCloseOutputFile(out, std::cout);
        return;
    }

    for (Long64_t i = 0; i < nEntries; i++) {
        // Read only the signal branch: the other event branches have no
        // dictionary in a plain root session.
        if (signalBranch->GetEntry(i) <= 0) {
            std::cout << "ERROR: cannot reread the legacy detector signal branch at entry " << i << "."
                      << std::endl;
            REST_LegacyRecovery::CheckAndCloseOutputFile(out, std::cout);
            return;
        }
        std::uint64_t eventSignals = 0;
        std::uint64_t eventPoints = 0;
        if (!ValidateLegacyEvent(event, i, eventSignals, eventPoints, nullptr, std::cout)) {
            REST_LegacyRecovery::CheckAndCloseOutputFile(out, std::cout);
            return;
        }

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
        }
        if (eventSignals != signalID.size() || eventPoints != times.size() ||
            times.size() != charges.size()) {
            std::cout << "ERROR: legacy event " << i << " changed while flattening the validated signal data."
                      << std::endl;
            REST_LegacyRecovery::CheckAndCloseOutputFile(out, std::cout);
            return;
        }

        if (dataTree.Fill() < 0) {
            std::cout << "ERROR: failed to write intermediate entry " << i << "." << std::endl;
            REST_LegacyRecovery::CheckAndCloseOutputFile(out, std::cout);
            return;
        }
    }

    if (dataTree.Write() <= 0) {
        std::cout << "ERROR: failed to write the LegacySignalData tree." << std::endl;
        REST_LegacyRecovery::CheckAndCloseOutputFile(out, std::cout);
        return;
    }

    REST_LegacyRecovery::RecoveryProvenance provenance;
    provenance.formatVersion = REST_LegacyRecovery::kRecoveryFormatVersion;
    provenance.kind = REST_LegacyRecovery::kIntermediateProvenanceKind;
    provenance.source = sourceIdentity;
    provenance.recovered = recoveredCounts;
    provenance.intermediateUuid = out.GetUUID().AsString();
    if (!REST_LegacyRecovery::WriteRecoveryProvenance(out, provenance, std::cout) ||
        TNamed("sourceFile", sourcePath.c_str()).Write() <= 0 ||
        TNamed("onDiskSignalVersion", std::to_string(onDiskVersion).c_str()).Write() <= 0) {
        std::cout << "ERROR: failed to write complete intermediate provenance." << std::endl;
        REST_LegacyRecovery::CheckAndCloseOutputFile(out, std::cout);
        return;
    }
    if (!REST_LegacyRecovery::CheckAndCloseOutputFile(out, std::cout)) {
        std::cout << "ERROR: intermediate output is incomplete and must not be used." << std::endl;
        return;
    }

    std::cout << std::endl;
    std::cout << "Recovered " << nEntries << " entries: " << recoveredCounts.signals << " signals, "
              << recoveredCounts.points << " points." << std::endl;
    if (suspectValues > 0)
        std::cout << "WARNING: " << suspectValues
                  << " suspicious values (|v| > 1e12) found — the recovered data may be corrupted!"
                  << std::endl;
    std::cout << "Signal data written to: " << outName << std::endl;
    std::cout << "Run REST_RebuildLegacySignalFile.C in a fresh restRoot process to rebuild the final file."
              << std::endl;
    std::cout << "  original:     " << inputFile << std::endl;
    std::cout << "  intermediate: " << outName << std::endl;
    gLegacySignalExtractionStatus = 0;
}

int recoverLegacySignalDataWithStatus(const char* inputFile, const char* outputFile = "") {
    recoverLegacySignalData(inputFile, outputFile);
    return gLegacySignalExtractionStatus;
}

// No-argument entry point used only by the restRoot one-command orchestrator.
// Paths come from the child environment and never enter ROOT's command parser.
void recoverLegacySignalData() {
    const char* input = gSystem->Getenv("REST_LEGACY_RECOVERY_INPUT");
    const char* intermediate = gSystem->Getenv("REST_LEGACY_RECOVERY_INTERMEDIATE");
    const char* workDirectory = gSystem->Getenv("REST_LEGACY_RECOVERY_WORK_DIR");
    if (input == nullptr || intermediate == nullptr || workDirectory == nullptr) {
        std::cerr << "ERROR: incomplete stage-1 recovery environment." << std::endl;
        gSystem->Exit(64);
        return;
    }
    gSystem->Exit(recoverLegacySignalDataWithStatus(input, intermediate));
}
