#include "TRestTask.h"
#include "TRestHitsEvent.h"
Int_t REST_Printer_HitsEvent( TString fName, Int_t firstEvent = 0 )
{
    TRestRun *run = new TRestRun();

    run->OpenInputFile( fName );

    run->PrintInfo();

    TRestHitsEvent *evt = new TRestHitsEvent();

    run->SetInputEvent( evt );

    run->GetEntry( firstEvent );

    evt->PrintEvent();

    delete run;

    return 0;
}
