
Int_t REST_Printer_TrackEvent( TString fName, Int_t firstEvent = 0 )
{
    TRestRun *run = new TRestRun();

    run->OpenInputFile( fName );

    run->PrintInfo();

    TRestTrackEvent *evt = new TRestTrackEvent();

    run->SetInputEvent( evt );

    run->GetEntry( firstEvent );

    evt->PrintEvent();

    delete run;

    return 0;
}
