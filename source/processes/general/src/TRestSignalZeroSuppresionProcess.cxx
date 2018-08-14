///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalZeroSuppresionProcess.cxx
///
///             jan 2016:  Javier Galan
///_______________________________________________________________________________

#include <TRestDetectorSetup.h>

#include "TRestSignalZeroSuppresionProcess.h"
using namespace std;

const double cmTomm = 10.;

ClassImp(TRestSignalZeroSuppresionProcess)
    //______________________________________________________________________________
TRestSignalZeroSuppresionProcess::TRestSignalZeroSuppresionProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestSignalZeroSuppresionProcess::TRestSignalZeroSuppresionProcess( char *cfgFileName )
{
    Initialize();

    LoadConfig( cfgFileName );

    // TRestSignalZeroSuppresionProcess default constructor
}

//______________________________________________________________________________
TRestSignalZeroSuppresionProcess::~TRestSignalZeroSuppresionProcess()
{
    delete fRawSignalEvent;
    delete fSignalEvent;
    // TRestSignalZeroSuppresionProcess destructor
}

void TRestSignalZeroSuppresionProcess::LoadDefaultConfig( )
{
    SetName( "signalZeroSuppresionProcess-Default" );
    SetTitle( "Default config" );

    cout << "Signal to hits metadata not found. Loading default values" << endl;

    TVector2 fBaseLineRange = TVector2( 10, 90);
    fPointThreshold = 2.;
    fSignalThreshold = 2.;
    fNPointsOverThreshold = 10;

}

void TRestSignalZeroSuppresionProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );

}

//______________________________________________________________________________
void TRestSignalZeroSuppresionProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fRawSignalEvent = new TRestRawSignalEvent();
    fSignalEvent = new TRestSignalEvent();

    fInputEvent = fRawSignalEvent;
    fOutputEvent = fSignalEvent;
}

//______________________________________________________________________________
void TRestSignalZeroSuppresionProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();
}

//______________________________________________________________________________
void TRestSignalZeroSuppresionProcess::BeginOfEventProcess() 
{
    fSignalEvent->Initialize(); 
}


//______________________________________________________________________________
TRestEvent* TRestSignalZeroSuppresionProcess::ProcessEvent( TRestEvent *evInput )
{
    fRawSignalEvent = (TRestRawSignalEvent *) evInput;

    if( GetVerboseLevel() >= REST_Debug )
        fSignalEvent->PrintEvent();

    fSignalEvent->SetID( fRawSignalEvent->GetID() );
    fSignalEvent->SetSubID( fRawSignalEvent->GetSubID() );
    fSignalEvent->SetTimeStamp( fRawSignalEvent->GetTimeStamp() );
    fSignalEvent->SetSubEventTag( fRawSignalEvent->GetSubEventTag() );

    Int_t numberOfSignals = fRawSignalEvent->GetNumberOfSignals();

    Double_t totalIntegral = 0;
    for( int i = 0; i < numberOfSignals; i++ )
    {
        TRestRawSignal *s = fRawSignalEvent->GetSignal( i );
		TRestSignal sgn;

		sgn.SetID(s->GetID());
		double baseline = s->GetBaseLine(fBaseLineRange.X(), fBaseLineRange.Y());
		double baselinerms = s->GetBaseLineSigma(fBaseLineRange.X(), fBaseLineRange.Y());
		for (int i = fIntegralRange.X(); i < fIntegralRange.Y(); i++)
		{
			if (s->GetData(i) > baseline + fPointThreshold * baselinerms) {
				int pos = i;
				vector<double> pulse;
				pulse.push_back(s->GetData(i));
				i++;
				int flatN = 0;
				while (i < fIntegralRange.Y() && s->GetData(i) > baseline + fPointThreshold * baselinerms) {
					if (TMath::Abs(s->GetData(i) - s->GetData(i - 1)) > fPointThreshold * baselinerms)
					{
						flatN = 0;
					}
					else
					{
						flatN++;
					}
					
					if (flatN < fNPointsFlatThreshold) 
					{
						pulse.push_back(s->GetData(i));
						i++;
					}
					else
					{
						break;
					}
				}
				if (pulse.size() > fNPointsOverThreshold) {
					auto stdev = TMath::StdDev(begin(pulse), end(pulse));
					if (stdev > fSignalThreshold*baselinerms) {
						for (int j = pos; j < i; j++)
						{
							sgn.NewPoint(j, s->GetData(j));
						}
					}
				}
			}
		}

		if(sgn.GetNumberOfPoints()>0)
			fSignalEvent->AddSignal(sgn);

/*
        Double_t integral = sgnl->GetIntegralWithThreshold( 0, sgnl->GetNumberOfPoints(),
                fBaseLineRange.X(), fBaseLineRange.Y(), fPointThreshold, fNPointsOverThreshold, fSignalThreshold );

        if( integral > 0 )
        {
            totalIntegral += integral;

            vector <Int_t> poinsOver = sgnl->GetPointsOverThreshold();

            TRestSignal outSignal;
            outSignal.SetID( signalID );
            for( unsigned int n = 0; n < poinsOver.size(); n++ )
                outSignal.NewPoint( poinsOver[n], sgnl->GetData( poinsOver[n] ) );

            fSignalEvent->AddSignal( outSignal );
        }*/
    }

    if( this->GetVerboseLevel() >= REST_Debug ) 
    {
        fSignalEvent->PrintEvent();
        cout << "TRestSignalZeroSuppresionProcess. Signals added : " << fSignalEvent->GetNumberOfSignals() << endl;
        cout << "TRestSignalZeroSuppresionProcess. Threshold integral : " << totalIntegral << endl;
        GetChar();
    }

    if( fSignalEvent->GetNumberOfSignals() <= 0 ) return NULL;

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestSignalZeroSuppresionProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestSignalZeroSuppresionProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestSignalZeroSuppresionProcess::InitFromConfigFile( )
{
	//keep up with TRestRawSignalAnalysisProcess
	fBaseLineRange = StringTo2DVector(GetParameter("baseLineRange", "(5,55)"));
	fIntegralRange = StringTo2DVector(GetParameter("integralRange", "(10,500)"));
    fPointThreshold = StringToDouble( GetParameter( "pointThreshold", "2" ) );
    fNPointsOverThreshold = StringToInteger( GetParameter( "pointsOverThreshold", "5" ) );
    fSignalThreshold = StringToDouble( GetParameter( "signalThreshold", "5" ) );

	//introduced to prevent daq abnormal response: flat high signal tail
	fNPointsFlatThreshold = StringToInteger(GetParameter("pointsFlatThreshold", "512"));
}

