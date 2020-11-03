#include <TCanvas.h>
#include <TF1.h>
#include <TH1D.h>
#include <TMath.h>
#include <TRestRun.h>
#include <TRestTask.h>
#include <TSystem.h>

#ifndef RestTask_HitCentering
#define RestTask_HitCentering

//*******************************************************************************************************
//*** Description: This macro reads each hits event inside the file.root given by argument and centers it
//*** around 0 by subtracting the mean event position to each individual hit position. It creates a
//*** histogram including events n1 to n2 and stores it in file.root. It can be executed for both XZ
//*** events (isX = true) or YZ events (isX = false).
//*** --------------
//*** The output histogram shows the standard shape of the hits events, so their symmetry, gaussianity
//*** and others can be estimated. 
//*** --------------
//*** Usage: restManager HitCentering bool /full/path/file.root [histoName] [firstBin] [lastBin]
//*** [#bins] [firstEvent] [lastEvent]
//*******************************************************************************************************

Int_t REST_HitCentering(bool isX, TString rootFileName, TString histoName, int startVal = -30,
                        int endVal = 30, int bins = 120, int n1 = 0, int n2 = 60000) {
	/*
	Set first argument to true to run the macro on the X direction, and to false to run it on the Y direction.
	*/
    TRestStringOutput cout;

    std::vector<string> inputFilesNew = TRestTools::GetFilesMatchingPattern((string)rootFileName);

    TH1D* h = new TH1D(histoName, histoName, bins, startVal, endVal);

    if (inputFilesNew.size() == 0) {
        cout << "Files not found!" << endl;
        return -1;
    }

    for (unsigned int n = 0; n < inputFilesNew.size(); n++) {
        TRestRun* run = new TRestRun();

        run->OpenInputFile(inputFilesNew[n]);

    // Reading events

    TRestHitsEvent* ev = new TRestHitsEvent();

    run->SetInputEvent(ev);
    for (int i = n1; i < n2 + 1; i++) {
        run->GetEntry(i);

		//Fill in the histogram with position and energy

		for( int n = 0; n < ev->GetNumberOfHits(); n++ ) {
			Double_t val, mean;
			if (isX) {
				val = ev->GetX(n);   
				mean = ev->GetMeanPosition().X();  	
			}
			else {
				val = ev->GetY(n);
				mean = ev->GetMeanPosition().Y();
			}
            
			if (val != -0.125){
				// I subtract the mean position of the event to center the histogram.
				Double_t cent_val = val - mean;

				Double_t en = ev->GetEnergy(n);

				// Fill in the histogram

				h->Fill( cent_val, en);
			}
		}
    }

    delete ev;

    /////////////////////////////

        delete run;
    }


    TFile* f = new TFile(rootFileName, "update");
    h->Write(histoName);
    f->Close();

    cout << "Written histogram " << histoName << " into " << rootFileName << endl;

    return 0;
};


#endif
