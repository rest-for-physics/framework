
#include <TRestMetadata.h>
#include <TRestRun.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

const auto filesPath = fs::path(__FILE__).parent_path().parent_path() / "files";
const auto basicRunRml = filesPath / "TRestRunBasic.rml";
const auto basicMetadataRml = filesPath / "TRestMetadataTest.rml";

TEST(FrameworkCore, TestFiles) {
    cout << "FrameworkCore test files path: " << filesPath << endl;

    // Check dir exists and is a directory
    EXPECT_TRUE(fs::is_directory(filesPath));
    // Check it's not empty
    EXPECT_TRUE(!fs::is_empty(filesPath));
    // Check required files exist
    EXPECT_TRUE(fs::exists(basicRunRml));
    EXPECT_TRUE(fs::exists(basicMetadataRml));
}

TEST(FrameworkCore, TRestRun) {
    TRestRun run;

    run.LoadConfigFromFile(basicRunRml);

    run.PrintAllMetadata();

    EXPECT_TRUE(run.GetExperimentName() == "TRestRun Basic Test");
    EXPECT_TRUE(run.GetRunType() == "Test");
    EXPECT_TRUE(run.GetRunNumber() == 1);
    EXPECT_TRUE(run.GetRunTag() == "Test");
    // EXPECT_TRUE(run.GetOutputFileName() == "Run[fRunNumber]_[fRunTag]_[fExperimentName].root");
    EXPECT_TRUE(run.GetRunDescription() == "This is a test for TRestRun");
    EXPECT_TRUE(run.GetVerboseLevelString() == "debug");
}

TEST(FrameworkCore, TRestMetadata) {
    // Create new TRestMetadata class
    class TRestMetadataTest : public TRestMetadata {
        int fP1;
        double fP2;
        string fP3;
    };

    TRestMetadataTest restMetadataTest;
    restMetadataTest.LoadConfigFromFile(basicMetadataRml, "TRestMetadataTest");

    restMetadataTest.PrintMetadata();

    EXPECT_TRUE(restMetadataTest.GetParameter("p1") == "75");
    EXPECT_TRUE(restMetadataTest.GetParameter("p2") == "12.32");
    EXPECT_TRUE(restMetadataTest.GetParameter("p3") == "Aloha");
}
