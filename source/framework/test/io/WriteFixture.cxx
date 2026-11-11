#include <TFile.h>
#include <TTree.h>

#include <cstdlib>
#include <memory>

#include "TRestIOFixturePayload.h"

int main(int argc, char** argv) {
    if (argc != 3) return 2;
    std::unique_ptr<TFile> file(TFile::Open(argv[1], argv[2]));
    if (!file || file->IsZombie() || !file->IsWritable()) return 3;

    TRestIOFixturePayload payload;
    const char* key = TRestIOFixturePayload::Class_Version() == 1 ? "payload-v1" : "payload-v2";
    file->WriteObject(&payload, key);
    if (TRestIOFixturePayload::Class_Version() == 1) {
        TTree events("events", "realistic tree-bearing update fixture");
        double value = 42.5;
        events.Branch("value", &value);
        events.Fill();
        events.Write();
    }
    file->Write();
    file->Close();
    return file->TestBit(TFile::kWriteError) ? 4 : EXIT_SUCCESS;
}
