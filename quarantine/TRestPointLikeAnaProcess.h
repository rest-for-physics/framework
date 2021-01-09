///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestPointLikeAnaProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestPointLikeAnaProcess
#define RestCore_TRestPointLikeAnaProcess

#include <TRestSignalEvent.h>
#include "TRestDetectorReadout.h"

#include "TRestEventProcess.h"

class TRestPointLikeAnaProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestSignalEvent* fSignalEvent;  //!
                                     // TODO We must get here a pointer to TRestDaqMetadata
                                     // In order to convert the parameters to time using the sampling time
#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

    Int_t fThreshold;
    Int_t fPedLevel;
    Int_t fNChannels;
    Double_t fPitch;
    TString fGainFilename;

    Double_t fCalGain;

   protected:
    // add here the members of your event process
    TRestDetectorReadout* fReadout;

   public:
    any GetInputEvent() { return fSignalEvent; }
    any GetOutputEvent() { return fSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename);

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Calibration File Name : " << fGainFilename << endl;

        metadata << "Selection threshold : " << fThreshold << " ADC units" << endl;
        metadata << "Pedestal level : " << fPedLevel << " ADC units" << endl;
        metadata << "Number of channels: " << fNChannels << " ADC units" << endl;
        metadata << "Readout pitch: " << fPitch << " mm" << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "pointLikeAna"; }
    //       Double_t GetCalibFactor();           ///< Calibration factor is
    //       found.

    // Constructor
    TRestPointLikeAnaProcess();
    TRestPointLikeAnaProcess(char* cfgFileName);
    // Destructor
    ~TRestPointLikeAnaProcess();

    ClassDef(TRestPointLikeAnaProcess, 1);  // Template for a REST "event process" class inherited from
                                            // TRestEventProcess
};
#endif
