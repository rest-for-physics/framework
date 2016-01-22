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
///             Jan 2016:   First concept
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestReadoutMapping
#define RestCore_TRestReadoutMapping

#include <iostream>
using namespace std;

#include <TObject.h>
#include <TMatrixD.h>


class TRestReadoutMapping: public TObject {

    private:

        Int_t fNodesX;
        Int_t fNodesY;

        Double_t fNetSizeX;
        Double_t fNetSizeY;
        
        TMatrixD fChannel;
        TMatrixD fPixel;
        
    public:
	
        Int_t GetNumberOfNodesX( ) { return fNodesX; }
        Int_t GetNumberOfNodesY( ) { return fNodesY; }
        //Getters
        Bool_t isNodeSet( Int_t i, Int_t j );
        Bool_t AllNodesSet( );
        Int_t GetNumberOfNodesNotSet( )
        {
            Int_t counter = 0;
            for( int i = 0; i < fNodesX; i++ )
                for( int j = 0; j < fNodesY; j++ )
                {
                    if( !isNodeSet( i, j ) ) counter++;
                }
            return counter;

        }

        Int_t GetNodeX_ForChannelAndPixel( Int_t ch, Int_t px )
        {
            for( int i = 0; i < fNodesX; i++ )
                for( int j = 0; j < fNodesY; j++ )
                {
                    if( fChannel[i][j] == ch && fPixel[i][j] == px ) return i;
                }
            return -1;
        }

        Int_t GetNodeY_ForChannelAndPixel( Int_t ch, Int_t px )
        {
            for( int i = 0; i < fNodesX; i++ )
                for( int j = 0; j < fNodesY; j++ )
                {
                    if( fChannel[i][j] == ch && fPixel[i][j] == px ) return j;
                }
            return -1;
        }
        
        //! Gets the nodeX index corresponding to the x coordinate
        Int_t GetNodeX( Double_t x );
        //! Gets the nodeY index corresponding to the y coordinate
        Int_t GetNodeY( Double_t y );

        //! Gets the X position of node using nodeX index
        Double_t GetX( Int_t nodeX );
        //! Gets the Y position of node using nodeY index
        Double_t GetY( Int_t nodeY );
        
        //! Gets the readout channel number corresponding to coordinates (x,y) 
        Int_t GetChannel( Double_t x, Double_t y );

        Int_t GetChannelByNode( Int_t i, Int_t j ) { return fChannel[i][j]; }
        Int_t GetPixelByNode( Int_t i, Int_t j ) { return fPixel[i][j]; }
        
        //! Gets the readout pixel number corresponding to coordinates (x,y) 
        Int_t GetPixel( Double_t x, Double_t y );

        //Setters

        //! Sets the readout channel and pixel corresponding to a mapping node
        void SetNode( Int_t i, Int_t j, Int_t ch, Int_t pix );

        void Initialize( Int_t nX, Int_t nY, Double_t sX, Double_t sY );
                
        //Construtor
        TRestReadoutMapping();
        //Destructor
        ~TRestReadoutMapping();
        
        ClassDef(TRestReadoutMapping, 1);

};
#endif



