
Int_t REST_UTILS_PrintHitsEvent( TString fName, Int_t firstEvent = 0 )
{
    TRestRun *run = new TRestRun();

    run->OpenInputFile( fName );

    run->PrintInfo();

    TRestHitsEvent *evt = new TRestHitsEvent();

    run->SetInputEvent( evt );

    run->GetInputEventTree()->GetEntry( firstEvent );

    evt->PrintEvent();

    delete run;
}
