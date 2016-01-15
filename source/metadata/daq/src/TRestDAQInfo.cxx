///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDaqInfo inherited from TObject
///
///             Store DAQ info of the run
///
///             now 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia
///_______________________________________________________________________________


#include "TRestDaqInfo.h"


ClassImp(TRestDaqInfo)
//______________________________________________________________________________
    TRestDaqInfo::TRestDaqInfo()
{
    Initialize();

}

//______________________________________________________________________________
TRestDaqInfo::~TRestDaqInfo()
{
    cout << "Deleting TRestDaqInfo" << endl;
}

void TRestDaqInfo::SetPedStringBuffer(TString fName){

ifstream file(fName);
if( file == NULL ) { cout<< __PRETTY_FUNCTION__ << " ERROR: " << fName <<" not found " <<endl; return; }

string buffer;
string line;
while(getline(file, line)){ buffer += line; buffer}

file.close();

return buffer;

}

void SetParFromBuffer(string buffer){



}




