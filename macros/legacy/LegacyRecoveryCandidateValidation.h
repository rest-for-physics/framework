#ifndef REST_LEGACY_RECOVERY_CANDIDATE_VALIDATION_H
#define REST_LEGACY_RECOVERY_CANDIDATE_VALIDATION_H

#include <TBranchElement.h>
#include <TFile.h>
#include <TRestDetectorSignalEvent.h>
#include <TTree.h>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "LegacyRecoveryDataUtils.h"
#include "LegacyRecoveryProvenance.h"

namespace REST_LegacyRecovery {

namespace CandidateValidationDetail {

inline void CollectSignalSchemaVersions(TBranch* branch, SignalSchemaVersions& versions, bool& timeSeen,
                                        bool& chargeSeen) {
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
        CollectSignalSchemaVersions(static_cast<TBranch*>(children->At(index)), versions, timeSeen,
                                    chargeSeen);
    }
}

}  // namespace CandidateValidationDetail

inline SignalSchemaVersions DetectSignalSchemaVersions(TBranch* branch) {
    SignalSchemaVersions versions;
    bool timeSeen = false;
    bool chargeSeen = false;
    CandidateValidationDetail::CollectSignalSchemaVersions(branch, versions, timeSeen, chargeSeen);
    if (!timeSeen) versions.time = -1;
    if (!chargeSeen) versions.charge = -1;
    return versions;
}

struct CandidateExpectations {
    RecoveryProvenance provenance;
    Long64_t analysisEntries = -1;
    std::vector<std::string> metadataKeys;
    std::vector<std::string> otherEventBranches;
    std::vector<std::string> analysisBranches;
};

inline bool ValidateRecoveryCandidate(const std::filesystem::path& candidatePath,
                                      const CandidateExpectations& expected, std::ostream& errors) {
    std::unique_ptr<TFile> candidate(TFile::Open(candidatePath.string().c_str(), "READ"));
    if (candidate == nullptr || candidate->IsZombie()) {
        errors << "ERROR: cannot reopen rebuilt candidate: " << candidatePath.string() << ".\n";
        return false;
    }
    if (candidate->TestBit(TFile::kRecovered)) {
        errors << "ERROR: rebuilt candidate required ROOT file recovery and is not safe to install.\n";
        return false;
    }

    RecoveryProvenance actualProvenance;
    if (!ReadRecoveryProvenance(*candidate, actualProvenance, errors) ||
        !ValidateResultProvenance(actualProvenance, expected.provenance, candidate->GetUUID().AsString(),
                                  errors)) {
        return false;
    }

    for (const auto& key : expected.metadataKeys) {
        if (candidate->GetKey(key.c_str()) == nullptr) {
            errors << "ERROR: rebuilt candidate is missing copied metadata key '" << key << "'.\n";
            return false;
        }
    }

    auto eventTree = dynamic_cast<TTree*>(candidate->Get("EventTree"));
    if (eventTree == nullptr) {
        errors << "ERROR: rebuilt candidate has no readable EventTree.\n";
        return false;
    }
    if (eventTree->GetEntries() != static_cast<Long64_t>(expected.provenance.recovered.entries)) {
        errors << "ERROR: rebuilt candidate EventTree has " << eventTree->GetEntries()
               << " entries, expected " << expected.provenance.recovered.entries << ".\n";
        return false;
    }

    for (const auto& branch : expected.otherEventBranches) {
        if (eventTree->GetBranch(branch.c_str()) == nullptr) {
            errors << "ERROR: rebuilt candidate is missing copied event branch '" << branch << "'.\n";
            return false;
        }
    }

    auto signalBranch = eventTree->GetBranch("TRestDetectorSignalEventBranch");
    if (signalBranch == nullptr) {
        errors << "ERROR: rebuilt candidate has no TRestDetectorSignalEventBranch.\n";
        return false;
    }
    const auto schema = DetectSignalSchemaVersions(signalBranch);
    if (!ValidateCurrentSignalSchema(schema, errors, "rebuilt candidate")) return false;

    auto eventOwner = std::make_unique<TRestDetectorSignalEvent>();
    TRestDetectorSignalEvent* event = eventOwner.get();
    const int bindStatus = eventTree->SetBranchAddress("TRestDetectorSignalEventBranch", &event);
    if (bindStatus < TTree::kMatch) {
        errors << "ERROR: cannot bind rebuilt candidate signal branch (status " << bindStatus << ").\n";
        return false;
    }

    RecoveryCounts actualCounts;
    actualCounts.entries = static_cast<std::uint64_t>(eventTree->GetEntries());
    for (Long64_t entry = 0; entry < eventTree->GetEntries(); ++entry) {
        if (signalBranch->GetEntry(entry) <= 0 || event == nullptr) {
            errors << "ERROR: cannot read rebuilt signal event at entry " << entry << ".\n";
            eventTree->ResetBranchAddresses();
            return false;
        }

        const int signalCount = event->GetNumberOfSignals();
        if (signalCount < 0 || !CheckedAdd(actualCounts.signals, static_cast<std::uint64_t>(signalCount),
                                           errors, "rebuilt signals")) {
            eventTree->ResetBranchAddresses();
            return false;
        }
        for (int signalIndex = 0; signalIndex < signalCount; ++signalIndex) {
            auto signal = event->GetSignal(signalIndex);
            if (signal == nullptr) {
                errors << "ERROR: rebuilt signal event " << entry << " has a null signal at index "
                       << signalIndex << ".\n";
                eventTree->ResetBranchAddresses();
                return false;
            }
            const int pointCount = signal->GetNumberOfPoints();
            if (pointCount < 0 || !CheckedAdd(actualCounts.points, static_cast<std::uint64_t>(pointCount),
                                              errors, "rebuilt points")) {
                eventTree->ResetBranchAddresses();
                return false;
            }
        }
    }
    eventTree->ResetBranchAddresses();

    if (actualCounts.entries != expected.provenance.recovered.entries ||
        actualCounts.signals != expected.provenance.recovered.signals ||
        actualCounts.points != expected.provenance.recovered.points) {
        errors << "ERROR: rebuilt candidate readback counts differ from the validated intermediate:"
               << " entries " << actualCounts.entries << "/" << expected.provenance.recovered.entries
               << ", signals " << actualCounts.signals << "/" << expected.provenance.recovered.signals
               << ", points " << actualCounts.points << "/" << expected.provenance.recovered.points << ".\n";
        return false;
    }

    auto analysisTree = dynamic_cast<TTree*>(candidate->Get("AnalysisTree"));
    if (expected.analysisEntries < 0) {
        if (analysisTree != nullptr) {
            errors << "ERROR: rebuilt candidate unexpectedly contains an AnalysisTree.\n";
            return false;
        }
    } else if (analysisTree == nullptr || analysisTree->GetEntries() != expected.analysisEntries) {
        errors << "ERROR: rebuilt candidate AnalysisTree is missing or has the wrong entry count.\n";
        return false;
    } else {
        for (const auto& branch : expected.analysisBranches) {
            if (analysisTree->GetBranch(branch.c_str()) == nullptr) {
                errors << "ERROR: rebuilt candidate AnalysisTree is missing branch '" << branch << "'.\n";
                return false;
            }
        }
    }

    candidate->Close();
    return true;
}

}  // namespace REST_LegacyRecovery

#endif
