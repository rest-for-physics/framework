/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
/// The TRestDetectorSignalChannelActivityProcess allows to generate different
/// histograms in order to monitor the number of times a channel has observed
/// a signal given a set of conditions on the threshold and number of active
/// channels.
///
/// TRestDetectorSignalChannelActivityProcess produces different channel activity
/// histograms involving signals. These histograms thus show the activity
/// of the channels after zero suppression.
///
/// The following list describes the different histograms that are generated:
///
/// * **daqChannelActivity**: Histogram based on the DAQ channels.
/// The following figure shows the DAQ channel activity histogram for
/// signals in:
/// 	* a) the case where only channels that have been hit are saved.
/// 	* b) the case where all the channels are saved. The left plot shows
/// a flat distribution of raw activity and after signal analysis it turns into
/// the right plot where only *good signals* are present.
///
/// \htmlonly <style>div.image img[src="daqChAct.png"]{width:1000px;}</style> \endhtmlonly
/// ![An ilustration of the daq signals channel activity](daqChAct.png)
///
/// * **rChannelActivity**: histogram based on the readout channels, i.e.,
/// after converting the daq channel numbering into readout channel numbering
/// based on the .dec file.
///
/// * **rChannelActivity_N**: where *N* can be 1, 2, 3 or M (multi), is
/// a histogram based on the readout channels, i.e., after converting the daq
/// channel numbering into readout channel numbering based on the .dec
/// file, that contains the events with *N* number of signals above the **lowThreshold**
/// set by the user.
///
/// * **rChannelActivity_NH**: where *N* can be 1, 2, 3 or M (multi), is
/// a histogram based on the readout channels, i.e., after converting the daq
/// channel numbering into readout channel numbering based on the .dec
/// file, that contains the events with *N* number of signals above the **highThreshold**
/// set by the user.
///
/// The number of channels and their numbering can be specified by the user to
/// match the detector being used.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2017-February: First implementation of signal channel activity process.
///              Javier Galan
///
/// \class      TRestDetectorSignalChannelActivityProcess
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestDetectorSignalChannelActivityProcess.h"
using namespace std;

