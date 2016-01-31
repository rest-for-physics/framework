
Int_t REST_UTILS_PrintMetadata( TString fName )
{

    TRestRun *run = new TRestRun( );

    run->OpenInputFile( fName );

    run->PrintAllMetadata();
    

    delete run;
}
