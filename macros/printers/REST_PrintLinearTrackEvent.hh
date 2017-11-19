#include "TRestTask.h"
#include "TRestLinearTrackEvent.h"
Int_t REST_Printer_LinearTrackEvent( TString fName, Int_t firstEvent = 0 )
{
    TRestRun *run = new TRestRun();

    run->OpenInputFile( fName );

    run->PrintInfo();

    TRestLinearTrackEvent *evt = new TRestLinearTrackEvent();

    run->SetInputEvent( evt );

    run->GetEntry( firstEvent );

    evt->PrintEvent();

    delete run;

    return 0;
}
