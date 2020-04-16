///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSmearingProcess.h
///
///             feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________

#ifndef RestCore_TRestSmearingProcess
#define RestCore_TRestSmearingProcess

#include <TRestGas.h>
#include <TRestHitsEvent.h>

#include <TRandom3.h>

#include "TRestEventProcess.h"

class TRestSmearingProcess : public TRestEventProcess {
   private:
#ifndef __CINT__

    TRestHitsEvent* fHitsInputEvent;   //!
    TRestHitsEvent* fHitsOutputEvent;  //!

    TRandom3* fRandom;  //!

    TRestGas* fGas;  //!
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
    TRestSmearingProcess();
    TRestSmearingProcess(char* cfgFileName);
    // Destructor
    ~TRestSmearingProcess();

    ClassDef(TRestSmearingProcess, 1);  // Template for a REST "event process"
                                        // class inherited from TRestEventProcess
};
#endif