ClassImp(TRestDetectorSignalChannelActivityProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDetectorSignalChannelActivityProcess::TRestDetectorSignalChannelActivityProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
///
TRestDetectorSignalChannelActivityProcess::TRestDetectorSignalChannelActivityProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDetectorSignalChannelActivityProcess::~TRestDetectorSignalChannelActivityProcess() { delete fSignalEvent; }

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestDetectorSignalChannelActivityProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestDetectorSignalChannelActivityProcess::Initialize() {
    SetSectionName(this->ClassName());

    fSignalEvent = new TRestDetectorSignalEvent();
}

///////////////////////////////////////////////
/// \brief Function to load the configuration from an external configuration
/// file.
///
/// If no configuration path is defined in TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// correspondig TRestGeant4AnalysisProcess section inside the RML.
///
void TRestDetectorSignalChannelActivityProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Process initialization. The ROOT TH1 histograms are created here using
/// the limits defined in the process metadata members.
///
/// The readout histograms will only be created in case an appropiate readout definition
/// is found in the processing chain.
///
void TRestDetectorSignalChannelActivityProcess::InitProcess() {
    fReadout = GetMetadata<TRestDetectorReadout>();

    debug << "TRestDetectorSignalChannelActivityProcess::InitProcess. Readout pointer : " << fReadout << endl;
    if (GetVerboseLevel() >= REST_Info && fReadout) fReadout->PrintMetadata();

    if (!fReadOnly) {
        fDaqChannelsHisto = new TH1D("daqChannelActivity", "daqChannelActivity", fDaqHistogramChannels,
                                     fDaqStartChannel, fDaqEndChannel);
        if (fReadout) {
            fReadoutChannelsHisto =
                new TH1D("rChannelActivity", "readoutChannelActivity", fReadoutHistogramChannels,
                         fReadoutStartChannel, fReadoutEndChannel);
            fReadoutChannelsHisto_OneSignal =
                new TH1D("rChannelActivity_1", "readoutChannelActivity", fReadoutHistogramChannels,
                         fReadoutStartChannel, fReadoutEndChannel);
            fReadoutChannelsHisto_OneSignal_High =
                new TH1D("rChannelActivity_1H", "readoutChannelActivity", fReadoutHistogramChannels,
                         fReadoutStartChannel, fReadoutEndChannel);
            fReadoutChannelsHisto_TwoSignals =
                new TH1D("rChannelActivity_2", "readoutChannelActivity", fReadoutHistogramChannels,
                         fReadoutStartChannel, fReadoutEndChannel);
            fReadoutChannelsHisto_TwoSignals_High =
                new TH1D("rChannelActivity_2H", "readoutChannelActivity", fReadoutHistogramChannels,
                         fReadoutStartChannel, fReadoutEndChannel);
            fReadoutChannelsHisto_ThreeSignals =
                new TH1D("rChannelActivity_3", "readoutChannelActivity", fReadoutHistogramChannels,
                         fReadoutStartChannel, fReadoutEndChannel);
            fReadoutChannelsHisto_ThreeSignals_High =
                new TH1D("rChannelActivity_3H", "readoutChannelActivity", fReadoutHistogramChannels,
                         fReadoutStartChannel, fReadoutEndChannel);
            fReadoutChannelsHisto_MultiSignals =
                new TH1D("rChannelActivity_M", "readoutChannelActivity", fReadoutHistogramChannels,
                         fReadoutStartChannel, fReadoutEndChannel);
            fReadoutChannelsHisto_MultiSignals_High =
                new TH1D("rChannelActivity_MH", "readoutChannelActivity", fReadoutHistogramChannels,
                         fReadoutStartChannel, fReadoutEndChannel);
        }
    }
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestDetectorSignalChannelActivityProcess::ProcessEvent(TRestEvent* evInput) {
    TString obsName;

    TRestDetectorSignalEvent* fInputSignalEvent = (TRestDetectorSignalEvent*)evInput;

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
        TRestDetectorSignal* sgnl = fSignalEvent->GetSignal(s);
        if (sgnl->GetMaxValue() > fHighThreshold) Nhigh++;
        if (sgnl->GetMaxValue() > fLowThreshold) Nlow++;
    }

    for (int s = 0; s < fSignalEvent->GetNumberOfSignals(); s++) {
        TRestDetectorSignal* sgnl = fSignalEvent->GetSignal(s);
        // Adding signal to the channel activity histogram
        if (!fReadOnly && fReadout) {
            Int_t signalID = fSignalEvent->GetSignal(s)->GetID();

            Int_t p, m, readoutChannel;
            fReadout->GetPlaneModuleChannel(signalID, p, m, readoutChannel);

            fReadoutChannelsHisto->Fill(readoutChannel);

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

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been
/// processed. In this process it will take care of writing the histograms
/// to disk.
///
void TRestDetectorSignalChannelActivityProcess::EndProcess() {
    if (!fReadOnly) {
        fDaqChannelsHisto->Write();
        if (fReadout) {
            fReadoutChannelsHisto->Write();

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

///////////////////////////////////////////////
/// \brief Function to read input parameters from the RML
/// TRestDetectorSignalChannelActivityProcess metadata section
///
void TRestDetectorSignalChannelActivityProcess::InitFromConfigFile() {
    fLowThreshold = StringToDouble(GetParameter("lowThreshold", "25"));
    fHighThreshold = StringToDouble(GetParameter("highThreshold", "50"));

    fDaqHistogramChannels = StringToInteger(GetParameter("daqChannels", "300"));
    fDaqStartChannel = StringToInteger(GetParameter("daqStartCh", "4320"));
    fDaqEndChannel = StringToInteger(GetParameter("daqEndCh", "4620"));
    fReadoutHistogramChannels = StringToInteger(GetParameter("readoutChannels", "128"));
    fReadoutStartChannel = StringToInteger(GetParameter("readoutStartCh", "0"));
    fReadoutEndChannel = StringToInteger(GetParameter("readoutEndCh", "128"));
}
