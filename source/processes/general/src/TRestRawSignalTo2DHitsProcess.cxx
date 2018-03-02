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







		if (fNoiseReductionLevel == 0) {
			for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++)
			{
				TRestRawSignal*s = fInputSignalEvent->GetSignal(n);
				fOutputSignalEvent->AddSignal(s);
			}
		}
		else if (fNoiseReductionLevel == 1)
		{
			for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++)
			{
				TRestRawSignal*s = fInputSignalEvent->GetSignal(n);
				s->SubstractBaseline(fBaseLineRange.X(),fBaseLineRange.Y());
				if (fEnergyCalculation == 0) {
					fOutputSignalEvent->AddSignal(s);
				}
				else if(fEnergyCalculation==1)
				{
					double time = s->GetMaxPeakBin();
					double energy = s->GetMaxPeakValue();
					fOutputSignalEvent->SetSignal(time, s->GetID(), energy);
				}
				else if (fEnergyCalculation == 2) {
					double time = s->GetMaxPeakBin();
					double energy = s->GetIntegral((Int_t)fIntegralRange.X(), (Int_t)fIntegralRange.Y());
					fOutputSignalEvent->SetSignal(time, s->GetID(), energy);
				}
				else
				{
					double time = s->GetMaxPeakBin();
					double energy = s->GetIntegralWithThreshold((Int_t)fIntegralRange.X(), (Int_t)fIntegralRange.Y(), fBaseLineRange.X(), fBaseLineRange.Y(), fPointThreshold, fNPointsOverThreshold, fSignalThreshold);
					fOutputSignalEvent->SetSignal(time, s->GetID(), energy);
				}
			}
		}
		else
		{
			for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++)
			{
				TRestRawSignal*s = fInputSignalEvent->GetSignal(n);
				s->SubstractBaseline(fBaseLineRange.X(), fBaseLineRange.Y());
				if (fEnergyCalculation == 0) {
					vector <Int_t> poinsOver = s->GetPointsOverThreshold();
					TRestSignal outSignal;
					outSignal.SetID(s->GetID());
					for (unsigned int n = 0; n < poinsOver.size(); n++)
						outSignal.NewPoint(poinsOver[n], s->GetData(poinsOver[n]));
					fOutputSignalEvent->AddSignal(&outSignal);
				}
				else if (fEnergyCalculation == 1)
				{
					double time = s->GetMaxPeakBin();
					double energy = s->GetMaxPeakValue();
					fOutputSignalEvent->SetSignal(time, s->GetID(), energy);
					//cout << fInputSignalEvent->GetID() <<"  "<<time << "  " << energy << endl;
				}
				else if (fEnergyCalculation == 2) {
					double time = s->GetMaxPeakBin();
					double energy = s->GetIntegral((Int_t)fIntegralRange.X(), (Int_t)fIntegralRange.Y());
					fOutputSignalEvent->SetSignal(time, s->GetID(), energy);
				}
				else
				{
					double time = s->GetMaxPeakBin();
					double energy = s->GetIntegralWithThreshold((Int_t)fIntegralRange.X(), (Int_t)fIntegralRange.Y(), fBaseLineRange.X(), fBaseLineRange.Y(), fPointThreshold, fNPointsOverThreshold, fSignalThreshold);
					fOutputSignalEvent->SetSignal(time, s->GetID(), energy);
				}
			}

			//cout << fOutputSignalEvent->GetNumberOfSignals() << endl;

		}

		if (fOutputSignalEvent->GetNumberOfXZSignals() < 2 || fOutputSignalEvent->GetNumberOfYZSignals() < 2) {
			return NULL;
		}
		//cout << fOutputSignalEvent->GetNumberOfSignals() << endl;
	}

	return fOutputSignalEvent;
}

void TRestRawSignalTo2DHitsProcess::InitFromConfigFile()
{
	fNoiseReductionLevel = StringToInteger(GetParameter("noiseReduction", "0"));
	fEnergyCalculation = StringToInteger(GetParameter("energyCalculation", "0"));

	fBaseLineRange = StringTo2DVector(GetParameter("baseLineRange", "(5,55)"));
	fIntegralRange = StringTo2DVector(GetParameter("integralRange", "(10,500)"));
	fPointThreshold = StringToDouble(GetParameter("pointThreshold", "2"));
	fNPointsOverThreshold = StringToInteger(GetParameter("pointsOverThreshold", "5"));
	fSignalThreshold = StringToDouble(GetParameter("signalThreshold", "5"));
}

