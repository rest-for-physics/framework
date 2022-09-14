#include <iostream>
#include <string>

#include "TRestTask.h"
using namespace std;

#ifndef RestTask_PrintTrees
#define RestTask_PrintTrees

#include <TString.h>

//*******************************************************************************************************
//*** Description: This macro will print on screen the contents of the event tree, analysis tree and
//*** observable values inside the file.root given by argument.
//*** --------------
//*** Remark : If we specify a particular #entry, the analysisTree values of that entry will be printed.
//*** --------------
//*** Usage: restManager PrintTrees file.root [#entry]
//*******************************************************************************************************
Int_t REST_PrintTrees(TString fName, Int_t Entry = 0) {
    TRestRun* run = new TRestRun();

    run->OpenInputFile(fName);

    run->PrintTrees();

    run->GetEntry(Entry);

    TRestStringOutput RESTLog;
    RESTLog.setcolor(COLOR_BOLDBLUE);
    RESTLog << RESTendl;
    RESTLog << "=====AnalysisTree Contents, Entry " << Entry << "=====" << RESTendl;

    run->PrintObservables();

    delete run;

    return 0;
}
#endif
