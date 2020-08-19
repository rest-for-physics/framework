///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalChannelActivityProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestRawSignalChannelActivityProcess
#define RestCore_TRestRawSignalChannelActivityProcess

#include <TH1D.h>

#include <TRestReadout.h>
#include <TRestRawSignalEvent.h>

#include "TRestEventProcess.h"

class TRestRawSignalChannelActivityProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestRawSignalEvent* fSignalEvent;  //!
    TRestReadout* fReadout;          //!
#endif
    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process
    //
    Double_t fLowThreshold;
    Double_t fHighThreshold;

    Int_t fDaqHistogramChannels;
    Int_t fReadoutHistogramChannels;

    Int_t fDaqStartChannel;
    Int_t fDaqEndChannel;

    TH1D* fDaqChannelsHisto;  //!

    TH1D* fReadoutChannelsHisto_OneSignal;          //!
    TH1D* fReadoutChannelsHisto_OneSignal_High;     //!
    TH1D* fReadoutChannelsHisto_TwoSignals;         //!
    TH1D* fReadoutChannelsHisto_TwoSignals_High;    //!
    TH1D* fReadoutChannelsHisto_ThreeSignals;       //!
    TH1D* fReadoutChannelsHisto_ThreeSignals_High;  //!
    TH1D* fReadoutChannelsHisto_MultiSignals;       //!
    TH1D* fReadoutChannelsHisto_MultiSignals_High;  //!

   public:
    any GetInputEvent() { return fSignalEvent; }
    any GetOutputEvent() { return fSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Low signal threshold activity : " << fLowThreshold << endl;
        metadata << "High signal threshold activity : " << fHighThreshold << endl;

        metadata << "Number of daq histogram channels : " << fDaqHistogramChannels << endl;
        metadata << "Start daq channel : " << fDaqStartChannel << endl;
        metadata << "End daq channel : " << fDaqEndChannel << endl;

        metadata << "Number of readout histogram channels : " << fReadoutHistogramChannels << endl;



        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "rawSignalChannelActivity"; }

    // Constructor
    TRestRawSignalChannelActivityProcess();
    TRestRawSignalChannelActivityProcess(char* cfgFileName);
    // Destructor
    ~TRestRawSignalChannelActivityProcess();

    ClassDef(TRestRawSignalChannelActivityProcess, 2);
};
#endif
