///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutAnalysisProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestSignalAnalysisProcess
///             Date : Jun/2018
///             Author : Ni Kaixiang
///
///_______________________________________________________________________________

#include <TPaveText.h>
#include <TLegend.h>

#include "TRestReadoutAnalysisProcess.h"
using namespace std;

ClassImp(TRestReadoutAnalysisProcess)
//______________________________________________________________________________
TRestReadoutAnalysisProcess::TRestReadoutAnalysisProcess()
{
	Initialize();
}

//______________________________________________________________________________
TRestReadoutAnalysisProcess::~TRestReadoutAnalysisProcess()
{
}

//______________________________________________________________________________
void TRestReadoutAnalysisProcess::Initialize()
{
	SetSectionName(this->ClassName());

	fSignalEvent = new TRestSignalEvent();

	fOutputEvent = fSignalEvent;
	fInputEvent = fSignalEvent;

}

//______________________________________________________________________________
void TRestReadoutAnalysisProcess::InitProcess()
{
	fReadout = (TRestReadout*)GetReadoutMetadata();
	if (fReadout != NULL) {
		auto iter = fChannelsHistos.begin();
		while (iter != fChannelsHistos.end()) {
			TRestReadoutModule*mod = fReadout->GetReadoutModule(iter->first);
			if (mod == NULL) {
				warning << "REST Warning(TRestReadoutAnalysisProcess): readout module with id " << iter->first << " not found!" << endl;
			}
			else
			{
				iter->second = new TH1D((TString)"module" + ToString(iter->first),
					(TString)"moduleChannelActivity" + ToString(iter->first),
					mod->GetNumberOfChannels(), 0, mod->GetNumberOfChannels());
			}
			iter++;
		}
	}
}

//______________________________________________________________________________
void TRestReadoutAnalysisProcess::BeginOfEventProcess()
{


}

//______________________________________________________________________________
TRestEvent* TRestReadoutAnalysisProcess::ProcessEvent(TRestEvent *evInput)
{
	fSignalEvent = (TRestSignalEvent *)evInput;
	fOutputEvent = fSignalEvent;
	if (fReadout != NULL) {
		double integral = 0;

		for (int i = 0; i < fSignalEvent->GetNumberOfSignals(); i++) {
			TRestSignal*sgn = fSignalEvent->GetSignal(i);
			// channel histo
			int plane = -1, mod = -1, channel = -1;
			fReadout->GetPlaneModuleChannel(sgn->GetID(), plane, mod, channel);
			if (mod != -1) {
				if (fChannelsHistos[mod] != NULL)
					fChannelsHistos[mod]->Fill(channel);

				// amplification, integral
				if (fModuldeAmplification[mod] == 0)fModuldeAmplification[mod] = 1;
				for (int j = 0; j < sgn->GetNumberOfPoints(); j++) {
					auto timecharge = sgn->GetPoint(j);
					sgn->SetPoint(j, timecharge.X(), timecharge.Y() / fModuldeAmplification[mod]);
				}
				integral += sgn->GetIntegral();
			}
		}

		fAnalysisTree->SetObservableValue(this, "CalibratedIntegral", integral);
	}
	return fSignalEvent;
}

//______________________________________________________________________________
void TRestReadoutAnalysisProcess::EndOfEventProcess()
{

}

//______________________________________________________________________________
void TRestReadoutAnalysisProcess::EndProcess()
{
	if (fReadout != NULL) {
		auto iter = fChannelsHistos.begin();
		while (iter != fChannelsHistos.end()) {
			if (iter->second != NULL)
				iter->second->Write();
			iter++;
		}
	}
}

//______________________________________________________________________________
// setting amplification:
// <parameter name="modulesAmp" value = "2-1:5-1.2:6-0.8:8-0.9" />
// setting readout modules to draw:
// <parameter name="modulesHist" value="2:5:6:8"/>
void TRestReadoutAnalysisProcess::InitFromConfigFile()
{
	string moduleAmp = GetParameter("modulesAmp", "");
	auto ampdef = Spilt(moduleAmp, ":");
	for (int i = 0; i < ampdef.size(); i++) {
		auto amppair = Spilt(ampdef[i], "-");
		if (amppair.size() == 2) {
			fModuldeAmplification[StringToInteger(amppair[0])] = StringToDouble(amppair[1]);
		}
	}

	string moduleHist = GetParameter("modulesHist", "");
	auto histdef = Spilt(moduleHist, ":");
	for (int i = 0; i < histdef.size(); i++) {
		fChannelsHistos[StringToInteger(histdef[i])] = NULL;
	}
}

