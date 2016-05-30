///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutPlane.h
///
///             mar 2016:   First concept
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestReadoutPlane
#define RestCore_TRestReadoutPlane

#include <iostream>

#include "TObject.h"

#include "TRestMetadata.h"
#include "TRestReadoutModule.h"
#include "TRestReadoutChannel.h"

#include <TGraph.h>
#include <TH2Poly.h>

class TRestReadoutPlane: public TObject {
    private:

        Int_t fPlaneID;

        TVector3 fPosition;
        TVector3 fPlaneVector;
        TVector3 fCathodePosition;
        Double_t fChargeCollection;
        Double_t fTotalDriftDistance;

        Int_t fNModules;
        std::vector <TRestReadoutModule> fReadoutModules;

        void Initialize();

    protected:

    public:
        
        // Setters
        void SetID( int id ) { fPlaneID = id; }
        void SetPosition( TVector3 pos ) { fPosition = pos; }
        void SetCathodePosition( TVector3 pos ) { fCathodePosition = pos; }
        void SetPlaneVector( TVector3 vect ) { fPlaneVector = vect.Unit(); }
        void SetChargeCollection( Double_t charge ) { fChargeCollection = charge; }
        void SetTotalDriftDistance( Double_t d ) { fTotalDriftDistance = d; }

        void SetDriftDistance( )
        {
            Double_t tDriftDistance = this->GetDistanceTo( this->GetCathodePosition() );
            this->SetTotalDriftDistance( tDriftDistance );
        }

        Int_t GetID ( ) { return fPlaneID; }
        TVector3 GetPosition( ) { return fPosition; }
        TVector3 GetCathodePosition( ) { return fCathodePosition; }
        TVector3 GetPlaneVector( ) { return fPlaneVector; }
        Double_t GetChargeCollection( ) { return fChargeCollection; }

        Double_t GetDistanceTo( TVector3 pos );
        Double_t GetDistanceTo( Double_t x, Double_t y, Double_t z ) { return GetDistanceTo( TVector3( x, y, z ) ); }
        Double_t GetTotalDriftDistance( ) { return fTotalDriftDistance; }

        Int_t isInsideDriftVolume( Double_t x, Double_t y, Double_t z  );
        Int_t isInsideDriftVolume( TVector3 pos );


        void Draw();

        void Print( Int_t fullDetail = 0 );

        void PrintMetadata( ) { Print( ); }

        void AddModule( TRestReadoutModule &rModule ) { fReadoutModules.push_back( rModule ); fNModules++; }

        TRestReadoutModule *GetModuleByID( Int_t modID );
        TRestReadoutChannel *GetChannelByID( Int_t modID, Int_t chID );

        TRestReadoutModule *GetModule( int mod ) { return &fReadoutModules[mod]; }
        TRestReadoutChannel *GetChannel( Int_t mod, Int_t ch ) { return fReadoutModules[mod].GetChannel( ch ); }

        TRestReadoutModule *GetReadoutModule( int mod ) { return GetModule( mod ); }
        TRestReadoutChannel *GetReadoutChannel( Int_t mod, Int_t ch ) { return GetChannel( mod, ch ); }

        Int_t FindChannel( Int_t module, Double_t absX, Double_t absY )
        {
            Double_t modX = absX - fPosition.X();
            Double_t modY = absY - fPosition.Y();
            return GetModule( module )->FindChannel( modX, modY );
        }

        Int_t GetNumberOfModules( ) { return fReadoutModules.size(); }
        Int_t GetNumberOfChannels( );

        Double_t GetX( Int_t modID, Int_t chID );
        Double_t GetY( Int_t modID, Int_t chID );
        
        TH2Poly *GetReadoutHistogram( );
        void GetBoundaries(double &xmin,double &xmax,double &ymin,double &ymax);
        
        //Construtor
        TRestReadoutPlane();
        //Destructor
        virtual ~ TRestReadoutPlane();


        ClassDef(TRestReadoutPlane, 1);     // REST run class
};
#endif
