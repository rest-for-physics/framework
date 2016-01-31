///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutChannel.cxx
///
///             Base class for managing run data storage. It contains a TRestEvent and TRestMetadata array. 
///
///             apr 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///             aug 2015    Javier Galan
///_______________________________________________________________________________


#include "TRestReadoutChannel.h"
using namespace std;

ClassImp(TRestReadoutChannel)
//______________________________________________________________________________
    TRestReadoutChannel::TRestReadoutChannel()
{
    Initialize();

}


//______________________________________________________________________________
TRestReadoutChannel::~TRestReadoutChannel()
{
 //   cout << "Deleting TRestReadoutChannel" << endl;
}


void TRestReadoutChannel::Initialize()
{
    fChannelID = -1;
}

TRestReadoutPixel *TRestReadoutChannel::GetPixelByID( int id )
{
    Int_t pxNumber = -1;
    for( unsigned int i = 0; i < fReadoutPixel.size(); i++ )
        if( fReadoutPixel[i].GetID() == id )
        {
            if( pxNumber != -1 ) cout << "REST Warning : Found several pixels with the same ID" << endl;
            pxNumber = i;
        }
    if( pxNumber != -1 )
        return &fReadoutPixel[pxNumber];

    cout << "REST Warning : Readout pixel with ID : " << id << " not found in channel : " << fChannelID << endl;

    return NULL; 
}

void TRestReadoutChannel::PrintReadoutChannel( )
{
        cout << "++++ Channel ID : " << GetID( ) << " Total pixels : " << GetNumberOfPixels() << endl;
        cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

        for( int n = 0; n < GetNumberOfPixels(); n++ )
        {
            GetPixel(n)->PrintReadoutPixel();
        }
}
