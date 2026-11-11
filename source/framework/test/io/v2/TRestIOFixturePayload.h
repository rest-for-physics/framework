#ifndef RestIOFixtureV2_TRestIOFixturePayload
#define RestIOFixtureV2_TRestIOFixturePayload

#include <TObject.h>

#include <vector>

class TRestIOFixturePayload : public TObject {
   public:
    int fValue = 22;
    std::vector<double> fSamples{3.75, 5.0};
    double fRenamedCode = 73.5;
    double fExtra = 9.5;

    ClassDef(TRestIOFixturePayload, 2);
};

#endif
