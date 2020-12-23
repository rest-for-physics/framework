///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorAvalancheProcess.h
///
///             feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorAvalancheProcess
#define RestCore_TRestDetectorAvalancheProcess

#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorAvalancheProcess : public TRestEventProcess {
   private:
    TRestDetectorHitsEvent* fHitsInputEvent;   //!
    TRestDetectorHitsEvent* fHitsOutputEvent;  //!

    Double_t fEnergyRef;         ///< reference energy for the FWHM
    Double_t fResolutionAtEref;  ///< FWHM at Energy of reference
    Double_t fDetectorGain;      ///< Detector's gain.

    void InitFromConfigFile();
    void Initialize();
    void LoadDefaultConfig();

   protected:
    // add here the members of your event process
    TRestDetectorGas* fGas;  //!

   public:
    any GetInputEvent() { return fHitsInputEvent; }
    any GetOutputEvent() { return fHitsOutputEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename);

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " reference energy (Eref): " << fEnergyRef << endl;
        metadata << " resolution at Eref : " << fResolutionAtEref << endl;
        metadata << " detector gain : " << fDetectorGain << endl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() { return fGas; }

    TString GetProcessName() { return (TString) "avalancheProcess"; }

    Double_t GetEnergyReference() { return fEnergyRef; }
    Double_t GetResolutionReference() { return fResolutionAtEref; }
    Double_t GetDetectorGain() { return fDetectorGain; }

    // Constructor
    TRestDetectorAvalancheProcess();
    TRestDetectorAvalancheProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorAvalancheProcess();

    ClassDef(TRestDetectorAvalancheProcess, 1);  // Template for a REST "event process"
                                         // class inherited from TRestEventProcess
};
#endif
