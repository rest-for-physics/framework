#include <TArrayC.h>
#include <TFile.h>
#include <TList.h>
#include <TNamed.h>
#include <TStreamerInfo.h>

#include <cstdlib>
#include <memory>
#include <set>

#include "TRestIOFixturePayload.h"

int main(int argc, char** argv) {
    if (argc != 3) return 2;
    TRestIOFixturePayload::Class();

    std::set<Int_t> numbers;
    {
        std::unique_ptr<TFile> source(TFile::Open(argv[1], "READ"));
        if (!source || source->IsZombie()) return 3;
        std::unique_ptr<TList> infos(source->GetStreamerInfoList());
        if (!infos) return 4;
        infos->SetOwner(kFALSE);
        TIter next(infos.get());
        while (TObject* object = next()) {
            auto* info = dynamic_cast<TStreamerInfo*>(object);
            if (info == nullptr) {
                object->SetBit(TObject::kCanDelete);
                continue;
            }
            info->BuildCheck(source.get());
            if (info->GetNumber() > 0) numbers.insert(info->GetNumber());
        }
        infos->Clear();
    }

    TFile output(argv[2], "RECREATE");
    if (!output.IsOpen() || output.IsZombie()) return 5;
    TNamed marker("schema-only-worker", "schema-only-worker");
    marker.Write();
    TArrayC* classIndex = output.GetClassIndex();
    if (classIndex == nullptr || numbers.empty()) return 6;
    const Int_t maximumNumber = *numbers.rbegin();
    if (maximumNumber >= classIndex->GetSize()) classIndex->Set(maximumNumber + 1);
    for (const Int_t number : numbers) classIndex->fArray[number] = 1;
    classIndex->fArray[0] = 1;
    output.WriteStreamerInfo();
    output.Close();
    return output.TestBit(TFile::kWriteError) ? 7 : EXIT_SUCCESS;
}
