#include <algorithm>
#include <vector>
#include "TRestReadout.h"
#include "TRestTask.h"

#include <iostream>
using namespace std;

TGraph* GetHittedStripMap(TRestReadoutPlane* p, Int_t mask[4], Double_t region[4], Int_t N);

Int_t REST_CheckReadout(TString rootFile, TString name, Double_t region[4], Int_t stripsMask[4],
                        Int_t N = 1E4, Int_t plane = 0) {
    TFile* f = new TFile(rootFile);
    TRestReadout* readout = (TRestReadout*)f->Get(name);
    readout->PrintMetadata();

    TRestReadoutPlane* readoutPlane = &(*readout)[plane];

    Int_t nModules = readoutPlane->GetNumberOfModules();

    Int_t totalPixels = 0;
    Int_t totalChannels = 0;
    for (int mdID = 0; mdID < nModules; mdID++) {
        TRestReadoutModule* module = &(*readoutPlane)[mdID];
        Int_t nChannels = module->GetNumberOfChannels();
        totalChannels += nChannels;

        for (int ch = 0; ch < nChannels; ch++) {
            TRestReadoutChannel* channel = &(*module)[ch];
            Int_t nPixels = channel->GetNumberOfPixels();
            totalPixels += nPixels;
        }
    }

    const Int_t nPixConst = totalPixels;
    const Int_t nModConst = nModules;
    const Int_t nChConst = totalChannels;

    vector<TGraph*> pixelGraph(nPixConst);
    vector<TGraph*> channelGraph(nChConst);
    vector<TGraph*> modGraph(nModConst);
    vector<TLatex*> channelIDLabel(nPixConst);

    double xmin = 1E9, xmax = -1E9, ymin = 1E9, ymax = -1E9;

    Int_t graph = 0;
    Int_t modGraphID = 0;
    Int_t chGraph = 0;
    for (int mdID = 0; mdID < nModules; mdID++) {
        TRestReadoutModule* module = &(*readoutPlane)[mdID];
        Int_t nChannels = module->GetNumberOfChannels();

        Double_t x[5];
        Double_t y[5];

        for (int v = 0; v < 5; v++) {
            x[v] = module->GetVertex(v).X();
            y[v] = module->GetVertex(v).Y();

            if (x[v] < xmin) xmin = x[v];
            if (y[v] < ymin) ymin = y[v];
            if (x[v] > xmax) xmax = x[v];
            if (y[v] > ymax) ymax = y[v];
        }
        modGraph[modGraphID] = new TGraph(5, x, y);

        modGraph[modGraphID]->SetLineColor(kBlack);
        modGraph[modGraphID]->SetLineWidth(2);

        modGraphID++;

        for (int ch = 0; ch < nChannels; ch++) {
            TRestReadoutChannel* channel = &(*module)[ch];

            Int_t nPixels = channel->GetNumberOfPixels();

            Double_t xCH[1000], yCH[1000];
            for (int px = 0; px < nPixels; px++) {
                xCH[px] = module->GetPixelCenter(ch, px).X();
                yCH[px] = module->GetPixelCenter(ch, px).Y();

                for (int v = 0; v < 5; v++) {
                    x[v] = module->GetPixelVertex(ch, px, v).X();
                    y[v] = module->GetPixelVertex(ch, px, v).Y();
                }
                pixelGraph[graph] = new TGraph(5, x, y);

                pixelGraph[graph]->SetLineColor(kRed);
                pixelGraph[graph]->SetLineWidth(2);
                graph++;
            }
        }
    }

    TCanvas* c = new TCanvas("ReadoutGraphViewer", "  ", 900, 900);
    c->DrawFrame(xmin, ymin, xmax, ymax);
    c->SetTicks();

    GetHittedStripMap(readoutPlane, stripsMask, region, N)->Draw("Psame");

    for (int i = 0; i < modGraphID; i++) modGraph[i]->Draw("same");

    for (int i = 0; i < graph; i++) pixelGraph[i]->Draw("same");

    return 0;
}

TGraph* GetHittedStripMap(TRestReadoutPlane* p, Int_t mask[4], Double_t region[4], Int_t N) {
    Double_t xmin, xmax, ymin, ymax;

    p->GetBoundaries(xmin, xmax, ymin, ymax);

    cout << "Xmax : " << xmax << " Xmin : " << xmin << endl;
    cout << "Ymax : " << ymax << " Ymin : " << ymin << endl;

    cout << "region 1 : " << region[0] << endl;
    cout << "region 2 : " << region[1] << endl;
    cout << "region 3 : " << region[2] << endl;
    cout << "region 4 : " << region[3] << endl;

    Double_t xMin = region[0] * (xmax - xmin) + xmin;
    Double_t xMax = region[1] * (xmax - xmin) + xmin;

    Double_t yMin = region[2] * (ymax - ymin) + ymin;
    Double_t yMax = region[3] * (ymax - ymin) + ymin;

    cout << "Xmax : " << xMax << " Xmin : " << xMin << endl;
    cout << "Ymax : " << yMax << " Ymin : " << yMin << endl;

    TRandom* rnd = new TRandom();

    Double_t xR, yR;

    std::vector<Int_t> channelIds;

    for (int n = 0; n < 128; n++) {
        Int_t bit = n % 32;
        Int_t level = n / 32;

        Int_t msk = 0x1 << bit;
        if (msk & mask[level]) {
            cout << "Adding channel : " << 32 * level + bit << endl;
            channelIds.push_back(32 * level + bit);
        }
    }

    std::vector<Double_t> xPos;
    std::vector<Double_t> yPos;

    Int_t cont = 0;
    while (N > 0) {
        xR = rnd->Uniform(xMin, xMax);
        yR = rnd->Uniform(yMin, yMax);
        for (int mod = 0; mod < p->GetNumberOfModules(); mod++) {
            Int_t chFound = p->FindChannel(mod, xR, yR);

            if (chFound != -1 &&
                std::find(channelIds.begin(), channelIds.end(), chFound) != channelIds.end()) {
                xPos.push_back(xR);
                yPos.push_back(yR);
            }
        }
        N--;

        if (N % 10000 == 0) {
            system("date");
            cout << "N: " << N << endl;
        }
    }

    Int_t points = xPos.size();
    TGraph* gr = new TGraph(points, &xPos[0], &yPos[0]);
    gr->SetMarkerStyle(8);

    gr->SetMarkerSize(0.5);

    return gr;
}
