///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalTo2DHitsProcess.cxx
///
///_______________________________________________________________________________


#include "TRestRawSignalTo2DHitsProcess.h"
using namespace std;

ClassImp(TRestRawSignalTo2DHitsProcess)
    //______________________________________________________________________________
TRestRawSignalTo2DHitsProcess::TRestRawSignalTo2DHitsProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestRawSignalTo2DHitsProcess::~TRestRawSignalTo2DHitsProcess()
{
    delete fOutputSignalEvent;
    delete fInputSignalEvent;
}

//______________________________________________________________________________
void TRestRawSignalTo2DHitsProcess::Initialize()
{
    // We define the section name (by default we use the name of the class)
    SetSectionName( this->ClassName() );

    // We create the input/output specific event data
    fInputSignalEvent = new TRestRawSignalEvent();
    fOutputSignalEvent = new TRest2DHitsEvent();

    // We connect the TRestEventProcess input/output event pointers
    fInputEvent = fInputSignalEvent;
    fOutputEvent = fOutputSignalEvent;
}


void TRestRawSignalTo2DHitsProcess::InitProcess() 
{
	fReadout = (TRestReadout*)GetReadoutMetadata();
	if (fReadout != NULL)
		fOutputSignalEvent->SetReadout(fReadout);
}

//______________________________________________________________________________
TRestEvent* TRestRawSignalTo2DHitsProcess::ProcessEvent( TRestEvent *evInput )
{
    fInputSignalEvent = (TRestRawSignalEvent *) evInput;

	if (fInputSignalEvent->GetNumberOfSignals() <= 0) return fOutputEvent;

	if (fReadout == NULL) {
		error << "Lack readout definition! Cannot process..." << endl;
		return NULL;
	}
	else
	{
		fOutputSignalEvent->Initialize();
		fOutputSignalEvent->SetEventInfo(fInputSignalEvent);

		for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++) 
		{
			TRestRawSignal*s = fInputSignalEvent->GetSignal(n);
			fOutputSignalEvent->AddSignal(s);
		}
		//cout << fOutputSignalEvent->GetNumberOfSignals() << endl;
	}

    return fOutputSignalEvent;
}

void TRestRawSignalTo2DHitsProcess::InitFromConfigFile( )
{
    fMyDummyParameter = StringToInteger( GetParameter( "aDummyParameter" ) );
}

