#ifndef RestIOFixtureV1_TRestIOFixturePayload
#define RestIOFixtureV1_TRestIOFixturePayload

#include <TObject.h>

#include <vector>

class TRestIOFixturePayload : public TObject {
   public:
    int fValue = 11;
    std::vector<float> fSamples{1.25F, 2.5F};
    int fLegacyCode = 37;

    ClassDef(TRestIOFixturePayload, 1);
};

#endif
