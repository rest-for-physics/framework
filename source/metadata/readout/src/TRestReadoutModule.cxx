///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutModule.cxx
///
///             Base class for managing run data storage. It contains a TRestEvent and TRestMetadata array. 
///
///             apr 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///             aug 2015    Javier Galan
///_______________________________________________________________________________


#include "TRestReadoutModule.h"

ClassImp(TRestReadoutModule)
//______________________________________________________________________________
    TRestReadoutModule::TRestReadoutModule()
{
    Initialize();

}


//______________________________________________________________________________
TRestReadoutModule::~TRestReadoutModule()
{
 //   cout << "Deleting TRestReadoutModule" << endl;
}


void TRestReadoutModule::Initialize()
{
    fReadoutChannel.clear();
    fPlaneIndex = -1;
    fModuleID = -1;
    
    fModuleOriginX = 0;
    fModuleOriginY = 0;

    fModuleSizeX = 0;
    fModuleSizeY = 0;

    fModuleRotation = 0;
}

TRestReadoutChannel *TRestReadoutModule::GetChannelByID( int id )
{
    Int_t chNumber = -1;
    for( unsigned int i = 0; i < fReadoutChannel.size(); i++ )
        if( fReadoutChannel[i].GetID() == id )
        {
            if( chNumber != -1 ) cout << "REST Warning : Found several channels with the same ID" << endl;
            chNumber = i;
        }
    if( chNumber != -1 )
        return &fReadoutChannel[chNumber];

    cout << "REST Warning : Readout channel with ID : " << id << " not found in module : " << fModuleID << " (plane : " << fPlaneIndex << ")" << endl;

    return NULL; 
}

void TRestReadoutModule::Draw()
{

}

void TRestReadoutModule::PrintReadoutModule( )
{
        cout << "-- Readout module : " << GetModuleID( ) << " (Plane : " << GetPlaneIndex( ) << ")" << endl;
        cout << "----------------------------------------------------------------" << endl;
        cout << "-- Origin position : X = " << fModuleOriginX << " mm " << " Y : " << fModuleOriginY << " mm" << endl;
        cout << "-- Size : X = " << fModuleSizeX << " Y : " << fModuleSizeY << endl;
        cout << "-- Rotation : " << fModuleRotation << " degrees" << endl;
        cout << "-- Total channels : " << GetNumberOfChannels() << endl;
        cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

        for( int n = 0; n < GetNumberOfChannels(); n++ )
        {
            fReadoutChannel[n].PrintReadoutChannel();
        }



}
