///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestExternalFileProcess.cxx
///
///             Template to use to design "event process" classes inherited from 
///             TRestExternalFileProcess
///             How to use: replace TRestExternalFileProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________


#include "TRestExternalFileProcess.h"
using namespace std;
#include "TTimeStamp.h"

ClassImp(TRestExternalFileProcess)
//______________________________________________________________________________
TRestExternalFileProcess::TRestExternalFileProcess()
{
	Initialize();
}

TRestExternalFileProcess::TRestExternalFileProcess(char *cfgFileName)
{
  
}


//______________________________________________________________________________
TRestExternalFileProcess::~TRestExternalFileProcess()
{

   // TRestExternalFileProcess destructor
} 


//______________________________________________________________________________
void TRestExternalFileProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fInputEvent = NULL;
    fInputExtFiles.clear();

    fSingleThreadOnly = true;

}


Int_t TRestExternalFileProcess::OpenInputExtFiles(vector<TString> files)
{

	nFiles = 0;
	fInputExtFiles.clear();
	fInputFileNames.clear();

	for (int i = 0; i < files.size(); i++) {

		FILE *f = fopen(files[i].Data(), "rb");

		if (f == NULL)
		{
			cout << "WARNING. Input file does not exist" << endl;
			cout << "File : " << files[i] << endl;
			continue;
		}

		fInputExtFiles.push_back(f);
		fInputFileNames.push_back(files[i]);
		nFiles++;
	}

	debug << this->GetName() << " : opened " << nFiles << " files" << endl;
	return nFiles;
}




