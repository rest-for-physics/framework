
#ifndef TRestSoft_TRestDetectorHitsEvent
#define TRestSoft_TRestDetectorHitsEvent

#include <iostream>

#include "TArrayI.h"
#include "TAxis.h"
#include "TGraph2D.h"
#include "TH2F.h"
#include "TMath.h"
#include "TObject.h"

#include <TGraph.h>
#include "TH2F.h"

#include "TVector3.h"

#include "TRestEvent.h"
#include "TRestHits.h"
#include "TVector3.h"

//! An event data type that register a vector of TRestHits,
//! allowing us to save a 3-coordinate position and energy.
class TRestDetectorHitsEvent : public TRestEvent {
   private:
    /// An auxiliar TRestHits structure to register hits on XZ projection
    TRestHits* fXZHits;  //!
    /// An auxiliar TRestHits structure to register hits on YZ projection
    TRestHits* fYZHits;  //!

    /// An auxiliar TRestHits structure to register hits on XYZ projection
    TRestHits* fXYZHits;  //!

    /// Stores the minimum x-position value. It is initialized by SetBoundaries.
    Double_t fMinX;  //!
    /// Stores the maximum x-position value. It is initialized by SetBoundaries.
    Double_t fMaxX;  //!

    /// Stores the minimum y-position value. It is initialized by SetBoundaries.
    Double_t fMinY;  //!
    /// Stores the maximum y-position value. It is initialized by SetBoundaries.
    Double_t fMaxY;  //!

    /// Stores the minimum z-position value. It is initialized by SetBoundaries.
    Double_t fMinZ;  //!
    /// Stores the maximum z-position value. It is initialized by SetBoundaries.
    Double_t fMaxZ;  //!

   protected:
    // TODO These graphs should be placed in TRestHits?
    // (following similar GetGraph implementation in TRestDetectorSignal)

    /// An auxiliar TGraph pointer to visualize hits on XY-projection.
    TGraph* fXYHitGraph;  //!
    /// An auxiliar TGraph pointer to visualize hits on XZ-projection.
    TGraph* fXZHitGraph;  //!
    /// An auxiliar TGraph pointer to visualize hits on YZ-projection.
    TGraph* fYZHitGraph;  //!

    /// An auxiliar TH2F histogram to visualize hits on XY-projection.
    TH2F* fXYHisto;  //!
    /// An auxiliar TH2F histogram to visualize hits on YZ-projection.
    TH2F* fYZHisto;  //!
    /// An auxiliar TH2F histogram to visualize hits on XZ-projection.
    TH2F* fXZHisto;  //!

    /// An auxiliar TH1F histogram to visualize hits on X-projection.
    TH1F* fXHisto;  //!
    /// An auxiliar TH1F histogram to visualize hits on Y-projection.
    TH1F* fYHisto;  //!
    /// An auxiliar TH1F histogram to visualize hits on Z-projection.
    TH1F* fZHisto;  //!

    /// The hits structure that is is saved to disk.
    TRestHits* fHits;  //->

   public:
    void AddHit(Double_t x, Double_t y, Double_t z, Double_t en, Double_t t = 0, REST_HitType type = XYZ);
    void AddHit(TVector3 pos, Double_t en, Double_t t = 0, REST_HitType type = XYZ);

    void SetBoundaries();
    void Sort(bool(comparecondition)(const TRestHits::iterator& hit1, const TRestHits::iterator& hit2) = 0);
    void Shuffle(int NLoop);

    Int_t GetNumberOfHits() { return fHits->GetNumberOfHits(); }

    TRestHits* GetHits() { return fHits; }

    /// Returns the X-coordinate of hit entry `n` in mm.
    Double_t GetX(int n) { return fHits->GetX(n); }

    /// Returns the Y-coordinate of hit entry `n` in mm.
    Double_t GetY(int n) { return fHits->GetY(n); }

    /// Returns the Z-coordinate of hit entry `n` in mm.
    Double_t GetZ(int n) { return fHits->GetZ(n); }

    REST_HitType GetType(int n) { return fHits->GetType(n); }

    /// Returns the squared distance between hits `n` and `m`.
    Double_t GetDistance2(int n, int m) { return fHits->GetDistance2(n, m); }

    TRestHits* GetXZHits();
    TRestHits* GetYZHits();
    TRestHits* GetXYZHits();

    virtual void Initialize();

    virtual void PrintEvent() { TRestDetectorHitsEvent::PrintEvent(-1); };
    virtual void PrintEvent(Int_t nHits);

    TVector3 GetPosition(int n) { return fHits->GetPosition(n); }

    TVector3 GetMeanPosition() { return fHits->GetMeanPosition(); }

