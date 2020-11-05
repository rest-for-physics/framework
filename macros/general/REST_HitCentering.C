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
//*** around 0 by subtracting the mean event position to each individual hit position. It creates two
//*** histograms including events n1 to n2 and stores it in file.root. One for the XZ events (with an 'X'
//*** suffix) and one for YZ events (with a 'Y' suffix).
//*** The given number of bins and bin range will be applied to both histograms.
//***
//*** Hits will only be added to the histograms, iff they have a valid coordinate. That is if their
//*** coordinate is unequal to `invalidVal`. The default value of -0.125 corresponds to an invalid
//*** coordinate for the IAXO/CAST readout.
//*** Each hit only has one coordinate that is valid (either X or Y) while the other is `invalidVal`,
//*** even if two strips are produced from one physical event.
//*** --------------
//*** The output histograms show the standard shape of the hits events, so their symmetry, gaussianity
//*** and others can be estimated.
//*** --------------
//*** Usage: restManager HitCentering /full/path/file.root [histoName] [firstBin] [lastBin]
//*** [#bins] [firstEvent] [lastEvent] [invalidVal]
//*******************************************************************************************************

Int_t REST_HitCentering(TString rootFileName, TString histoName, int startVal = -30, int endVal = 30,
                        int bins = 120, int n1 = 0, int n2 = 60000, double invalidVal = -0.125) {
    TRestStringOutput cout;

    std::vector<string> inputFilesNew = TRestTools::GetFilesMatchingPattern((string)rootFileName);

    TH1D* hX = new TH1D(histoName + "X", histoName + "X", bins, startVal, endVal);
    TH1D* hY = new TH1D(histoName + "Y", histoName + "Y", bins, startVal, endVal);

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

            // Fill in the histogram with position and energy if the coordinate is valid
            // i.e. unequal to `invalidVal`
            for (int n = 0; n < ev->GetNumberOfHits(); n++) {
                Double_t en = ev->GetEnergy(n);
                Double_t valX = ev->GetX(n);
                Double_t meanX = ev->GetMeanPosition().X();
                Double_t valY = ev->GetY(n);
                Double_t meanY = ev->GetMeanPosition().Y();

                if (valX != invalidVal) hX->Fill(valX - meanX, en);
                if (valY != invalidVal) hY->Fill(valY - meanY, en);
            }
        }

        // cleanup
        delete ev;
        delete run;
    }

    TFile* f = new TFile(rootFileName, "update");
    hX->Write(histoName + "X");
    hY->Write(histoName + "Y");
    f->Close();

    cout << "Written histograms " << histoName << "X/Y into " << rootFileName << endl;

    return 0;
};

#endif
