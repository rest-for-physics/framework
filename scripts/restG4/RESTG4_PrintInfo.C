//#include <TObject.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TBranch.h>

#include <iostream>
using namespace std;

Int_t RESTG4_PrintInfo( TString fName )
{
    cout << "Filename : " << fName << endl;

    gSystem->Load("librestcore.so");

    TRestRun *run = new TRestRun();
    string fname = fName.Data();

    if( !run->fileExists( fname ) ) { cout << "WARNING. Input file does not exist" << endl; exit(1); }

    TFile *f = new TFile( fName );

    /////////////////////////////
    // Reading metadata classes
    TRestG4Metadata *metadata = new TRestG4Metadata();

    TIter nextkey(f->GetListOfKeys());
    TKey *key;
    while (key = (TKey*)nextkey()) {
        string className = key->GetClassName();
        if ( className == "TRestG4Metadata" )
        {
            metadata = (TRestG4Metadata *) f->Get( key->GetName() );
        }
        if ( className == "TRestRun" )
        {
            run = (TRestRun *) f->Get( key->GetName() );
        }
    }

    if( metadata == NULL ) { cout << "WARNING no TRestG4Metadata class was found" << endl; exit(1); }
    if( run == NULL ) { cout << "WARNING no TRestRun class was found" << endl; exit(1); }

    metadata->PrintMetadata();
    /////////////////////////////
    
    delete metadata;

    f->Close();

}
