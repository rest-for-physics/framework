// REST G4 main program. restG4.cc
//
// Author : J. Galan
// Date : Jul-2015
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#include "TRestRun.h"
#include "TRestG4Track.h"
#include "TRestG4Event.h"
#include "TRestGeometry.h"
#include "TRestG4Metadata.h"
#include "TRestPhysicsLists.h"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "G4SystemOfUnits.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm> 
using namespace std;

#include "string"

#include "TH1D.h"
#include "TH2D.h"

// We define rest objects that will be used in Geant4
TRestRun *restRun;
TRestG4Track *restTrack;
TRestG4Event *restG4Event, *subRestG4Event;
TRestG4Metadata *restG4Metadata;
TRestPhysicsLists *restPhysList;

#include <TGeoVolume.h>

const Int_t maxBiasingVolumes = 50;
Int_t biasing = 0;

// This histograms would be better placed inside TRestBiasingVolume
TH1D *biasingSpectrum[maxBiasingVolumes];
TH1D *angularDistribution[maxBiasingVolumes];
TH2D *spatialDistribution[maxBiasingVolumes];

TH1D initialEnergySpectrum;
TH1D initialAngularDistribution;

Int_t N_events;
char inputConfigFile[256];
char restG4Name[256];
char physListName[256];
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

