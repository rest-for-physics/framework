/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
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
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestMetadataPlot
#define RestCore_TRestMetadataPlot

#include <TLatex.h>
#include "TCanvas.h"

#include <TRestRun.h>

/// A helper class to draw the evolution or correlation of metadata information from a set of REST files
class TRestMetadataPlot : public TRestMetadata {
   public:
    /// This structure is used to register the values from a <graph definition inside the RML
    struct Graph_Info_Set {
        /// The name to be used for the corresponding ROOT TGraph object
        string name;
        /// The title to be used inside TLegend object
        string title;
        /// The metadata class and data member definition to be plotted
        string yVariable;
        /// An optional rule to be used for run selection
        string metadataRule;
        /// The option to be passed to the Draw method
        string drawOption;
        /// The line color to be assigned to the corresponding TGraph
        Int_t lineColor;
        /// The line width to be assigned to the corresponding TGraph
        Int_t lineWidth;
        /// The line style to be assigned to the corresponding TGraph
        Int_t lineStyle;
        /// The fill color to be assigned to the corresponding TGraph
        Int_t fillColor;
        /// The fill style to be assigned to the corresponding TGraph
        Int_t fillStyle;
        /// The marker style to be assigned to the corresponding TGraph
        Int_t markerStyle;
        /// The marker size to be assigned to the corresponding TGraph
        Int_t markerSize;
        /// The marker color to be assigned to the corresponding TGraph
        Int_t markerColor;
    };

    /// This structure is used to register the values from a <plot definition inside the RML
    struct Plot_Info_Set {
        /// The name that will be used for the TGraph object.
        string name;
        /// A title that will be visible in top of the plot
        string title;
        /// The corresponding metadata variable to be used in the X-axis
        string xVariable;
        /// It true a logarithmic scale will be used on the x-axis.
        Bool_t logX;
        /// It true a logarithmic scale will be used on the y-axis.
        Bool_t logY;
        /// It true a legend will be shown on the corresponding plot.
        Bool_t legendOn;
        /// It true a time/date calendar format will be used on the x-axis
        Bool_t timeDisplay;
        /// The label or title to be given to the x-axis.
        string labelX;
        /// The label or title to be given to the y-axis.
        string labelY;
        /// The filename where the file will be saved
        string save;
        /// The user defined range in the x-axis
        TVector2 xRange;
        /// The user defined range in the y-axis
        TVector2 yRange;
        /// A vector containning the properties of the graphs inside the plot
        vector<Graph_Info_Set> graphs;
    };

    /// This structure is used to register the values from a <panel definition inside the RML
    struct Panel_Info {
        /// The font size to be used in the panel labels
        Float_t font_size;
        /// The x-position of labels used inside the panel
        vector<Float_t> posX;
        /// The y-position of labels used inside the panel
        vector<Float_t> posY;
        /// The string containing the text to be drawn in the panel
        vector<string> label;
    };

   private:
    void InitFromConfigFile() override;
    Graph_Info_Set SetupGraphFromConfigFile(TiXmlElement* ele, Plot_Info_Set info);

    /// The total number of files for plotting added to this class
    Int_t fNFiles;

    /// The size of the canvas window in pixels.
    TVector2 fCanvasSize;

    /// The number of canvas divisions on X and Y.
    TVector2 fCanvasDivisions;

    /// A string to define the output filename where to store the canvas.
    TString fCanvasSave;

    /// The font label size
    Int_t fLabelFont;

    /// A label offset to be applied in the x-label (not tested)
    Double_t fLabelOffsetX = 1.1;
    /// A label offset to be applied in the y-label (not tested)
    Double_t fLabelOffsetY = 1.3;

    /// A label title scale to be applied in the x-label (not tested)
    Double_t fLabelScaleX = 1.2;
    /// A label title scale to be applied in the y-label (not tested)
    Double_t fLabelScaleY = 1.3;

    /// A label ticks scale to be applied in the x-label (not tested)
    Double_t fTicksScaleX = 1.5;
    /// A label ticks scale to be applied in the y-label (not tested)
    Double_t fTicksScaleY = 1.5;

    /// The X1 legend position
    Double_t fLegendX1 = 0.7;
    /// The Y1 legend position
    Double_t fLegendY1 = 0.75;
    /// The X2 legend position
    Double_t fLegendX2 = 0.88;
    /// The Y2 legend position
    Double_t fLegendY2 = 0.88;

    /// The legend drawing option
    TString fLegendOption = "lp";

    /// A vector with the defined plots
    vector<Plot_Info_Set> fPlots;

    /// A vector with the defined panels
    vector<Panel_Info> fPanels;

    /// A vector to double check that there are no repeated graph names
    vector<string> fPlotNamesCheck;  //!

    /// TRestRun to handle output file
    TRestRun* fRun;  //!

    /// To keep a list of files used
    std::vector<string> fRunInputFileName;  //!

    /// Output canvas
    TCanvas* fCombinedCanvas;  //!

    void AddFile(TString fileName);
    void AddFileFromExternalRun();
    void AddFileFromEnv();

   protected:
   public:
    void Initialize() override;

    void PrintMetadata() override {}

    void SaveCanvasToPDF(TString fileName);
    void SavePlotToPDF(TString fileName, Int_t n = 0);
    void SaveGraphToPDF(TString fileName, Int_t nPlot = 0, Int_t nGraph = 0);

    /// It will set the filename of the plots output file to be generated when GenerateCanvas is called.
    void SetOutputPlotsFilename(TString fname) { fCanvasSave = fname; }

    Int_t GetPlotIndex(TString plotName);

    /// It will return the canvas size in pixel units
    TVector2 GetCanvasSize() { return fCanvasSize; }

    /// It will return the number of divisions inside the canvas
    TVector2 GetCanvasDivisions() { return fCanvasDivisions; }

    void GenerateCanvas();

    TRestMetadataPlot();
    TRestMetadataPlot(const char* cfgFileName, const char* name = "");

    virtual ~TRestMetadataPlot();

    ClassDefOverride(TRestMetadataPlot, 3);
};
#endif
