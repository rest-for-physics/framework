///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalChannelActivityProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestRawSignalAnalysisProcess
///             Date : feb/2017
///             Author : J. Galan
///
///_______________________________________________________________________________

#include <TLegend.h>
#include <TPaveText.h>

#include "TRestRawSignalChannelActivityProcess.h"
using namespace std;

//int rawCounter = 0;
//int considerForMeanRate = 0;
//int reject = 1;

ClassImp(TRestRawSignalChannelActivityProcess)
    //______________________________________________________________________________
    TRestRawSignalChannelActivityProcess::TRestRawSignalChannelActivityProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestRawSignalChannelActivityProcess::TRestRawSignalChannelActivityProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestRawSignalChannelActivityProcess::~TRestRawSignalChannelActivityProcess() { delete fSignalEvent; }

void TRestRawSignalChannelActivityProcess::LoadDefaultConfig() { SetTitle("Default config"); }

//______________________________________________________________________________
void TRestRawSignalChannelActivityProcess::Initialize() {
    SetSectionName(this->ClassName());

    fSignalEvent = new TRestRawSignalEvent();
}

void TRestRawSignalChannelActivityProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestRawSignalChannelActivityProcess::InitProcess() {
    fReadout = GetMetadata<TRestReadout>();

    info << "TRestRawSignalChannelActivityProcess::InitProcess. Readout pointer : " << fReadout << endl;
    if (GetVerboseLevel() >= REST_Info && fReadout) fReadout->PrintMetadata();

    if (!fReadOnly) {
       // fDaqChannelsHisto = new TH1D("daqChannelActivity", "daqChannelActivity", fDaqHistogramChannels, 0,
       //                              fDaqHistogramChannels);
		//fDaqChannelsHisto = new TH1D( "daqChannelActivityRaw", "daqChannelActivity", 300, 4320, 4620 );
        fDaqChannelsHisto = new TH1D( "daqChannelActivityRaw", "daqChannelActivity", fDaqHistogramChannels, fDaqStartChannel, fDaqEndChannel );
        if (fReadout) {
            fReadoutChannelsHisto_OneSignal =
                new TH1D("rChannelActivityRaw_1", "readoutChannelActivity", fReadoutHistogramChannels, 0,
                         fReadoutHistogramChannels);
            fReadoutChannelsHisto_OneSignal_High =
                new TH1D("rChannelActivityRaw_1H", "readoutChannelActivity", fReadoutHistogramChannels, 0,
                         fReadoutHistogramChannels);
            fReadoutChannelsHisto_TwoSignals =
                new TH1D("rChannelActivityRaw_2", "readoutChannelActivity", fReadoutHistogramChannels, 0,
                         fReadoutHistogramChannels);
            fReadoutChannelsHisto_TwoSignals_High =
                new TH1D("rChannelActivityRaw_2H", "readoutChannelActivity", fReadoutHistogramChannels, 0,
                         fReadoutHistogramChannels);
            fReadoutChannelsHisto_ThreeSignals =
                new TH1D("rChannelActivityRaw_3", "readoutChannelActivity", fReadoutHistogramChannels, 0,
                         fReadoutHistogramChannels);
            fReadoutChannelsHisto_ThreeSignals_High =
                new TH1D("rChannelActivityRaw_3H", "readoutChannelActivity", fReadoutHistogramChannels, 0,
                         fReadoutHistogramChannels);
            fReadoutChannelsHisto_MultiSignals =
                new TH1D("rChannelActivityRaw_M", "readoutChannelActivity", fReadoutHistogramChannels, 0,
                         fReadoutHistogramChannels);
            fReadoutChannelsHisto_MultiSignals_High =
                new TH1D("rChannelActivityRaw_MH", "readoutChannelActivity", fReadoutHistogramChannels, 0,
                         fReadoutHistogramChannels);
        }
    }
}

