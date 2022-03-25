
#include <TRestRun.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

#define FILES_PATH fs::path(__FILE__).parent_path().parent_path() / "files"
#define BASIC_TRESTRUN_RML FILES_PATH / "TRestRunBasic.rml"

TEST(FrameworkCore, TestFiles) {
    cout << "FrameworkCore test files path: " << FILES_PATH << endl;

    // Check dir exists and is a directory
    EXPECT_TRUE(fs::is_directory(FILES_PATH));
    // Check it's not empty
    EXPECT_TRUE(!fs::is_empty(FILES_PATH));
    // Check required files exist
    EXPECT_TRUE(fs::exists(BASIC_TRESTRUN_RML));
}

TEST(FrameworkCore, TRestRun) {
    TRestRun run;

    run.LoadConfigFromFile(BASIC_TRESTRUN_RML);

    run.PrintAllMetadata();

    EXPECT_TRUE(run.GetExperimentName() == "TRestRun Basic Test");
    EXPECT_TRUE(run.GetRunType() == "Test");
    EXPECT_TRUE(run.GetRunNumber() == 1);
    EXPECT_TRUE(run.GetRunTag() == "Test");
    // EXPECT_TRUE(run.GetOutputFileName() == "Run[fRunNumber]_[fRunTag]_[fExperimentName].root");
    EXPECT_TRUE(run.GetRunDescription() == "This is a test for TRestRun");
    EXPECT_TRUE(run.GetVerboseLevelString() == "debug");
}
