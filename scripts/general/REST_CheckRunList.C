//#include <TObject.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TBranch.h>

#include <vector>
#include <iostream>
using namespace std;


Int_t REST_CheckRunList( TString namePattern, Int_t N = 100000 )
{

    TGeoManager::SetVerboseLevel( 0 );

    vector <TString> filesNotWellClosed;

    TString command = "ls -d -1 " + namePattern + " > /tmp/CheckRunListCommand_72nd72jdl";
    char *cmd = command.Data();
    system( cmd );

    gSystem->Load("librestcore.so");

    int cont = 0;

    char filename[256];
    FILE *flist = fopen( "/tmp/CheckRunListCommand_72nd72jdl", "rt" ); 
    while ( fscanf( flist, "%s\n", filename ) != EOF && cont < N )
    {
        cout << filename << endl;
        cont++;
        TRestRun *run = new TRestRun();
 //       TRestGeometry *geo = new TRestGeometry();

        TFile *f = new TFile( filename );

 //       string fname = fileName.Data();

        if( !run->fileExists( filename ) ) { cout << "WARNING. Input file does not exist" << endl; exit(1); }

        /////////////////////////////
        // Reading metadata classes

        TIter nextkey(f->GetListOfKeys());
        TKey *key;
        while (key = (TKey*)nextkey()) {
            string className = key->GetClassName();
            if ( className == "TRestRun" )
            {
                cout << key->GetName() << endl;
                run = (TRestRun *) f->Get( key->GetName() );
            }
        }

 //       if( geo == NULL ) { cout << "WARNING no TGeoManager class was found" << endl; exit(1); }

        cout << "Run time (hours) : " << run->GetRunLength()/3600. << endl;

        if( run->GetRunLength() == -1 )
        {
            filesNotWellClosed.push_back( filename );
        }

 //       geo->PrintGeometry();
 //       geo->GetTopVolume()->Draw("ogl");
        /////////////////////////////

 //       delete geo;
        delete run;

        f->Close();
    }

    cout << "---------------------" << endl;
    cout << "Files not well closed" << endl;
    cout << "---------------------" << endl;
    for( int i = 0; i < filesNotWellClosed.size(); i++ )
        cout << filesNotWellClosed[i] << endl;
    cout << "---------------------" << endl;

}
