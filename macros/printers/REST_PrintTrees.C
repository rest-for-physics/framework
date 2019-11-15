#include <iostream>
#include <string>
#include "TRestTask.h"
using namespace std;

#include <TString.h>

//*******************************************************************************************************
//*** Description: This macro will print on screen the contents of the event tree, analysis tree and
//*** observable values inside the file.root given by argument.
//*** --------------
//*** Usage: restManager PrintTrees file.root [#entry]
//*******************************************************************************************************
Int_t REST_Trees(TString fName, Int_t Entry = 0) {
    TRestRun* run = new TRestRun();

    run->OpenInputFile(fName);

    run->PrintTrees();

    run->GetEntry(Entry);

    TRestStringOutput fout;
    fout.setcolor(COLOR_BOLDBLUE);
    fout << endl;
    fout << "=====AnalysisTree Contents, Entry " << Entry << "=====" << endl;

    run->PrintObservables();

    delete run;

    return 0;
}
