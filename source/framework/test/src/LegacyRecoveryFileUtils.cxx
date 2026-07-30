#include "../../../../macros/legacy/LegacyRecoveryFileUtils.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "../../../../macros/legacy/LegacyRecoveryDataUtils.h"
#include "../../../../macros/legacy/LegacyRecoveryProvenance.h"

namespace {

namespace fs = std::filesystem;
using REST_LegacyRecovery::BuildSiblingRootPath;
using REST_LegacyRecovery::ComparePaths;
using REST_LegacyRecovery::ParseInteger;
using REST_LegacyRecovery::RecoveryProvenance;
using REST_LegacyRecovery::RenamePath;
using REST_LegacyRecovery::ReplaceFileWithBackup;
using REST_LegacyRecovery::ResolvePathIdentity;
using REST_LegacyRecovery::SourceIdentity;
using REST_LegacyRecovery::ValidateAndReplaceFileWithBackup;
using REST_LegacyRecovery::ValidateFlattenedSignalData;
using REST_LegacyRecovery::ValidateIntermediateSource;
using REST_LegacyRecovery::ValidateNewOutputPath;
using REST_LegacyRecovery::ValidateSupportedLegacySchema;

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

TEST(LegacyRecoveryFileUtils, BuildsSiblingNamesFromTheFilenameExtensionOnly) {
    EXPECT_EQ(BuildSiblingRootPath("/data/file.root", "_Fixed"), "/data/file_Fixed.root");
    EXPECT_EQ(BuildSiblingRootPath("/data.root/file", "_Fixed"), "/data.root/file_Fixed.root");
    EXPECT_EQ(BuildSiblingRootPath("/data/file.root.backup", "_Fixed"), "/data/file.root.backup_Fixed.root");
    EXPECT_EQ(BuildSiblingRootPath("relative.root", "_LegacySignalData"), "relative_LegacySignalData.root");
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

TEST(LegacyRecoveryFileUtils, RenameDoesNotOverwriteDestinationCreatedAfterPreflight) {
    TemporaryDirectory temporary;
    const auto source = temporary.path / "source.root";
    const auto destination = temporary.path / "destination.root";
    WriteText(source, "source");
    WriteText(destination, "racer");

    std::error_code error;
    RenamePath(source, destination, error);

    EXPECT_TRUE(error);
    EXPECT_EQ(ReadText(source), "source");
    EXPECT_EQ(ReadText(destination), "racer");
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

TEST(LegacyRecoveryFileUtils, ResolvesSymlinkSpellingsToTheSameSourceIdentity) {
    TemporaryDirectory temporary;
    const auto input = temporary.path / "input.root";
    const auto symlink = temporary.path / "input-link.root";
    WriteText(input, "input");

    std::error_code symlinkError;
    fs::create_symlink(input, symlink, symlinkError);
    if (symlinkError) GTEST_SKIP() << "Cannot create symlink: " << symlinkError.message();

    std::string inputIdentity;
    std::string symlinkIdentity;
    std::string error;
    ASSERT_TRUE(ResolvePathIdentity(input, inputIdentity, error)) << error;
    ASSERT_TRUE(ResolvePathIdentity(symlink, symlinkIdentity, error)) << error;
    EXPECT_EQ(inputIdentity, symlinkIdentity);
}

TEST(LegacyRecoveryDataUtils, AcceptsOnlyKnownLegacySchemaVersions) {
    std::ostringstream errors;
    EXPECT_TRUE(ValidateSupportedLegacySchema({1, 1}, errors, "test"));
    EXPECT_TRUE(ValidateSupportedLegacySchema({2, 2}, errors, "test"));
    EXPECT_TRUE(ValidateSupportedLegacySchema({3, 3}, errors, "test"));

    for (const auto versions :
         {REST_LegacyRecovery::SignalSchemaVersions{-1, -1}, REST_LegacyRecovery::SignalSchemaVersions{0, 0},
          REST_LegacyRecovery::SignalSchemaVersions{4, 4}, REST_LegacyRecovery::SignalSchemaVersions{2, 3}}) {
        errors.str("");
        EXPECT_FALSE(ValidateSupportedLegacySchema(versions, errors, "test"));
        EXPECT_FALSE(errors.str().empty());
    }
}

TEST(LegacyRecoveryDataUtils, RejectsMalformedFlattenedSignalArrays) {
    const std::vector<int> ids{7, 9};
    const std::vector<int> counts{2, 1};
    const std::vector<float> times{1.F, 2.F, 3.F};
    const std::vector<float> charges{4.F, 5.F, 6.F};
    std::uint64_t points = 0;
    std::ostringstream errors;

    EXPECT_TRUE(ValidateFlattenedSignalData(&ids, &counts, &times, &charges, points, errors, "test"));
    EXPECT_EQ(points, 3U);

    const std::vector<int> tooFewCounts{3};
    errors.str("");
    EXPECT_FALSE(ValidateFlattenedSignalData(&ids, &tooFewCounts, &times, &charges, points, errors, "test"));

    const std::vector<float> tooFewCharges{4.F, 5.F};
    errors.str("");
    EXPECT_FALSE(ValidateFlattenedSignalData(&ids, &counts, &times, &tooFewCharges, points, errors, "test"));

    const std::vector<float> nonFiniteTimes{1.F, std::numeric_limits<float>::infinity(), 3.F};
    errors.str("");
    EXPECT_FALSE(
        ValidateFlattenedSignalData(&ids, &counts, &nonFiniteTimes, &charges, points, errors, "test"));

    const std::vector<int> negativeCounts{2, -1};
    errors.str("");
    EXPECT_FALSE(
        ValidateFlattenedSignalData(&ids, &negativeCounts, &times, &charges, points, errors, "test"));

    const std::vector<int> countsTooLarge{2, 2};
    errors.str("");
    EXPECT_FALSE(
        ValidateFlattenedSignalData(&ids, &countsTooLarge, &times, &charges, points, errors, "test"));

    const std::vector<int> countsTooSmall{1, 1};
    errors.str("");
    EXPECT_FALSE(
        ValidateFlattenedSignalData(&ids, &countsTooSmall, &times, &charges, points, errors, "test"));

    const std::vector<int>* nullIds = nullptr;
    errors.str("");
    EXPECT_FALSE(ValidateFlattenedSignalData(nullIds, &counts, &times, &charges, points, errors, "test"));
}

TEST(LegacyRecoveryDataUtils, RejectsMismatchedIntermediateProvenance) {
    SourceIdentity source;
    source.uuid = "source-uuid";
    source.normalizedPath = "/canonical/input.root";
    source.fileSize = 1234;
    source.entries = 17;
    source.signalVersion = 3;

    RecoveryProvenance provenance;
    provenance.formatVersion = REST_LegacyRecovery::kRecoveryFormatVersion;
    provenance.kind = REST_LegacyRecovery::kIntermediateProvenanceKind;
    provenance.source = source;
    provenance.intermediateUuid = "intermediate-uuid";

    std::ostringstream errors;
    EXPECT_TRUE(ValidateIntermediateSource(provenance, source, provenance.intermediateUuid, errors));

    const auto expectRejected = [&](const RecoveryProvenance& changed) {
        errors.str("");
        EXPECT_FALSE(ValidateIntermediateSource(changed, source, "intermediate-uuid", errors));
        EXPECT_FALSE(errors.str().empty());
    };

    auto changed = provenance;
    changed.formatVersion++;
    expectRejected(changed);
    changed = provenance;
    changed.kind = REST_LegacyRecovery::kResultProvenanceKind;
    expectRejected(changed);
    changed = provenance;
    changed.source.uuid = "wrong-source";
    expectRejected(changed);
    changed = provenance;
    changed.source.normalizedPath = "/different/input.root";
    expectRejected(changed);
    changed = provenance;
    changed.source.fileSize++;
    expectRejected(changed);
    changed = provenance;
    changed.source.entries++;
    expectRejected(changed);
    changed = provenance;
    changed.source.signalVersion--;
    expectRejected(changed);
    changed = provenance;
    changed.intermediateUuid = "wrong-intermediate";
    expectRejected(changed);
}

TEST(LegacyRecoveryProvenance, ParsesOnlyCompleteAndRepresentableIntegers) {
    std::uint64_t unsignedValue = 0;
    EXPECT_TRUE(ParseInteger("42", unsignedValue));
    EXPECT_EQ(unsignedValue, 42U);
    EXPECT_FALSE(ParseInteger("", unsignedValue));
    EXPECT_FALSE(ParseInteger("-1", unsignedValue));
    EXPECT_FALSE(ParseInteger("42x", unsignedValue));
    EXPECT_FALSE(ParseInteger("18446744073709551616", unsignedValue));

    int signedValue = 0;
    EXPECT_TRUE(ParseInteger("-3", signedValue));
    EXPECT_EQ(signedValue, -3);
    EXPECT_FALSE(ParseInteger(" 3", signedValue));
    EXPECT_FALSE(ParseInteger("2147483648", signedValue));
}

TEST(LegacyRecoveryFileUtils, FailedCandidateValidationLeavesAllFilesUntouched) {
    TemporaryDirectory temporary;
    const auto original = temporary.path / "input.root";
    const auto replacement = temporary.path / "fixed.root";
    const auto backup = temporary.path / "input.root.bak";
    WriteText(original, "original");
    WriteText(replacement, "invalid candidate");
    WriteText(backup, "previous backup");

    int validationCalls = 0;
    int renameCalls = 0;
    const auto rejectCandidate = [&validationCalls](const fs::path& path, std::ostream& errors) {
        validationCalls++;
        errors << "candidate rejected: " << path.string() << '\n';
        return false;
    };
    const auto countRename = [&renameCalls](const fs::path& source, const fs::path& destination,
                                            std::error_code& error) {
        renameCalls++;
        fs::rename(source, destination, error);
    };

    std::ostringstream errors;
    EXPECT_FALSE(ValidateAndReplaceFileWithBackup(replacement, original, backup, errors, rejectCandidate,
                                                  countRename));
    EXPECT_EQ(validationCalls, 1);
    EXPECT_EQ(renameCalls, 0);
    EXPECT_EQ(ReadText(original), "original");
    EXPECT_EQ(ReadText(replacement), "invalid candidate");
    EXPECT_EQ(ReadText(backup), "previous backup");
    EXPECT_NE(errors.str().find("were not touched"), std::string::npos);
}

}  // namespace
