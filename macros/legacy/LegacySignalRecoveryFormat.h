#ifndef REST_LEGACY_SIGNAL_RECOVERY_FORMAT_H
#define REST_LEGACY_SIGNAL_RECOVERY_FORMAT_H

#include <TBranch.h>
#include <TBranchElement.h>
#include <TDirectory.h>
#include <TNamed.h>
#include <TParameter.h>

#include <string>

namespace REST_LegacySignalRecovery {

inline constexpr const char* kEventTree = "EventTree";
inline constexpr const char* kSignalBranch = "TRestDetectorSignalEventBranch";
inline constexpr const char* kDataTree = "LegacySignalData";

inline int DetectSignalVersion(TBranch* branch) {
    int time = -1;
    int charge = -1;
    const auto visit = [&](auto&& self, TBranch* current) -> void {
        if (auto* element = dynamic_cast<TBranchElement*>(current)) {
            const std::string name = current->GetName();
            if (name == "fSignal.fSignalTime") time = element->GetClassVersion();
            if (name == "fSignal.fSignalCharge") charge = element->GetClassVersion();
        }
        auto* children = current->GetListOfBranches();
        for (int index = 0; index <= children->GetLast(); ++index)
            self(self, static_cast<TBranch*>(children->At(index)));
    };
    if (branch != nullptr) visit(visit, branch);
    return time == charge ? time : -2;
}

inline bool WriteText(TDirectory& directory, const char* key, const std::string& value) {
    directory.cd();
    return TNamed(key, value.c_str()).Write(key, TObject::kOverwrite) > 0;
}

inline bool WriteNumber(TDirectory& directory, const char* key, Long64_t value) {
    directory.cd();
    return TParameter<Long64_t>(key, value).Write(key, TObject::kOverwrite) > 0;
}

inline bool ReadText(TDirectory& directory, const char* key, std::string& value) {
    auto* object = dynamic_cast<TNamed*>(directory.Get(key));
    if (object == nullptr) return false;
    value = object->GetTitle();
    return true;
}

inline bool ReadNumber(TDirectory& directory, const char* key, Long64_t& value) {
    auto* object = dynamic_cast<TParameter<Long64_t>*>(directory.Get(key));
    if (object == nullptr) return false;
    value = object->GetVal();
    return true;
}

}  // namespace REST_LegacySignalRecovery

#endif