int main(int argc,char** argv) {

    // {{{ Getting by argument the simulation config file 
    sprintf( inputConfigFile, "%s", "myConfig.rml");
    if( argc >= 2 ) sprintf( inputConfigFile, "%s", argv[1] );
    if( argc >= 3 ) sprintf( restG4Name, "%s", argv[2] );

    sprintf( physListName, "%s", "default" );
    // }}} 

    // {{{ Initializing REST classes
    restG4Metadata = new TRestG4Metadata( inputConfigFile, (string) restG4Name );
    restPhysList = new TRestPhysicsLists( inputConfigFile, (string) physListName );

    restRun = new TRestRun( inputConfigFile );
    restRun->SetRunTag( restG4Metadata->GetName() );
    restRun->OpenOutputFile();

    restG4Event = new TRestG4Event( );
    subRestG4Event = new TRestG4Event( );
    restRun->SetOutputEvent( subRestG4Event );

    restRun->AddMetadata( restG4Metadata );
    restRun->AddMetadata( restPhysList );
    restRun->PrintInfo();

    restTrack = new TRestG4Track( );
    // }}} 

    // {{{ Setting the biasing spectra histograms 
    biasing = restG4Metadata->GetNumberOfBiasingVolumes();
    for( int i = 0; i < biasing; i++ )
    {
        TString spctName = "Bias_Spectrum_" + TString( Form("%d",i) );
        TString angDistName = "Bias_Angular_Distribution_" + TString( Form("%d",i) );
        TString spatialDistName = "Bias_Spatial_Distribution_" + TString( Form("%d",i) );

        Double_t maxEnergy = restG4Metadata->GetBiasingVolume( i ).GetMaxEnergy();
        Double_t minEnergy = restG4Metadata->GetBiasingVolume( i ).GetMinEnergy();
        Int_t nbins = (Int_t) ( maxEnergy-minEnergy );

        Double_t biasSize = restG4Metadata->GetBiasingVolume( i ).GetBiasingVolumeSize();
        TString biasType = restG4Metadata->GetBiasingVolume( i ).GetBiasingVolumeType();

        cout << "Initializing biasing histogram : " << spctName << endl;
        biasingSpectrum[i] = new TH1D( spctName, "Biasing gamma spectrum", nbins, minEnergy, maxEnergy  );
        angularDistribution[i] = new TH1D( angDistName, "Biasing angular distribution", 150, 0, M_PI/2  );

        if( biasType  == "virtualSphere" )
            spatialDistribution[i] = new TH2D( spatialDistName, "Biasing spatial (virtualSphere) distribution ", 100, -M_PI, M_PI, 100, 0, M_PI  );
        else if( biasType  == "virtualBox" )
            spatialDistribution[i] = new TH2D( spatialDistName, "Biasing spatial (virtualBox) distribution", 100, -biasSize/2., biasSize/2., 100, -biasSize/2., biasSize/2. );
        else
            spatialDistribution[i] = new TH2D( spatialDistName, "Biasing spatial distribution", 100, -1, 1, 100, -1, 1  );
    }
    // }}}

    //choose the Random engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
    time_t systime = time(NULL);
    long seed = (long) systime + restRun->GetRunNumber()*13;
    CLHEP::HepRandom::setTheSeed(seed);

    // Construct the default run manager
    G4RunManager * runManager = new G4RunManager;

    // set mandatory initialization classes
    DetectorConstruction *det = new DetectorConstruction( );

    runManager->SetUserInitialization( det );
    runManager->SetUserInitialization(new PhysicsList( restPhysList ) );

    // set user action classes
    PrimaryGeneratorAction* prim  = new PrimaryGeneratorAction( det );

    if( restG4Metadata->GetParticleSource(0).GetEnergyDistType() == "TH1D" )
    {
        TString fileFullPath = (TString ) restG4Metadata->GetParticleSource(0).GetSpectrumFilename();

        TFile fin( fileFullPath );

        TString sptName = restG4Metadata->GetParticleSource(0).GetSpectrumName();

        TH1D *h = (TH1D *) fin.Get( sptName );;
        if( h == NULL ) 
        { 
            cout << "REST ERROR  when trying to find energy spectrum" << endl;
            cout << "File : " << fileFullPath << endl;
            cout << "Spectrum name : " << sptName << endl; 
            exit(1);
        }

        initialEnergySpectrum = *h;


        Double_t minEnergy = restG4Metadata->GetParticleSource(0).GetMinEnergy();
        if( minEnergy < 0 ) minEnergy = 0;

        Double_t maxEnergy = restG4Metadata->GetParticleSource(0).GetMaxEnergy();
        if( maxEnergy < 0 ) maxEnergy = 0;

        // We set the initial spectrum energy provided from TH1D
        prim->SetSpectrum( &initialEnergySpectrum, minEnergy, maxEnergy );
    }

    if( restG4Metadata->GetParticleSource(0).GetAngularDistType() == "TH1D" )
    {
        TString fileFullPath = (TString ) restG4Metadata->GetParticleSource(0).GetAngularFilename();

        TFile fin( fileFullPath );

        TString sptName = restG4Metadata->GetParticleSource(0).GetAngularName();
        TH1D *h = (TH1D *) fin.Get( sptName );;
        if( h == NULL ) 
        { 
            cout << "REST ERROR  when trying to find angular spectrum" << endl;
            cout << "File : " << fileFullPath << endl;
            cout << "Spectrum name : " << sptName << endl; 
            exit(1);
        }

        initialAngularDistribution = *h;

        // We set the initial angular distribution provided from TH1D
        prim->SetAngularDistribution( &initialAngularDistribution );
    }

    RunAction*              run   = new RunAction(prim);  

    EventAction*            event = new EventAction();  
    
    TrackingAction*         track = new TrackingAction(run,event);
    SteppingAction*         step = new SteppingAction( );

    runManager->SetUserAction(run);
    runManager->SetUserAction(prim);
    runManager->SetUserAction(event);    
    runManager->SetUserAction(track);
    runManager->SetUserAction(step);

    //Initialize G4 kernel
    runManager->Initialize();

    // get the pointer to the User Interface manager 
    G4UImanager* UI = G4UImanager::GetUIpointer();  

#ifdef G4VIS_USE
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
#endif

    N_events = restG4Metadata->GetNumberOfEvents();
    // We pass the volume definition to Stepping action so that it records gammas entering in
    // We pass also the biasing spectrum so that gammas energies entering the volume are recorded
    if( biasing )
    { step->SetBiasingVolume( restG4Metadata->GetBiasingVolume( biasing-1 ) );
        step->SetBiasingSpectrum( biasingSpectrum[biasing-1]  ); 
        step->SetAngularDistribution( angularDistribution[biasing-1]  ); 
        step->SetSpatialDistribution( spatialDistribution[biasing-1]  ); }

    cout << "Events : " << N_events << endl;
    if ( N_events > 0 )   // batch mode  
    { 
        G4String command = "/tracking/verbose 0";
        UI->ApplyCommand(command);  
        command = "/run/initialize";
        UI->ApplyCommand(command);  

        char tmp[256]; 
        sprintf( tmp, "/run/beamOn %d", N_events );

        command = tmp;
        UI->ApplyCommand(command);  

        //TH1D *spt = step->GetBiasingSpectrum();
        /*
           for( int i = 1; i <= 1200; i++ )
           if( biasingSpectrum[biasing-1]->GetBinContent( i ) > 0 )
           cout << i << " : " << biasingSpectrum[biasing-1]->GetBinContent( i ) << endl;

           cout << "Integral : " << biasingSpectrum[biasing-1]->Integral() << endl;
           */
        //      spt->Draw("same");

        //      biasingSpectrum[biasing-1]->Draw("same");
        //

        restRun->GetOutputFile()->cd();

        while( biasing )
        {
            // Definning isotropic gammas using the spectrum and angular distribution previously obtained
            restG4Metadata->RemoveSources();

            TRestParticleSource src;
            src.SetParticleName( "gamma" );
            src.SetEnergyDistType( "TH1D" );
            src.SetAngularDistType( "TH1D" );
            restG4Metadata->AddSource( src );
            
            // We set the spectrum from previous biasing volume inside the primary generator
            prim->SetSpectrum( biasingSpectrum[biasing-1] );
            // And we set the angular distribution
            prim->SetAngularDistribution( angularDistribution[biasing-1] );

            //src.SetAngularDistType( "distribution" );

            // We re-define the generator inside restG4Metadata to be launched from the biasing volume
            restG4Metadata->SetGeneratorType( restG4Metadata->GetBiasingVolume( biasing-1 ).GetBiasingVolumeType() );
            restG4Metadata->SetGeneratorSize( restG4Metadata->GetBiasingVolume( biasing-1 ).GetBiasingVolumeSize() );
            restG4Metadata->GetBiasingVolume( biasing-1 ).PrintBiasingVolume();

            // Definning biasing the number of event to be re-launched
            Double_t biasingFactor = restG4Metadata->GetBiasingVolume( biasing-1 ).GetBiasingFactor();
            cout << "Number of events that reached the biasing volume : " <<  (Int_t) ( biasingSpectrum[biasing-1]->Integral() ) << endl;
            cout << "Number of events to be launched : " <<  (Int_t) ( biasingSpectrum[biasing-1]->Integral() * biasingFactor ) << endl;

            sprintf( tmp, "/run/beamOn %d", (Int_t) ( biasingSpectrum[biasing-1]->Integral() * biasingFactor )  );

            restRun->GetOutputFile()->cd();
    
            biasingSpectrum[biasing-1]->Write();
            angularDistribution[biasing-1]->Write();
            spatialDistribution[biasing-1]->Write();

            command = tmp;

            biasing--;
            // We pass the volume definition to Stepping action so that it records gammas entering in
            // We pass also the biasing spectrum so that gammas energies entering the volume are recorded
            if( biasing )
            { step->SetBiasingVolume( restG4Metadata->GetBiasingVolume( biasing-1 ) );
                step->SetBiasingSpectrum( biasingSpectrum[biasing-1]  ); 
                step->SetAngularDistribution( angularDistribution[biasing-1] );  
                step->SetSpatialDistribution( spatialDistribution[biasing-1] );  }
            UI->ApplyCommand(command);  
        }
    }

    else           // define visualization and UI terminal for interactive mode 
    { 
        cout << "The number of events to be simulated is Zero!" << endl;
        cout << "Make sure you did not forget the number of events entry in TRestG4Metadata." << endl;
        cout << endl;
        cout << "<parameter name=\"Nevents\" value=\"100\"/>" << endl;
        cout << endl;
        cout << "Trying to enter vis mode" << endl;
#ifdef G4UI_USE
        G4UIExecutive * ui = new G4UIExecutive(argc,argv);      
#ifdef G4VIS_USE
        cout << "Executing G4 macro : /control/execute macros/vis.mac"  << endl;
        UI->ApplyCommand("/control/execute macros/vis.mac");          
#endif
        ui->SessionStart();
        delete ui;
#endif
    }
    restRun->GetOutputFile()->cd();

    /*
    initialEnergySpectrum.SetName("initialEnergySpectrum");
    initialAngularDistribution.SetName("initialAngularDistribution");

    initialEnergySpectrum.SetTitle( "Primary source energy spectrum" );
    initialAngularDistribution.SetTitle( "Primary source Theta angular distribution" );

    initialEnergySpectrum.Write();
    initialAngularDistribution.Write();
    */

#ifdef G4VIS_USE
    delete visManager;
#endif     

    // job termination
    //
    delete runManager;

    restRun->CloseOutputFile();

    TString Filename = restRun->GetOutputFilename();

    delete restRun;

    delete restG4Event;
    delete subRestG4Event;
    delete restTrack;

    // Writting the geometry in TGeoManager format to the ROOT file
    char originDirectory[256];
    sprintf( originDirectory, "%s", get_current_dir_name() );

    char buffer[256];
    sprintf( buffer, "%s", (char *) restG4Metadata->GetGeometryPath().Data() );
    chdir( buffer );

    TGeoManager *geo2 = new TGeoManager( );
    geo2->Import( restG4Metadata->Get_GDML_Filename() );
    
    // And coming back to origin directory
    chdir( originDirectory );

    TFile *f1 = new TFile( Filename, "update" );
    cout << "Writting geometry" << endl;
    f1->cd();
    geo2->Write();
    cout << "Closing file" << endl;
    f1->Close();

    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo..... 

