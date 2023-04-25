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

#ifndef REST_TRestDataSetPlot
#define REST_TRestDataSetPlot

#include "TRestMetadata.h"

#include "TRestDataSet.h"

#include "THStack.h"
#include "TLegend.h"
#include "TLatex.h"

/// Perform the plot over datasets
class TRestDataSetPlot : public TRestMetadata {

/// Nested classes for internal use only
public:

/// Auxiliary class for histograms
class HistoInfo {
public:
    std::string name="";
    std::string drawOption="";
    std::vector<std::string> variable;
    std::vector<TVector2> range;
    std::vector<Int_t> nBins;
    Int_t lineColor;
    Int_t lineWidth;
    Int_t lineStyle;
    Int_t fillColor;
    Int_t fillStyle;
    Bool_t statistics = false;
    TRestCut *histoCut = nullptr;
    TH1 *histo = nullptr;
};

/// Auxiliary class for plots/pads
class PlotInfo {
public:
    std::string name="";
    std::string title="";

    std::string save="";

    std::string stackDrawOption="nostack";

    Double_t normalize;
    std::string scale ="";
    Bool_t logX = false;
    Bool_t logY= false;
    Bool_t logZ = false;
    Bool_t gridX = false;
    Bool_t gridY = false;
    Bool_t legendOn = false;

    Double_t xOffset;
    Double_t yOffset;

    Double_t marginLeft;
    Double_t marginRight;
    Double_t marginTop;
    Double_t marginBottom;

    Bool_t timeDisplay;

    std::string labelX;
    std::string labelY;

    std::vector<HistoInfo> histos;

    THStack *hs = nullptr;
    TLegend *legend = nullptr;

};

/// Auxiliary class for panels/labels
class PanelInfo {
public:
    Float_t font_size;
    Int_t precision;

    std::vector<std::pair <std::array <std::string, 3>, TVector2 >> variablePos;
    std::vector<std::pair <std::array <std::string, 3>, TVector2 >> metadataPos;
    std::vector<std::pair <std::array <std::string, 3>, TVector2 >> obsPos;

    TRestCut *panelCut = nullptr;

    std::vector<TLatex*> text;
};

/// Maps for internal use only
private:

/// ColorIdMap as enum "EColor" defined in Rtypes.h
const std::map<std::string, int> ColorIdMap{
    {"white", kWhite},   {"black", kBlack},   {"gray", kGray},       {"red", kRed},       {"green", kGreen},
    {"blue", kBlue},     {"yellow", kYellow}, {"magenta", kMagenta}, {"cyan", kCyan},     {"orange", kOrange},
    {"spring", kSpring}, {"teal", kTeal},     {"azure", kAzure},     {"violet", kViolet}, {"pink", kPink}};

/// LineStyleMap as enum "ELineStyle" defined in TAttLine.h
const std::map<std::string, int> LineStyleMap{
    {"solid", kSolid}, {"dashed", kDashed}, {"dotted", kDotted}, {"dashDotted", kDashDotted}};

/// FillStyleMap as enum "EFillStyle" defined in TAttFill.h
const std::map<std::string, int> FillStyleMap{
    {"dotted", kFDotted1},        {"dashed", kFDashed1},    {"dotted1", kFDotted1},
    {"dotted2", kFDotted2},       {"dotted3", kFDotted3},   {"hatched1", kFHatched1},
    {"hatched2", kHatched2},      {"hatched3", kFHatched3}, {"hatched4", kFHatched4},
    {"wicker", kFWicker},         {"scales", kFScales},     {"bricks", kFBricks},
    {"snowflakes", kFSnowflakes}, {"circles", kFCircles},   {"tiles", kFTiles},
    {"mondrian", kFMondrian},     {"diamonds", kFDiamonds}, {"waves1", kFWaves1},
    {"dashed1", kFDashed1},       {"dashed2", kFDashed2},   {"alhambra", kFAlhambra},
    {"waves2", kFWaves2},         {"stars1", kFStars1},     {"stars2", kFStars2},
    {"pyramids", kFPyramids},     {"frieze", kFFrieze},     {"metopes", kFMetopes},
    {"empty", kFEmpty},           {"solid", kFSolid}};

private:

    /// Name of the dataset to be imported
    std::string fDataSetName ="";

    /// OutputFileName
    std::string fOutputFileName ="";

    /// Global cut for the entire dataSet
    TRestCut *fCut = nullptr;

    /// Palette style
    Int_t fPaletteStyle = 57;

    /// Preview plot
    Bool_t fPreviewPlot = true;

    /// Legend position and size
    Double_t fLegendX1 = 0.7;
    Double_t fLegendY1 = 0.75;
    Double_t fLegendX2 = 0.88;
    Double_t fLegendY2 = 0.88;

    /// Canvas options, size, divisions and margins
    TVector2 fCanvasSize = TVector2(800,600);
    TVector2 fCanvasDivisions = TVector2(2,2);
    TVector2 fCanvasDivisionMargins = TVector2(0.01, 0.01);

    /// Vector with plots/pads options
    std::vector<PlotInfo> fPlots;

    /// Vector with panels/label options
    std::vector<PanelInfo> fPanels;

    void InitFromConfigFile() override;

public:
    void Initialize() override;
    void PrintMetadata() override;

    void GenerateDataSetFromFilePattern(TRestDataSet &dataSet);

    void PlotCombinedCanvas();

    void ReadPlotInfo();
    void ReadPanelInfo();

    void CleanUp();

    TRestCut * ReadCut(TRestCut *cut, TiXmlElement* ele = nullptr);
    Int_t GetIDFromMapString(const std::map<std::string, int> &mapStr, const std::string& in);

    TRestDataSetPlot();
    TRestDataSetPlot(const char* configFilename, std::string name = "");
    ~TRestDataSetPlot();

    ClassDefOverride(TRestDataSetPlot, 1);

};
#endif
