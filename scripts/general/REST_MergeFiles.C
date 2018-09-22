

int REST_MergeFiles( TString pathAndPattern, TString outputFilename  )
{
    TRestRunMerger *runMerger = new TRestRunMerger( pathAndPattern );

    runMerger->MergeFiles( outputFilename );

    runMerger->PrintFilelist();

    delete runMerger;
}
