#include <TFile.h>
#include <TNamed.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "TRestTools.h"

int main(int argc, char** argv) {
    if (argc != 3) return 2;
    const std::string action = argv[2];
    if (action == "preserve" || action == "preserve-with-auto-registration-disabled") {
        if (action == "preserve-with-auto-registration-disabled") TFile::SetReadStreamerInfo(kFALSE);
        auto file = TRestRootFileHandle::Open(argv[1], TRestRootFileMode::Update);
        TFile::SetReadStreamerInfo(kTRUE);
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
        TRestTools::ForceNextRootUpdatePreflightFailureForTesting();
        std::string error;
        if (TRestRootFileHandle::PrepareBorrowedUpdate(*file, &error)) return 7;
        if (file->IsWritable()) return 8;
        return error.find("Forced writable ROOT preflight failure") != std::string::npos ? EXIT_SUCCESS : 9;
    }
    return 10;
}
