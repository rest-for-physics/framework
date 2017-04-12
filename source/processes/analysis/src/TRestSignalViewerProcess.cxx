///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalViewerProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestSignalViewerProcess
///             Date : feb/2017
///             Author : J. Galan
///
///_______________________________________________________________________________

#include <TPaveText.h>
#include <TLegend.h>

#include "TRestSignalViewerProcess.h"
using namespace std;

int rawCounter3 = 0;

ClassImp(TRestSignalViewerProcess)
    //______________________________________________________________________________
TRestSignalViewerProcess::TRestSignalViewerProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestSignalViewerProcess::TRestSignalViewerProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestSignalViewerProcess::~TRestSignalViewerProcess()
{
    delete fSignalEvent;
}

void TRestSignalViewerProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestSignalViewerProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fSignalEvent = new TRestSignalEvent();

    fOutputEvent = fSignalEvent;
    fInputEvent = fSignalEvent;

    fDrawRefresh = 0;
}

void TRestSignalViewerProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestSignalViewerProcess::InitProcess()
{
    this->CreateCanvas();
}

//______________________________________________________________________________
void TRestSignalViewerProcess::BeginOfEventProcess() 
{
    fSignalEvent->Initialize();
}

//______________________________________________________________________________
TRestEvent* TRestSignalViewerProcess::ProcessEvent( TRestEvent *evInput )
{
    TString obsName;

    TRestSignalEvent *fInputSignalEvent = (TRestSignalEvent *) evInput;

    /// Copying the signal event to the output event

    fSignalEvent->SetID( fInputSignalEvent->GetID() );
    fSignalEvent->SetSubID( fInputSignalEvent->GetSubID() );
    fSignalEvent->SetTimeStamp( fInputSignalEvent->GetTimeStamp() );
    fSignalEvent->SetSubEventTag( fInputSignalEvent->GetSubEventTag() );

    //for( int sgnl = 0; sgnl < fInputSignalEvent->GetNumberOfSignals(); sgnl++ )
    Int_t N = fInputSignalEvent->GetNumberOfSignals();
   // if( GetVerboseLevel() >= REST_Debug ) N = 1;
    for( int sgnl = 0; sgnl < N; sgnl++ )
	fSignalEvent->AddSignal( *fInputSignalEvent->GetSignal( sgnl ) );
    /////////////////////////////////////////////////

    GetCanvas()->cd(); 
    GetCanvas()->SetGrid();
    rawCounter3++;
    if( rawCounter3 >= fDrawRefresh )
    {
	rawCounter3 = 0;
	for( unsigned int i = 0; i < fDrawingObjects.size(); i++ )
	    delete fDrawingObjects[i];
	fDrawingObjects.clear();

	//TPad *pad2 = fSignalEvent->DrawEvent();

	TPad *pad2 = DrawSignal(0);

	GetCanvas()->cd(); 
	GetCanvas()->SetGrid();

	pad2->Draw();
	pad2->cd();

	GetCanvas()->Update();

	if( GetVerboseLevel() >= REST_Debug )
	{
	    GetAnalysisTree()->PrintObservables();
	    cout << "TRestSignalViewerProcess. Place mouse cursor on top of canvas : " << this->GetTitle() << " and press a KEY to continue ... " << endl;
	    pad2->WaitPrimitive();
	}
    }

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestSignalViewerProcess::EndOfEventProcess() 
{
}

//______________________________________________________________________________
void TRestSignalViewerProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();

}


//______________________________________________________________________________
void TRestSignalViewerProcess::InitFromConfigFile( )
{
    fDrawRefresh = StringToDouble( GetParameter( "refreshEvery", "0" ) );

    fBaseLineRange = StringTo2DVector( GetParameter( "baseLineRange", "(5,55)") );
}

TPad *TRestSignalViewerProcess::DrawSignal( Int_t signal )
{
    TPad *pad = new TPad( this->GetName(), this->GetTitle(), 0, 0, 1, 1 );
    pad->cd();
    pad->DrawFrame( fSignalEvent->GetMinTime()-1, -10, fSignalEvent->GetMaxTime()+1, fSignalEvent->GetMaxValue() + 10, this->GetTitle() );
   
    TMultiGraph *mg = new TMultiGraph();
    mg->SetTitle(this->GetTitle());
    mg->GetXaxis()->SetTitle("time bins");
    mg->GetYaxis()->SetTitleOffset(1.4);
    mg->GetYaxis()->SetTitle("Energy");

    for( int n = 0; n < fSignalEvent->GetNumberOfSignals(); n++ )
    {
        TGraph *gr = fSignalEvent->GetSignal(n)->GetGraph( (n + 1)%6 );
        mg->Add(gr);
    }

     mg->Draw("");

    return pad;
}
