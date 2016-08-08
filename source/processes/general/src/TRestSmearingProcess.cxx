///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSmearingProcess.cxx
///
///             Template to use to design "event process" classes inherited from 
///             TRestSmearingProcess
///             How to use: replace TRestSmearingProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________


#include "TRestSmearingProcess.h"
using namespace std;

ClassImp(TRestSmearingProcess)
//______________________________________________________________________________
TRestSmearingProcess::TRestSmearingProcess()
{
    Initialize();

}

//______________________________________________________________________________
TRestSmearingProcess::TRestSmearingProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );

    PrintMetadata();

   // TRestSmearingProcess default constructor
}

//______________________________________________________________________________
TRestSmearingProcess::~TRestSmearingProcess()
{
    delete fHitsInputEvent;
    delete fHitsOutputEvent;
   // TRestSmearingProcess destructor
}

void TRestSmearingProcess::LoadDefaultConfig()
{
    	SetTitle( "Default config" );

	fEnergyRef = 5.9;	
	fResolutionAtEref = 15.0; 

}

//______________________________________________________________________________
void TRestSmearingProcess::Initialize()
{
    SetName( "smearingProcess" );

    fGas = NULL;

    fEnergyRef = 5.9;	
    fResolutionAtEref = 15.0; 

    fWvalue = 0;

    fHitsInputEvent = new TRestHitsEvent();
    fHitsOutputEvent = new TRestHitsEvent();

    fOutputEvent = fHitsOutputEvent;
    fInputEvent = fHitsInputEvent;

    fRandom = new TRandom3(0);
}

void TRestSmearingProcess::LoadConfig( string cfgFilename, string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );

    PrintMetadata();

    fGas = (TRestGas *) GetGasMetadata();
}

//______________________________________________________________________________
void TRestSmearingProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();

    if( fGas == NULL )
    {
        cout << "REST ERRORRRR : Gas has not been initialized" << endl;
        fWvalue = 1;
    }
    else
    {
        if( fGasPressure == -1 ) 
            fGasPressure = fGas->GetPressure();
        fGas->SetPressure( fGasPressure );

        if( fWvalue == 0 )
            fWvalue = fGas->GetWvalue();
    }

    cout << __PRETTY_FUNCTION__ << endl;

}

//______________________________________________________________________________
void TRestSmearingProcess::BeginOfEventProcess() 
{
    fHitsOutputEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestSmearingProcess::ProcessEvent( TRestEvent *evInput )
{

    fHitsInputEvent = (TRestHitsEvent *) evInput;

    Double_t eDep = fHitsInputEvent->GetTotalEnergy() * fWvalue / 1000.0;
    Double_t eRes = fResolutionAtEref * TMath::Sqrt(fEnergyRef / eDep) / 2.35 / 100.0;

    Double_t gain = fRandom->Gaus(1.0, eRes);
    for( int hit = 0; hit < fHitsInputEvent->GetNumberOfHits(); hit++ )
        fHitsOutputEvent->AddHit( fHitsInputEvent->GetX(hit), fHitsInputEvent->GetY(hit), fHitsInputEvent->GetZ(hit), fHitsInputEvent->GetEnergy(hit) * gain );   

    return fHitsOutputEvent;
}

//______________________________________________________________________________
void TRestSmearingProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestSmearingProcess::EndProcess()
{
   // Function to be executed once at the end of the process 
   // (after all events have been processed)

   //Start by calling the EndProcess function of the abstract class. 
   //Comment this if you don't want it.
   //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestSmearingProcess::InitFromConfigFile( )
{
    fGasPressure = StringToDouble( GetParameter( "gasPressure", "-1" ) );
    fEnergyRef = GetDblParameterWithUnits( "energyReference"  );
    fResolutionAtEref = StringToDouble( GetParameter( "resolutionReference" ) );
    fWvalue = GetDblParameterWithUnits( "Wvalue" , 0) * REST_Units::eV;
}
