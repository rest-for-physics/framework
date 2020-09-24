#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4RunManager.hh"
#include "G4UserSteppingAction.hh"
#include "globals.hh"

#include "TH1D.h"
#include "TH2D.h"

#include <TRestGeant4BiasingVolume.h>

#include <fstream>
#include <iostream>
using namespace std;

class SteppingAction : public G4UserSteppingAction {
   public:
    SteppingAction();
    ~SteppingAction();

    void SetBiasingVolume(TRestGeant4BiasingVolume biasVol) { restBiasingVolume = biasVol; }
    void SetBiasingSpectrum(TH1D* bSpectrum) { fBiasingSpectrum = bSpectrum; }
    void SetAngularDistribution(TH1D* aDist) { fAngularDistribution = aDist; }
    void SetSpatialDistribution(TH2D* sDist) { fSpatialDistribution = sDist; }
    void UserSteppingAction(const G4Step*);

    TRestGeant4BiasingVolume GetBiasingVolume() { return restBiasingVolume; }
    TH1D* GetBiasingSpectrum() { return fBiasingSpectrum; }
    TH1D* GetAngularDistribution() { return fAngularDistribution; }
    TH2D* GetSpatialDistribution() { return fSpatialDistribution; }

   private:
    G4String nom_vol, nom_part, nom_proc;
    G4double dif_ener, ener_dep, ener, eKin;
    G4int trackID, parentID;

    TH1D* fBiasingSpectrum;
    TH1D* fAngularDistribution;
    TH2D* fSpatialDistribution;

    Double_t absDouble(Double_t x) {
        if (x < 0) return -x;
        return x;
    }

    TRestGeant4BiasingVolume restBiasingVolume;

    G4ThreeVector previousDirection;

    /*
       G4ThreeVector momentum;
       G4double x, y, z;
       G4int step;


       void WriteDefaultInt();
       */
};
#endif
