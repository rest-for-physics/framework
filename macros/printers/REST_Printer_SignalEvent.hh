#include "TRestTask.h"
#include "TRestSignalEvent.h"
Int_t REST_Printer_SignalEvent( TString fName, Int_t firstEvent = 0 )
{
    TRestRun *run = new TRestRun();

    run->OpenInputFile( fName );

    run->PrintInfo();

    TRestSignalEvent *evt = new TRestSignalEvent();

    run->SetInputEvent( evt );

    run->GetEntry( firstEvent );

    evt->PrintEvent();

    delete run;

    return 0;
}
