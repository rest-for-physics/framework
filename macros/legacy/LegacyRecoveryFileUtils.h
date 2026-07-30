#ifndef REST_LEGACY_RECOVERY_FILE_UTILS_H
#define REST_LEGACY_RECOVERY_FILE_UTILS_H

#include <filesystem>
#include <functional>
#include <ostream>
#include <string>
#include <system_error>

namespace REST_LegacyRecovery {

namespace fs = std::filesystem;

struct PathComparison {
    bool ok = false;
    bool equivalent = false;
    std::string error;
};

inline fs::path BuildSiblingRootPath(const fs::path& input, const std::string& suffix) {
    fs::path output = input;
    const auto filename = output.filename();
    if (filename.extension() == ".root")
        output.replace_filename(filename.stem().string() + suffix + ".root");
    else
        output += suffix + ".root";
    return output;
}

inline bool ResolvePathIdentity(const fs::path& path, std::string& identity, std::string& error) {
    std::error_code absoluteError;
    const auto absolute = fs::absolute(path, absoluteError);
    if (absoluteError) {
        error = "cannot resolve '" + path.string() + "': " + absoluteError.message();
        return false;
    }

    std::error_code canonicalError;
    const auto canonical = fs::weakly_canonical(absolute, canonicalError);
    if (canonicalError) {
        error = "cannot resolve '" + path.string() + "': " + canonicalError.message();
        return false;
    }

    identity = canonical.string();
    return true;
}

inline PathComparison ComparePaths(const fs::path& first, const fs::path& second) {
    std::string firstIdentity;
    std::string firstError;
    if (!ResolvePathIdentity(first, firstIdentity, firstError)) return {false, false, firstError};

    std::string secondIdentity;
    std::string secondError;
    if (!ResolvePathIdentity(second, secondIdentity, secondError)) return {false, false, secondError};

    const fs::path firstPath(firstIdentity);
    const fs::path secondPath(secondIdentity);

    if (firstPath == secondPath) return {true, true, ""};

    std::error_code firstExistsError;
    const bool firstExists = fs::exists(firstPath, firstExistsError);
    if (firstExistsError) {
        return {false, false, "cannot inspect '" + first.string() + "': " + firstExistsError.message()};
    }

    std::error_code secondExistsError;
    const bool secondExists = fs::exists(secondPath, secondExistsError);
    if (secondExistsError) {
        return {false, false, "cannot inspect '" + second.string() + "': " + secondExistsError.message()};
    }

    if (firstExists && secondExists) {
        std::error_code equivalentError;
        const bool equivalent = fs::equivalent(firstPath, secondPath, equivalentError);
        if (equivalentError) {
            return {false, false,
                    "cannot compare '" + first.string() + "' and '" + second.string() +
                        "': " + equivalentError.message()};
        }
        if (equivalent) return {true, true, ""};
    }

    return {true, false, ""};
}

inline bool PathEntryExists(const fs::path& path, bool& exists, std::string& error) {
    std::error_code statusError;
    const auto status = fs::symlink_status(path, statusError);
    if (status.type() == fs::file_type::not_found) {
        exists = false;
        return true;
    }
    if (statusError) {
        error = "cannot inspect '" + path.string() + "': " + statusError.message();
        return false;
    }

    exists = true;
    return true;
}

inline bool ValidateUnusedPath(const fs::path& path, const std::string& description, std::ostream& errors) {
    bool pathExists = false;
    std::string existenceError;
    if (!PathEntryExists(path, pathExists, existenceError)) {
        errors << "ERROR: " << existenceError << '\n';
        return false;
    }
    if (pathExists) {
        errors << "ERROR: " << description << " already exists: " << path.string()
               << "\nChoose a different path or move the existing file first.\n";
        return false;
    }
    return true;
}

inline bool ValidateNewOutputPath(const fs::path& inputPath, const fs::path& outputPath,
                                  const std::string& description, std::ostream& errors) {
    const auto comparison = ComparePaths(inputPath, outputPath);
    if (!comparison.ok) {
        errors << "ERROR: " << comparison.error << '\n';
        return false;
    }
    if (comparison.equivalent) {
        errors << "ERROR: " << description << " resolves to the input file. Refusing to overwrite '"
               << inputPath.string() << "'.\n";
        return false;
    }

    return ValidateUnusedPath(outputPath, description, errors);
}

using RenameOperation = std::function<void(const fs::path&, const fs::path&, std::error_code&)>;
using CandidateValidation = std::function<bool(const fs::path&, std::ostream&)>;

inline void RenamePath(const fs::path& source, const fs::path& destination, std::error_code& error) {
    // Recovery operates on regular files in one directory. A hard link plus
    // unlink provides no-replace semantics on POSIX, unlike rename(), which
    // would silently overwrite a path created after the preflight check.
    fs::create_hard_link(source, destination, error);
    if (error) return;

    std::error_code removeError;
    if (fs::remove(source, removeError)) return;

    std::error_code rollbackError;
    fs::remove(destination, rollbackError);
    error = removeError ? removeError : std::make_error_code(std::errc::io_error);
}

inline bool ReplaceFileWithBackup(const fs::path& replacementPath, const fs::path& originalPath,
                                  const fs::path& backupPath, std::ostream& errors,
                                  const RenameOperation& renameOperation = RenamePath) {
    const auto replacementComparison = ComparePaths(replacementPath, originalPath);
    if (!replacementComparison.ok) {
        errors << "ERROR: " << replacementComparison.error << '\n';
        return false;
    }
    if (replacementComparison.equivalent) {
        errors << "ERROR: replacement and original resolve to the same path: " << originalPath.string()
               << '\n';
        return false;
    }

    bool backupExists = false;
    std::string existenceError;
    if (!PathEntryExists(backupPath, backupExists, existenceError)) {
        errors << "ERROR: " << existenceError << '\n';
        return false;
    }
    if (backupExists) {
        errors << "ERROR: backup path already exists: " << backupPath.string()
               << "\nRefusing to overwrite it. Move or rename that backup and retry.\n";
        return false;
    }

    std::error_code backupError;
    renameOperation(originalPath, backupPath, backupError);
    if (backupError) {
        errors << "ERROR: could not move original file '" << originalPath.string() << "' to backup '"
               << backupPath.string() << "': " << backupError.message() << '\n';
        return false;
    }

    std::error_code replacementError;
    renameOperation(replacementPath, originalPath, replacementError);
    if (!replacementError) return true;

    errors << "ERROR: original was backed up, but moving fixed file '" << replacementPath.string()
           << "' into place failed: " << replacementError.message() << '\n';

    std::error_code rollbackError;
    renameOperation(backupPath, originalPath, rollbackError);
    if (!rollbackError) {
        errors << "Rollback succeeded: the original file was restored. The fixed file remains at '"
               << replacementPath.string() << "'.\n";
    } else {
        errors << "CRITICAL: rollback also failed: " << rollbackError.message()
               << "\nThe original remains at '" << backupPath.string() << "' and the fixed file remains at '"
               << replacementPath.string() << "'.\n";
    }
    return false;
}

inline bool ValidateAndReplaceFileWithBackup(const fs::path& replacementPath, const fs::path& originalPath,
                                             const fs::path& backupPath, std::ostream& errors,
                                             const CandidateValidation& validateCandidate,
                                             const RenameOperation& renameOperation = RenamePath) {
    if (!validateCandidate(replacementPath, errors)) {
        errors << "ERROR: candidate validation failed. The original file and any existing backup "
                  "were not touched.\n";
        return false;
    }
    return ReplaceFileWithBackup(replacementPath, originalPath, backupPath, errors, renameOperation);
}

}  // namespace REST_LegacyRecovery

#endif
