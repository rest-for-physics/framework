#include "TRestDetectorGainMap.h"

#include "TGraph2D.h"
#include "TLegend.h"
#include "TRandom.h"
#include "TStyle.h"
#include "TView.h"

#include "TRestDetectorReadout.h"
ClassImp(TRestDetectorGainMap);

void TRestDetectorGainMap::InitFromConfigFile() {
    // read config from rml section
}

void TRestDetectorGainMap::DrawChannelGainMap(TRestDetectorReadoutModule* mod) {
    if (mod == NULL) {
        int min = 1e9;
        int max = 0;
        auto iter = fChannelGain.begin();
        while (iter != fChannelGain.end()) {
            if (max < iter->first) max = iter->first;
            if (min > iter->first) min = iter->first;
            iter++;
        }

        TH1D* h = new TH1D("GainMap", "GainMap", max - min + 2, min - 1, max + 1);
        iter = fChannelGain.begin();
        while (iter != fChannelGain.end()) {
            h->SetBinContent(h->FindBin(iter->first), iter->second);
            iter++;
        }
        h->Draw();
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
            double x = xmax * (r->Rndm());
            double y = ymax * (r->Rndm());
            gr->SetPoint(i, x, y, 1);
        }

        for (int i = 0; i < mod->GetNumberOfChannels(); i++) {
            TRestDetectorReadoutChannel* channel = mod->GetChannel(i);
            int id = channel->GetDaqID();
            if (fChannelGain.count(id) == 0) fChannelGain[id] = 1;

            auto pos = channel->GetPixel(channel->GetNumberOfPixels() / 2)->GetCenter();
            gr->SetPoint(i + N, pos.X(), pos.Y(), fChannelGain[id]);

            cout << pos.X() << " " << pos.Y() << " " << fChannelGain[id] << endl;

            // pos = channel->GetPixel(channel->GetNumberOfPixels() - 1)->GetCenter();
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