    Int_t GetNumberOfHitsX() { return fHits->GetNumberOfHitsX(); }
    Int_t GetNumberOfHitsY() { return fHits->GetNumberOfHitsY(); }

    Double_t GetMeanPositionX() { return fHits->GetMeanPositionX(); }
    Double_t GetMeanPositionY() { return fHits->GetMeanPositionY(); }
    Double_t GetMeanPositionZ() { return fHits->GetMeanPositionZ(); }
    Double_t GetSigmaXY2() { return fHits->GetSigmaXY2(); }
    Double_t GetSigmaX() { return fHits->GetSigmaX(); }
    Double_t GetGaussSigmaX(Int_t readoutChannels, Int_t startChannel, Int_t endChannel, Double_t pitch) {
	    return GetXZHits()->GetGaussSigmaX(readoutChannels, startChannel, endChannel, pitch);
	}
    Double_t GetSigmaY() { return fHits->GetSigmaY(); }
    Double_t GetGaussSigmaY(Int_t readoutChannels, Int_t startChannel, Int_t endChannel, Double_t pitch) {
	    return GetYZHits()->GetGaussSigmaY(readoutChannels, startChannel, endChannel, pitch);
	}
    Double_t GetSigmaZ2() { return fHits->GetSigmaZ2(); }
    Double_t GetSkewXY() { return fHits->GetSkewXY(); }
    Double_t GetSkewZ() { return fHits->GetSkewZ(); }

    Double_t GetMaximumHitEnergy() { return fHits->GetMaximumHitEnergy(); }
    Double_t GetMinimumHitEnergy() { return fHits->GetMinimumHitEnergy(); }
    Double_t GetMeanHitEnergy() { return fHits->GetMeanHitEnergy(); }

    Double_t GetEnergyX() { return fHits->GetEnergyX(); }
    Double_t GetEnergyY() { return fHits->GetEnergyY(); }
    Double_t GetTotalDepositedEnergy() { return fHits->fTotEnergy; }
    Double_t GetTotalEnergy() { return fHits->fTotEnergy; }
    Double_t GetEnergy() { return fHits->GetEnergy(); }
    Double_t GetEnergy(int n) { return fHits->GetEnergy(n); }
    Double_t GetTime(int n) { return GetHits()->GetTime(n); }  // return value in us

    Int_t GetClosestHit(TVector3 position) { return fHits->GetClosestHit(position); }

    // Inside Cylinder methods
    Bool_t anyHitInsideCylinder(TVector3 x0, TVector3 x1, Double_t radius);
    Bool_t allHitsInsideCylinder(TVector3 x0, TVector3 x1, Double_t radius);
    Double_t GetEnergyInCylinder(TVector3 x0, TVector3 x1, Double_t radius);
    Int_t GetNumberOfHitsInsideCylinder(TVector3 x0, TVector3 x1, Double_t radius);
    TVector3 GetMeanPositionInCylinder(TVector3 x0, TVector3 x1, Double_t radius);

    // Inside Prim methods
    Bool_t anyHitInsidePrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta);
    Bool_t allHitsInsidePrism(TVector3 x0, TVector3 x1, Double_t sX, Double_t sY, Double_t theta);

    Double_t GetEnergyInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta);
    Int_t GetNumberOfHitsInsidePrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                     Double_t theta);
    TVector3 GetMeanPositionInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta);

    // Get closest distance to cylinder walls methods
    Double_t GetClosestHitInsideDistanceToCylinderWall(TVector3 x0, TVector3 x1, Double_t radius);
    Double_t GetClosestHitInsideDistanceToCylinderTop(TVector3 x0, TVector3 x1, Double_t radius);
    Double_t GetClosestHitInsideDistanceToCylinderBottom(TVector3 x0, TVector3 x1, Double_t radius);

    // Get closest distance to prism walls methods
    Double_t GetClosestHitInsideDistanceToPrismWall(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                                    Double_t theta);
    Double_t GetClosestHitInsideDistanceToPrismTop(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                                   Double_t theta);
    Double_t GetClosestHitInsideDistanceToPrismBottom(TVector3 x0, TVector3 x1, Double_t sizeX,
                                                      Double_t sizeY, Double_t theta);

    TPad* DrawEvent(TString option = "");
    void DrawHistograms(Int_t& column, Double_t pitch = 3, TString histOption = "");
    void DrawGraphs(Int_t& column);

    // Construtor
    TRestDetectorHitsEvent();
    // Destructor
    ~TRestDetectorHitsEvent();

    ClassDef(TRestDetectorHitsEvent, 1);
};
#endif
