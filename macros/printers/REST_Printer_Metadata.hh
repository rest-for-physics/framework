
Int_t REST_Printer_Metadata( TString fName )
{
    TRestRun *run = new TRestRun( );

    run->OpenInputFile( fName );

    run->PrintAllMetadata();

    delete run;

    return 0;
}
