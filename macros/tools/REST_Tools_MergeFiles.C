
Int_t REST_Tools_MergeFiles( TString pathAndPattern, TString outputFilename  )
{

    TRestRunMerger *runMerger = new TRestRunMerger( pathAndPattern );

    runMerger->MergeFiles( outputFilename );

    delete runMerger;

    return 0;
}
