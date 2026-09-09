#include <TObject.h>

// Deliberately invalid evolution: the layout changed without a version bump.
class TRestIOConflictPayload : public TObject {
   public:
#ifdef REST_IO_OLD_LAYOUT
    int fValue = 123;
#else
    double fValue = 123.5;
#endif
    ClassDef(TRestIOConflictPayload, 2);
};
