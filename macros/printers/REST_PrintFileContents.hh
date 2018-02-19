#include <TSystem.h>
#include <TRint.h>
#include <TApplication.h>
#include <TMath.h>
#include <TRestTools.h>
#include "TRestTask.h"
#include <TRestRun.h>

#ifndef RESTTask_PrintFileContents
#define RESTTask_PrintFileContents

Int_t REST_Printer_FileContents(TString fName)
{
	TRestStringOutput fout;
	TFile * fFile = new TFile(fName);
	if (fFile == NULL) { fout << "I could not open file : " << fName << endl; return 0; }
	TIter nextkey(fFile->GetListOfKeys());
	TKey *key;
	Int_t c = 0;
	fout << "==========================================" << endl;
	while ((key = (TKey*)nextkey())!=NULL) {
		if (c > 0) fout << " ------------------------------------- " << endl;

		fout << "ClassName : " << key->GetClassName() << endl;
		fout << "Name : " << key->GetName() << endl;
		fout << "Title : " << key->GetTitle() << endl;

		c++;
	}
	fout << "==========================================" << endl;

	return 0;
}


#endif