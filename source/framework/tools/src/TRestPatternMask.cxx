/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see https://gifna.unizar.es/trex                 *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see https://www.gnu.org/licenses/.                            *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

/////////////////////////////////////////////////////////////////////////
/// This pure abstract class will help to encapsulate different pattern
/// masks. It defines common public methods TRestPatternMask::GetRegion and
/// TRestPatternMask::HitsPattern that allow to identify the region were
/// a (x,y) coordinate was found, or if has hitted the pattern structure
/// defined at the inherited class.
///
/// \note The mask will always be centered at (0,0). The offset applied
/// in this class will only affect the pattern. Any global offset needs
/// to be introduced externally.
///
/// The rotation members are stored in this class to allow a common
/// definition of rotation and translation parameters for any pattern.
///
/// ### Parameters
/// The following parameters are common to any TRestPatternMask.
/// * **offset**: A parameter to shift the pattern window mask.
/// * **rotationAngle**: An angle given in radians to rotate the pattern.
/// * **maskRadius**: A radius defining the limits of the circular mask. If
/// it is equal to zero, the mask has no limits.
///
/// The inherited class should implement the pure abstract method
/// TRestPatternMask::GetRegion integrating the logic to identify regions
/// and the pattern mask. The inherited GetRegion method implementation
/// should call TRestPatternMask::ApplyCommonMaskTransformation to define mask
/// offset, rotation and radial limit.
///
/// Some examples are : TRestGridMask, TRestStrippedMask, TRestSpiderMask
/// and TRestRingsMask. More details on each class documentation.
///
/// \htmlonly <style>div.image img[src="masks.png"]{width:1000px;}</style> \endhtmlonly
/// ![An illustration of the different masks implemented](ringsmask.png)
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-May: First implementation of TRestPatternMask
/// Javier Galan
///
/// \class TRestPatternMask
/// \author: Javier Galan - javier.galan@unizar.es
///
/// <hr>
///

#include "TAxis.h"
#include "TColor.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TRandom3.h"

#include "TRestPatternMask.h"

ClassImp(TRestPatternMask);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestPatternMask::TRestPatternMask() : TRestMetadata() { Initialize(); }

/////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// corresponding TRestAxionMagneticField section inside the RML.
///
TRestPatternMask::TRestPatternMask(const char* cfgFileName, std::string name) : TRestMetadata(cfgFileName) {
    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestPatternMask::~TRestPatternMask() {}

///////////////////////////////////////////////
/// \brief It produces an effective mask rotation and translation for the
/// point x,y.
///
/// Remark: The window is centered, it is just the pattern that we want
/// to shift.
///
Int_t TRestPatternMask::ApplyCommonMaskTransformation(Double_t& x, Double_t& y) {
    if (fMaskRadius > 0 && x * x + y * y > fMaskRadius * fMaskRadius) return 0;

    TVector2 pos(x, y);

    pos = pos.Rotate(-fRotationAngle);
    pos -= fOffset;

    x = pos.X();
    y = pos.Y();

    return 1;
}

///////////////////////////////////////////////
/// \brief Returns true if the pattern was hitted.
///
bool TRestPatternMask::HitsPattern(Double_t x, Double_t y) {
    if (GetRegion(x, y) > 0) return true;
    return false;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestPatternMask::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << " - Type : " << fPatternType << RESTendl;
    RESTMetadata << " - Mask radius : " << fMaskRadius << " mm" << RESTendl;
    RESTMetadata << " - Offset : (" << fOffset.X() << ", " << fOffset.Y() << ") mm" << RESTendl;
    RESTMetadata << " - Rotation angle : " << fRotationAngle * 180. / TMath::Pi() << " degrees" << RESTendl;
    RESTMetadata << "----" << RESTendl;
}

/////////////////////////////////////////////
/// \brief It generates a Monte Carlo with positions and paints them the returned canvas.
/// Each unique region is associated with different colors. If there are too many colors,
/// they might be repited for different region ids.
///
TCanvas* TRestPatternMask::DrawMonteCarlo(Int_t nSamples) {
    if (fCanvas != NULL) {
        delete fCanvas;
        fCanvas = NULL;
    }
    fCanvas = new TCanvas("canv", "This is the canvas title", 1400, 1200);
    fCanvas->Draw();

    TPad* pad1 = new TPad("pad1", "This is pad1", 0.01, 0.02, 0.99, 0.97);
    pad1->Draw();

    fCanvas->SetRightMargin(0.09);
    fCanvas->SetLeftMargin(0.15);
    fCanvas->SetBottomMargin(0.15);

    /// Generating Montecarlo
    std::map<int, std::vector<TVector2> > points;

    TRandom3* rnd = new TRandom3(0);

    for (int n = 0; n < nSamples; n++) {
        Double_t x = 2.5 * (rnd->Rndm() - 0.5) * fMaskRadius;
        Double_t y = 2.5 * (rnd->Rndm() - 0.5) * fMaskRadius;

        Int_t id = GetRegion(x, y);

        if (points.count(id) == 0) {
            std::vector<TVector2> a;
            a.push_back(TVector2(x, y));
            points[id] = a;
        } else {
            points[id].push_back(TVector2(x, y));
        }
    }

    std::vector<TGraph*> gridGraphs;
    Int_t nGraphs = 0;
    for (const auto& x : points) {
        std::string grname = "gr" + IntegerToString(nGraphs);
        TGraph* gr = new TGraph();
        gr->SetName(grname.c_str());

        for (int n = 0; n < x.second.size(); n++) {
            gr->SetPoint(gr->GetN(), x.second[n].X(), x.second[n].Y());
        }

        if (nGraphs == 0) {
            gr->SetLineColor(kBlack);
            gr->SetMarkerColor(kBlack);
            gr->SetMarkerSize(0.6);
            gr->SetLineWidth(2);
        } else {
            gr->SetMarkerColor((nGraphs * 3) % 29 + 20);
            gr->SetLineColor((nGraphs * 3) % 29 + 20);
            gr->SetMarkerSize(0.6);
            gr->SetLineWidth(2);
        }

        gr->SetMarkerStyle(20);
        gridGraphs.push_back(gr);
        nGraphs++;
    }

    gridGraphs[0]->GetXaxis()->SetLimits(-1.25 * fMaskRadius, 1.25 * fMaskRadius);
    gridGraphs[0]->GetHistogram()->SetMaximum(1.25 * fMaskRadius);
    gridGraphs[0]->GetHistogram()->SetMinimum(-1.25 * fMaskRadius);

    gridGraphs[0]->GetXaxis()->SetTitle("X [mm]");
    gridGraphs[0]->GetXaxis()->SetTitleSize(0.04);
    gridGraphs[0]->GetXaxis()->SetLabelSize(0.04);
    gridGraphs[0]->GetYaxis()->SetTitle("Y[mm]");
    gridGraphs[0]->GetYaxis()->SetTitleOffset(1.5);
    gridGraphs[0]->GetYaxis()->SetTitleSize(0.04);
    gridGraphs[0]->GetYaxis()->SetLabelSize(0.04);
    pad1->SetLogy();
    gridGraphs[0]->Draw("AP");
    for (int n = 1; n < nGraphs; n++) gridGraphs[n]->Draw("P");

    return fCanvas;
}
