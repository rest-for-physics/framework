#include <TFile.h>
#include <TNamed.h>
#include <TUUID.h>
#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace {
std::vector<char> Bytes(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

class RootIOConflict : public ::testing::Test {
   protected:
    std::filesystem::path directory;

    void SetUp() override {
        directory =
            std::filesystem::temp_directory_path() / (std::string("rest-conflict-") + TUUID().AsString());
        ASSERT_TRUE(std::filesystem::create_directory(directory));
    }

    void TearDown() override { std::filesystem::remove_all(directory); }

    int Run(const char* executable, const std::vector<std::filesystem::path>& arguments) {
        auto quote = [](const std::string& value) {
            std::string result = "'";
            for (const char c : value) result += c == '\'' ? "'\\''" : std::string(1, c);
            return result + "'";
        };
        std::string command = quote(executable);
        for (const auto& argument : arguments) command += " " + quote(argument.string());
        return std::system(command.c_str());
    }
};
}  // namespace

TEST_F(RootIOConflict, RejectUpdateWithoutChangingHistoricalSchemaOrBytes) {
    const auto file = directory / "old.root";
    ASSERT_EQ(Run(REST_IO_CONFLICT_Old, {file}), 0);
    const auto before = Bytes(file);
    ASSERT_FALSE(before.empty());
    EXPECT_EQ(Run(REST_IO_CONFLICT_New, {file}), 0);
    EXPECT_EQ(Bytes(file), before);
}

TEST_F(RootIOConflict, RejectWorkerSchemaConflictWithoutReplacingTargetOrRemovingInput) {
    const auto source = directory / "old.root";
    const auto target = directory / "target.root";
    ASSERT_EQ(Run(REST_IO_CONFLICT_Old, {source}), 0);
    {
        TFile output(target.c_str(), "CREATE");
        TNamed marker("marker", "original target");
        marker.Write();
    }
    const auto sourceBefore = Bytes(source);
    const auto targetBefore = Bytes(target);
    EXPECT_EQ(Run(REST_IO_CONFLICT_New, {source, target}), 0);
    EXPECT_EQ(Bytes(source), sourceBefore);
    EXPECT_EQ(Bytes(target), targetBefore);
}
