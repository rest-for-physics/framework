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

inline PathComparison ComparePaths(const fs::path& first, const fs::path& second) {
    std::error_code firstError;
    const auto firstAbsolute = fs::absolute(first, firstError);
    if (firstError) {
        return {false, false, "cannot resolve '" + first.string() + "': " + firstError.message()};
    }
    const auto firstPath = fs::weakly_canonical(firstAbsolute, firstError);
    if (firstError) {
        return {false, false, "cannot resolve '" + first.string() + "': " + firstError.message()};
    }

    std::error_code secondError;
    const auto secondAbsolute = fs::absolute(second, secondError);
    if (secondError) {
        return {false, false, "cannot resolve '" + second.string() + "': " + secondError.message()};
    }
    const auto secondPath = fs::weakly_canonical(secondAbsolute, secondError);
    if (secondError) {
        return {false, false, "cannot resolve '" + second.string() + "': " + secondError.message()};
    }

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

inline void RenamePath(const fs::path& source, const fs::path& destination, std::error_code& error) {
    fs::rename(source, destination, error);
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

}  // namespace REST_LegacyRecovery

#endif
