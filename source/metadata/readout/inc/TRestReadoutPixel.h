///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutPixel.h
///
///             Base class from which to inherit all other event classes in REST 
///
///             apr 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestReadoutPixel
#define RestCore_TRestReadoutPixel

#include <iostream>
using namespace std;

#include "TObject.h"
#include <TVector2.h>
#include <TMath.h>

class TRestReadoutPixel : public TObject {
    private:
        Int_t fPixelID;
        Double_t fPixelOriginX;
        Double_t fPixelOriginY;

        Double_t fPixelSizeX;
        Double_t fPixelSizeY;

        Double_t fRotation;

        void Initialize();

    protected:

    public:
        Int_t GetID() { return fPixelID; }

        Double_t GetOriginX( ) const { return fPixelOriginX; }
        Double_t GetOriginY( ) const { return fPixelOriginY; }
        TVector2 GetOrigin( ) const { return TVector2( fPixelOriginX, fPixelOriginY ); }

        Double_t GetRotation( ) const { return fRotation; }

        Double_t GetSizeX( ) { return fPixelSizeX; }
        Double_t GetSizeY( ) { return fPixelSizeY; }

        TVector2 GetCenter( ) const;

        TVector2 GetVertex( int n ) const;

        void SetID( Int_t id ) { fPixelID = id; }

        void SetOrigin( Double_t x, Double_t y ) { fPixelOriginX = x; fPixelOriginY = y; }
        void SetOrigin( TVector2 origin ) { fPixelOriginX = origin.X(); fPixelOriginY = origin.Y(); }

        void SetSize( Double_t x, Double_t y ) { fPixelSizeX = x; fPixelSizeY = y; }
        void SetSize( TVector2 size ) { fPixelSizeX = size.X(); fPixelSizeY = size.Y(); }

        void SetRotation( Double_t rot ) { fRotation = rot; }
        
        Bool_t isInside( TVector2 pos );
        Bool_t isInside( Int_t x, Int_t y );

        TVector2 TransformToPixelCoordinates( TVector2 p );

        
        void PrintReadoutPixel( );

        //Construtor
        TRestReadoutPixel();
        //Destructor
        virtual ~ TRestReadoutPixel();


        ClassDef(TRestReadoutPixel, 1);     // REST run class
};
#endif
