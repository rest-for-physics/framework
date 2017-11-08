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


void TRestExternalFileProcess::ReadFormat()
{
	//format example:
	//run[aaaa]_cobo[bbbb]_frag[cccc]_[time].graw
	//we are going to match it with inputfile:
	//run00042_cobo1_frag0000.graw

	string format = GetParameter("inputFormat", "");
	string name = (string)fInputFileNames[0];

	//去除后缀名
	if (format.find(".") != -1) { format = Spilt(format, ".")[0]; }
	if (name.find(".") != -1) { name = Spilt(name, ".")[0]; }

	//去除路径名
	if (format.find("/") != -1) { format = Spilt(format, "/")[Spilt(format, "/").size() - 1]; }
	if (name.find("/") != -1) { name = Spilt(name, "/")[Spilt(name, "/").size() - 1]; }

	vector<string> formatsectionlist = Spilt(format, "_");
	vector<string> inputfilesectionlist = Spilt(name, "_");
	if (formatsectionlist.size() == 0)return;

	for (int i = 0; i < formatsectionlist.size(); i++)
	{
		int pos1 = formatsectionlist[i].find("[");
		int pos2 = formatsectionlist[i].find("]");
		if (pos1 == -1 || pos2 == -1) { continue; }

		string prefix = formatsectionlist[i].substr(0, pos1 - 0);
		string FormatName = formatsectionlist[i].substr(pos1 + 1, pos2 - pos1 - 1);
		string FormatValue = "";
		if (prefix == "")
		{
			//如果参数是最后一个，则直接对应inputfile的最后几个参数
			if (i == formatsectionlist.size() - 1 && inputfilesectionlist.size() >= formatsectionlist.size())
			{
				for (int j = 0; j <= inputfilesectionlist.size() - formatsectionlist.size(); j++)
				{
					FormatValue += inputfilesectionlist[j + i] + "_";
				}
				FormatValue = FormatValue.substr(0, FormatValue.size() - 1);
			}
			//如果参数不是最后一个，则直接对应inputfile相应位置参数
			else if (i < formatsectionlist.size() - 1 && inputfilesectionlist.size() >i)
			{
				FormatValue = inputfilesectionlist[i];
			}
			else continue;
		}
		else
		{
			bool find;
			for (int j = 0; j < inputfilesectionlist.size(); j++)
			{
				if (inputfilesectionlist[j].find(prefix) == 0)
				{
					FormatValue = inputfilesectionlist[j].substr(prefix.size(), -1);
					find = true;
					break;
				}
				else if (j == inputfilesectionlist.size() - 1) find = false;
			}
			if (find == false) continue;
		}




	}

}



