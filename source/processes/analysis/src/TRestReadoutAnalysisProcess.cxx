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
		{
			auto iter = fChannelsHistos.begin();
			while (iter != fChannelsHistos.end()) {
				TRestReadoutModule*mod = fReadout->GetReadoutModule(iter->first);
				if (mod == NULL) {
					warning << "REST Warning(TRestReadoutAnalysisProcess): readout module with id " << iter->first << " not found!" << endl;
				}
				else
				{
					iter->second = new TH1D((TString)"channelHisto" + ToString(iter->first),
						(TString)"Readout Channel Activity of Module " + ToString(iter->first),
						mod->GetNumberOfChannels(), 0, mod->GetNumberOfChannels());
				}
				iter++;
			}
		}

		{
			auto iter = fChannelsHitMaps.begin();
			while (iter != fChannelsHitMaps.end()) {
				TRestReadoutModule*mod = fReadout->GetReadoutModule(iter->first);
				if (mod == NULL) {
					warning << "REST Warning(TRestReadoutAnalysisProcess): readout module with id " << iter->first << " not found!" << endl;
				}
				else
				{
					iter->second = new TH2D((TString)"channelHitmap" + ToString(iter->first),
						(TString)"FirstX/Y Hitmap of Module " + ToString(iter->first),
						mod->GetNumberOfChannels() / 2, 0, mod->GetNumberOfChannels() / 2, mod->GetNumberOfChannels() / 2, 0, mod->GetNumberOfChannels() / 2);
				}
				iter++;
			}
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
		Double_t firstX_id = -1.;
		Double_t firstY_id = -1.;
		Double_t firstX_t = 512.0;
		Double_t firstY_t = 512.0;

		Double_t lastX_id = -1.;
		Double_t lastY_id = -1.;
		Double_t lastX_t = 0.0;
		Double_t lastY_t = 0.0;

		double nan = numeric_limits<double>::quiet_NaN();
		fAnalysisTree->SetObservableValue(this, "FirstX", nan);
		fAnalysisTree->SetObservableValue(this, "FirstY", nan);
		fAnalysisTree->SetObservableValue(this, "LastX", nan);
		fAnalysisTree->SetObservableValue(this, "LastY", nan);

		for (int i = 0; i < fSignalEvent->GetNumberOfSignals(); i++) {
			TRestSignal*sgnl = fSignalEvent->GetSignal(i);

			if (sgnl->GetMaxPeakTime() < firstX_t) {
				if (!TMath::IsNaN(fReadout->GetX(sgnl->GetID()))) {
					firstX_id = sgnl->GetID();
					firstX_t = sgnl->GetMaxPeakTime();
				}
			}
			if (sgnl->GetMaxPeakTime() < firstY_t) {
				if (!TMath::IsNaN(fReadout->GetY(sgnl->GetID()))) {
					firstY_id = sgnl->GetID();
					firstY_t = sgnl->GetMaxPeakTime();
				}
			}

			if (sgnl->GetMaxPeakTime() > lastX_t) {
				if (!TMath::IsNaN(fReadout->GetX(sgnl->GetID()))) {
					lastX_id = sgnl->GetID();
					lastX_t = sgnl->GetMaxPeakTime();
				}
			}
			if (sgnl->GetMaxPeakTime() > lastY_t) {
				if (!TMath::IsNaN(fReadout->GetY(sgnl->GetID()))) {
					lastY_id = sgnl->GetID();
					lastY_t = sgnl->GetMaxPeakTime();
				}
			}
		}

		if (firstX_id > -1 && firstY_id > -1) {
			double firstx = fReadout->GetX(firstX_id);
			double firsty = fReadout->GetY(firstY_id);
			double lastx = fReadout->GetX(lastX_id);
			double lasty = fReadout->GetY(lastY_id);
			fAnalysisTree->SetObservableValue(this, "FirstX", firstx);
			fAnalysisTree->SetObservableValue(this, "FirstY", firsty);
			fAnalysisTree->SetObservableValue(this, "LastX", lastx);
			fAnalysisTree->SetObservableValue(this, "LastY", lasty);

			int mod1 = -1, mod2 = -1;
			int channel1 = -1, channel2 = -1;
			int plane = -1;
			fReadout->GetPlaneModuleChannel(firstX_id, plane, mod1, channel1);
			fReadout->GetPlaneModuleChannel(firstY_id, plane, mod2, channel2);
			if (mod1 == mod2 && mod1 > -1) {
				int x=-1, y=-1;
				int n = fReadout->GetReadoutModule(mod1)->GetNumberOfChannels() / 2;
				if (channel1 >= n && channel2 < n)
				{
					x = channel2;
					y = channel1 - n;
				}
				else if (channel2 >= n && channel1 < n)
				{
					x = channel1;
					y = channel2 - n;
				}
				fChannelsHitMaps[mod1]->Fill(x,y);
				//cout << n<<" "<<channel1 <<" "<< channel2 << endl;
				//cout << x << " " << y << endl;
				//cout << fReadout->GetX(firstX_id) << " " << fReadout->GetY(firstY_id) << endl;
				//cout << endl;
			}
		}
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
		{
			auto iter = fChannelsHistos.begin();
			while (iter != fChannelsHistos.end()) {
				if (iter->second != NULL)
					iter->second->Write();
				iter++;
			}
		}
		{
			auto iter = fChannelsHitMaps.begin();
			while (iter != fChannelsHitMaps.end()) {
				if (iter->second != NULL)
					iter->second->Write();
				iter++;
			}
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
		fChannelsHitMaps[StringToInteger(histdef[i])] = NULL;
	}

}

