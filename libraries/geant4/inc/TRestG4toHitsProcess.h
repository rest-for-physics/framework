///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4toHitsProcess.cxx
///
///
///             Simple process to convert a TRestG4Event class into a
///             TRestHitsEvent, that is, we just "extract" the hits information
///             Date : oct/2016
///             Author : I. G. Irastorza
///
///_______________________________________________________________________________

#ifndef RestCore_TRestG4toHitsProcess
#define RestCore_TRestG4toHitsProcess

#include <TRestG4Event.h>
#include <TRestG4Metadata.h>
#include <TRestGas.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestG4toHitsProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestG4Event* fG4Event;        //!
    TRestG4Metadata* fG4Metadata;  //!
    TRestHitsEvent* fHitsEvent;    //!

    vector<Int_t> fVolumeId;  //!
#endif

    vector<TString> fVolumeSelection;

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    void InitProcess();
    void BeginOfEventProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndOfEventProcess();
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        for (unsigned int n = 0; n < fVolumeSelection.size(); n++)
            std::cout << "Volume added : " << fVolumeSelection[n] << std::endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "g4toHitsEvent"; }

    // Constructor
    TRestG4toHitsProcess();
    TRestG4toHitsProcess(char* cfgFileName);
    // Destructor
    ~TRestG4toHitsProcess();

    ClassDef(TRestG4toHitsProcess, 1);  // Transform a TRestG4Event event to a
                                        // TRestHitsEvent (hits-collection event)
};
#endif
