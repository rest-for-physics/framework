///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutPixel.cxx
///
///             Base class for managing run data storage. It contains a TRestEvent and TRestMetadata array. 
///
///             apr 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///             aug 2015    Javier Galan
///_______________________________________________________________________________


#include "TRestReadoutPixel.h"
using namespace std;

ClassImp(TRestReadoutPixel)
//______________________________________________________________________________
    TRestReadoutPixel::TRestReadoutPixel()
{
    Initialize();

}


//______________________________________________________________________________
TRestReadoutPixel::~TRestReadoutPixel()
{
 //   cout << "Deleting TRestReadoutPixel" << endl;
}


void TRestReadoutPixel::Initialize()
{
}

TVector2 TRestReadoutPixel::GetCenter( ) const
{ 
    TVector2 center( 0, 0 );
    TVector2 origin( fPixelOriginX, fPixelOriginY );
    TVector2 opositeVertex = GetVertex( 2 );

    center = (origin + opositeVertex)/2;

    return center;
}

TVector2 TRestReadoutPixel::GetVertex( int n ) const 
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

Bool_t TRestReadoutPixel::isInside( Int_t x, Int_t y ) 
{
    TVector2 pos(x,y);
    return isInside( pos );
}

TVector2 TRestReadoutPixel::TransformToPixelCoordinates( TVector2 p )
{
    TVector2 pos( p.X() - fPixelOriginX, p.Y() - fPixelOriginY );
    pos = pos.Rotate( -fRotation * TMath::Pi()/ 180. );
    return pos;
}

Bool_t TRestReadoutPixel::isInside( TVector2 pos )
{
    pos = TransformToPixelCoordinates( pos );

    if( pos.X() >= 0 && pos.X() <= fPixelSizeX )
        if( pos.Y() >= 0 && pos.Y() <= fPixelSizeY )
            return true;

    return false;
}

void TRestReadoutPixel::PrintReadoutPixel( )
{
    cout << "      ## Pixel ID : " << GetID() << " position : (" << GetOriginX() << "," << GetOriginY() << ") mm size : (" << GetSizeX() << "," << GetSizeY() << ") mm rotation : " << fRotation << " degrees" << endl;
}
