//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file radioactivedecay/rdecay01/src/EventAction.cc
/// \brief Implementation of the EventAction class
//
// $Id: EventAction.cc 68030 2013-03-13 13:51:27Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "EventAction.hh"

#include "G4Event.hh"
#include <iomanip>

#include "Randomize.hh"

#include <TRestRun.h>

extern TRestRun *restRun;
extern TRestG4Metadata *restG4Metadata;
extern TRestG4Event *restG4Event;
extern TRestG4Track *restTrack;

extern double eMinROI;
extern double eMaxROI;

#include <fstream>
using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
:G4UserEventAction()
{
    restG4Metadata->isFullChainActivated();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* evt)
{
    if( evt->GetEventID() % 50000 == 0 ) cout << "Starting event : " << evt->GetEventID() << endl;

    restTrack->Initialize();

 //   G4cout << "Event id : " << evt->GetEventID() << G4endl;
    restG4Event->SetEventID( evt->GetEventID() );
    restG4Event->SetOK( true );
    time_t systime = time(NULL);
 //   G4cout << "Event time : " << (Double_t) systime << G4endl;
    restG4Event->SetEventTime( (Double_t) systime );

 //   G4cout << "Number of tracks : " << restG4Event->GetNumberOfTracks() << G4endl;


    // Defining if the hits in a given volume will be stored

    for( int i = 0; i < restG4Metadata->GetNumberOfActiveVolumes(); i++ )
    {
        Double_t rndNumber = G4UniformRand();
        //G4cout << "Storage chance volume : " << i << " chance : " <<  restG4Metadata->GetStorageChance(i) << G4endl;

        if ( restG4Metadata->GetStorageChance(i) >= rndNumber )
        {
             restG4Event->ActivateVolumeForStorage( i );
            //  G4cout << "Activating volume : " << restG4Metadata->GetActiveVolumeName( i ) << G4endl;
        }
        else
        {
             restG4Event->DisableVolumeForStorage( i );
   //          G4cout << "Disabling volume : " << restG4Metadata->GetActiveVolumeName( i ) << G4endl;
        }
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* evt)
{

    G4int evtNb = evt->GetEventID(); 
    if ( restG4Metadata->GetVerboseLevel() >= REST_Info ) cout << "End of event " << evtNb << endl;
    if( restG4Metadata->GetVerboseLevel() >= REST_Info ) 
    {
        restG4Event->PrintEvent();
        cout << "End of event : " << evtNb << endl;
    }

    /*
    for( int i = 0; i < restG4Event->GetNumberOfTracks(); i++ )
    {
        G4cout << "Particle track : " << restG4Event->GetTrack( i ).GetParticleName() << G4endl;
        G4cout << "Length : " << restG4Event->GetTrack(i).GetTrackLength() << " mm" << G4endl;
    }
    */


    Double_t totEnergy = restG4Event->GetTotalSensitiveVolumeEnergy();
    if( totEnergy > restG4Metadata->GetMinimumEnergyStored() &&
            totEnergy < restG4Metadata->GetMaximumEnergyStored() )
    {
        SetSubeventIDs();

   //     G4cout << "Storing event " << G4endl;

        fDepositSpectrum->Fill( totEnergy );
        if( totEnergy > eMinROI && totEnergy < eMaxROI )
            fDepositSpectrum_ROI->Fill( totEnergy );
        restRun->GetOutputEventTree( )->Fill();
    }
}

void EventAction::SetSubeventIDs()
{
    Int_t nTracks = restG4Event->GetNumberOfTracks();

    //fTrackTimestampList.push_back(0)
    cout.precision(20);
    for( int n = 0; n < nTracks; n++ )
    {
        Double_t trkTime = restG4Event->GetTrack(n)->GetGlobalTrackTime();
        modf( trkTime, &trkTime); // we remove the decimal part (we keep only seconds)

        Int_t Ifound = 0;
        for( unsigned int id = 0; id < fTrackTimestampList.size(); id++ )
        {
            if( fTrackTimestampList[id] == trkTime ) Ifound = id;
        }

        if ( Ifound == 0 ) fTrackTimestampList.push_back( trkTime );
    }

    for( unsigned int id = 0; id < fTrackTimestampList.size(); id++ )
    {
        for( int n = 0; n < nTracks; n++ )
        {
            Double_t trkTime = restG4Event->GetTrack(n)->GetGlobalTrackTime();
            modf( trkTime, &trkTime); // we remove the decimal part (we keep only seconds)

            if( trkTime == fTrackTimestampList[id] ) { restG4Event->SetTrackSubEventID( n, id ); }
 //           G4cout << "SubEvID : " << id << " --> " << fTrackTimestampList[id] << G4endl;
        }
    }

    /*
        for( int n = 0; n < nTracks; n++ )
        {
            G4cout << " Track n : " << restG4Event->GetTrack(n).GetTrackID() << " subEvtID : " << restG4Event->GetTrack(n).GetSubEventID( ) << G4endl;

        }
        */


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


