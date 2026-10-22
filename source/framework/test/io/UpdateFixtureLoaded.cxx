#include <TNamed.h>

#include <cstdlib>

#include "TRestIOFixturePayload.h"
#include "TRestTools.h"

int main(int argc, char** argv) {
    if (argc != 2) return 2;
    TRestIOFixturePayload::Class();
    for (int update = 0; update < 3; ++update) {
        auto file = TRestRootFileHandle::Open(argv[1], TRestRootFileMode::Update);
        if (!file) return 3;
        const std::string name = update == 0 ? "unloaded-update" : "loaded-update-" + std::to_string(update);
        TNamed marker(name.c_str(), "loaded dictionary");
        marker.Write();
        if (!file.Close()) return 4;
    }
    return EXIT_SUCCESS;
}
