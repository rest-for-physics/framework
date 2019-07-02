#include <iostream>
#include <vector>
#include "TGeoManager.h"
#include "TRestTask.h"
#include "TSystem.h"
using namespace std;

Int_t REST_CheckRunFileList(TString namePattern, Int_t N = 100000) {
    TGeoManager::SetVerboseLevel(0);

    vector<TString> filesNotWellClosed;

    // TString command = "ls -d -1 " + namePattern + " > /tmp/CheckRunListCommand_72nd72jdl";
    // char cmd[512];
    // sprintf( cmd, "%s", command.Data() );
    // system( cmd );

    // gSystem->Load("librestcore.so");

    // int cont = 0;

    // char filename[256];
    // FILE *flist = fopen( "/tmp/CheckRunListCommand_72nd72jdl", "rt" );
    TRestStringOutput cout;

    string a = TRestTools::Execute((string)("ls -d -1 " + namePattern));
    vector<string> b = Split(a, "\n");

    int cont = 0;
    for (int i = 0; i < b.size(); i++) {
        string filename = b[i];
        cout << filename << endl;
        cont++;
        TRestRun* run = new TRestRun();

        TFile* f = new TFile(filename.c_str());

        if (!TRestTools::fileExists(filename)) {
            cout << "WARNING. Input file does not exist" << endl;
            exit(1);
        }

        /////////////////////////////
        // Reading metadata classes

        TIter nextkey(f->GetListOfKeys());
        TKey* key;
        while ((key = (TKey*)nextkey())) {
            string className = key->GetClassName();
            if (className == "TRestRun") {
                cout << key->GetName() << endl;
                run = (TRestRun*)f->Get(key->GetName());
            }
        }

        cout << "Run time (hours) : " << run->GetRunLength() / 3600. << endl;

        if (run->GetRunLength() == -1) {
            filesNotWellClosed.push_back(filename);
        }

        delete run;

        f->Close();
    }

    // while ( fscanf( flist, "%s\n", filename ) != EOF && cont < N )
    //{
    //    cout << filename << endl;
    //    cont++;
    //    TRestRun *run = new TRestRun();

    //    TFile *f = new TFile( filename );

    //    if( !fileExists( filename ) ) { cout << "WARNING. Input file does not exist" << endl; exit(1); }

    //    /////////////////////////////
    //    // Reading metadata classes

    //    TIter nextkey(f->GetListOfKeys());
    //    TKey *key;
    //    while ( ( key =  (TKey*)nextkey() ) ) {
    //        string className = key->GetClassName();
    //        if ( className == "TRestRun" )
    //        {
    //            cout << key->GetName() << endl;
    //            run = (TRestRun *) f->Get( key->GetName() );
    //        }
    //    }

    //    cout << "Run time (hours) : " << run->GetRunLength()/3600. << endl;

    //    if( run->GetRunLength() == -1 )
    //    {
    //        filesNotWellClosed.push_back( filename );
    //    }

    //    delete run;

    //    f->Close();
    //}

    cout << "---------------------" << endl;
    cout << "Files not well closed" << endl;
    cout << "---------------------" << endl;
    for (int i = 0; i < filesNotWellClosed.size(); i++) cout << filesNotWellClosed[i] << endl;
    cout << "---------------------" << endl;

    return 0;
}
