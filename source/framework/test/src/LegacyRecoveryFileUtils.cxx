#include "../../../../macros/legacy/LegacyRecoveryFileUtils.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>

namespace {

namespace fs = std::filesystem;
using REST_LegacyRecovery::ComparePaths;
using REST_LegacyRecovery::ReplaceFileWithBackup;
using REST_LegacyRecovery::ValidateNewOutputPath;

class TemporaryDirectory {
   public:
    TemporaryDirectory() {
        static std::atomic<unsigned long> sequence{0};
        const auto suffix =
            std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count()) + "_" +
            std::to_string(sequence++);
        path = fs::temp_directory_path() / ("rest_legacy_recovery_test_" + suffix);
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

TEST(LegacyRecoveryFileUtils, RejectsEquivalentAndExistingOutputPaths) {
    TemporaryDirectory temporary;
    const auto input = temporary.path / "input.root";
    const auto existingOutput = temporary.path / "existing.root";
    const auto newOutput = temporary.path / "new.root";
    const auto nested = temporary.path / "nested";
    fs::create_directory(nested);
    WriteText(input, "input");
    WriteText(existingOutput, "existing");

    std::ostringstream errors;
    EXPECT_FALSE(ValidateNewOutputPath(input, input, "output", errors));

    errors.str("");
    EXPECT_FALSE(ValidateNewOutputPath(input, nested / ".." / "input.root", "output", errors));

    std::error_code symlinkError;
    const auto symlink = temporary.path / "input-link.root";
    fs::create_symlink(input, symlink, symlinkError);
    if (!symlinkError) {
        errors.str("");
        EXPECT_FALSE(ValidateNewOutputPath(input, symlink, "output", errors));
    }

    std::error_code hardLinkError;
    const auto hardLink = temporary.path / "input-hard-link.root";
    fs::create_hard_link(input, hardLink, hardLinkError);
    if (!hardLinkError) {
        errors.str("");
        EXPECT_FALSE(ValidateNewOutputPath(input, hardLink, "output", errors));
    }

    errors.str("");
    EXPECT_FALSE(ValidateNewOutputPath(input, existingOutput, "output", errors));
    EXPECT_EQ(ReadText(existingOutput), "existing");

    errors.str("");
    EXPECT_TRUE(ValidateNewOutputPath(input, newOutput, "output", errors));
    EXPECT_FALSE(fs::exists(newOutput));
}

TEST(LegacyRecoveryFileUtils, ReplacesOriginalAndKeepsBackup) {
    TemporaryDirectory temporary;
    const auto original = temporary.path / "input.root";
    const auto replacement = temporary.path / "fixed.root";
    const auto backup = temporary.path / "input.root.bak";
    WriteText(original, "original");
    WriteText(replacement, "fixed");

    std::ostringstream errors;
    EXPECT_TRUE(ReplaceFileWithBackup(replacement, original, backup, errors));
    EXPECT_EQ(ReadText(original), "fixed");
    EXPECT_EQ(ReadText(backup), "original");
    EXPECT_FALSE(fs::exists(replacement));
    EXPECT_TRUE(errors.str().empty());
}

TEST(LegacyRecoveryFileUtils, RefusesExistingBackupWithoutChangingFiles) {
    TemporaryDirectory temporary;
    const auto original = temporary.path / "input.root";
    const auto replacement = temporary.path / "fixed.root";
    const auto backup = temporary.path / "input.root.bak";
    WriteText(original, "original");
    WriteText(replacement, "fixed");
    WriteText(backup, "previous backup");

    std::ostringstream errors;
    EXPECT_FALSE(ReplaceFileWithBackup(replacement, original, backup, errors));
    EXPECT_EQ(ReadText(original), "original");
    EXPECT_EQ(ReadText(replacement), "fixed");
    EXPECT_EQ(ReadText(backup), "previous backup");
    EXPECT_NE(errors.str().find("Refusing to overwrite"), std::string::npos);
}

TEST(LegacyRecoveryFileUtils, PreservesFilesWhenBackupMoveFails) {
    TemporaryDirectory temporary;
    const auto original = temporary.path / "input.root";
    const auto replacement = temporary.path / "fixed.root";
    const auto backup = temporary.path / "input.root.bak";
    WriteText(original, "original");
    WriteText(replacement, "fixed");

    int renameCalls = 0;
    const auto failBackup = [&renameCalls](const fs::path&, const fs::path&, std::error_code& error) {
        renameCalls++;
        error = std::make_error_code(std::errc::permission_denied);
    };

    std::ostringstream errors;
    EXPECT_FALSE(ReplaceFileWithBackup(replacement, original, backup, errors, failBackup));
    EXPECT_EQ(renameCalls, 1);
    EXPECT_EQ(ReadText(original), "original");
    EXPECT_EQ(ReadText(replacement), "fixed");
    EXPECT_FALSE(fs::exists(backup));
    EXPECT_NE(errors.str().find("could not move original"), std::string::npos);
}

TEST(LegacyRecoveryFileUtils, RestoresOriginalWhenReplacementMoveFails) {
    TemporaryDirectory temporary;
    const auto original = temporary.path / "input.root";
    const auto replacement = temporary.path / "fixed.root";
    const auto backup = temporary.path / "input.root.bak";
    WriteText(original, "original");
    WriteText(replacement, "fixed");

    int renameCalls = 0;
    const auto failReplacement = [&renameCalls](const fs::path& source, const fs::path& destination,
                                                std::error_code& error) {
        renameCalls++;
        if (renameCalls == 2) {
            error = std::make_error_code(std::errc::permission_denied);
            return;
        }
        fs::rename(source, destination, error);
    };

    std::ostringstream errors;
    EXPECT_FALSE(ReplaceFileWithBackup(replacement, original, backup, errors, failReplacement));
    EXPECT_EQ(renameCalls, 3);
    EXPECT_EQ(ReadText(original), "original");
    EXPECT_EQ(ReadText(replacement), "fixed");
    EXPECT_FALSE(fs::exists(backup));
    EXPECT_NE(errors.str().find("Rollback succeeded"), std::string::npos);
}

TEST(LegacyRecoveryFileUtils, ReportsLocationsWhenReplacementAndRollbackFail) {
    TemporaryDirectory temporary;
    const auto original = temporary.path / "input.root";
    const auto replacement = temporary.path / "fixed.root";
    const auto backup = temporary.path / "input.root.bak";
    WriteText(original, "original");
    WriteText(replacement, "fixed");

    int renameCalls = 0;
    const auto failReplacementAndRollback =
        [&renameCalls](const fs::path& source, const fs::path& destination, std::error_code& error) {
            renameCalls++;
            if (renameCalls > 1) {
                error = std::make_error_code(std::errc::permission_denied);
                return;
            }
            fs::rename(source, destination, error);
        };

    std::ostringstream errors;
    EXPECT_FALSE(ReplaceFileWithBackup(replacement, original, backup, errors, failReplacementAndRollback));
    EXPECT_EQ(renameCalls, 3);
    EXPECT_FALSE(fs::exists(original));
    EXPECT_EQ(ReadText(backup), "original");
    EXPECT_EQ(ReadText(replacement), "fixed");
    EXPECT_NE(errors.str().find("CRITICAL"), std::string::npos);
    EXPECT_NE(errors.str().find(backup.string()), std::string::npos);
    EXPECT_NE(errors.str().find(replacement.string()), std::string::npos);
}

TEST(LegacyRecoveryFileUtils, ComparesNonexistentDestinationsWithoutThrowing) {
    TemporaryDirectory temporary;
    const auto first = temporary.path / "does-not-exist.root";
    const auto second = temporary.path / "also-does-not-exist.root";

    const auto comparison = ComparePaths(first, second);
    EXPECT_TRUE(comparison.ok);
    EXPECT_FALSE(comparison.equivalent);
    EXPECT_TRUE(comparison.error.empty());
}

}  // namespace
