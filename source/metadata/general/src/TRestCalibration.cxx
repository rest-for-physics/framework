#include "TRestCalibration.h"

#include "TGraph2D.h"
#include "TRandom.h"
#include "TRestReadout.h"
#include "TStyle.h"
#include "TView.h"

ClassImp(TRestCalibration);

void TRestCalibration::DrawChannelGainMap(TRestReadoutModule* mod) {
    if (mod == NULL) {
        vector<double> id;
        vector<double> gain;
        auto iter = fChannelGain.begin();
        while (iter != fChannelGain.end()) {
            id.push_back(iter->first);
            gain.push_back(iter->second);
            iter++;
        }
        TGraph* gr = new TGraph(id.size(), &id[0], &gain[0]);
        gr->Draw();
    } else {
        double xmin = 0, xmax = mod->GetModuleSizeX(), ymin = 0, ymax = mod->GetModuleSizeY();
        cout << xmin << " " << xmax << " " << ymin << " " << ymax << endl;

        TCanvas* c1 = new TCanvas();
        gStyle->SetPalette(kBlueYellow);
        c1->SetTheta(90);
        c1->SetPhi(0);

        TGraph2D* gr = new TGraph2D();
        // we throw randomly 1000 points to fill the graph
         TRandom* r = new TRandom();
        int N = mod->GetNumberOfChannels() * mod->GetNumberOfChannels() / 4;
         for (int i = 0; i < N; i++) {
            double x =  xmax * (r->Rndm());
            double y = ymax * (r->Rndm());
            gr->SetPoint(i, x, y, 1);
        }

        for (int i = 0; i < mod->GetNumberOfChannels(); i++) {
            TRestReadoutChannel* channel = mod->GetChannel(i);
            int id = channel->GetDaqID();
            if (fChannelGain.count(id) == 0) fChannelGain[id] = 1;

            auto pos = channel->GetPixel(channel->GetNumberOfPixels()/2)->GetCenter();
            gr->SetPoint(i+N, pos.X(), pos.Y(), fChannelGain[id]);

            cout << pos.X() << " " << pos.Y() << " " << fChannelGain[id] << endl;

            //pos = channel->GetPixel(channel->GetNumberOfPixels() - 1)->GetCenter();
            // gr->SetPoint(2 * i + 1, pos.X(), pos.Y(), 1);

            // cout << pos.X() << " " << pos.Y() << " " << 1 << endl;
        }

        gr->SetPoint(0, xmin, ymin, 1);
        gr->SetPoint(1, xmin, ymax, 1);
        gr->SetPoint(2, xmax, ymin, 1);
        gr->SetPoint(3, xmax, ymax, 1);

        gr->SetTitle(Form("Channel gain map for readout module%i", mod->GetModuleID()));

        gr->SetNpx(mod->GetNumberOfChannels());
        gr->SetNpy(mod->GetNumberOfChannels());
        gr->Draw("colz");
    }
}