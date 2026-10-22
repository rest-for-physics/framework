#include <TFile.h>
#include <TNamed.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "TRestIOFixturePayload.h"

namespace {
bool Near(double left, double right) { return std::abs(left - right) < 1e-12; }
}  // namespace

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) return 2;
    std::unique_ptr<TFile> file(TFile::Open(argv[1], "READ"));
    if (!file || file->IsZombie()) return 3;

    std::unique_ptr<TRestIOFixturePayload> oldPayload(file->Get<TRestIOFixturePayload>("payload-v1"));
    std::unique_ptr<TRestIOFixturePayload> newPayload(file->Get<TRestIOFixturePayload>("payload-v2"));
    if (!oldPayload || !newPayload) return 4;
    if (oldPayload->fValue != 11 || oldPayload->fSamples.size() != 2 ||
        !Near(oldPayload->fSamples[0], 1.25) || !Near(oldPayload->fSamples[1], 2.5) ||
        !Near(oldPayload->fRenamedCode, 37.0))
        std::cerr << "Unexpected v1 payload after evolution: value=" << oldPayload->fValue
                  << " sample-count=" << oldPayload->fSamples.size()
                  << (oldPayload->fSamples.empty() ? "" : " first=" + std::to_string(oldPayload->fSamples[0]))
                  << " renamed-code=" << oldPayload->fRenamedCode << '\n';
    if (oldPayload->fValue != 11 || oldPayload->fSamples.size() != 2 ||
        !Near(oldPayload->fSamples[0], 1.25) || !Near(oldPayload->fSamples[1], 2.5) ||
        !Near(oldPayload->fRenamedCode, 37.0))
        return 5;
    if (newPayload->fValue != 22 || newPayload->fSamples.size() != 2 ||
        !Near(newPayload->fSamples[0], 3.75) || !Near(newPayload->fSamples[1], 5.0) ||
        !Near(newPayload->fRenamedCode, 73.5) || !Near(newPayload->fExtra, 9.5))
        return 6;
    if (argc == 3 && std::string(argv[2]) == "require-update" &&
        file->Get<TNamed>("unloaded-update") == nullptr)
        return 7;
    return EXIT_SUCCESS;
}
