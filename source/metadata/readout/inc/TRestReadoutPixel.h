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

        /*
            TVector2 pixPosition( GetChannel( channel )->GetPixel(pixel)->GetOrigin() );
            pixPosition.Rotate( fModuleRotation * TMath::Pi()/ 180. );
            return pixPosition;
            */

        TVector2 GetVertex( int n ) const 
        {
            TVector2 vertex( 0, 0 );
            TVector2 origin( fPixelOriginX, fPixelOriginY );

            if( n%4 == 0 ) return origin;
            else if( n%4 == 1 )
            {
                vertex.Set( fPixelSizeX, 0 );
                vertex = vertex.Rotate( fRotation * TMath::Pi()/180. ); 

                vertex = vertex + origin;
            }
            else if( n%4 == 2 )
            {
                vertex.Set( fPixelSizeX, fPixelSizeY );
                vertex = vertex.Rotate( fRotation * TMath::Pi()/180. ); 

                vertex = vertex + origin;
            }
            else if( n%4 == 3 )
            {
                vertex.Set( 0, fPixelSizeY );
                vertex = vertex.Rotate( fRotation * TMath::Pi()/180. ); 

                vertex = vertex + origin;
            }
            return vertex;
        }

        Double_t GetSizeX( ) { return fPixelSizeX; }
        Double_t GetSizeY( ) { return fPixelSizeY; }

        void SetID( Int_t id ) { fPixelID = id; }

        void SetOrigin( Double_t x, Double_t y ) { fPixelOriginX = x; fPixelOriginY = y; }
        void SetOrigin( TVector2 origin ) { fPixelOriginX = origin.X(); fPixelOriginY = origin.Y(); }

        void SetSize( Double_t x, Double_t y ) { fPixelSizeX = x; fPixelSizeY = y; }
        void SetSize( TVector2 size ) { fPixelSizeX = size.X(); fPixelSizeY = size.Y(); }

        void SetRotation( Double_t rot ) { fRotation = rot; }
        
        Bool_t isInside( Int_t x, Int_t y ) 
        {
            TVector2 pos(x,y);
            return isInside( pos );
        }

        TVector2 TransformToPixelCoordinates( TVector2 p )
        {
            TVector2 pos( p.X() - fPixelOriginX, p.Y() - fPixelOriginY );
            pos = pos.Rotate( -fRotation * TMath::Pi()/ 180. );
            return pos;
        }

        Bool_t isInside( TVector2 pos )
        {
            pos = TransformToPixelCoordinates( pos );

            if( pos.X() >= 0 && pos.X() <= fPixelSizeX )
                if( pos.Y() >= 0 && pos.Y() <= fPixelSizeY )
                    return true;

            return false;
        }
        
        void PrintReadoutPixel( );

 //       TRestReadoutPixel *GetPixelByID( int id );

        //Construtor
        TRestReadoutPixel();
        //Destructor
        virtual ~ TRestReadoutPixel();


        ClassDef(TRestReadoutPixel, 1);     // REST run class
};
#endif
