#include "TFileMerger.h"
#include "TRestTask.h"

#ifndef RESTTask_MergeFiles
#define RESTTask_MergeFiles

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_MergeFiles(TString pathAndPattern, TString outputFilename) {
    vector<string> files = TRestTools::GetFilesMatchingPattern((string)pathAndPattern);
    TFileMerger* m = new TFileMerger();
    m->OutputFile(outputFilename);
    for (auto f : files) {
        m->AddFile(f.c_str());
    }
    int a = m->Merge();
    delete m;
    return a;

    // TRestRunMerger *runMerger = new TRestRunMerger( pathAndPattern );

    // runMerger->MergeFiles( outputFilename );

    // delete runMerger;

    // return 0;
}
