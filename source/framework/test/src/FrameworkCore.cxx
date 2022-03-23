
#include <TRestRun.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

#define FILES_PATH fs::path(__FILE__).parent_path().parent_path() / "files"

TEST(FrameworkCore, TestFiles) {
    cout << "FrameworkCore test files path: " << FILES_PATH << endl;

    // Check dir exists and is a directory
    EXPECT_TRUE(fs::is_directory(FILES_PATH));
    // Check it's not empty
    EXPECT_TRUE(!fs::is_empty(FILES_PATH));
}

TEST(FrameworkCore, TRestRun) {
    GTEST_SKIP();

    const auto basicRunRml = FILES_PATH / "TRestRunBasic.rml";

    auto run = TRestRun(basicRunRml);

    run.PrintAllMetadata();

    EXPECT_TRUE(run.GetExperimentName() == "TRestRun Basic Test");

    EXPECT_TRUE(run.GetRunType() == "Test");

    EXPECT_TRUE(run.GetRunNumber() == 1);

    EXPECT_TRUE(run.GetRunTag() == "Test");

    // EXPECT_TRUE(run.GetOutputFileName() == "Run1_Test_[fExperimentName].root");

    EXPECT_TRUE(run.GetRunDescription() == "This is a test for TRestRun");

    // EXPECT_TRUE(run.GetVerboseLevel() == 3);
}
