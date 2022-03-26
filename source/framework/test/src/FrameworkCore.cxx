
#include <TRestRun.h>
#include <TRestMetadata.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

#define FILES_PATH fs::path(__FILE__).parent_path().parent_path() / "files"
#define BASIC_TRESTRUN_RML FILES_PATH / "TRestRunBasic.rml"
#define BASIC_TRESTMETADATA_RML FILES_PATH / "metadata.rml"

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

TEST(FrameworkCore, TRestMetadata) {
    // Check required files exist
    EXPECT_TRUE(fs::exists(BASIC_TRESTMETADATA_RML));
    
    // Create new TRestMetadata class
    class metadataTestClass: public TRestMetadata{
        int fP1;
        double fP2;
        string fP3;
    };
    
    metadataTestClass meta;
    meta.LoadConfigFromFile(BASIC_TRESTMETADATA_RML, "metadata");

    meta.PrintMetadata();
    //cout << meta.GetParameter("p1") << endl;

    EXPECT_TRUE(meta.GetParameter("p1") == "75");
    EXPECT_TRUE(meta.GetParameter("p2") == "12.32");
    EXPECT_TRUE(meta.GetParameter("p3") == "Aloha");
    
}
