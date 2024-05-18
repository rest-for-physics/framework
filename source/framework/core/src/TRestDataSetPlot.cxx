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
/// This class is meant to perform the plots over a TRestDataSet and it is
/// meant to replace TRestAnalysisPlot. A dataset is should be provided
/// as input file in order to perform de plots, in addition the possibility
/// to provide a file pattern is also supported for backward compatibility
/// although this method will be deprecated. The rml file has been modified
/// with respect to TRestAnalysisPlot, major changes are that now a TRestCut
/// must be defined in advance in order to perform the cuts over the dataset,
/// in addition the panel/label metadata has been changed in order to distinguish
/// between metadata, variables or observable values.
///
/// The rml file can be splitted in three main blocks that are detailed below;
/// * Basic parameters: global variables to perform the plot such as canvas size,
/// pad divisions, preview or outputFile info.
/// * Panel parameters: A panel defines different labels that will be written in
/// a pad, it accepts different parameters such as metadata info, variable values,
/// and observable values, that can be written in the panel. A cut can be provided
/// to a panel in order to print the info after appliying a particular cut.
/// * Plot parameters: It contains the info to produce a plot at the pad level.
/// A plot consists of a set of histograms that can be plot together using
/// THStack, note that only 1D and 2D histograms are supported. The histograms
/// are defined inside the plot metadata section and one or two variables must
/// be provided (1D or 2D histos) besides the range, while a cut can be set at the
/// histogram level.
///
/// ### Parameters
/// * Basic parameters of TRestDataSetPlot:
/// * **name**: Name of the TRestDataSetPlot class and canvas
/// * **title**: Title of the TRestDataSetPlot class and canvas
/// * **dataSetName**: Name of the dataSet to be used to perform the plots. Note
/// that if this variable is empty, it will use the name of the inputfile provided
/// at restManager.
/// * **outputFileName**: Name of the output file to be generated. Note that if this
/// variable is empty, it will use the name of the outputFile provided at restManager,
/// if still empty no outputFile will we generated. Note that it supports several formats
/// such as root, pdf, eps, jpg,... If the output file is a root file, the different
/// histograms and metadata will be saved besides the canvas.
/// * **previewPlot**: If true the plot (canvas) is displayed and can be modified before
/// saving it.
/// * **paletteStyle**: Style of the palette to be used in all the plots.
/// * **legendX1**: Start position of the legend in the X axis
/// * **legendY1**: Start position of the legend in the X axis
/// * **legendX2**: End position of the legend in the X axis
/// * **legendY2**: End position of the legend in the X axis
/// * **canvasSize**: TVector2 with the size of the canvas in points
/// * **canvasDivisions**: TVector2 with the divisions of the canvas e.g. (3, 2) --> 3 x 2
/// The key `addCut` can be povided in order to perform the cut to the entire dataSet. Note
/// that the TRestCut must be defined inside the rml and several cuts can be added.
///
/// Example of TRestDataSetPlot basic parameters with a set TRestCuts:
/// \code
///    <TRestDataSetPlot name="restplot" title="Basic Plots" previewPlot="true" canvasSize="(1200,1200)"
///    canvasDivisions="(2,2)" verboseLevel="info">
///      <addCut name="North"/>
///      <addCut name="TrackBalance"/>
///      <TRestCut name="North" verboseLevel="info">
///          <cut name="c1" variable="alphaTrackAna_originZ" condition="<0"/>
///      </TRestCut>
///      <TRestCut name="TrackBalance" verboseLevel="info">
///          <cut name="c2" variable="alphaTrackAna_trackBalanceX" condition=">0.9"/>
///          <cut name="c3" variable="alphaTrackAna_trackBalanceY" condition=">0.9"/>
///      </TRestCut>
///    </TRestDataSetPlot>
/// \endcode
///
/// * Panel parameters:
/// Basic panel parameters:
/// * **font_size**: Font size of the entire panel.
/// * **precision**: Precision for the values to be written.
/// * **value**: If true/ON panel is displayed, otherwise is ignored
/// Different keys are provided: `metadata` is meant for the metadata info inside the
/// TRestDataSet (as a RelevantQuantity), `variable` for a predefined variable e.g. rate,
/// `observable` for an observable value and `expression` for a mathematical expression
/// that can contain any of the previous.
/// All the keys have the same structure which is detailed below:
/// * **value**: Name of the metadata, variable or observable value.
/// * **label**: String of the label that will be written before the observable value.
/// * **units**: String with the units to be appended to the label.
/// * **x**: X position of the label inside the pad
/// * **y**: Y position of the label inside the pad
/// The key `addCut` can be povided in order to perform the cut to the panel. Note
/// that the TRestCut must be defined inside the rml.
///
/// Example of panel rule:
/// \code
///    <panel font_size="0.05" precision="3">
///          <metadata value="[TRestRun::fRunTag]" label="Run tag" x="0.25" y="0.82" />
///          <variable value="[[entries]]" label="Entries" x="0.25" y="0.58" />
///          <observable value="alphaTrackAna_angle" label="Mean Angle" units="rad" x="0.25" y="0.01" />
///          <addCut name="Fiducial"/>
///    </panel>
/// \endcode
///
/// * Plot parameters:
/// * **name**: Name of the plot
/// * **title**: Title of the plot
/// * **xlabel**: Title of the label in the X axis
/// * **ylabel**: Title of the label in the Y axis
/// * **logX;logY;logZ**: If true plot log scale in the corresponding axis (X, Y or Z)
/// * **gridX;gridY**: If true plot grid in the corresponding axis (X or Y)
/// * **marginLeft;marginRight;marginTop;marginBottom**: Margins size
/// * **legend**: If true/ON it plots the legend
/// * **stackOption*: THStack draw option to be used, note that by default is set to `NOSTACK`
/// * **timeDisplay**: If true/ON time display is set in the X axis
/// * **norm**: Normalization constant in which the plot will be normalized, e.g. use `1` in
/// case you want to normalize by 1.
/// * **scale**: Divide all the histogram bins by a constant given by scale. You may use any
/// mathematical expression in combination with the special keywords: `binSize`, `entries`,
/// `runLength` (in hours) and `integral`. Adding a scale will make the histogram to be
/// drawn with errors. To avoid this, set parameter option to 'hist' in the histogram.
/// * **value**: If true/ON plot is displayed, otherwise is ignored
/// * **save**: String with the name of the output file in which the plot will be saved
/// in a separated file, several formats are supported (root, pdf, eps, jpg,...)
/// * Histogram parameters:
/// Please, use the `histo` key inside `plot` to add the plot parameters. You can add as many
/// histograms as you want but note that only 1D and 2D histograms are supported.
/// * **name**: Name of the histogram
/// * **option*: Draw option to be used, note that by default is set to `COLZ`
/// * **lineColor*: Color of the line used in the histogram.
/// * **lineWidth*: Width of the line used in the histogram.
/// * **lineStyle*: Line style to be used in the histogram.
/// * **fillStyle*: Fill style to used in the histogram.
/// * **fillColor*: Fill color to used in the histogram.
/// * **stats**: If true/ON it plots histogram statistics
/// In order to add one or two variables to the histogram please use the `variable` key.
/// In order to perform a 2D plot you should add two variable` keys, where the first one
/// corresponds to the X axis. The `variable` key has the following parameters:
/// * **name*: Name of the observable to be plotted, the observable must be defined inside
/// the dataSet.
/// * **range*: TVector2 with the range of the variable to be plotted
/// * **nBins*: Number of bins that will be used in the histogram.
/// The key `addCut` can be povided in order to perform the cut to the histogram. Note
/// that the TRestCut must be defined inside the rml.
///
/// Example of plot rule:
/// \code
/// 	<plot name="TrackOrigin" title="TrackOrigin" xlabel="X [mm]" ylabel="Y [mm]" save="HitMapUpwards.pdf"
/// value="ON" >
///        <histo name="HitMapUpwards" >
///            <variable name="alphaTrackAna_originX" range="(0,250)" nbins="256" />
///            <variable name="alphaTrackAna_originY" range="(0,250)" nbins="256" />
///            <addCut name="Upwards"/>
///        </histo>
///     </plot>
/// \endcode
///
/// ### Examples
/// Following example creates one panel and three plots including one or two histograms:
/// \code
///  <TRestManager name="alphaTrack" title="alphaTrack" verboseLevel="info">
///    <TRestDataSetPlot name="alphaTrackPlot" title="Basic Plots for alpha tracks" previewPlot="true"
///    canvasSize="(1200,1200)" canvasDivisions="(2,2)" verboseLevel="info"
///    outputFileName="Plots_[TRestRun::fRunTag]_plots.root">
///      <addCut name="North"/>
///      <addCut name="TrackBalance"/>
///      <TRestCut name="North" verboseLevel="info">
///          <cut name="c1" variable="alphaTrackAna_originZ" condition="<0"/>
///      </TRestCut>
///      <TRestCut name="Fiducial" verboseLevel="info">
///          <cut name="c2" variable="alphaTrackAna_originX" condition=">20"/>
///          <cut name="c3" variable="alphaTrackAna_originX" condition="<225"/>
///          <cut name="c4" variable="alphaTrackAna_originY" condition=">20"/>
///          <cut name="c5" variable="alphaTrackAna_originY" condition="<225"/>
///      </TRestCut>
///      <TRestCut name="Upwards" verboseLevel="info">
///          <cut name="c6" variable="alphaTrackAna_downwards" condition="==false"/>
///      </TRestCut>
///      <TRestCut name="Downwards" verboseLevel="info">
///          <cut name="c7" variable="alphaTrackAna_downwards" condition="==true"/>
///      </TRestCut>
///      <TRestCut name="TrackBalance" verboseLevel="info">
///          <cut name="c8" variable="alphaTrackAna_trackBalanceX" condition=">0.9"/>
///          <cut name="c9" variable="alphaTrackAna_trackBalanceY" condition=">0.9"/>
///      </TRestCut>
///      <panel font_size="0.05" precision="3">
///          <variable label="Summary" x="0.25" y="0.9"/>
///          <metadata value="[TRestRun::fRunTag]" label="Run tag" x="0.25" y="0.82"/>
///          <variable value="[[startTime]]" label="Data starts" x="0.25" y="0.74"/>
///          <variable value="[[endTime]]" label="Data ends" x="0.25" y="0.66"/>
///          <variable value="[[entries]]" label="Entries" x="0.25" y="0.58"/>
///          <variable value="[[runLength]]" label="Run duration" units="hours" x="0.25" y="0.50"/>
///          <observable value="alphaTrackAna_angle" label="Mean Angle" units="rad" x="0.25" y="0.42"/>
///          <addCut name="Fiducial"/>
///      </panel>
///      <plot name="TrackOrigin" title="TrackOrigin" xlabel="X [mm]" ylabel="Y [mm]" save="" value="ON"
///      stats="ON">
///          <variable name="alphaTrackAna_originX" range="(0,250)" nbins="256"/>
///          <variable name="alphaTrackAna_originY" range="(0,250)" nbins="256"/>
///      </plot>
///      <plot name="Spectra" title="Spectra" xlabel="Amplitude(ADC)" ylabel="Counts" legend="ON" value="ON"
///      save="Spectra.pdf">
///          <histo name="SpectraDownwards">
///              <variable name="alphaTrackAna_totalEnergy" range="(0,1000000)" nbins="200"/>
///              <addCut name="Fiducial"/>
///              <addCut name="Downwards"/>
///          </histo>
///          <histo name="SpectraUpwards">
///              <variable name="alphaTrackAna_totalEnergy" range="(0,1000000)" nbins="200"/>
///              <parameter name="lineColor" value="1"/>
///              <addCut name="Fiducial"/>
///              <addCut name="Upwards"/>
///          </histo>
///      </plot>
///      <plot name="Rate" title="Rate" xlabel="Date/time" ylabel="Rate (Hz)" legend="ON" value="ON"
///      scale="binSize" timeDisplay="ON">
///          <histo name="RateDownwards">
///              <variable name="timeStamp" range="" nbins="100"/>
///              <addCut name="Downwards"/>
///          </histo>
///          <histo name="RateUpwards">
///              <variable name="timeStamp" range="" nbins="100"/>
///              <parameter name="lineColor" value="1"/>
///              <addCut name="Upwards"/>
///          </histo>
///      </plot>
///    </TRestDataSetPlot>
///    <addTask command="alphaTrackPlot->PlotCombinedCanvas()" value="ON"/>
///  </TRestManager>
/// \endcode
///
/// ### Running example
/// \code
///   restManager --c thisExample.rml --f FileWithDataSet.root
/// \endcode
///
/// Figure to be added (a png image should be uploaded to the ./images/ directory)
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2023-04: First implementation of TRestDataSetPlot, based on TRestAnalysisPlot
/// JuanAn Garcia
///
/// 2024-05: Extend some functionalities, Álvaro Ezquerro
///
/// \class TRestDataSetPlot
/// \author: JuanAn Garcia   e-mail: juanangp@unizar.es
///
/// <hr>
///

