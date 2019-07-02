///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsRotateAndTraslateProcess.h
///
///             march 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________

#ifndef RestCore_TRestHitsRotateAndTraslateProcess
#define RestCore_TRestHitsRotateAndTraslateProcess

#include <TRestGas.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestHitsRotateAndTraslateProcess : public TRestEventProcess {
   private:
    TRestHitsEvent* fHitsInputEvent;   //!
    TRestHitsEvent* fHitsOutputEvent;  //!

    Double_t fDeltaX;  ///< shift in X-axis
    Double_t fDeltaY;  ///< shift in X-axis
    Double_t fDeltaZ;  ///< shift in X-axis

    Double_t fAlpha;  ///< rotation angle around z-axis
    Double_t fBeta;   ///< rotation angle around y-axis
    Double_t fGamma;  ///< rotation angle around x-axis

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

    void LoadConfig(std::string cfgFilename);

    void PrintMetadata() {
        BeginPrintProcess();

        std::cout << " delta x : " << fDeltaX << std::endl;
        std::cout << " delta y : " << fDeltaY << std::endl;
        std::cout << " delta z : " << fDeltaZ << std::endl;
        std::cout << " alpha : " << fAlpha << std::endl;
        std::cout << " beta : " << fBeta << std::endl;
        std::cout << " gamma : " << fGamma << std::endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "rotateAndTraslate"; }

    Double_t GetDeltaX() { return fDeltaX; }
    Double_t GetDeltaY() { return fDeltaY; }
    Double_t GetDeltaZ() { return fDeltaZ; }

    Double_t GetAlpha() { return fAlpha; }
    Double_t GetBeta() { return fBeta; }
    Double_t GetGamma() { return fGamma; }

    // Constructor
    TRestHitsRotateAndTraslateProcess();
    TRestHitsRotateAndTraslateProcess(char* cfgFileName);
    // Destructor
    ~TRestHitsRotateAndTraslateProcess();

    ClassDef(TRestHitsRotateAndTraslateProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
