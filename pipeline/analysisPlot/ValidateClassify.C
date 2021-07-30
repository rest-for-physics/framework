Int_t ValidateClassify() {
    TFile* f = new TFile("/tmp/canvas.root");

    TH1D* h1 = (TH1D*)f->Get("NumberSignals10852");
    if (!h1) {
        cout << "Histogram NumberSignals10852 not found!" << endl;
        return 1;
    }

    cout << h1->GetEntries() << endl;

    if (h1->GetEntries() != 7584) {
        cout << "The number of entries at Run 10852 is not 7584!" << endl;
        return 2;
    }

    TH1D* h2 = (TH1D*)f->Get("NumberSignals10964");
    if (!h2) {
        cout << "Histogram NumberSignals10964 not found!" << endl;
        return 3;
    }

    cout << h2->GetEntries() << endl;

    if (h2->GetEntries() != 3812) {
        cout << "The number of entries at Run 10964 is not 3812!" << endl;
        return 4;
    }

    return 0;
}
