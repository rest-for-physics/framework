///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutMapping.cxx
///
///             Metadata readout class to map the readout using a uniform grid and optimize channel/pixel find function
///
///             Jan 2016:  
///                 Javier Galan
///_______________________________________________________________________________


#include "TRestReadoutMapping.h"

ClassImp(TRestReadoutMapping)
//______________________________________________________________________________
    TRestReadoutMapping::TRestReadoutMapping()
{
   // TRestReadoutMapping default constructor
}

//______________________________________________________________________________
TRestReadoutMapping::~TRestReadoutMapping()
{
   // TRestReadoutMapping destructor
}

//! Gets the X position of node (i,j)
Double_t TRestReadoutMapping::GetX( Int_t nodeX )
{
    return (fNetSizeX/fNodesX) * nodeX;

}

//! Gets the Y position of node (i,j)
Double_t TRestReadoutMapping::GetY( Int_t nodeY )
{
    return (fNetSizeY/fNodesY) * nodeY;

}

//! Gets the nodeX index corresponding to the x coordinate
Int_t TRestReadoutMapping::GetNodeX( Double_t x )
{
    return (Int_t) ( ( x / fNetSizeX ) * fNodesX );

}

//! Gets the nodeY index corresponding to the y coordinate
Int_t TRestReadoutMapping::GetNodeY( Double_t y )
{
    return (Int_t) ( ( y / fNetSizeY ) * fNodesY );

}

//! Gets the channel number corresponding to coordinates (x,y) 
Int_t TRestReadoutMapping::GetChannel( Double_t x, Double_t y )
{
    return fChannel[GetNodeX(x)][GetNodeY(y)];
}

//! Gets the pixel number corresponding to coordinates (x,y) 
Int_t TRestReadoutMapping::GetPixel( Double_t x, Double_t y )
{
    return fPixel[GetNodeX(x)][GetNodeY(y)];

}

void TRestReadoutMapping::Initialize( Int_t nX, Int_t nY, Double_t sX, Double_t sY )
{
    fNodesX = nX;
    fNodesY = nY;
    fNetSizeX = sX;
    fNetSizeY = sY;
    fChannel.ResizeTo( fNodesX, fNodesY );
    fPixel.ResizeTo( fNodesX, fNodesY );

    for( int i = 0; i < fNodesX; i++ )
        for( int j = 0; j < fNodesX; j++ )
        {
            fChannel[i][j] = -1;
            fPixel[i][j] = -1;
        }
}

//! Sets the readout channel and pixel corresponding to a mapping node
void TRestReadoutMapping::SetNode( Int_t i, Int_t j, Int_t ch, Int_t pix )
{
    fChannel[i][j] = ch;
    fPixel[i][j] = pix;
}

Bool_t TRestReadoutMapping::isNodeSet( Int_t i, Int_t j )
{
    if( fChannel[i][j] == -1 || fPixel[i][j] == -1 ) return false;
    return true;
}

Bool_t TRestReadoutMapping::AllNodesSet( )
{
    for( int i = 0; i < fNodesX; i++ )
        for( int j = 0; j < fNodesY; j++ )
        {
            if( !isNodeSet( i, j ) ) 
            { 
                cout << "Node : " << i << " , " << j << " is NOT set. Ch : " << fChannel[i][j] << " Pix : " << fPixel[i][j] << endl;
                return false; 
            }
        }
    return true;
}
