#include "TRestTask.h"
#include <string>
#include <iostream>
using namespace std;

#include <TString.h>

Int_t REST_Printer_Trees( TString fName )
{
	TRestRun *run = new TRestRun();

	run->OpenInputFile(fName);

	run->PrintTrees();

	run->PrintObservables();

	delete run;

	return 0;
}
