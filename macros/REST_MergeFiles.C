#include "TRestTask.h"
#include "TRestTools.h"

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
    std::string error;
    const bool success =
        TRestTools::MergeRootFilesTransactionally(outputFilename.Data(), files, "", false, &error);
    if (!success) {
        RESTError << error << RESTendl;
    }
    return success;

    // TRestRunMerger *runMerger = new TRestRunMerger( pathAndPattern );

    // runMerger->MergeFiles( outputFilename );

    // delete runMerger;

    // return 0;
}
#endif
