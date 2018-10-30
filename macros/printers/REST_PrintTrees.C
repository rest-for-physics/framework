#include "TRestTask.h"
#include <string>
#include <iostream>
using namespace std;

#include <TString.h>

Int_t REST_Trees( TString fName, Int_t Entry = 0 )
{
	TRestRun *run = new TRestRun();

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
