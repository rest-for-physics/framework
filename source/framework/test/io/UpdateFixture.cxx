#include <TClass.h>
#include <TFile.h>
#include <TNamed.h>
#include <TObjArray.h>
#include <TStreamerInfo.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "TRestTools.h"

namespace {
bool RemoveHistoricalStreamerInfo() {
    TClass* payload = TClass::GetClass("TRestIOFixturePayload", kFALSE);
    if (payload == nullptr) return false;
    auto* infos = const_cast<TObjArray*>(payload->GetStreamerInfos());
    if (infos == nullptr) return false;
    for (Int_t index = 0; index < infos->GetEntriesFast(); ++index) {
        auto* info = dynamic_cast<TStreamerInfo*>(infos->UncheckedAt(index));
        if (info != nullptr && info->GetClassVersion() == 1) {
            // The subprocess exits immediately after the check; ROOT still owns
            // the removed object, so deliberately do not delete it here.
            infos->RemoveAt(index);
            return true;
        }
    }
    return false;
}
}  // namespace

int main(int argc, char** argv) {
    if (argc != 3) return 2;
    const std::string action = argv[2];
    if (action == "preserve") {
        auto file = TRestRootFileHandle::Open(argv[1], TRestRootFileMode::Update);
        if (!file) {
            std::cerr << file.Error() << '\n';
            return 3;
        }
        TNamed marker("unloaded-update", "dictionary intentionally unavailable");
        marker.Write();
        return file.Close() ? EXIT_SUCCESS : 4;
    }

    if (action == "borrowed-failure") {
        std::unique_ptr<TFile> file(TFile::Open(argv[1], "READ"));
        if (!file || file->IsZombie() || file->IsWritable()) return 5;
        if (!RemoveHistoricalStreamerInfo()) return 6;
        std::string error;
        if (TRestRootFileHandle::PrepareBorrowedUpdate(*file, &error)) return 7;
        if (file->IsWritable()) return 8;
        return error.find("TRestIOFixturePayload") != std::string::npos ? EXIT_SUCCESS : 9;
    }
    return 10;
}
