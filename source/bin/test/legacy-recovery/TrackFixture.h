#ifndef REST_RECOVERY_TRACK_FIXTURE_H
#define REST_RECOVERY_TRACK_FIXTURE_H

#include <TObject.h>

#include <vector>

// Reproduce TRestTrack's removal of TObject without depending on tracklib.
#ifdef RECOVERY_NEW_TRACK_SCHEMA
class RecoveryTrack {
#else
class RecoveryTrack : public TObject {
#endif
   public:
    Int_t fID = 0;
    Double_t fEnergy = 0;
#ifdef RECOVERY_NEW_TRACK_SCHEMA
    ClassDef(RecoveryTrack, 3);
#else
    ClassDef(RecoveryTrack, 2);
#endif
};

class RecoveryTrackEvent : public TObject {
   public:
    std::vector<RecoveryTrack> fTrack;
    ClassDef(RecoveryTrackEvent, 1);
};

#endif