//______________________________________________________________________________
TRestEvent* TRestRawSignalChannelActivityProcess::ProcessEvent(TRestEvent* evInput) {
    TString obsName;

    TRestRawSignalEvent* fInputSignalEvent = (TRestRawSignalEvent*)evInput;

    /// Copying the signal event to the output event

    fSignalEvent->SetID(fInputSignalEvent->GetID());
    fSignalEvent->SetSubID(fInputSignalEvent->GetSubID());
    fSignalEvent->SetTimeStamp(fInputSignalEvent->GetTimeStamp());
    fSignalEvent->SetSubEventTag(fInputSignalEvent->GetSubEventTag());

    Int_t N = fInputSignalEvent->GetNumberOfSignals();
    for (int sgnl = 0; sgnl < N; sgnl++) fSignalEvent->AddSignal(*fInputSignalEvent->GetSignal(sgnl));
    /////////////////////////////////////////////////

    Int_t Nlow = 0;
    Int_t Nhigh = 0;
    for (int s = 0; s < fSignalEvent->GetNumberOfSignals(); s++) {
        TRestRawSignal* sgnl = fSignalEvent->GetSignal(s);
        if (sgnl->GetMaxValue() > fHighThreshold) Nhigh++;
        if (sgnl->GetMaxValue() > fLowThreshold) Nlow++;
    }

    for (int s = 0; s < fSignalEvent->GetNumberOfSignals(); s++) {
        TRestRawSignal* sgnl = fSignalEvent->GetSignal(s);
        // Adding signal to the channel activity histogram
        if (!fReadOnly && fReadout) {
            Int_t signalID = fSignalEvent->GetSignal(s)->GetID();

            Int_t p, m, readoutChannel;
            fReadout->GetPlaneModuleChannel(signalID, p, m, readoutChannel);

            if (sgnl->GetMaxValue() > fLowThreshold) {
                if (Nlow == 1) fReadoutChannelsHisto_OneSignal->Fill(readoutChannel);
                if (Nlow == 2) fReadoutChannelsHisto_TwoSignals->Fill(readoutChannel);
                if (Nlow == 3) fReadoutChannelsHisto_ThreeSignals->Fill(readoutChannel);
                if (Nlow > 3 && Nlow < 10) fReadoutChannelsHisto_MultiSignals->Fill(readoutChannel);
            }

            if (sgnl->GetMaxValue() > fHighThreshold) {
                if (Nhigh == 1) fReadoutChannelsHisto_OneSignal_High->Fill(readoutChannel);
                if (Nhigh == 2) fReadoutChannelsHisto_TwoSignals_High->Fill(readoutChannel);
                if (Nhigh == 3) fReadoutChannelsHisto_ThreeSignals_High->Fill(readoutChannel);
                if (Nhigh > 3 && Nhigh < 10) fReadoutChannelsHisto_MultiSignals_High->Fill(readoutChannel);
            }
        }

        if (!fReadOnly) {
            Int_t daqChannel = fSignalEvent->GetSignal(s)->GetID();
            fDaqChannelsHisto->Fill(daqChannel);
        }
    }

    if (GetVerboseLevel() >= REST_Debug) fAnalysisTree->PrintObservables();

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestRawSignalChannelActivityProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();

    if (!fReadOnly) {
        fDaqChannelsHisto->Write();
        if (fReadout) {
            fReadoutChannelsHisto_OneSignal->Write();
            fReadoutChannelsHisto_TwoSignals->Write();
            fReadoutChannelsHisto_ThreeSignals->Write();
            fReadoutChannelsHisto_MultiSignals->Write();

            fReadoutChannelsHisto_OneSignal_High->Write();
            fReadoutChannelsHisto_TwoSignals_High->Write();
            fReadoutChannelsHisto_ThreeSignals_High->Write();
            fReadoutChannelsHisto_MultiSignals_High->Write();
        }
    }
}

//______________________________________________________________________________
void TRestRawSignalChannelActivityProcess::InitFromConfigFile() {
    fLowThreshold = StringToDouble(GetParameter("lowThreshold", "25"));
    fHighThreshold = StringToDouble(GetParameter("highThreshold", "50"));

    fDaqHistogramChannels = StringToInteger(GetParameter("daqChannels", "300"));
    fDaqStartChannel = StringToInteger(GetParameter("daqStartCh", "4320"));
    fDaqEndChannel = StringToInteger(GetParameter("daqEndCh", "4620"));
    fReadoutHistogramChannels = StringToInteger(GetParameter("readoutChannels", "128"));
}
