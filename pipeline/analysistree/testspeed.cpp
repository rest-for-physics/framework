#include <chrono>
using namespace chrono;

void testspeedRESTTree() {
    TFile* f = new TFile("abc.root", "recreate");
    TRestAnalysisTree* tree = new TRestAnalysisTree();
    ///////////////////////////////////////////////
    // A. do not use observable
    // double& gaus = tree->AddObservable<double>("gaus");
    // int& poisson = tree->AddObservable<int>("poisson");
    // double& rndm = tree->AddObservable<double>("rndm");
    // double& landau = tree->AddObservable<double>("landau");

    ///////////////////////////////////////////////
    // B. Use quick observable(default)
    // tree->EnableQuickObservableValueSetting();

    ///////////////////////////////////////////////
    // C. Do not use quick observable
    // tree->DisableQuickObservableValueSetting();

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
        ///////////////////////////////////////////////
        // A. do not use observable
        // gaus = gRandom->Gaus(100, 20);
        // poisson = gRandom->Poisson(36);
        // rndm = gRandom->Rndm();
        // landau = gRandom->Landau(10, 2);

        ///////////////////////////////////////////////
        // B.C. Use (quick) observable
        tree->SetObservableValue("gaus", gRandom->Gaus(100, 20));
        tree->SetObservableValue("poisson", gRandom->Poisson(36));
        tree->SetObservableValue("rndm", gRandom->Rndm());
        tree->SetObservableValue("landau", gRandom->Landau(10, 2));

        ///////////////////////////////////////////////
        // D. Use reflected observable
        //tree->SetObservable("gaus", gRandom->Gaus(100, 20));
        //tree->SetObservable("poisson", gRandom->Poisson(36));
        //tree->SetObservable("rndm", gRandom->Rndm());
        //tree->SetObservable("landau", gRandom->Landau(10, 2));

        tree->Fill();
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    cout << (int)duration_cast<microseconds>(t2 - t1).count() << endl;

    tree->Write();
    f->Close();
}

Int_t fEventID = 0;       //!
Int_t fSubEventID = 0;    //!
Double_t fTimeStamp = 0;  //!
TString* fSubEventTag;    //!
Int_t fRunOrigin = 0;     //!
Int_t fSubRunOrigin = 0;  //!

double gaus = 0;
int poisson = 0;
double rndm = 0;
double landau = 0;

void testspeedTTree() {
    TFile* f = new TFile("abc.root", "recreate");
    TTree* tree = new TTree();

    fSubEventTag = new TString();

    tree->Branch("runOrigin", &fRunOrigin);
    tree->Branch("subRunOrigin", &fSubRunOrigin);
    tree->Branch("eventID", &fEventID);
    tree->Branch("subEventID", &fSubEventID);
    tree->Branch("timeStamp", &fTimeStamp);
    tree->Branch("subEventTag", fSubEventTag);

    tree->Branch("gaus", &gaus);
    tree->Branch("poisson", &poisson);
    tree->Branch("rndm", &rndm);
    tree->Branch("landau", &landau);

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
        gaus = gRandom->Gaus(100, 20);
        poisson = gRandom->Poisson(36);
        rndm = gRandom->Rndm();
        landau = gRandom->Landau(10, 2);
        tree->Fill();
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    cout << (int)duration_cast<microseconds>(t2 - t1).count() << endl;

    tree->Write();
    f->Close();
}
