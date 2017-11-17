
Int_t REST_Printer_G4Event( TString fName, Int_t firstEvent = 0 )
{
    TRestRun *run = new TRestRun();

    run->OpenInputFile( fName );

    run->PrintInfo();

    TRestG4Event *evt = new TRestG4Event();

    run->SetInputEvent( evt );

    run->GetEntry( firstEvent );

    evt->PrintEvent();

    delete run;

    return 0;
}
