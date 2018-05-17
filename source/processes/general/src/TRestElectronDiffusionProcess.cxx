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

    Int_t totalElectrons = inputHitsEvent->GetEnergy() * REST_Units::eV/fWvalue;

    Double_t wValue = fWvalue;
    if( fMaxHits > 0 && totalElectrons > fMaxHits ) 
        wValue = inputHitsEvent->GetEnergy() * REST_Units::eV / fMaxHits;

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

                if ( plane->isZInsideDriftVolume( z ) >= 0 )
                {
                    Double_t xDiff, yDiff, zDiff;

                    Double_t driftDistance = plane->GetDistanceTo( x, y, z );

                    Int_t numberOfElectrons = (Int_t) (eDep*REST_Units::eV/wValue);

                    if( numberOfElectrons == 0 && eDep > 0 )
                        numberOfElectrons = 1;

                    Double_t localWValue = eDep*REST_Units::eV/numberOfElectrons;
                    Double_t localEnergy = 0;

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

                            localEnergy +=  localWValue * REST_Units::keV / REST_Units::eV;
                            if( GetVerboseLevel() >= REST_Extreme )
                                cout << "Adding hit. x : " << xDiff << " y : " << yDiff << " z : " << zDiff << " en : " << localWValue * REST_Units::keV / REST_Units::eV << " keV" << endl;
                            fOutputHitsEvent->AddHit( xDiff, yDiff, zDiff, localWValue * REST_Units::keV / REST_Units::eV );
                        }
                    }
                }
            }
        }
    }

    if( fOutputHitsEvent->GetNumberOfHits() == 0 ) return NULL;


    if( this->GetVerboseLevel() >= REST_Debug ) 
    {
        cout << "TRestElectronDiffusionProcess. Input hits energy : " << inputHitsEvent->GetEnergy() << endl;
        cout << "TRestElectronDiffusionProcess. Hits added : " << fOutputHitsEvent->GetNumberOfHits() << endl;
        cout << "TRestElectronDiffusionProcess. Hits total energy : " << fOutputHitsEvent->GetEnergy() << endl;
        if( GetVerboseLevel() >= REST_Extreme )
            GetChar();
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
    fMaxHits = StringToInteger( GetParameter( "maxHits", "0" ) );
}