#include "TRestDataSetPlot.h"

#include "TCanvas.h"
#include "TDirectory.h"
#include "TStyle.h"

ClassImp(TRestDataSetPlot);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDataSetPlot::TRestDataSetPlot() { Initialize(); }

/////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
TRestDataSetPlot::TRestDataSetPlot(const char* configFilename, std::string name)
    : TRestMetadata(configFilename) {
    Initialize();
    LoadConfigFromFile(fConfigFileName, name);
    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDataSetPlot::~TRestDataSetPlot() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define
/// the section name
///
void TRestDataSetPlot::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief Initialization of specific TRestDataSetPlot members through an RML file
///
void TRestDataSetPlot::InitFromConfigFile() {
    TRestMetadata::InitFromConfigFile();

    if (fDataSetName == "") fDataSetName = GetParameter("inputFileName", "");
    if (fOutputFileName == "") fOutputFileName = GetParameter("outputFileName", "");

    fCut = ReadCut(fCut);

    ReadPlotInfo();
    ReadPanelInfo();
}

///////////////////////////////////////////////
/// \brief this function is used to add the different cuts
/// provided in different metadata sections, it uses as parameter
/// the cut where the cut is meant to be added and returns the
/// added cut. If *cut is a nullptr it just instantiate it
///
TRestCut* TRestDataSetPlot::ReadCut(TRestCut* cut, TiXmlElement* ele) {
    TiXmlElement* cutele = GetElement("addCut", ele);
    while (cutele != nullptr) {
        std::string cutName = GetParameter("name", cutele, "");
        if (!cutName.empty()) {
            if (cut == nullptr) {
                cut = (TRestCut*)InstantiateChildMetadata("TRestCut", cutName);
            } else {
                cut->AddCut((TRestCut*)InstantiateChildMetadata("TRestCut", cutName));
            }
        }
        cutele = GetNextElement(cutele);
    }

    return cut;
}

///////////////////////////////////////////////
/// \brief This function reads the config file panel info and
/// stores it in a vector of PanelInfo
///
void TRestDataSetPlot::ReadPanelInfo() {
    if (!fPanels.empty()) {
        RESTWarning << "Plot metadata already initialized" << RESTendl;
    }

    TiXmlElement* panelele = GetElement("panel");
    while (panelele != nullptr) {
        std::string active = GetParameter("value", panelele, "ON");
        if (ToUpper(active) != "ON") continue;

        PanelInfo panel;
        panel.font_size = StringToDouble(GetParameter("font_size", panelele, "0.1"));
        panel.precision = StringToInteger(GetParameter("precision", panelele, "2"));

        panel.panelCut = ReadCut(panel.panelCut, panelele);

        TiXmlElement* labelele = GetElement("variable", panelele);
        while (labelele != nullptr) {
            std::array<std::string, 3> label;
            label[0] = GetParameter("value", labelele, "");
            label[1] = GetParameter("label", labelele, "");
            label[2] = GetParameter("units", labelele, "");
            double posX = StringToDouble(GetParameter("x", labelele, "0.1"));
            double posY = StringToDouble(GetParameter("y", labelele, "0.1"));

            panel.variablePos.push_back(std::make_pair(label, TVector2(posX, posY)));

            labelele = GetNextElement(labelele);
        }
        TiXmlElement* metadata = GetElement("metadata", panelele);
        while (metadata != nullptr) {
            std::array<std::string, 3> label;
            label[0] = GetParameter("value", metadata, "");
            label[1] = GetParameter("label", metadata, "");
            label[2] = GetParameter("units", metadata, "");
            double posX = StringToDouble(GetParameter("x", metadata, "0.1"));
            double posY = StringToDouble(GetParameter("y", metadata, "0.1"));

            panel.metadataPos.push_back(std::make_pair(label, TVector2(posX, posY)));

            metadata = GetNextElement(metadata);
        }
        TiXmlElement* observable = GetElement("observable", panelele);
        while (observable != nullptr) {
            std::array<std::string, 3> label;
            label[0] = GetParameter("value", observable, "");
            label[1] = GetParameter("label", observable, "");
            label[2] = GetParameter("units", observable, "");
            double posX = StringToDouble(GetParameter("x", observable, "0.1"));
            double posY = StringToDouble(GetParameter("y", observable, "0.1"));

            panel.obsPos.push_back(std::make_pair(label, TVector2(posX, posY)));

            observable = GetNextElement(observable);
        }
        TiXmlElement* expression = GetElement("expression", panelele);
        while (expression != nullptr) {
            std::array<std::string, 3> label;
            label[0] = GetParameter("value", expression, "");
            label[1] = GetParameter("label", expression, "");
            label[2] = GetParameter("units", expression, "");
            double posX = StringToDouble(GetParameter("x", expression, "0.1"));
            double posY = StringToDouble(GetParameter("y", expression, "0.1"));

            panel.expPos.push_back(std::make_pair(label, TVector2(posX, posY)));

            expression = GetNextElement(expression);
        }

        fPanels.push_back(panel);
        panelele = GetNextElement(panelele);
    }
}

///////////////////////////////////////////////
/// \brief This function reads the config file plot info and
/// stores it in a vector of PlotInfo
///
void TRestDataSetPlot::ReadPlotInfo() {
    if (!fPlots.empty()) {
        RESTWarning << "Plot metadata already initialized" << RESTendl;
        return;
    }

    TiXmlElement* plotele = GetElement("plot");
    while (plotele != nullptr) {
        std::string active = GetParameter("value", plotele, "ON");
        if (ToUpper(active) != "ON") continue;
        int N = fPlots.size();
        PlotInfo plot;
        plot.name = RemoveWhiteSpaces(GetParameter("name", plotele, "plot_" + ToString(N)));
        plot.title = GetParameter("title", plotele, plot.name);
        plot.logX = StringToBool(GetParameter("logX", plotele, "false"));
        plot.logY = StringToBool(GetParameter("logY", plotele, "false"));
        plot.logZ = StringToBool(GetParameter("logZ", plotele, "false"));
        plot.gridY = StringToBool(GetParameter("gridY", plotele, "false"));
        plot.gridX = StringToBool(GetParameter("gridX", plotele, "false"));
        plot.normalize = StringToDouble(GetParameter("norm", plotele, ""));
        plot.scale = GetParameter("scale", plotele, "");
        plot.labelX = GetParameter("xlabel", plotele, "");
        plot.labelY = GetParameter("ylabel", plotele, "");
        plot.marginBottom = StringToDouble(GetParameter("marginBottom", plotele, "0.15"));
        plot.marginTop = StringToDouble(GetParameter("marginTop", plotele, "0.07"));
        plot.marginLeft = StringToDouble(GetParameter("marginLeft", plotele, "0.25"));
        plot.marginRight = StringToDouble(GetParameter("marginRight", plotele, "0.1"));
        plot.legendOn = StringToBool(GetParameter("legend", plotele, "OFF"));
        plot.stackDrawOption = GetParameter("stackOption", plotele, "nostack");
        // plot.annotationOn = StringToBool(GetParameter("annotation", plotele, "OFF"));
        plot.xOffset = StringToDouble(GetParameter("xOffset", plotele, "0"));
        plot.yOffset = StringToDouble(GetParameter("yOffset", plotele, "0"));
        plot.timeDisplay = StringToBool(GetParameter("timeDisplay", plotele, "OFF"));
        plot.save = RemoveWhiteSpaces(GetParameter("save", plotele, ""));

        TiXmlElement* histele = GetElement("histo", plotele);
        if (histele == nullptr) {
            histele = plotele;
        }
        while (histele != nullptr) {
            HistoInfo hist;
            hist.name = RemoveWhiteSpaces(GetParameter("name", histele, plot.name));
            hist.drawOption = GetParameter("option", histele, "colz");
            TiXmlElement* varele = GetElement("variable", histele);
            while (varele != nullptr) {
                hist.variable.push_back(GetParameter("name", varele));
                std::string rangeStr = GetParameter("range", varele);
                hist.range.push_back(StringTo2DVector(rangeStr));
                hist.nBins.push_back(StringToInteger(GetParameter("nbins", varele)));
                varele = GetNextElement(varele);
            }

            hist.lineColor = GetIDFromMapString(ColorIdMap, GetParameter("lineColor", histele, "602"));
            hist.lineWidth = StringToInteger(GetParameter("lineWidth", histele, "1"));
            hist.lineStyle = GetIDFromMapString(LineStyleMap, GetParameter("lineStyle", histele, "1"));
            hist.fillStyle = GetIDFromMapString(FillStyleMap, GetParameter("fillStyle", histele, "1001"));
            hist.fillColor = GetIDFromMapString(ColorIdMap, GetParameter("fillColor", histele, "0"));
            hist.statistics = StringToBool(GetParameter("stats", histele, "OFF"));
            // hist.weight = GetParameter("weight", histele, "");
            hist.histoCut = ReadCut(hist.histoCut, histele);
            plot.histos.push_back(hist);

            if (histele == plotele) {
                break;
            }
            histele = GetNextElement(histele);
        }

        fPlots.push_back(plot);
        plotele = GetNextElement(plotele);
    }
}

///////////////////////////////////////////////
/// \brief This functions generates a dataSet based on the information
/// of the rml file. A TRestDataSet is pased as reference and will be
/// overwritten.
///
void TRestDataSetPlot::GenerateDataSetFromFilePattern(TRestDataSet& dataSet) {
    std::vector<std::string> obsList;

    // Add obserbables from global cut
    if (fCut != nullptr) {
        const auto paramCut = fCut->GetParamCut();
        for (const auto& [param, condition] : paramCut) {
            obsList.push_back(param);
        }
    }

    // Add obserbables from plot info, both variables and cuts
    for (const auto& plots : fPlots) {
        for (const auto& hist : plots.histos) {
            for (const auto& var : hist.variable) {
                obsList.push_back(var);
            }
            if (hist.histoCut == nullptr) continue;
            const auto paramCut = hist.histoCut->GetParamCut();
            for (const auto& [param, condition] : paramCut) {
                obsList.push_back(param);
            }
        }
    }

    std::map<std::string, TRestDataSet::RelevantQuantity> quantity;

    for (auto& panel : fPanels) {
        // Add obserbables from panel info, both variables and cuts
        for (auto& [key, posLabel] : panel.obsPos) {
            auto&& [obs, label, units] = key;
            obsList.push_back(obs);
        }
        // Add relevant quantity to metadata from the panel info
        for (auto& [key, posLabel] : panel.metadataPos) {
            auto&& [metadata, label, units] = key;
            TRestDataSet::RelevantQuantity quant;
            quant.metadata = metadata;
            quant.strategy = "unique";
            quantity[label] = quant;
        }
    }

    // Remove duplicated observables if any
    std::sort(obsList.begin(), obsList.end());
    obsList.erase(std::unique(obsList.begin(), obsList.end()), obsList.end());
    dataSet.SetFilePattern(fDataSetName);
    dataSet.SetObservablesList(obsList);
    dataSet.SetQuantity(quantity);
    dataSet.GenerateDataSet();
}

///////////////////////////////////////////////
/// \brief This functions performs the plot of the combined
/// canvas with the different panels and plots
///
void TRestDataSetPlot::PlotCombinedCanvas() {
    TRestDataSet dataSet;
    dataSet.EnableMultiThreading(true);

    // Import dataSet
    dataSet.Import(fDataSetName);

    // If dataSet is not valid, try to generate it, but note that this is deprecated
    if (dataSet.GetTree() == nullptr) {
        RESTWarning << "Cannot import dataSet, trying to generate it with pattern " << fDataSetName
                    << RESTendl;
        RESTWarning << "Note that the generation of a dataSet inside TRestDataSetPlot is deplecated. Check "
                       "TRestDataSet documentation to generate a dataSet"
                    << RESTendl;
        GenerateDataSetFromFilePattern(dataSet);
        if (dataSet.GetTree() == nullptr) {
            RESTError << "Cannot generate dataSet " << RESTendl;
            exit(1);
        }
    }

    // Perform the global cut over the dataSet
    dataSet.SetDataFrame(dataSet.MakeCut(fCut));

    TCanvas combinedCanvas(this->GetName(), this->GetName(), 0, 0, fCanvasSize.X(), fCanvasSize.Y());
    combinedCanvas.Divide((Int_t)fCanvasDivisions.X(), (Int_t)fCanvasDivisions.Y(),
                          fCanvasDivisionMargins.X(), fCanvasDivisionMargins.Y());

    gStyle->SetPalette(fPaletteStyle);

    const double duration = dataSet.GetTotalTimeInSeconds();
    const double startTime = dataSet.GetStartTime();
    const double endTime = dataSet.GetEndTime();

    // Fill general variables for the panel
    std::map<std::string, std::string> paramMap;
    paramMap["[[startTime]]"] = ToDateTimeString(startTime);
    paramMap["[[endTime]]"] = ToDateTimeString(endTime);

    // DataSet quantity is used to replace metadata parameters
    const auto quantity = dataSet.GetQuantity();

    int canvasIndex = 1;

    for (auto& panel : fPanels) {
        combinedCanvas.cd(canvasIndex);
        // Gets a dataFrame with the panel cut
        auto dataFrame = dataSet.MakeCut(panel.panelCut);
        const int entries = *dataFrame.Count();
        const double meanRate = entries / duration;
        const double runLength = duration / 3600.;
        paramMap["[[runLength]]"] = StringWithPrecision(runLength, panel.precision);
        paramMap["[[entries]]"] = StringWithPrecision(entries, panel.precision);
        paramMap["[[meanRate]]"] = StringWithPrecision(meanRate, panel.precision);

        paramMap["[[cutNames]]"] = "";
        paramMap["[[cuts]]"] = "";
        if (fCut) {
            for (const auto& cut : fCut->GetCuts()) {
                if (paramMap["[[cutNames]]"].empty())
                    paramMap["[[cutNames]]"] += cut.GetName();
                else
                    paramMap["[[cutNames]]"] += "," + (std::string)cut.GetName();
                if (paramMap["[[cuts]]"].empty())
                    paramMap["[[cuts]]"] += cut.GetTitle();
                else
                    paramMap["[[cuts]]"] += " && " + (std::string)cut.GetTitle();
            }
        }

        paramMap["[[panelCutNames]]"] = "";
        paramMap["[[panelCuts]]"] = "";
        if (panel.panelCut) {
            for (const auto& cut : panel.panelCut->GetCuts()) {
                if (paramMap["[[panelCutNames]]"].empty())
                    paramMap["[[panelCutNames]]"] += cut.GetName();
                else
                    paramMap["[[panelCutNames]]"] += "," + (std::string)cut.GetName();
                if (paramMap["[[panelCuts]]"].empty())
                    paramMap["[[panelCuts]]"] += cut.GetTitle();
                else
                    paramMap["[[panelCuts]]"] += " && " + (std::string)cut.GetTitle();
            }
        }

        RESTInfo << "Global cuts: " << paramMap["[[cuts]]"] << RESTendl;
        if (!paramMap["[[panelCuts]]"].empty())
            RESTInfo << "Additional panel cuts: " << paramMap["[[panelCuts]]"] << RESTendl;

        // Replace panel variables and generate a TLatex label
        for (const auto& [key, posLabel] : panel.variablePos) {
            auto&& [variable, label, units] = key;
            bool found = false;
            std::string var = variable;
            if (!var.empty()) {
                for (const auto& [param, val] : paramMap) {
                    if (var == param) {
                        size_t pos = 0;
                        var = Replace(var, param, val, pos);
                        found = true;
                        break;
                    }
                }
                if (!found) RESTWarning << "Variable " << variable << " not found" << RESTendl;
            }
            std::string lab = label + ": " + StringWithPrecision(var, panel.precision) + " " + units;
            panel.text.emplace_back(new TLatex(posLabel.X(), posLabel.Y(), lab.c_str()));
        }

        // Replace metadata variables and generate a TLatex label
        for (const auto& [key, posLabel] : panel.metadataPos) {
            auto&& [metadata, label, units] = key;
            std::string value = "";

            for (const auto& [name, quant] : quantity) {
                if (quant.metadata == metadata) value = quant.value;
            }

            if (value.empty()) {
                RESTWarning << "Metadata quantity " << metadata << " not found in dataSet" << RESTendl;
                continue;
            }

            std::string lab = label + ": " + StringWithPrecision(value, panel.precision) + " " + units;
            panel.text.emplace_back(new TLatex(posLabel.X(), posLabel.Y(), lab.c_str()));
        }

        // Replace observable variables and generate a TLatex label
        for (const auto& [key, posLabel] : panel.obsPos) {
            auto&& [obs, label, units] = key;
            auto value = *dataFrame.Mean(obs);

            std::string lab = label + ": " + StringWithPrecision(value, panel.precision) + " " + units;
            panel.text.emplace_back(new TLatex(posLabel.X(), posLabel.Y(), lab.c_str()));
        }

        // Replace any expression and generate TLatex label
        for (const auto& [key, posLabel] : panel.expPos) {
            auto&& [text, label, units] = key;
            std::string var = text;

            // replace variables
            for (const auto& [param, val] : paramMap) {
                var = Replace(var, param, val);
            }
            // replace metadata
            for (const auto& [name, quant] : quantity) {
                var = Replace(var, name, quant.value);
            }
            // replace observables
            for (const auto& obs : dataFrame.GetColumnNames()) {
                if (var.find(obs) == std::string::npos) continue;
                // here there should be a checking that the mean(obs) can be calculated
                // (checking obs data type?)
                double value = *dataFrame.Mean(obs);
                var = Replace(var, obs, DoubleToString(value));
            }
            var = Replace(var, "[", "(");
            var = Replace(var, "]", ")");
            var = EvaluateExpression(var);

            std::string lab = label + ": " + var + " " + units;
            panel.text.emplace_back(new TLatex(posLabel.X(), posLabel.Y(), lab.c_str()));
        }

        // Draw the labels inside the pad
        for (const auto& text : panel.text) {
            text->SetTextColor(1);
            text->SetTextSize(panel.font_size);
            text->Draw("same");
        }
        canvasIndex++;
    }

    for (auto& plots : fPlots) {
        // Histograms are added to a THStack and will be ploted later on
        combinedCanvas.cd(canvasIndex);
        plots.hs = new THStack(plots.name.c_str(), plots.title.c_str());
        if (plots.legendOn) plots.legend = new TLegend(fLegendX1, fLegendY1, fLegendX2, fLegendY2);
        /// Build the histograms and add them to the THStack
        for (auto& hist : plots.histos) {
            auto dataFrame = dataSet.MakeCut(hist.histoCut);
            if (hist.variable.front() == "timeStamp") {
                hist.range.front().SetX(startTime);
                hist.range.front().SetY(endTime);
            }
            // 1-D Histograms
            if (hist.variable.size() == 1) {
                auto histo = dataFrame.Histo1D({hist.name.c_str(), hist.name.c_str(), hist.nBins.front(),
                                                hist.range.front().X(), hist.range.front().Y()},
                                               hist.variable.front());
                hist.histo = static_cast<TH1*>(histo->DrawClone());
                // 2-D Histograms
            } else if (hist.variable.size() == 2) {
                auto histo = dataFrame.Histo2D(
                    {hist.name.c_str(), hist.name.c_str(), hist.nBins.front(), hist.range.front().X(),
                     hist.range.front().Y(), hist.nBins.back(), hist.range.back().X(), hist.range.back().Y()},
                    hist.variable.front(), hist.variable.back());
                hist.histo = static_cast<TH1*>(histo->DrawClone());
            } else {
                RESTError << "Only 1D or 2D histograms are supported " << RESTendl;
                continue;
            }
            hist.histo->SetLineColor(hist.lineColor);
            hist.histo->SetLineWidth(hist.lineWidth);
            hist.histo->SetLineStyle(hist.lineStyle);
            hist.histo->SetFillColor(hist.fillColor);
            hist.histo->SetFillStyle(hist.fillStyle);
            // If stats are on histos must we drawn
            if (hist.statistics) {
                hist.histo->SetStats(true);
                hist.histo->Draw();
                combinedCanvas.Update();
            } else {
                hist.histo->SetStats(false);
            }
            // Normalize histos
            if (plots.normalize > 0) {
                const double integral = hist.histo->Integral();
                if (integral > 0) hist.histo->Scale(plots.normalize / integral);
            }
            // Scale histos
            if (plots.scale != "") {
                std::string inputScale = plots.scale;
                while (inputScale.find("binSize") != std::string::npos) {
                    double binSize = hist.histo->GetXaxis()->GetBinWidth(1);
                    inputScale.replace(inputScale.find("binSize"), 7, DoubleToString(binSize));
                }
                while (inputScale.find("entries") != std::string::npos) {
                    double entries = hist.histo->GetEntries();
                    inputScale.replace(inputScale.find("entries"), 7, DoubleToString(entries));
                }
                while (inputScale.find("runLength") != std::string::npos) {
                    double runLength = dataSet.GetTotalTimeInSeconds() / 3600.;  // in hours
                    inputScale.replace(inputScale.find("runLength"), 9, DoubleToString(runLength));
                }
                while (inputScale.find("integral") != std::string::npos) {
                    double integral = hist.histo->Integral("width");
                    inputScale.replace(inputScale.find("integral"), 8, DoubleToString(integral));
                }
                std::string scale = "1./(" + inputScale + ")";
                hist.histo->Scale(StringToDouble(EvaluateExpression(scale)));
            }

            // Add histos to the THStack
            plots.hs->Add(hist.histo, hist.drawOption.c_str());
            // Add histos to the legend
            if (plots.legend != nullptr) plots.legend->AddEntry(hist.histo, hist.histo->GetName(), "lf");
        }
    }

    // This function do the actual drawing of the THStack with the different options
    for (auto& plots : fPlots) {
        if (plots.hs == nullptr) continue;
        // TPad parameters
        TPad* targetPad = (TPad*)combinedCanvas.cd(canvasIndex);
        targetPad->SetLogx(plots.logX);
        targetPad->SetLogy(plots.logY);
        targetPad->SetLogz(plots.logZ);
        targetPad->SetGridx(plots.gridX);
        targetPad->SetGridy(plots.gridY);
        targetPad->SetLeftMargin(plots.marginLeft);
        targetPad->SetRightMargin(plots.marginRight);
        targetPad->SetBottomMargin(plots.marginBottom);
        targetPad->SetTopMargin(plots.marginTop);

        // HStack draw parameters
        plots.hs->Draw(plots.stackDrawOption.c_str());
        plots.hs->GetXaxis()->SetTitle(plots.labelX.c_str());
        plots.hs->GetYaxis()->SetTitle(plots.labelY.c_str());
        plots.hs->GetXaxis()->SetLabelSize(1.1 * plots.hs->GetXaxis()->GetLabelSize());
        plots.hs->GetYaxis()->SetLabelSize(1.1 * plots.hs->GetYaxis()->GetLabelSize());
        plots.hs->GetXaxis()->SetTitleSize(1.1 * plots.hs->GetXaxis()->GetTitleSize());
        plots.hs->GetYaxis()->SetTitleSize(1.1 * plots.hs->GetYaxis()->GetTitleSize());

        if (plots.timeDisplay) plots.hs->GetXaxis()->SetTimeDisplay(1);
        if (plots.legend != nullptr) plots.legend->Draw();

        targetPad->Update();
        combinedCanvas.Update();
        canvasIndex++;
    }

    // Preview plot. User can make some changed before saving
    if (!REST_Display_CompatibilityMode && fPreviewPlot) {
        combinedCanvas.Resize();
        GetChar();
    }

    // Save single pads if save is marked
    for (auto& plots : fPlots) {
        if (plots.save.empty()) continue;
        std::unique_ptr<TCanvas> canvas(new TCanvas());
        canvas->SetLogx(plots.logX);
        canvas->SetLogy(plots.logY);
        canvas->SetLogz(plots.logZ);
        canvas->SetGridx(plots.gridX);
        canvas->SetGridy(plots.gridY);
        canvas->SetLeftMargin(plots.marginLeft);
        canvas->SetRightMargin(plots.marginRight);
        canvas->SetBottomMargin(plots.marginBottom);
        canvas->SetTopMargin(plots.marginTop);
        plots.hs->Draw(plots.stackDrawOption.c_str());
        canvas->Print(plots.save.c_str());
    }

    // Save combined canvas
    if (!fOutputFileName.empty()) {
        for (const auto& [name, quant] : quantity) {
            size_t pos = 0;
            fOutputFileName = Replace(fOutputFileName, quant.metadata, quant.value, pos);
        }
        combinedCanvas.Print(fOutputFileName.c_str());
        // In case of root file save also the histograms
        if (TRestTools::GetFileNameExtension(fOutputFileName) == "root") {
            std::unique_ptr<TFile> f(TFile::Open(fOutputFileName.c_str(), "UPDATE"));
            for (auto& plots : fPlots) {
                for (auto& hist : plots.histos) {
                    hist.histo->Write();
                }
            }
            this->Write();
        }
    }

    CleanUp();
}

///////////////////////////////////////////////
/// \brief Clean up histos and text but note that the
/// metadata is unchanged
///
void TRestDataSetPlot::CleanUp() {
    for (auto& plots : fPlots) {
        for (auto& hist : plots.histos) {
            delete hist.histo;
        }
        delete plots.hs;
        delete plots.legend;
    }

    for (auto& panel : fPanels) {
        for (auto& text : panel.text) {
            delete text;
        }
        panel.text.clear();
    }
}

///////////////////////////////////////////////
/// \brief This functions gets the ID from a map string that is passed
/// by reference. It is used to translate colors, line styles and fill
/// styles from the string to an integer value.
///
Int_t TRestDataSetPlot::GetIDFromMapString(const std::map<std::string, int>& mapStr, const std::string& in) {
    if (in.find_first_not_of("0123456789") == std::string::npos) {
        return StringToInteger(in);
    }
    auto it = mapStr.find(in);
    if (it != mapStr.end()) {
        return it->second;
    } else {
        RESTWarning << "cannot find ID with name \"" << in << "\"" << RESTendl;
    }
    return -1;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestDataSetPlot
///
void TRestDataSetPlot::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << "DataSet name: " << fDataSetName << RESTendl;
    RESTMetadata << "PaletteStyle: " << fPaletteStyle << RESTendl;
    if (fPreviewPlot) RESTMetadata << "Preview plot is ACTIVE" << RESTendl;
    RESTMetadata << "Canvas size: (" << fCanvasSize.X() << " ," << fCanvasSize.Y() << ")" << RESTendl;
    RESTMetadata << "Canvas divisions: (" << fCanvasDivisions.X() << " ," << fCanvasDivisions.Y() << ")"
                 << RESTendl;
    RESTMetadata << "-------------------" << RESTendl;
    for (const auto& plot : fPlots) {
        RESTMetadata << "-------------------" << RESTendl;
        RESTMetadata << "Plot name/title: " << plot.name << " " << plot.title << RESTendl;
        RESTMetadata << "Save string: " << plot.save << RESTendl;
        RESTMetadata << "Set log X,Y,Z: " << plot.logX << ", " << plot.logY << ", " << plot.logZ << RESTendl;
        RESTMetadata << "Stack draw Option: " << plot.stackDrawOption << RESTendl;
        if (plot.legend) RESTMetadata << "Legend is ON" << RESTendl;
        if (plot.timeDisplay) RESTMetadata << "Time display is ON" << RESTendl;
        RESTMetadata << "Labels X,Y: " << plot.labelX << ", " << plot.labelY << RESTendl;
        for (const auto& hist : plot.histos) {
            RESTMetadata << "****************" << RESTendl;
            RESTMetadata << "Histo name: " << hist.name << RESTendl;
            RESTMetadata << "Draw Option: " << hist.drawOption << RESTendl;
            RESTMetadata << "Histogram size: " << hist.variable.size() << " with parameters:" << RESTendl;
            for (size_t i = 0; i < hist.variable.size(); i++) {
                RESTMetadata << "\t" << i << " " << hist.variable[i] << ", " << hist.nBins[i] << ", "
                             << hist.range[i].X() << ", " << hist.range[i].Y() << RESTendl;
            }
            RESTMetadata << "****************" << RESTendl;
        }
    }
    RESTMetadata << "-------------------" << RESTendl;
    for (auto& panel : fPanels) {
        RESTMetadata << "-------------------" << RESTendl;
        RESTMetadata << "Panel font size/precision " << panel.font_size << ", " << panel.precision
                     << RESTendl;
        RESTMetadata << "****************" << RESTendl;
        for (auto& [key, posLabel] : panel.variablePos) {
            auto&& [obs, label, units] = key;
            RESTMetadata << "Label variable " << obs << ", label " << label << ", units " << units << " Pos ("
                         << posLabel.X() << ", " << posLabel.Y() << ")" << RESTendl;
        }
        RESTMetadata << "****************" << RESTendl;
        for (auto& [key, posLabel] : panel.metadataPos) {
            auto&& [obs, label, units] = key;
            RESTMetadata << "Label metadata " << obs << ", label " << label << ", units " << units << " Pos ("
                         << posLabel.X() << ", " << posLabel.Y() << ")" << RESTendl;
        }
        RESTMetadata << "****************" << RESTendl;
        for (auto& [key, posLabel] : panel.obsPos) {
            auto&& [obs, label, units] = key;
            RESTMetadata << "Label Observable " << obs << ", label " << label << ", units " << units
                         << " Pos (" << posLabel.X() << ", " << posLabel.Y() << ")" << RESTendl;
        }
        RESTMetadata << "****************" << RESTendl;
    }
    RESTMetadata << "-------------------" << RESTendl;

    RESTMetadata << RESTendl;
}
