#include <TBranch.h>
#include <TRestDetectorSignalEvent.h>
#include <TRestRawSignalEvent.h>
#include <TRestRun.h>
#include <TSystem.h>
#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <string>

namespace {

namespace fs = std::filesystem;

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

}  // namespace
