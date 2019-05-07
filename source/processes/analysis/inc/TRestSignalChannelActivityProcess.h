///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalChannelActivityProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestSignalChannelActivityProcess
#define RestCore_TRestSignalChannelActivityProcess

#include <TH1D.h>

#include <TRestReadout.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class TRestSignalChannelActivityProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestSignalEvent* fSignalEvent;  //!
    TRestReadout* fReadout;          //!

    Int_t fDaqHistogramChannels;      //!
    Int_t fReadoutHistogramChannels;  //!
#endif
    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process
    //
    Double_t fLowThreshold;
    Double_t fHighThreshold;

    TH1D* fDaqChannelsHisto;

    TH1D* fReadoutChannelsHisto_OneSignal;
    TH1D* fReadoutChannelsHisto_OneSignal_High;
    TH1D* fReadoutChannelsHisto_TwoSignals;
    TH1D* fReadoutChannelsHisto_TwoSignals_High;
    TH1D* fReadoutChannelsHisto_ThreeSignals;
    TH1D* fReadoutChannelsHisto_ThreeSignals_High;
    TH1D* fReadoutChannelsHisto_MultiSignals;
    TH1D* fReadoutChannelsHisto_MultiSignals_High;

   public:
    void InitProcess();
    void BeginOfEventProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndOfEventProcess();
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        cout << "Low signal threshold activity : " << fLowThreshold << endl;
        cout << "High signal threshold activity : " << fHighThreshold << endl;

        cout << "Number of daq histogram channels : " << fDaqHistogramChannels << endl;
        cout << "Number of readout histogram channels : " << fReadoutHistogramChannels << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "signalChannelActivity"; }

    // Constructor
    TRestSignalChannelActivityProcess();
    TRestSignalChannelActivityProcess(char* cfgFileName);
    // Destructor
    ~TRestSignalChannelActivityProcess();

    ClassDef(TRestSignalChannelActivityProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
