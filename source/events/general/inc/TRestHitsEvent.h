
#ifndef TRestSoft_TRestHitsEvent
#define TRestSoft_TRestHitsEvent

#include <iostream>

#include "TObject.h"
#include "TMath.h"
#include "TArrayI.h"
#include <TVector3.h>

#include <TRestEvent.h>
#include <TRestHits.h>

// Storage class

// It let save an event as a set of punctual deposition.
// It saves a 3-coordinate position and an energy for each punctual deposition.
class TRestHitsEvent : public TRestEvent
{
    public:
        
        TRestHits *fHits;	//->

        //! Changes the orgin of the Cartesian coordinate system
        void ChangeOrigin(double origx, double origy, double origz);

        void AddHit( Double_t x, Double_t y, Double_t z, Double_t en, Double_t t = 0 );
        void AddHit( TVector3 pos, Double_t en, Double_t t = 0 );
        void RemoveHits( );

        void MergeHits( int n, int m );
        void RemoveHit( int n );

        Int_t GetNumberOfHits( ) { return fHits->fNHits; }

        TRestHits *GetHits( ) { return fHits; }

       
        Double_t GetX( int n ) { return fHits->GetX(n); } // return value in mm
        Double_t GetY( int n ) { return fHits->GetY(n); } // return value in mm
        Double_t GetZ( int n ) { return fHits->GetZ(n); } // return value in mm
        Double_t GetEnergy( int n ) { return fHits->GetEnergy(n); } //return value in keV

        Double_t GetDistance2( int n, int m ) { return fHits->GetDistance2( n, m ); }

        TRestHits *GetXZHits();
        TRestHits *GetYZHits();
        TRestHits *GetXYZHits();

        virtual void Initialize();

        virtual void PrintEvent( )  { TRestHitsEvent::PrintEvent(-1); };
        virtual void PrintEvent( Int_t nHits );

        TVector3 GetPosition( int n ) { return fHits->GetPosition(n); }

        TVector3 GetMeanPosition( ) { return fHits->GetMeanPosition(); }

        Int_t GetNumberOfHitsX( ) { return fHits->GetNumberOfHitsX(); }
        Int_t GetNumberOfHitsY( ) { return fHits->GetNumberOfHitsY(); }

        Double_t GetMeanPositionX( ) { return fHits->GetMeanPositionX(); }
        Double_t GetMeanPositionY( ) { return fHits->GetMeanPositionY(); }
        Double_t GetMeanPositionZ( ) { return fHits->GetMeanPositionZ(); }

        Double_t GetTotalDepositedEnergy() { return fHits->fTotEnergy; }
        Double_t GetTotalEnergy() { return fHits->fTotEnergy; }
        Double_t GetEnergy() { return fHits->fTotEnergy; }
        Double_t GetTime( int n ) { return GetHits()->GetTime(n); } // return value in us


        // Inside Cylinder methods
        Bool_t isHitsEventInsideCylinder( TVector3 x0, TVector3 x1, Double_t radius );

        Int_t  GetEnergyInCylinder( TVector3 x0, TVector3 x1, Double_t radius )
            { return fHits-> GetEnergyInCylinder(x0, x1, radius); }

        Int_t GetNumberOfHitsInsideCylinder( TVector3 x0, TVector3 x1, Double_t radius )
            { return fHits->GetNumberOfHitsInsideCylinder(x0, x1, radius); }

        Bool_t areHitsFullyContainnedInsideCylinder( TVector3 x0, TVector3 x1, Double_t radius )
            { return isHitsEventInsideCylinder( x0, x1, radius ); }


        // Inside Prim methods
        Bool_t isHitsEventInsidePrism( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY );

        Int_t GetNumberOfHitsInsidePrism( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY)
            { return fHits->GetNumberOfHitsInsidePrism(x0, x1, sizeX, sizeY); }

        Int_t GetEnergyInPrism( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY)
            { return fHits->GetEnergyInPrism(x0, x1, sizeX, sizeY); }

        Bool_t areHitsFullyContainnedInsidePrism( TVector3 x0, TVector3 x1, Double_t sX, Double_t sY )
        { return isHitsEventInsidePrism( x0, x1, sX, sY ); }


        // Get closest distance to cylinder walls methods
        Double_t GetClosestHitInsideDistanceToCylinderWall( TVector3 x0, TVector3 x1, Double_t radius );
        Double_t GetClosestHitInsideDistanceToCylinderTop( TVector3 x0, TVector3 x1, Double_t radius );
        Double_t GetClosestHitInsideDistanceToCylinderBottom( TVector3 x0, TVector3 x1, Double_t radius );

        // Get closest distance to prism walls methods
        Double_t GetClosestHitInsideDistanceToPrismWall( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY );
        Double_t GetClosestHitInsideDistanceToPrismTop( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY );
        Double_t GetClosestHitInsideDistanceToPrismBottom( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY );


        TPad *DrawEvent( TString option = "" ) { std::cout << "TRestHitsEvent::DrawEvent not implemented. TODO" << std::endl; return NULL; }

        //Construtor
        TRestHitsEvent();
        //Destructor
        ~TRestHitsEvent();

        ClassDef(TRestHitsEvent, 1);
};
#endif
