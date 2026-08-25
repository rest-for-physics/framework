#include <TBranch.h>
#include <TFile.h>
#include <TRestDetectorSignalEvent.h>
#include <TRestRawSignalEvent.h>
#include <TRestRun.h>
#include <TSystem.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../../../../macros/legacy/LegacyRecoveryCandidateValidation.h"
#include "../../../../macros/legacy/LegacyRecoveryFileUtils.h"
#include "../../../../macros/legacy/LegacyRecoveryProvenance.h"

namespace {

namespace fs = std::filesystem;

class TemporaryDirectory {
   public:
    TemporaryDirectory() {
        static std::atomic<unsigned long> sequence{0};
        const auto suffix =
            std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count()) + "_" +
            std::to_string(sequence++);
        path = fs::temp_directory_path() / ("rest_legacy_candidate_test_" + suffix);
        fs::create_directories(path);
    }

    ~TemporaryDirectory() {
        std::error_code error;
        fs::remove_all(path, error);
    }

    fs::path path;
};

void WriteText(const fs::path& path, const std::string& text) {
    std::ofstream output(path);
    ASSERT_TRUE(output.is_open());
    output << text;
}

std::string ReadText(const fs::path& path) {
    std::ifstream input(path);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

class UnknownEvent : public TRestRawSignalEvent {
   public:
    const char* ClassName() const override { return "UnknownEvent"; }
};

TBranch* FindBranch(TBranch* branch, const std::string& name) {
    if (branch == nullptr) return nullptr;
    if (branch->GetName() == name) return branch;

    auto children = branch->GetListOfBranches();
    for (int i = 0; i <= children->GetLast(); i++) {
        auto found = FindBranch((TBranch*)children->At(i), name);
        if (found != nullptr) return found;
    }
    return nullptr;
}

bool IsEnabled(TBranch* branch) { return branch != nullptr && !branch->TestBit(::kDoNotProcess); }

fs::path GetLegacySignalTestFile() {
    const char* environmentFile = std::getenv("REST_LEGACY_SIGNAL_TEST_FILE");
    if (environmentFile != nullptr && environmentFile[0] != '\0') return environmentFile;
    return REST_LEGACY_SIGNAL_TEST_FILE;
}

class LegacyDetectorSignalTest : public ::testing::Test {
   protected:
    void SetUp() override {
        filename = GetLegacySignalTestFile();
        if (filename.empty() || !fs::is_regular_file(filename)) {
            GTEST_SKIP() << "Set REST_LEGACY_SIGNAL_TEST_FILE to the canonical legacy Signal ROOT file";
        }

        ASSERT_GE(gSystem->Load("libRestDetector.so"), 0);
        ASSERT_GE(gSystem->Load("libRestRaw.so"), 0);
        ASSERT_GE(gSystem->Load("libRestTrack.so"), 0);
    }

    void AssertLegacySignalDisabled(TRestRun& run) {
        auto signalBranch = run.GetEventTree()->GetBranch("TRestDetectorSignalEventBranch");
        ASSERT_NE(signalBranch, nullptr);
        auto timeBranch = FindBranch(signalBranch, "fSignal.fSignalTime");
        auto chargeBranch = FindBranch(signalBranch, "fSignal.fSignalCharge");
        ASSERT_NE(timeBranch, nullptr);
        ASSERT_NE(chargeBranch, nullptr);
        EXPECT_FALSE(IsEnabled(signalBranch));
        EXPECT_FALSE(IsEnabled(timeBranch));
        EXPECT_FALSE(IsEnabled(chargeBranch));
    }

    fs::path filename;
};

TEST_F(LegacyDetectorSignalTest, DetectsAndDisablesUnsafeBranch) {
    TRestRun run;
    run.OpenInputFile(filename.string().c_str());

    ASSERT_NE(run.GetInputEvent(), nullptr);
    EXPECT_STREQ(run.GetInputEvent()->ClassName(), "TRestTrackEvent");
    AssertLegacySignalDisabled(run);

    for (Long64_t entry = 0; entry < run.GetEntries(); entry++) run.GetEntry(entry);
}

TEST_F(LegacyDetectorSignalTest, RejectedLegacySelectionPreservesCurrentEvent) {
    TRestRun run;
    run.OpenInputFile(filename.string().c_str());
    run.GetEntry(0);
    AssertLegacySignalDisabled(run);

    auto currentEvent = run.GetInputEvent();
    ASSERT_NE(currentEvent, nullptr);
    const std::string currentType = currentEvent->ClassName();

    auto requestedEvent = std::make_unique<TRestDetectorSignalEvent>();
    auto requestedEventPointer = requestedEvent.get();
    run.SetInputEvent(requestedEventPointer);
    if (run.GetInputEvent() == requestedEventPointer) requestedEvent.release();

    EXPECT_EQ(run.GetInputEvent(), currentEvent);
    EXPECT_EQ(std::string(run.GetInputEvent()->ClassName()), currentType);
    AssertLegacySignalDisabled(run);
}

TEST_F(LegacyDetectorSignalTest, InvalidSelectionPreservesCurrentEvent) {
    TRestRun run;
    run.OpenInputFile(filename.string().c_str());
    run.GetEntry(0);

    auto currentEvent = run.GetInputEvent();
    ASSERT_NE(currentEvent, nullptr);
    const std::string currentType = currentEvent->ClassName();

    auto requestedEvent = std::make_unique<UnknownEvent>();
    auto requestedEventPointer = requestedEvent.get();
    run.SetInputEvent(requestedEventPointer);
    if (run.GetInputEvent() == requestedEventPointer) requestedEvent.release();

    EXPECT_EQ(run.GetInputEvent(), currentEvent);
    EXPECT_EQ(std::string(run.GetInputEvent()->ClassName()), currentType);
}

TEST(LegacyRecoveryCandidateValidation, InvalidCandidateLeavesOriginalUntouched) {
    TemporaryDirectory temporary;
    const auto original = temporary.path / "input.root";
    const auto replacement = temporary.path / "fixed.root";
    const auto backup = temporary.path / "input.root.bak";
    WriteText(original, "original");

    REST_LegacyRecovery::RecoveryProvenance provenance;
    provenance.formatVersion = REST_LegacyRecovery::kRecoveryFormatVersion;
    provenance.kind = REST_LegacyRecovery::kResultProvenanceKind;
    provenance.source.uuid = "source-uuid";
    provenance.source.normalizedPath = "/canonical/input.root";
    provenance.source.fileSize = 1234;
    provenance.source.entries = 0;
    provenance.source.signalVersion = 3;
    provenance.intermediateUuid = "intermediate-uuid";
    provenance.intermediatePath = "/canonical/intermediate.root";

    std::ostringstream writeErrors;
    {
        TFile candidate(replacement.string().c_str(), "CREATE");
        ASSERT_FALSE(candidate.IsZombie());
        provenance.resultUuid = candidate.GetUUID().AsString();
        ASSERT_TRUE(REST_LegacyRecovery::WriteRecoveryProvenance(candidate, provenance, writeErrors))
            << writeErrors.str();
        ASSERT_TRUE(REST_LegacyRecovery::CheckAndCloseOutputFile(candidate, writeErrors))
            << writeErrors.str();
    }

    REST_LegacyRecovery::CandidateExpectations expectations;
    expectations.provenance = provenance;
    const auto validateCandidate = [&expectations](const fs::path& path, std::ostream& errors) {
        return REST_LegacyRecovery::ValidateRecoveryCandidate(path, expectations, errors);
    };

    std::ostringstream errors;
    EXPECT_FALSE(REST_LegacyRecovery::ValidateAndReplaceFileWithBackup(replacement, original, backup, errors,
                                                                       validateCandidate));
    EXPECT_EQ(ReadText(original), "original");
    EXPECT_TRUE(fs::is_regular_file(replacement));
    EXPECT_FALSE(fs::exists(backup));
    EXPECT_NE(errors.str().find("no readable EventTree"), std::string::npos);
    EXPECT_NE(errors.str().find("were not touched"), std::string::npos);
}

TEST(LegacyRecoveryCandidateValidation, CopiesAndValidatesEveryAdditionalTopLevelTree) {
    TemporaryDirectory temporary;
    const auto sourcePath = temporary.path / "source.root";
    const auto candidatePath = temporary.path / "candidate.root";
    {
        TFile source(sourcePath.string().c_str(), "CREATE");
        ASSERT_FALSE(source.IsZombie());
        TTree customTree("CustomTree", "custom data");
        int value = 1;
        customTree.Branch("value", &value);
        customTree.Fill();
        ASSERT_GT(customTree.Write(), 0);
        value = 2;
        customTree.Fill();
        ASSERT_GT(customTree.Write(), 0);  // newest cycle has two entries

        TTree secondTree("SecondTree", "second custom tree");
        double measurement = 3.5;
        secondTree.Branch("measurement", &measurement);
        secondTree.Fill();
        ASSERT_GT(secondTree.Write(), 0);
        source.Close();
    }

    std::vector<REST_LegacyRecovery::AdditionalTreeExpectation> expectations;
    std::ostringstream errors;
    {
        TFile source(sourcePath.string().c_str(), "READ");
        TFile candidate(candidatePath.string().c_str(), "CREATE");
        ASSERT_TRUE(REST_LegacyRecovery::CopyAdditionalTopLevelTrees(source, candidate, expectations, errors))
            << errors.str();
        ASSERT_TRUE(REST_LegacyRecovery::CheckAndCloseOutputFile(candidate, errors)) << errors.str();
        source.Close();
    }

    ASSERT_EQ(expectations.size(), 2U);
    const auto custom = std::find_if(expectations.begin(), expectations.end(), [](const auto& expectation) {
        return expectation.keyName == "CustomTree";
    });
    ASSERT_NE(custom, expectations.end());
    EXPECT_EQ(custom->entries, 2);
    EXPECT_EQ(custom->branches, std::vector<std::string>({"value"}));

    {
        TFile candidate(candidatePath.string().c_str(), "READ");
        ASSERT_TRUE(REST_LegacyRecovery::ValidateAdditionalTopLevelTrees(candidate, expectations, errors))
            << errors.str();
    }

    auto wrongInventory = expectations;
    wrongInventory.front().branches.push_back("missing");
    {
        TFile candidate(candidatePath.string().c_str(), "READ");
        errors.str("");
        EXPECT_FALSE(REST_LegacyRecovery::ValidateAdditionalTopLevelTrees(candidate, wrongInventory, errors));
        EXPECT_NE(errors.str().find("branch inventory"), std::string::npos);
    }

    {
        TFile candidate(candidatePath.string().c_str(), "UPDATE");
        TTree unexpected("UnexpectedTree", "unexpected");
        int extra = 1;
        unexpected.Branch("extra", &extra);
        unexpected.Fill();
        ASSERT_GT(unexpected.Write(), 0);
        candidate.Close();
    }
    {
        TFile candidate(candidatePath.string().c_str(), "READ");
        errors.str("");
        EXPECT_FALSE(REST_LegacyRecovery::ValidateAdditionalTopLevelTrees(candidate, expectations, errors));
        EXPECT_NE(errors.str().find("inventory differs"), std::string::npos);
    }

    const auto originalPath = temporary.path / "in-place.root";
    const auto backupPath = temporary.path / "in-place.root.bak";
    WriteText(originalPath, "original");
    const auto validateAdditionalTrees = [&expectations](const fs::path& path,
                                                         std::ostream& validationErrors) {
        TFile candidate(path.string().c_str(), "READ");
        return REST_LegacyRecovery::ValidateAdditionalTopLevelTrees(candidate, expectations,
                                                                    validationErrors);
    };
    errors.str("");
    EXPECT_FALSE(REST_LegacyRecovery::ValidateAndReplaceFileWithBackup(
        candidatePath, originalPath, backupPath, errors, validateAdditionalTrees));
    EXPECT_EQ(ReadText(originalPath), "original");
    EXPECT_TRUE(fs::is_regular_file(candidatePath));
    EXPECT_FALSE(fs::exists(backupPath));
    EXPECT_NE(errors.str().find("were not touched"), std::string::npos);
}

}  // namespace
