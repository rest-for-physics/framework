///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRunMerger.cxx
///
///             Base class from which to inherit all other event classes in REST 
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 J. Galan
///_______________________________________________________________________________


#include "TRestRunMerger.h"

ClassImp(TRestRunMerger)
//______________________________________________________________________________
    TRestRunMerger::TRestRunMerger()
{
   // TRestRunMerger default constructor
}

//______________________________________________________________________________
TRestRunMerger::~TRestRunMerger()
{
   // TRestRunMerger destructor

}


TRestRunMerger::TRestRunMerger( TString namePattern )
{


    SetFilelist( namePattern );

}

void TRestRunMerger::MergeFiles( TString outputFilename )
{
    for( unsigned int fID = 0; fID < fInputFileList.size(); fID++ )
    {
        AddFile( fInputFileList[ fID ] );
    }
    OutputFile( outputFilename );

    Merge();
}

void TRestRunMerger::SetFilelist( TString namePattern )
{
    fInputFileList.clear();

    TString command = "ls -d -1 " + namePattern + " > /tmp/RunMerger_X8asa2kf";
    char cmd[256];
    sprintf( cmd, "%s", command.Data() );
    if( system( cmd ) == -1 ) 
    {
        cout << "TRestRunMerger::SetFileList." << endl;
        cout << "REST Internal error. Contact rest-dev@cern.ch" << endl;
    }
    else
    {
        char filename[256];
        FILE *flist = fopen( "/tmp/RunMerger_X8asa2kf", "rt" ); 
        while ( fscanf( flist, "%s\n", filename ) != EOF )
        {
            fInputFileList.push_back( filename );
        }
    }

}


// TODO check the file list. If there is a run that was not properly closed we should remove it from the list

void TRestRunMerger::PrintFilelist()
{
    cout << " TRestRunMerger file list" << endl;
    for( unsigned int fN = 0; fN < fInputFileList.size(); fN++ )
    {
        cout << "File [" << fN << "] = " << fInputFileList[fN] << endl;
    }
}

