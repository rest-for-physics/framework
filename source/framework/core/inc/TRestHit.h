//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHit.h
///
///             Event class to store a single hit
///
///
//////////////////////////////////////////////////////////////////////////

#ifndef TRestSoft_TRestHit
#define TRestSoft_TRestHit

#include <TString.h>

class TRestHit {
   public:

    // Construtor
    TRestHit(){}
    TRestHit(Float_t x, Float_t y, Float_t z,Float_t en,Float_t t, TString type) : fX(x),fY(y),fZ(z),fT(t),fEnergy(en),fType(type){}
    // Destructor
    ~TRestHit(){}

    Float_t fX;          // Position on X axis for each punctual
    Float_t fY;          // Position on Y axis for each punctual
                                      // deposition (units mm)
    Float_t fZ;          // Position on Z axis for each punctual
                                      // deposition (units mm)
    Float_t fT;          // Absolute time information for each punctual deposition
                                      // (units us, 0 is time of decay)
    Float_t fEnergy;     // Energy deposited at each
                                      // 3-coordinate position (units keV)
    TString fType = "";  //

  static bool sortByX (const TRestHit &h1, const TRestHit &h2){//!
    return h1.fX > h2.fX;
  }
  static bool sortByY (const TRestHit &h1, const TRestHit &h2 ){//!
    return h1.fY > h2.fY;
  }
  static bool sortByZ (const TRestHit &h1, const TRestHit &h2 ){//!
    return h1.fZ > h2.fZ;
  }
  static bool sortByEn (const TRestHit &h1, const TRestHit &h2 ){//!
    return h1.fEnergy > h2.fEnergy;
  }
  static bool sortByTime (const TRestHit &h1, const TRestHit &h2 ){//!
    return h1.fT > h2.fT;
  }
   
   ClassDefInline(TRestHit, 1);
};


#endif


