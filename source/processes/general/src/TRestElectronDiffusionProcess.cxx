///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestElectronDiffusionProcess.cxx
///
///
///             First implementation of electron diffusion process into REST_v2
///             Date : Sep/2015
///             Author : J. Galan
///
///_______________________________________________________________________________


#include "TRestElectronDiffusionProcess.h"
using namespace std;

ClassImp(TRestElectronDiffusionProcess)
    //______________________________________________________________________________
TRestElectronDiffusionProcess::TRestElectronDiffusionProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestElectronDiffusionProcess::TRestElectronDiffusionProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestElectronDiffusionProcess::~TRestElectronDiffusionProcess()
{
    delete fOutputHitsEvent;
    delete fInputHitsEvent;
}

void TRestElectronDiffusionProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );

    fElectricField = 1000;
    fAttachment = 0;
    fGasPressure = 1;
}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fElectricField = 0;
    fAttachment = 0;
    fGasPressure = 1;

    fTransDiffCoeff = 0;
    fLonglDiffCoeff = 0;
    fWvalue = 0;

    fOutputHitsEvent = new TRestHitsEvent();
    fInputHitsEvent = new TRestHitsEvent();

    fOutputEvent = fOutputHitsEvent;
    fInputEvent = fInputHitsEvent;

    fGas = NULL;
    fReadout = NULL;

    fRandom = new TRandom3(0);
}

void TRestElectronDiffusionProcess::LoadConfig( string cfgFilename, string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::InitProcess()
{
    fGas = (TRestGas *) GetGasMetadata( );
    if( fGas == NULL )
    {
        cout << "REST WARNING : Gas has not been initialized" << endl;
    }
    else
    {
        if( fGasPressure == -1 ) 
            fGasPressure = fGas->GetPressure();
        fGas->SetPressure( fGasPressure );

        if( fWvalue == 0 )
            fWvalue = fGas->GetWvalue();

        if( fLonglDiffCoeff == 0 )
            fLonglDiffCoeff = fGas->GetLongitudinalDiffusion( fElectricField ); // (cm)^1/2

        if( fTransDiffCoeff == 0 )
            fTransDiffCoeff = fGas->GetTransversalDiffusion( fElectricField ); // (cm)^1/2
    }

    fReadout = (TRestReadout *) GetReadoutMetadata();
    if( fReadout == NULL )
    {
        cout << "REST ERRORRRR : Readout has not been initialized" << endl;
        exit(-1);
    }
}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::BeginOfEventProcess() 
{
    fOutputHitsEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestElectronDiffusionProcess::ProcessEvent( TRestEvent *evInput )
{

    TRestHitsEvent *inputHitsEvent = (TRestHitsEvent *) evInput;

    Int_t nHits = inputHitsEvent->GetNumberOfHits();
    if( nHits <= 0 ) return NULL;

    Int_t isAttached;

    for( int n = 0; n < nHits; n++ )
    {
        TRestHits *hits = inputHitsEvent->GetHits();

        Double_t eDep = hits->GetEnergy(n);

        if( eDep > 0 )
        {
            const Double_t x = hits->GetX(n);
            const Double_t y = hits->GetY(n);
            const Double_t z = hits->GetZ(n);

            for( int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++ )
            {
                TRestReadoutPlane *plane = fReadout->GetReadoutPlane( p );

                if ( plane->isInsideDriftVolume( x, y, z ) >= 0 )
                {
                    Double_t xDiff, yDiff, zDiff;

                    Double_t driftDistance = plane->GetDistanceTo( x, y, z );

                    Int_t numberOfElectrons = (Int_t) (eDep*1000./fWvalue);
                    while( numberOfElectrons > 0 )
                    {
                        numberOfElectrons--;

                        Double_t longHitDiffusion = 10. * TMath::Sqrt( driftDistance/10. ) * fLonglDiffCoeff; //mm

                        Double_t transHitDiffusion = 10. * TMath::Sqrt( driftDistance/10. ) * fTransDiffCoeff; //mm

                        if (fAttachment)
                            isAttached =  (fRandom->Uniform(0,1) > pow(1-fAttachment, driftDistance/10. ) );
                        else
                            isAttached = 0;

                        if ( isAttached == 0)
                        {
                            xDiff = x + fRandom->Gaus( 0, transHitDiffusion );

                            yDiff = y + fRandom->Gaus( 0, transHitDiffusion );

                            zDiff = z + fRandom->Gaus( 0, longHitDiffusion );

                            fOutputHitsEvent->AddHit( xDiff, yDiff, zDiff, 1. );
                        }
                    }

                }
            }
        }
    }

    if( fOutputHitsEvent->GetNumberOfHits() == 0 ) return NULL;

    if( this->GetVerboseLevel() >= REST_Debug ) 
    {
        cout << "TRestElectronDiffusionProcess. Hits added : " << fOutputHitsEvent->GetNumberOfHits() << endl;
        cout << "TRestElectronDiffusionProcess. Hits total energy : " << fOutputHitsEvent->GetEnergy() << endl;
    }

    return fOutputHitsEvent;
}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::InitFromConfigFile( )
{
    // TODO add pressure units
    fGasPressure = StringToDouble( GetParameter( "gasPressure", "-1" ) );
    fElectricField = GetDblParameterWithUnits( "electricField", 1000 );
    fAttachment = StringToDouble( GetParameter( "attachment", "0" ) );
    fLonglDiffCoeff = StringToDouble( GetParameter( "longitudinalDiffusionCoefficient" , "0") );
    fTransDiffCoeff = StringToDouble( GetParameter( "transversalDiffusionCoefficient" , "0") );
    fWvalue = GetDblParameterWithUnits( "Wvalue" , 0) * REST_Units::eV;
}
