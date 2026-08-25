#ifndef REST_LEGACY_RECOVERY_PROVENANCE_H
#define REST_LEGACY_RECOVERY_PROVENANCE_H

#include <TDirectory.h>
#include <TFile.h>
#include <TNamed.h>

#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>

#include "LegacyRecoveryDataUtils.h"

namespace REST_LegacyRecovery {

inline constexpr const char* kRecoveryFormatKey = "REST_LegacyRecovery_FormatVersion";
inline constexpr const char* kRecoveryKindKey = "REST_LegacyRecovery_Kind";
inline constexpr const char* kRecoverySourceUuidKey = "REST_LegacyRecovery_SourceUUID";
inline constexpr const char* kRecoverySourcePathKey = "REST_LegacyRecovery_SourcePath";
inline constexpr const char* kRecoverySourceSizeKey = "REST_LegacyRecovery_SourceSize";
inline constexpr const char* kRecoverySourceEntriesKey = "REST_LegacyRecovery_SourceEntries";
inline constexpr const char* kRecoverySourceSignalVersionKey = "REST_LegacyRecovery_SourceSignalVersion";
inline constexpr const char* kRecoveryEntriesKey = "REST_LegacyRecovery_RecoveredEntries";
inline constexpr const char* kRecoverySignalsKey = "REST_LegacyRecovery_RecoveredSignals";
inline constexpr const char* kRecoveryPointsKey = "REST_LegacyRecovery_RecoveredPoints";
inline constexpr const char* kRecoveryIntermediateUuidKey = "REST_LegacyRecovery_IntermediateUUID";
inline constexpr const char* kRecoveryIntermediatePathKey = "REST_LegacyRecovery_IntermediatePath";
inline constexpr const char* kRecoveryResultUuidKey = "REST_LegacyRecovery_ResultUUID";

inline bool WriteNamedValue(TDirectory& directory, const char* name, const std::string& value,
                            std::ostream& errors) {
    directory.cd();
    TNamed object(name, value.c_str());
    if (object.Write(name, TObject::kOverwrite) > 0) return true;

    errors << "ERROR: failed to write recovery provenance key '" << name << "'.\n";
    return false;
}

inline bool WriteRecoveryProvenance(TDirectory& directory, const RecoveryProvenance& provenance,
                                    std::ostream& errors) {
    if (!WriteNamedValue(directory, kRecoveryFormatKey, std::to_string(provenance.formatVersion), errors) ||
        !WriteNamedValue(directory, kRecoveryKindKey, provenance.kind, errors) ||
        !WriteNamedValue(directory, kRecoverySourceUuidKey, provenance.source.uuid, errors) ||
        !WriteNamedValue(directory, kRecoverySourcePathKey, provenance.source.normalizedPath, errors) ||
        !WriteNamedValue(directory, kRecoverySourceSizeKey, std::to_string(provenance.source.fileSize),
                         errors) ||
        !WriteNamedValue(directory, kRecoverySourceEntriesKey, std::to_string(provenance.source.entries),
                         errors) ||
        !WriteNamedValue(directory, kRecoverySourceSignalVersionKey,
                         std::to_string(provenance.source.signalVersion), errors) ||
        !WriteNamedValue(directory, kRecoveryEntriesKey, std::to_string(provenance.recovered.entries),
                         errors) ||
        !WriteNamedValue(directory, kRecoverySignalsKey, std::to_string(provenance.recovered.signals),
                         errors) ||
        !WriteNamedValue(directory, kRecoveryPointsKey, std::to_string(provenance.recovered.points),
                         errors) ||
        !WriteNamedValue(directory, kRecoveryIntermediateUuidKey, provenance.intermediateUuid, errors)) {
        return false;
    }

    if (!provenance.intermediatePath.empty() &&
        !WriteNamedValue(directory, kRecoveryIntermediatePathKey, provenance.intermediatePath, errors)) {
        return false;
    }
    if (!provenance.resultUuid.empty() &&
        !WriteNamedValue(directory, kRecoveryResultUuidKey, provenance.resultUuid, errors)) {
        return false;
    }
    return true;
}

inline bool ReadNamedValue(TDirectory& directory, const char* name, std::string& value,
                           std::ostream& errors) {
    auto object = dynamic_cast<TNamed*>(directory.Get(name));
    if (object == nullptr) {
        errors << "ERROR: required recovery provenance key '" << name << "' is missing or invalid.\n";
        return false;
    }
    value = object->GetTitle();
    return true;
}

template <typename Integer>
bool ParseInteger(const std::string& text, Integer& value) {
    if (text.empty()) return false;
    if constexpr (std::is_unsigned_v<Integer>) {
        if (text.front() == '-') return false;
    }

    // std::from_chars would be a natural fit, but older ROOT/Cling releases
    // cannot parse the GCC 14 <charconv> header. Stream extraction keeps these
    // recovery macros usable with the ROOT versions deployed with REST v2.4.
    std::istringstream input(text);
    input >> std::noskipws >> value;
    return !input.fail() && input.eof();
}

template <typename Integer>
bool ReadIntegerValue(TDirectory& directory, const char* name, Integer& value, std::ostream& errors) {
    std::string text;
    if (!ReadNamedValue(directory, name, text, errors)) return false;
    if (ParseInteger(text, value)) return true;

    errors << "ERROR: recovery provenance key '" << name << "' is not a valid integer: '" << text << "'.\n";
    return false;
}

inline bool ReadRecoveryProvenance(TDirectory& directory, RecoveryProvenance& provenance,
                                   std::ostream& errors) {
    if (!ReadIntegerValue(directory, kRecoveryFormatKey, provenance.formatVersion, errors) ||
        !ReadNamedValue(directory, kRecoveryKindKey, provenance.kind, errors) ||
        !ReadNamedValue(directory, kRecoverySourceUuidKey, provenance.source.uuid, errors) ||
        !ReadNamedValue(directory, kRecoverySourcePathKey, provenance.source.normalizedPath, errors) ||
        !ReadIntegerValue(directory, kRecoverySourceSizeKey, provenance.source.fileSize, errors) ||
        !ReadIntegerValue(directory, kRecoverySourceEntriesKey, provenance.source.entries, errors) ||
        !ReadIntegerValue(directory, kRecoverySourceSignalVersionKey, provenance.source.signalVersion,
                          errors) ||
        !ReadIntegerValue(directory, kRecoveryEntriesKey, provenance.recovered.entries, errors) ||
        !ReadIntegerValue(directory, kRecoverySignalsKey, provenance.recovered.signals, errors) ||
        !ReadIntegerValue(directory, kRecoveryPointsKey, provenance.recovered.points, errors) ||
        !ReadNamedValue(directory, kRecoveryIntermediateUuidKey, provenance.intermediateUuid, errors)) {
        return false;
    }

    if (provenance.kind == kResultProvenanceKind) {
        if (!ReadNamedValue(directory, kRecoveryIntermediatePathKey, provenance.intermediatePath, errors) ||
            !ReadNamedValue(directory, kRecoveryResultUuidKey, provenance.resultUuid, errors)) {
            return false;
        }
    }
    return true;
}

inline bool CheckAndCloseOutputFile(TFile& file, std::ostream& errors) {
    // TFile::GetErrno() forwards the process-global errno, which may contain a
    // stale value from an unrelated earlier operation. Reset it immediately
    // before each operation whose error state we inspect.
    file.ResetErrno();
    file.Flush();
    const int flushErrno = file.GetErrno();
    bool valid = !file.TestBit(TFile::kWriteError) && flushErrno == 0;
    if (!valid) {
        errors << "ERROR: write failure while flushing '" << file.GetName() << "'"
               << " (errno=" << flushErrno << ").\n";
    }

    file.ResetErrno();
    file.Close();
    const int closeErrno = file.GetErrno();
    if (file.TestBit(TFile::kWriteError) || closeErrno != 0) {
        errors << "ERROR: write failure while closing '" << file.GetName() << "'"
               << " (errno=" << closeErrno << ").\n";
        valid = false;
    }
    return valid;
}

inline bool ValidateResultProvenance(const RecoveryProvenance& actual, const RecoveryProvenance& expected,
                                     const std::string& openedResultUuid, std::ostream& errors) {
    bool valid = true;
    if (actual.formatVersion != kRecoveryFormatVersion || actual.formatVersion != expected.formatVersion) {
        errors << "ERROR: rebuilt file has an unexpected recovery provenance format.\n";
        valid = false;
    }
    if (actual.kind != kResultProvenanceKind || actual.kind != expected.kind) {
        errors << "ERROR: rebuilt file has an unexpected recovery provenance kind.\n";
        valid = false;
    }
    if (actual.source.uuid != expected.source.uuid ||
        actual.source.normalizedPath != expected.source.normalizedPath ||
        actual.source.fileSize != expected.source.fileSize ||
        actual.source.entries != expected.source.entries ||
        actual.source.signalVersion != expected.source.signalVersion) {
        errors << "ERROR: rebuilt file source provenance does not match the validated source.\n";
        valid = false;
    }
    if (actual.recovered.entries != expected.recovered.entries ||
        actual.recovered.signals != expected.recovered.signals ||
        actual.recovered.points != expected.recovered.points) {
        errors << "ERROR: rebuilt file recovery counts do not match the validated candidate.\n";
        valid = false;
    }
    if (actual.intermediateUuid != expected.intermediateUuid ||
        actual.intermediatePath != expected.intermediatePath) {
        errors << "ERROR: rebuilt file intermediate provenance does not match the validated input.\n";
        valid = false;
    }
    if (actual.resultUuid != openedResultUuid || actual.resultUuid != expected.resultUuid) {
        errors << "ERROR: rebuilt file UUID does not match its persisted recovery provenance.\n";
        valid = false;
    }
    return valid;
}

}  // namespace REST_LegacyRecovery

#endif
