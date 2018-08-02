
#ifndef TRestSoft_TRestHitsEvent
#define TRestSoft_TRestHitsEvent

#include <iostream>

#include "TObject.h"
#include "TMath.h"
#include "TArrayI.h"
#include "TAxis.h"

#include <TGraph.h>
#include "TH2F.h"

#include "TVector3.h"

#include "TRestEvent.h"
#include "TRestHits.h"


// Storage class

// It let save an event as a set of punctual deposition.
// It saves a 3-coordinate position and an energy for each punctual deposition.
class TRestHitsEvent : public TRestEvent
{
    private:
        #ifndef __CINT__
        TRestHits *fXZHits;
        TRestHits *fYZHits;

        TRestHits *fXYZHits;

        Double_t fMinX, fMaxX;
        Double_t fMinY, fMaxY;
        Double_t fMinZ, fMaxZ;
        #endif

    protected:
        #ifndef __CINT__
        // TODO These graphs should be placed in TRestTrack?
        // (following GetGraph implementation in TRestSignal)
        TGraph *fXYHitGraph;
        TGraph *fXZHitGraph;
        TGraph *fYZHitGraph;

        TH2F *fXYHisto;
        TH2F *fYZHisto;
        TH2F *fXZHisto;
        TH1F *fXHisto;
        TH1F *fYHisto;
        TH1F *fZHisto;

        #endif

    public:
        
        TRestHits *fHits;	// 

        //! Changes the orgin of the Cartesian coordinate system
        void ChangeOrigin(double origx, double origy, double origz);

        void AddHit( Double_t x, Double_t y, Double_t z, Double_t en, Double_t t = 0, Short_t mod = -1, Short_t ch = -1 );
        void AddHit( TVector3 pos, Double_t en, Double_t t = 0, Short_t mod = -1, Short_t ch = -1 );
        void RemoveHits( );

        void MergeHits( int n, int m );
        void RemoveHit( int n );

        void SetBoundaries( );

        Int_t GetNumberOfHits( ) { return fHits->GetNumberOfHits(); }

        TRestHits *GetHits( ) { return fHits; }

       
        Double_t GetX( int n ) { return fHits->GetX(n); } // return value in mm
        Double_t GetY( int n ) { return fHits->GetY(n); } // return value in mm
        Double_t GetZ( int n ) { return fHits->GetZ(n); } // return value in mm
        Double_t GetEnergy( int n ) { return fHits->GetEnergy(n); } //return value in keV
        Short_t GetModule( int n ) { return fHits->GetModule(n); } //return module ID
        Short_t GetChannel( int n ) { return fHits->GetChannel(n); } //return channel ID

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
	Double_t GetSigmaXY2(){return fHits->GetSigmaXY2();}
	Double_t GetSigmaX(){return fHits->GetSigmaX();}
	Double_t GetSigmaY(){return fHits->GetSigmaY();}
	Double_t GetSigmaZ2(){return fHits->GetSigmaZ2();}
	Double_t GetSkewXY(){return fHits->GetSkewXY();}
	Double_t GetSkewZ(){return fHits->GetSkewZ();}

	Double_t GetMaximumHitEnergy(){return fHits->GetMaximumHitEnergy( );}
	Double_t GetMinimumHitEnergy(){return fHits->GetMinimumHitEnergy( );}
	Double_t GetMeanHitEnergy(){return fHits->GetMeanHitEnergy( );}
       
        Double_t GetEnergyX( ) { return fHits->GetEnergyX(); }
        Double_t GetEnergyY( ) { return fHits->GetEnergyY(); }
        Double_t GetTotalDepositedEnergy() { return fHits->fTotEnergy; }
        Double_t GetTotalEnergy() { return fHits->fTotEnergy; }
        Double_t GetEnergy() { return fHits->fTotEnergy; }
        Double_t GetTime( int n ) { return GetHits()->GetTime(n); } // return value in us

        Int_t GetClosestHit( TVector3 position ) { return fHits->GetClosestHit( position ); }

        // Inside Cylinder methods
        Bool_t isHitsEventInsideCylinder( TVector3 x0, TVector3 x1, Double_t radius );
	

        Int_t  GetEnergyInCylinder( TVector3 x0, TVector3 x1, Double_t radius )
            { return fHits-> GetEnergyInCylinder(x0, x1, radius); }

        Int_t GetNumberOfHitsInsideCylinder( TVector3 x0, TVector3 x1, Double_t radius )
            { return fHits->GetNumberOfHitsInsideCylinder(x0, x1, radius); }

        Bool_t areHitsFullyContainnedInsideCylinder( TVector3 x0, TVector3 x1, Double_t radius );
          TVector3 GetMeanPositionInCylinder( TVector3 x0, TVector3 x1, Double_t radius ) { return fHits->GetMeanPositionInCylinder(x0, x1, radius); }


        // Inside Prim methods
        Bool_t isHitsEventInsidePrism( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta  );
	Bool_t areHitsFullyContainnedInsidePrism( TVector3 x0, TVector3 x1, Double_t sX, Double_t sY, Double_t theta );
     

        Int_t GetNumberOfHitsInsidePrism( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta)
            { return fHits->GetNumberOfHitsInsidePrism(x0, x1, sizeX, sizeY,theta); }

        Int_t GetEnergyInPrism( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta)
            { return fHits->GetEnergyInPrism(x0, x1, sizeX, sizeY,theta); }

            TVector3 GetMeanPositionInPrism( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta) { return fHits->GetMeanPositionInPrism(x0, x1, sizeX, sizeY,theta); }



        // Get closest distance to cylinder walls methods
        Double_t GetClosestHitInsideDistanceToCylinderWall( TVector3 x0, TVector3 x1, Double_t radius );
        Double_t GetClosestHitInsideDistanceToCylinderTop( TVector3 x0, TVector3 x1, Double_t radius );
        Double_t GetClosestHitInsideDistanceToCylinderBottom( TVector3 x0, TVector3 x1, Double_t radius );

        // Get closest distance to prism walls methods
        Double_t GetClosestHitInsideDistanceToPrismWall( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta );
        Double_t GetClosestHitInsideDistanceToPrismTop( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta );
        Double_t GetClosestHitInsideDistanceToPrismBottom( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta );


        TPad *DrawEvent( TString option = "" );
        void DrawHistograms( Int_t &column, Double_t pitch = 3, TString histOption = "" );
        void DrawGraphs( Int_t &column );

        //Construtor
        TRestHitsEvent();
        //Destructor
        ~TRestHitsEvent();

        ClassDef(TRestHitsEvent, 1);
};
#endif
