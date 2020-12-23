///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsSmearingProcess.h
///
///             feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorHitsSmearingProcess
#define RestCore_TRestDetectorHitsSmearingProcess

#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>

#include <TRandom3.h>

#include "TRestEventProcess.h"

class TRestDetectorHitsSmearingProcess : public TRestEventProcess {
   private:
#ifndef __CINT__

    TRestDetectorHitsEvent* fHitsInputEvent;   //!
    TRestDetectorHitsEvent* fHitsOutputEvent;  //!

    TRandom3* fRandom;  //!

    TRestDetectorGas* fGas;  //!
#endif

    void InitFromConfigFile();
    void Initialize();
    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

    Double_t fEnergyRef;         ///< reference energy for the FWHM
    Double_t fResolutionAtEref;  ///< FWHM at Energy of reference

   public:
    any GetInputEvent() { return fHitsInputEvent; }
    any GetOutputEvent() { return fHitsOutputEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " reference energy (Eref): " << fEnergyRef << endl;
        metadata << " resolution at Eref : " << fResolutionAtEref << endl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() { return NULL; }

    TString GetProcessName() { return (TString) "smearingProcess"; }

    Double_t GetEnergyReference() { return fEnergyRef; }
    Double_t GetResolutionReference() { return fResolutionAtEref; }

    // Constructor
    TRestDetectorHitsSmearingProcess();
    TRestDetectorHitsSmearingProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorHitsSmearingProcess();

    ClassDef(TRestDetectorHitsSmearingProcess, 1);  // Template for a REST "event process"
                                        // class inherited from TRestEventProcess
};
#endif
