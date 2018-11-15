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
#include <numeric>
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
	// !!!!!!!!!!!! BASELINE CORRECTION !!!!!!!!!!!!!!
	// TRestRawSignalAnalysisProcess subtracts baseline. Baseline is double value,
	// but data points in TRestRawSignalAnalysisProcess is only short integer type.
	// So we need to correct this by adding decimal part back.
	fBaseLineCorrection = false;
	for (int i = 0; i < fFriendlyProcesses.size(); i++) {
		if ((string)fFriendlyProcesses[i]->ClassName() == "TRestRawSignalAnalysisProcess") {
			fBaseLineCorrection = true;

			// setting parameters to the same as sAna
			fBaseLineRange = StringTo2DVector(fFriendlyProcesses[i]->GetParameter("baseLineRange", "(5,55)"));
			fIntegralRange = StringTo2DVector(fFriendlyProcesses[i]->GetParameter("integralRange", "(10,500)"));
			fPointThreshold = StringToDouble(fFriendlyProcesses[i]->GetParameter("pointThreshold", "2"));
			fNPointsOverThreshold = StringToInteger(fFriendlyProcesses[i]->GetParameter("pointsOverThreshold", "5"));
			fSignalThreshold = StringToDouble(fFriendlyProcesses[i]->GetParameter("signalThreshold", "5"));
		}
	}

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

    fSignalEvent->SetID( fRawSignalEvent->GetID() );
    fSignalEvent->SetSubID( fRawSignalEvent->GetSubID() );
    fSignalEvent->SetTimeStamp( fRawSignalEvent->GetTimeStamp() );
    fSignalEvent->SetSubEventTag( fRawSignalEvent->GetSubEventTag() );

    Int_t numberOfSignals = fRawSignalEvent->GetNumberOfSignals();

    Double_t totalIntegral = 0;
	Double_t rejectedSignal = 0;
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
				if (pulse.size() >= fNPointsOverThreshold) {
					//auto stdev = TMath::StdDev(begin(pulse), end(pulse));
					//calculate stdev
					double mean = std::accumulate(pulse.begin(), pulse.end(), 0.0) / pulse.size();
					double sq_sum = std::inner_product(pulse.begin(), pulse.end(), pulse.begin(), 0.0);
					double stdev = std::sqrt(sq_sum / pulse.size() - mean * mean);

					if (stdev > fSignalThreshold*baselinerms) {
						for (int j = pos; j < i; j++)
						{
							if (fBaseLineCorrection) {
								if (baseline >= 0 && baseline < 1) {
									sgn.NewPoint(j, (Double_t)s->GetData(j) - baseline);
								}
								else
								{
									cout << "REST Error! baseline(" << baseline << ") is without [0,1), check your code!" << endl;
								}
							}
							else
							{
								sgn.NewPoint(j, s->GetData(j));
							}

						}
					}
				}
			}
		}

		if (sgn.GetNumberOfPoints() > 0)
		{
			fSignalEvent->AddSignal(sgn);
			totalIntegral += sgn.GetIntegral();
		}
		else
		{
			rejectedSignal++;
		}


		//TRestRawSignal *sgnl = fRawSignalEvent->GetSignal(i);
  //      Double_t integral = sgnl->GetIntegralWithThreshold( fIntegralRange.X(), fIntegralRange.Y(),
  //              fBaseLineRange.X(), fBaseLineRange.Y(), fPointThreshold, fNPointsOverThreshold, fSignalThreshold );
		//Double_t baseline = sgnl->GetBaseLine(fBaseLineRange.X(), fBaseLineRange.Y());

  //      if( integral > 0 )
  //      {
  //          totalIntegral += integral;

  //          vector <Int_t> poinsOver = sgnl->GetPointsOverThreshold();

		//	TRestSignal outSignal;
		//	outSignal.SetID(sgnl->GetID());
		//	for (unsigned int n = 0; n < poinsOver.size(); n++) {
		//		if (fBaseLineCorrection) {
		//			if (baseline >= 0 && baseline < 1) {
		//				outSignal.NewPoint(poinsOver[n], (Double_t)sgnl->GetData(poinsOver[n]) - baseline);
		//			}
		//			else
		//			{
		//				cout << "REST Error! baseline(" << baseline << ") is without [0,1), check your code!" << endl;
		//			}
		//		}
		//		else
		//		{
		//			outSignal.NewPoint(poinsOver[n], sgnl->GetData(poinsOver[n]));
		//		}
		//	}

		//	fSignalEvent->AddSignal(outSignal);
  //      }
	}

	if (this->GetVerboseLevel() >= REST_Extreme) {
		fSignalEvent->PrintEvent();
	}

	debug << "TRestSignalZeroSuppresionProcess. Signals added : " << fSignalEvent->GetNumberOfSignals() << endl;
	debug << "TRestSignalZeroSuppresionProcess. Signals rejected : " << rejectedSignal << endl;
	debug << "TRestSignalZeroSuppresionProcess. Threshold integral : " << totalIntegral << endl;

	if (fSignalEvent->GetNumberOfSignals() <= 0) return NULL;

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

