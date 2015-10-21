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
#include "TRestReadoutChannel.h"

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

        void Initialize();

    protected:

    public:
        // Setters
        void SetPlaneIndex( Int_t index ) { fPlaneIndex = index; }
        void SetModuleID( Int_t modID ) { fModuleID = modID; }
        void SetSize( Double_t sX, Double_t sY ) { fModuleSizeX = sX; fModuleSizeY = sY; }
        void SetSize( TVector2 s ) { fModuleSizeX = s.X(); fModuleSizeY = s.Y(); }
        void SetOrigin( Double_t x, Double_t y ) { fModuleOriginX = x; fModuleOriginY = y; }
        void SetOrigin( TVector2 c ) { fModuleOriginX = c.X(); fModuleOriginY = c.Y(); }
        void SetRotation( Double_t rot ) { fModuleRotation = rot; }

        Bool_t isInside( Double_t x, Double_t y )
        {
            /*
            if( x < fModuleOriginX-fModuleSizeX/2. || x >= fModuleOriginX+fModuleSizeX/2. ) return false;
            if( y < fModuleOriginY-fModuleSizeY/2. || x >= fModuleOriginY+fModuleSizeY/2. ) return false;
            */
            return true;
        }

        TVector2 GetPixelOrigin( Int_t channel, Int_t pixel ) 
        {
            TVector2 pixPosition( GetChannel( channel )->GetPixel(pixel)->GetOrigin() );
            pixPosition = pixPosition.Rotate( fModuleRotation * TMath::Pi()/ 180. );
            return pixPosition;
        }

        TVector2 GetPixelVertex( Int_t channel, Int_t pixel, Int_t vertex ) 
        {
            TVector2 pixPosition = GetChannel( channel )->GetPixel(pixel)->GetVertex( vertex );

            //cout << "X : " << pixPosition.X() << " Y : " << pixPosition.Y() << endl;
            //cout << "Rot : " << fModuleRotation << endl;
             
            pixPosition = pixPosition.Rotate( fModuleRotation * TMath::Pi()/ 180. );
            //cout << "X : " << pixPosition.X() << " Y : " << pixPosition.Y() << endl;
 //           cout << "Module origin : " << 
            pixPosition = pixPosition + TVector2( fModuleOriginX, fModuleOriginY );
            return pixPosition;
        }

	TVector2 GetPixelCenter( Int_t channel, Int_t pixel )
	{
		TVector2 corner1( GetPixelVertex( channel, pixel, 0 ) );
		TVector2 corner2( GetPixelVertex( channel, pixel, 2 ) );

		TVector2 center = (corner1+corner2)/2.;
		return center;
	}

        TVector2 GetVertex( int n ) const 
        {
            TVector2 vertex( 0, 0 );
            TVector2 origin( fModuleOriginX, fModuleOriginY );

            if( n%4 == 0 ) return origin;
            else if( n%4 == 1 )
            {
                vertex.Set( fModuleSizeX, 0 );
                vertex = vertex.Rotate( fModuleRotation * TMath::Pi()/180. ); 

                vertex = vertex + origin;
            }
            else if( n%4 == 2 )
            {
                vertex.Set( fModuleSizeX, fModuleSizeY );
                vertex = vertex.Rotate( fModuleRotation * TMath::Pi()/180. ); 

                vertex = vertex + origin;
            }
            else if( n%4 == 3 )
            {
                vertex.Set( 0, fModuleSizeY );
                vertex = vertex.Rotate( fModuleRotation * TMath::Pi()/180. ); 

                vertex = vertex + origin;
            }
            return vertex;
        }


        Int_t GetNumberOfChannels( ) { return fReadoutChannel.size(); }

        void AddChannel( TRestReadoutChannel &rChannel ) 
        {
            Int_t channelError = 0;

            for( int i = 0; i < rChannel.GetNumberOfPixels(); i++ )
            {
                Double_t oX = rChannel.GetPixel( i )->GetOriginX();
                Double_t oY = rChannel.GetPixel( i )->GetOriginY();
                Double_t sX = rChannel.GetPixel( i )->GetSizeX();
                Double_t sY = rChannel.GetPixel( i )->GetSizeY();

                if( oX < 0 || oY < 0 || oX + sX > fModuleSizeX || oY + sY > fModuleSizeY )
                {
                    cout << "REST Warning (AddChannel) pixel outside the module boundaries" << endl;
                    cout << "Pixel origin = (" << oX << " , " << oY << ")" << endl;
                    cout << "Pixel size = (" << sX << " , " << sY << ")" << endl;
                    channelError++;
                }
            }

            if( channelError == 0 ) fReadoutChannel.push_back( rChannel ); 
            else {
                cout << "REST Warning (AddChannel) : Channel ID " << rChannel.GetID() << " not Added. Found " << channelError << " pixels outside the module boundaries" << endl;
            }
        }

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
