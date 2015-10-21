

int REST_MergeFiles( TString pathAndPattern, TString outputFilename  )
{
    gSystem->Load("librestcore.so");
    gSystem->Load("librestsim.so");

    TRestRunMerger *runMerger = new TRestRunMerger( pathAndPattern );

    runMerger->MergeFiles( outputFilename );

    delete runMerger;


}
