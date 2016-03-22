
Int_t REST_UTILS_PrintLinearLinearizedTrackEvent( TString fName, Int_t firstEvent = 0 )
{
    TRestRun *run = new TRestRun();

    run->OpenInputFile( fName );

    run->PrintInfo();

    TRestLinearizedTrackEvent *evt = new TRestLinearizedTrackEvent();

    run->SetInputEvent( evt );

    run->GetEntry( firstEvent );

    evt->PrintEvent();

    delete run;
}
