#include <TFile.h>

#include <cmath>
#include <cstdlib>
#include <memory>

#include "TRestIOFixturePayload.h"

namespace {
bool Near(float left, float right) { return std::abs(left - right) < 1e-6F; }
}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) return 2;
    std::unique_ptr<TFile> file(TFile::Open(argv[1], "READ"));
    if (!file || file->IsZombie()) return 3;

    std::unique_ptr<TRestIOFixturePayload> payload(file->Get<TRestIOFixturePayload>("payload-v1"));
    if (!payload) return 4;
    if (payload->fValue != 11 || payload->fSamples.size() != 2 || !Near(payload->fSamples[0], 1.25F) ||
        !Near(payload->fSamples[1], 2.5F) || payload->fLegacyCode != 37)
        return 5;
    return EXIT_SUCCESS;
}
