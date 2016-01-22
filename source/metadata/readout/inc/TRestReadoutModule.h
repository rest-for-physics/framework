///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutModule.h
///
///             Base class from which to inherit all other event classes in REST 
///
///             apr 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestReadoutModule
#define RestCore_TRestReadoutModule

#include <iostream>
using namespace std;

#include "TObject.h"
#include <TMath.h>

#include <TVector2.h>
#include <TRestReadoutChannel.h>
#include <TRestReadoutMapping.h>

class TRestReadoutModule : public TObject {
    private:
        Int_t fPlaneIndex;
        Int_t fModuleID;

        Double_t fModuleOriginX;
        Double_t fModuleOriginY;

        Double_t fModuleSizeX;
        Double_t fModuleSizeY;

        Double_t fModuleRotation;

        vector <TRestReadoutChannel> fReadoutChannel;

        TRestReadoutMapping fMapping;

        void Initialize();

        TVector2 TransformToModuleCoordinates( TVector2 p )
        {
            return TransformToModuleCoordinates( p.X(), p.Y() );
        }

        TVector2 TransformToModuleCoordinates( Double_t x, Double_t y )
        {
            TVector2 coords( x - fModuleOriginX, y - fModuleOriginY );
            TVector2 rot = coords.Rotate( -fModuleRotation * TMath::Pi()/ 180. );

            return rot;
        }

    protected:

    public:
        // Setters
        
        void DoReadoutMapping( );
        
        void SetPlaneIndex( Int_t index ) { fPlaneIndex = index; }
        void SetModuleID( Int_t modID ) { fModuleID = modID; }
        void SetSize( Double_t sX, Double_t sY ) { fModuleSizeX = sX; fModuleSizeY = sY; }
        void SetSize( TVector2 s ) { fModuleSizeX = s.X(); fModuleSizeY = s.Y(); }
        void SetOrigin( Double_t x, Double_t y ) { fModuleOriginX = x; fModuleOriginY = y; }
        void SetOrigin( TVector2 c ) { fModuleOriginX = c.X(); fModuleOriginY = c.Y(); }
        void SetRotation( Double_t rot ) { fModuleRotation = rot; }

        Bool_t isInside( Double_t x, Double_t y );
        Bool_t isInside( TVector2 pos );

        Bool_t isInsideChannel( Int_t channel, Double_t x, Double_t y );
        Bool_t isInsideChannel( Int_t channel, TVector2 pos );
        
        Bool_t isInsidePixel( Int_t channel, Int_t pixel, Double_t x, Double_t y );
        Bool_t isInsidePixel( Int_t channel, Int_t pixel, TVector2 pos );

        Int_t FindChannel( Double_t x, Double_t y );

        TVector2 GetPixelOrigin( Int_t channel, Int_t pixel );
        TVector2 GetPixelVertex( Int_t channel, Int_t pixel, Int_t vertex );
        TVector2 GetPixelCenter( Int_t channel, Int_t pixel );
        TVector2 GetVertex( int n ) const;

        Int_t GetNumberOfChannels( );

        void AddChannel( TRestReadoutChannel &rChannel );

        Int_t GetModuleID( ) { return fModuleID; }
        Int_t GetPlaneIndex( ) { return fPlaneIndex; }

        Double_t GetModuleOriginX() { return fModuleOriginX; }
        Double_t GetModuleOriginY() { return fModuleOriginY; }

        Double_t GetModuleSizeX() { return fModuleSizeX; }
        Double_t GetModuleSizeY() { return fModuleSizeY; }

        Double_t GetModuleRotation() { return fModuleRotation; }

        TRestReadoutChannel *GetChannelByID( int id );
        TRestReadoutChannel *GetChannel( int n ) { return &fReadoutChannel[n]; }

        void Draw();

        void PrintReadoutModule( );

        //Construtor
        TRestReadoutModule();
        //Destructor
        virtual ~ TRestReadoutModule();


        ClassDef(TRestReadoutModule, 1);     // REST run class
};
#endif
