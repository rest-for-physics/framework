#include "TRestTask.h"
Int_t REST_Printer_Metadata( TString fName )
{
	TRestStringOutput cout;

	TString fileName = fName;

	cout << "Filename : " << fileName << endl;

	string fname = fileName.Data();

	if (!fileExists(fname)) { cout << "WARNING. Input file does not exist" << endl; exit(1); }

	TFile *f = new TFile(fileName);

	TIter nextkey(f->GetListOfKeys());
	TKey *key;
	while ((key = (TKey*)nextkey()))
	{
		TObject*obj = f->Get(key->GetName());
		if (obj->InheritsFrom("TRestMetadata"))
		{
			((TRestMetadata*)obj)->PrintMetadata();
		}
	}
	/////////////////////////////

	f->Close();

	return 0;

}
