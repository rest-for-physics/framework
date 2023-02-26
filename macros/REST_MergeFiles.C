#include "TRestRunMerger.h"

//*******************************************************************************************************
//***
//*** Updated REST_MergeFiles macro with the new TRestRunMerger class
//*** See TRestRunMerger for more details about this implementation
//***
//*******************************************************************************************************
Int_t REST_MergeFiles(TString pathAndPattern, TString outputFilename) {
    vector<string> files = TRestTools::GetFilesMatchingPattern((string)pathAndPattern);
    TRestRunMerger merger;
    
    return merger.MergeFiles(files, std::string(outputFilename.Data() ));

}
