
#include <TCanvas.h>
#include <TF1.h>
#include <TH1D.h>
#include <TMath.h>
#include <TRestRun.h>
#include <TRestTask.h>
#include <TSystem.h>

#ifndef RestTask_SendMessage
#define RestTask_SendMessage

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_SendMessage(int token, TString message) {
    TRestMessenger m(token);

    m.SendMessage((string)message);

    return 0;
};

#endif
