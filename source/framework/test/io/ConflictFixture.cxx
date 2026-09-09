#include <TFile.h>
#include <TStreamerInfo.h>

#include <iostream>
#include <string>

#include "ConflictPayload.h"
#include "TRestTools.h"

ClassImp(TRestIOConflictPayload);

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) return 2;
    // Establish the active dictionary before ROOT reads the conflicting file.
    TRestIOConflictPayload::Class()->GetStreamerInfo()->Build();
#ifdef REST_IO_OLD_LAYOUT
    TFile file(argv[1], "CREATE");
    if (file.IsZombie()) return 3;
    TRestIOConflictPayload payload;
    payload.Write("payload");
    return 0;
#else
    if (argc == 3) {
        std::string error;
        const bool merged = TRestTools::MergeRootFilesTransactionally(argv[2], {argv[1]}, "", true, &error);
        std::cout << "merged=" << merged << " error=" << error << '\n';
        return merged ? 4 : 0;
    }
    auto file = TRestRootFileHandle::Open(argv[1], TRestRootFileMode::Update);
    std::cout << "update=" << bool(file) << " error=" << file.Error() << '\n';
    return file ? 4 : 0;
#endif
}
