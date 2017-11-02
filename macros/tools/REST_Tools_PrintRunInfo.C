//#include <TObject.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TBranch.h>

#include <string>
#include <iostream>
using namespace std;

#include <TString.h>


Int_t REST_PrintRunInfo( TString fName )
{
    TString fileName = fName;

    cout << "Filename : " << fileName << endl;

    /////////////////////////////
    // Reading TRestRun metadata class
    TRestRun *run = new TRestRun();

    string fname = fileName.Data();

    if( !run->fileExists( fname ) ) { cout << "WARNING. Input file does not exist" << endl; exit(1); }

    TFile *f = new TFile( fileName );

    TIter nextkey(f->GetListOfKeys());
    TKey *key;
    while ( (key = (TKey*) nextkey()) ) {
        string className = key->GetClassName();
        if ( className == "TRestRun" )
        {
            cout << "I found TRestRun" << endl;
            run = (TRestRun *) f->Get( key->GetName() );
        }
    }

    if( run == NULL ) { cout << "WARNING no TRestRun class was found" << endl; exit(1); }

    run->PrintInfo();
    /////////////////////////////
    
    f->Close();

    return 0;
}
