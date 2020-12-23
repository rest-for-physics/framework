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

#ifndef RestCore_TRestDetectorSignalChannelActivityProcess
#define RestCore_TRestDetectorSignalChannelActivityProcess

#include <TH1D.h>

#include <TRestDetectorReadout.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

//! A pure analysis process to generate histograms with detector channels activity
class TRestDetectorSignalChannelActivityProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestDetectorSignalEvent input
    TRestDetectorSignalEvent* fSignalEvent;  //!

    /// A pointer to the readout metadata information accessible to TRestRun
    TRestDetectorReadout* fReadout;  //!

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    /// The value of the lower signal threshold to add it to the histogram
    Double_t fLowThreshold;

    /// The value of the higher signal threshold to add it to the histogram
    Double_t fHighThreshold;

    /// The number of bins at the daq channels histogram
    Int_t fDaqHistogramChannels;

    /// The number of bins at the readout channels histogram
    Int_t fReadoutHistogramChannels;

    /// The first channel at the daq channels histogram
    Int_t fDaqStartChannel;

    /// The last channel at the daq channels histogram
    Int_t fDaqEndChannel;

    /// The first channel at the readout channels histogram
    Int_t fReadoutStartChannel;

    /// The last channel at the readout channels histogram
    Int_t fReadoutEndChannel;

    /// The daq channels histogram
    TH1D* fDaqChannelsHisto;  //!

    /// The readout channels histogram
    TH1D* fReadoutChannelsHisto;  //!

    /// The readout channels histogram built with 1-signal events (low threshold)
    TH1D* fReadoutChannelsHisto_OneSignal;  //!

    /// The readout channels histogram built with 1-signal events (high threshold)
    TH1D* fReadoutChannelsHisto_OneSignal_High;  //!

    /// The readout channels histogram built with 2-signal events (low threshold)
    TH1D* fReadoutChannelsHisto_TwoSignals;  //!

    /// The readout channels histogram built with 2-signal events (high threshold)
    TH1D* fReadoutChannelsHisto_TwoSignals_High;  //!

    /// The readout channels histogram built with 3-signal events (low threshold)
    TH1D* fReadoutChannelsHisto_ThreeSignals;  //!

    /// The readout channels histogram built with 3-signal events (high threshold)
    TH1D* fReadoutChannelsHisto_ThreeSignals_High;  //!

    /// The readout channels histogram built more than 3-signal events (low threshold)
    TH1D* fReadoutChannelsHisto_MultiSignals;  //!

    /// The readout channels histogram built more than 3-signal events (high threshold)
    TH1D* fReadoutChannelsHisto_MultiSignals_High;  //!

   public:
    any GetInputEvent() { return fSignalEvent; }
    any GetOutputEvent() { return fSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Low signal threshold activity : " << fLowThreshold << endl;
        metadata << "High signal threshold activity : " << fHighThreshold << endl;

        metadata << "Number of daq histogram channels : " << fDaqHistogramChannels << endl;
        metadata << "Start daq channel : " << fDaqStartChannel << endl;
        metadata << "End daq channel : " << fDaqEndChannel << endl;

        metadata << "Number of readout histogram channels : " << fReadoutHistogramChannels << endl;
        metadata << "Start readout channel : " << fReadoutStartChannel << endl;
        metadata << "End readout channel : " << fReadoutEndChannel << endl;

        EndPrintProcess();
    }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "SignalChannelActivity"; }

    // Constructor
    TRestDetectorSignalChannelActivityProcess();
    TRestDetectorSignalChannelActivityProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorSignalChannelActivityProcess();

    ClassDef(TRestDetectorSignalChannelActivityProcess, 3);
};
#endif
