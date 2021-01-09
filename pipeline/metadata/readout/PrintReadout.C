#include <fstream>
#include <iostream>
#include <string>
#include "TRestDetectorReadout.h"
#include "TRestTask.h"

void PrintReadout(TString fName) {
    TString fileName = fName;

    string fname = fileName.Data();

    if (!TRestTools::fileExists(fname)) {
        cout << "WARNING. Input file does not exist" << endl;
        exit(1);
    }

    fstream file;
    file.open("print.txt", ios::out);
    string line;

    streambuf* stream_buffer_cout = cout.rdbuf();
    streambuf* stream_buffer_file = file.rdbuf();

    cout.rdbuf(stream_buffer_file);

    cout << "Filename : " << fileName << endl;

    TFile* f = new TFile(fileName);

    TIter nextkey(f->GetListOfKeys());
    TKey* key;
    int n = 0;
    while ((key = (TKey*)nextkey())) {
        if (((string)(key->GetClassName())).find("TRestDetectorReadout") != -1) {
            TObject* obj = f->Get(key->GetName());
            TRestDetectorReadout readout = *(TRestDetectorReadout*)obj;
            readout[0].Print(1);
        }
        n++;
    }

    f->Close();

    cout.rdbuf(stream_buffer_cout);  // back to the old buffer
    file.close();

    return 0;
}
