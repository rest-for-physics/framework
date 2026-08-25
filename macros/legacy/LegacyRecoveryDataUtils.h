#ifndef REST_LEGACY_RECOVERY_DATA_UTILS_H
#define REST_LEGACY_RECOVERY_DATA_UTILS_H

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <ostream>
#include <string>
#include <type_traits>

namespace REST_LegacyRecovery {

inline constexpr int kRecoveryFormatVersion = 2;
inline constexpr const char* kIntermediateProvenanceKind = "legacy-signal-data";
inline constexpr const char* kResultProvenanceKind = "rebuilt-rest-file";

struct SignalSchemaVersions {
    int time = -1;
    int charge = -1;
};

struct RecoveryCounts {
    std::uint64_t entries = 0;
    std::uint64_t signals = 0;
    std::uint64_t points = 0;
};

struct SourceIdentity {
    std::string uuid;
    std::string normalizedPath;
    std::uint64_t fileSize = 0;
    std::uint64_t entries = 0;
    int signalVersion = -1;
};

struct RecoveryProvenance {
    int formatVersion = -1;
    std::string kind;
    SourceIdentity source;
    RecoveryCounts recovered;
    std::string intermediateUuid;
    std::string intermediatePath;
    std::string resultUuid;
};

inline bool ValidateSupportedLegacySchema(const SignalSchemaVersions& versions, std::ostream& errors,
                                          const std::string& context) {
    if (versions.time != versions.charge) {
        errors << "ERROR: " << context << " has inconsistent signal schema versions: time=" << versions.time
               << ", charge=" << versions.charge << ".\n";
        return false;
    }
    if (versions.time < 1 || versions.time > 3) {
        errors << "ERROR: " << context << " uses unsupported or unknown TRestDetectorSignal schema v"
               << versions.time << "; only legacy versions 1..3 are recoverable.\n";
        return false;
    }
    return true;
}

inline bool ValidateCurrentSignalSchema(const SignalSchemaVersions& versions, std::ostream& errors,
                                        const std::string& context) {
    if (versions.time != versions.charge) {
        errors << "ERROR: " << context << " has inconsistent signal schema versions: time=" << versions.time
               << ", charge=" << versions.charge << ".\n";
        return false;
    }
    if (versions.time < 4) {
        errors << "ERROR: " << context << " does not contain the current vector<double> signal schema"
               << " (detected version " << versions.time << ").\n";
        return false;
    }
    return true;
}

inline bool CheckedAdd(std::uint64_t& total, std::uint64_t increment, std::ostream& errors,
                       const std::string& description) {
    if (increment > std::numeric_limits<std::uint64_t>::max() - total) {
        errors << "ERROR: overflow while counting " << description << ".\n";
        return false;
    }
    total += increment;
    return true;
}

template <typename IdContainer, typename CountContainer, typename TimeContainer, typename ChargeContainer>
bool ValidateFlattenedSignalData(const IdContainer* signalIds, const CountContainer* pointCounts,
                                 const TimeContainer* times, const ChargeContainer* charges,
                                 std::uint64_t& points, std::ostream& errors, const std::string& context) {
    points = 0;
    if (signalIds == nullptr || pointCounts == nullptr || times == nullptr || charges == nullptr) {
        errors << "ERROR: " << context << " has a null required array pointer.\n";
        return false;
    }
    if (signalIds->size() != pointCounts->size()) {
        errors << "ERROR: " << context << " has " << signalIds->size() << " signal IDs but "
               << pointCounts->size() << " point counts.\n";
        return false;
    }
    if (times->size() != charges->size()) {
        errors << "ERROR: " << context << " has " << times->size() << " times but " << charges->size()
               << " charges.\n";
        return false;
    }
    for (std::size_t point = 0; point < times->size(); ++point) {
        if (!std::isfinite(static_cast<long double>(times->at(point))) ||
            !std::isfinite(static_cast<long double>(charges->at(point)))) {
            errors << "ERROR: " << context << " has a non-finite time or charge at point " << point << ".\n";
            return false;
        }
    }

    for (std::size_t signal = 0; signal < pointCounts->size(); ++signal) {
        const auto value = pointCounts->at(signal);
        using Count = std::decay_t<decltype(value)>;
        if constexpr (std::is_signed_v<Count>) {
            if (value < 0) {
                errors << "ERROR: " << context << " has a negative point count for signal " << signal
                       << ".\n";
                return false;
            }
        }

        const auto count = static_cast<std::uint64_t>(value);
        if (!CheckedAdd(points, count, errors, context + " points")) return false;
        if (points > times->size()) {
            errors << "ERROR: " << context << " point counts exceed the flattened arrays at signal " << signal
                   << ".\n";
            return false;
        }
    }

    if (points != times->size()) {
        errors << "ERROR: " << context << " point counts sum to " << points
               << " but the flattened arrays contain " << times->size() << " values.\n";
        return false;
    }
    return true;
}

inline bool ValidateIntermediateSource(const RecoveryProvenance& provenance,
                                       const SourceIdentity& expectedSource,
                                       const std::string& actualIntermediateUuid, std::ostream& errors) {
    bool valid = true;
    if (provenance.formatVersion != kRecoveryFormatVersion) {
        errors << "ERROR: unsupported recovery provenance format " << provenance.formatVersion
               << " (expected " << kRecoveryFormatVersion << ").\n";
        valid = false;
    }
    if (provenance.kind != kIntermediateProvenanceKind) {
        errors << "ERROR: recovery provenance kind is '" << provenance.kind << "', expected '"
               << kIntermediateProvenanceKind << "'.\n";
        valid = false;
    }
    if (provenance.source.uuid != expectedSource.uuid) {
        errors << "ERROR: intermediate source UUID does not match the requested source file.\n";
        valid = false;
    }
    if (provenance.source.normalizedPath != expectedSource.normalizedPath) {
        errors << "ERROR: intermediate source path '" << provenance.source.normalizedPath
               << "' does not match requested source path '" << expectedSource.normalizedPath << "'.\n";
        valid = false;
    }
    if (provenance.source.fileSize != expectedSource.fileSize) {
        errors << "ERROR: intermediate source size " << provenance.source.fileSize
               << " does not match requested source size " << expectedSource.fileSize << ".\n";
        valid = false;
    }
    if (provenance.source.entries != expectedSource.entries) {
        errors << "ERROR: intermediate source entry count " << provenance.source.entries
               << " does not match requested source entry count " << expectedSource.entries << ".\n";
        valid = false;
    }
    if (provenance.source.signalVersion != expectedSource.signalVersion) {
        errors << "ERROR: intermediate legacy signal version " << provenance.source.signalVersion
               << " does not match requested source version " << expectedSource.signalVersion << ".\n";
        valid = false;
    }
    if (provenance.intermediateUuid != actualIntermediateUuid) {
        errors << "ERROR: embedded intermediate UUID does not match the opened intermediate file.\n";
        valid = false;
    }
    return valid;
}

inline bool ValidateRecoveredCounts(const RecoveryProvenance& provenance, const RecoveryCounts& actual,
                                    std::ostream& errors, const std::string& context) {
    if (provenance.recovered.entries == actual.entries && provenance.recovered.signals == actual.signals &&
        provenance.recovered.points == actual.points) {
        return true;
    }

    errors << "ERROR: " << context << " recovery counts do not match provenance:"
           << " entries " << actual.entries << "/" << provenance.recovered.entries << ", signals "
           << actual.signals << "/" << provenance.recovered.signals << ", points " << actual.points << "/"
           << provenance.recovered.points << ".\n";
    return false;
}

}  // namespace REST_LegacyRecovery

#endif
