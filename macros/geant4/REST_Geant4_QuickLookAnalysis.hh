#include "TRestTask.h"
#include "TRestG4Metadata.h"
#include "TRestG4Event.h"

Int_t REST_Geant4_QuickLookAnalysis( TString fName )
{
    const double eMin_ROI = 2458-25;
    const double eMax_ROI = 2458+25;

    const Double_t xMax = 4000;
    const Double_t yMax = 4000;
    const Double_t zMax = 4000;

    Int_t nbinsROI = (Int_t ) ( eMax_ROI - eMin_ROI);

    cout << "Filename : " << fName << endl;

    TRestRun *run = new TRestRun();
    string fname = fName.Data();

    if( !fileExists( fname ) ) { cout << "WARNING. Input file does not exist" << endl; exit(1); }

    TFile *f = new TFile( fName );

    TRestG4Metadata *metadata = new TRestG4Metadata();

    // Getting metadata
    TIter nextkey(f->GetListOfKeys());
    TKey *key;
    while ( (key = (TKey*)nextkey()) ) {
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

    run->PrintMetadata();

    metadata->PrintMetadata();

    TRestG4Event *ev = new TRestG4Event();

    TTree *tr = (TTree *) f->Get("TRestG4EventTree");

    TBranch *br = tr->GetBranch( "eventBranch" );

    br->SetAddress( &ev );

    cout << "Total number of entries : " << tr->GetEntries() << endl;
    Double_t eDepMax = 0;
    Double_t eDepMin = 1e10;

    cout << "Obtainning max/min values" << endl;
    for( int evId = 0; evId < tr->GetEntries(); evId++ )
    {
        if( evId%100000 == 0 ) cout << "Event : " << evId << endl;
        tr->GetEntry(evId);
        Double_t eDep = ev->GetTotalDepositedEnergy();
        //cout << "Event : " << evID << " Energy : " << eDep << " keV" << endl;
        if( eDep > eDepMax ) eDepMax = eDep;
        if( eDep < eDepMin ) eDepMin = eDep;
    }

    Int_t nbins = (Int_t) (eDepMax-eDepMin);
    TH1D *depositedEnergy_Histogram = new TH1D( "Edep","Total deposited energy", nbins , eDepMin, eDepMax ); 
    TH1D *depositedEnergy_ROI_Histogram = new TH1D( "Edep_ROI","Total deposited energy", nbinsROI, eMin_ROI, eMax_ROI ); 

    // Define a function to define all these properties
    depositedEnergy_Histogram->GetXaxis()->SetTitle("Energy [keV]");
    depositedEnergy_Histogram->GetXaxis()->SetLabelSize( 0.06 );
    depositedEnergy_Histogram->GetXaxis()->SetTitleSize( 0.06 );

    depositedEnergy_Histogram->GetYaxis()->SetTitle("Counts [keV-1]");
    depositedEnergy_Histogram->GetYaxis()->SetLabelSize( 0.06 );
    depositedEnergy_Histogram->GetYaxis()->SetTitleSize( 0.06 );

    depositedEnergy_ROI_Histogram->GetXaxis()->SetTitle("Energy [keV]");
    depositedEnergy_ROI_Histogram->GetXaxis()->SetLabelSize( 0.04 );
    depositedEnergy_ROI_Histogram->GetXaxis()->SetTitleSize( 0.06 );

    depositedEnergy_ROI_Histogram->GetYaxis()->SetTitle("Counts [keV-1]");
    depositedEnergy_ROI_Histogram->GetYaxis()->SetLabelSize( 0.04 );
    depositedEnergy_ROI_Histogram->GetYaxis()->SetTitleSize( 0.06 );

    TH2D *eventOriginXY_Histo = new TH2D( "XY_Dist", "Event origin in XY (slide 20cm)", nbins, -xMax, xMax, nbins, -yMax, yMax );
    TH2D *eventOriginYZ_Histo = new TH2D( "YZ_Dist", "Event origing in ZY (slide 20cm)", nbins, -yMax, yMax, nbins, -zMax, zMax );

    cout << "Filling histograms" << endl;
    for( int evID = 0; evID < tr->GetEntries(); evID++ )
    {
        tr->GetEntry(evID);
        if( evID%100000 == 0 ) cout << "Event : " << evID << endl;

        Double_t eDep = ev->GetTotalDepositedEnergy();

        TVector3 evPosition = ev->GetPrimaryEventOrigin();
        if( eDep > 0 )
        {
            depositedEnergy_Histogram->Fill( eDep );

            if( eDep > eMin_ROI && eDep < eMax_ROI )
                depositedEnergy_ROI_Histogram->Fill( eDep );
            //       ev->PrintEvent();
            if( evPosition.Z() > -100 && evPosition.Z() < 100 )
                eventOriginXY_Histo->Fill( evPosition.X(), evPosition.Y() );
            if( evPosition.X() > -100 && evPosition.X() < 100 )
                eventOriginYZ_Histo->Fill( evPosition.Z(), evPosition.Y() );

        }



    }

    gStyle->SetOptStat("ne");
    gStyle->SetStatY(0.9);
    gStyle->SetStatX(0.8);
    gStyle->SetStatW(0.3);
    gStyle->SetStatH(0.3); 
    gStyle->SetPadLeftMargin( 0.15 );
    gStyle->SetPadBottomMargin( 0.15 );

    //   gPad->SetLeftMargin( 0.15 );


    TCanvas* c1 = new TCanvas("Energy Deposited", "  ");
    c1->SetWindowPosition( 0, 0 );
    c1->SetWindowSize( 600, 400 );
    //c1->Divide( 2, 2 );
    //c1->cd(1);

    depositedEnergy_Histogram->SetLineWidth( 2 );

    depositedEnergy_Histogram->Draw("same");

    c1->Update();
    getchar();

    TCanvas* c3 = new TCanvas("Energy Deposited (ROI)", "  ");
    c3->SetWindowPosition( 600, 0 );
    c3->SetWindowSize( 600, 400 );
    depositedEnergy_ROI_Histogram->SetLineWidth( 2 );
    depositedEnergy_ROI_Histogram->Draw("same");

    c3->Update();

    getchar();

    TCanvas* c2 = new TCanvas("Event origin XY (slide 20cm)", "  ");
    c2->SetWindowPosition( 0, 400 );
    c2->SetWindowSize( 600, 400 );
    eventOriginXY_Histo->Draw( "same" );

    c2->Update();
    getchar();

    TCanvas* c4 = new TCanvas("Event origin YZ (slide 20cm)", "  ");
    c4->SetWindowPosition( 600, 400 );
    c4->SetWindowSize( 600, 400 );
    eventOriginYZ_Histo->Draw( "same" );

    c4->Update();
    getchar();

    delete ev;

    f->Close();

    return 0;
}
