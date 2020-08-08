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

//////////////////////////////////////////////////////////////////////////
/// TRestMetadataPlot will allow to take a set or range of files and create TGraph
/// objects with any two metadata members defined in any metadata class inside the
/// REST-ROOT processed files. The class TRestMetadataPlot works in a similar way
/// to TRestAnalysisPlot. While TRestAnalysisPlot serves to draw and combine data
/// from different event observables found at the TRestAnalysisTree,
/// TRestMetadataPlot is used to draw and correlate the metadata information that
/// is found at different metadata classes inside TRestRun.
///
/// \htmlonly <style>div.image img[src="metadataPlot.png"]{width:800px;}</style> \endhtmlonly
///
/// ![A plot generated with TRestMetadataPlot using the example metadataPlot.rml](metadataPlot.png)
///
/// ## 1. Adding REST files for plotting
///
/// This class can be initialized through a RML configuration file where we define
/// the different plots and graphs to be produced. The files that will be used for
/// plot generation can be added to the class using the `restManager` command as
/// follows:
///
/// \code
/// restManager --c mdPlots.rml --f "/full/or/relative/path/to/data/R012*Hits*.root"
/// \endcode
///
/// The previous execution will search for the files matching the given pattern, at
/// the path given. I.e. it will collect all the files with run number starting by
/// `012` and containing `Hits` inside its name.
///
/// Each of the files on the filelist produced will be used to generate one point at
/// the graphs defined inside the `mdPlots.rml` file.
///
/// The metadata variables can be specified at the graphs or plots by using the
/// following format `TRestMetadataClass::fMetadataMember` or
/// `TRestMetadataClass->fMetadataMember`. Where `TRestMetadataClass` is the name of
/// the class/object or the name ROOT name associated to the class, and that it is
/// usually retrieved using the method `GetName()`.
///
/// ## 2. Writting the RML metadata section
///
/// The RML section allows to define the main properties of the canvas to be
/// generated, basic or common properties of each plot, and the definition of the
/// plots description and graph properties together with the specification of the
/// metadata to be used in the graph. A working example with minimum modifications
/// can be found at `examples/metadataPlot.rml`.
///
/// This metadata class receives only an optional parameter, *previewPlot*, that
/// is true by default if it has not been defined. If this parameter is true it will
/// show the produced canvas on screen, and the user will be allowed to interact
/// with it. If not, plots will be just written to disk as defined by the parameter
/// `save` defined in the main canvas, or in separate plots.
///
/// ### Canvas definition
///
/// The canvas will be divided in different pads where the plots defined inside this
/// class will be placed. The canvas section allows us to define the size of the
/// canvas in pixels, how it will be divided, and optionally the file where it will
/// be saved. It is important to notice that the number of pad divisions must be
/// enough to host the total number of plots given.
///
/// \code
///	<canvas size="(1000,400)" divide="(1,1)" save="/tmp/canvas.png"/>
/// \endcode
///
/// ### Legend properties
///
/// If we use a legend in our plots, we can define its properties and position using
/// the `legend` key.
///
/// An example, of definition of the legend key. The ROOT drawing option `lp` is
/// used to draw the line and the point. Please, check the documentation of
/// <a href="https://root.cern.ch/doc/master/classTLegend.html">TLegend</a>
/// for other available options.
///
/// \code
///	<legendPosition x1="0.25" x2="0.5" y1="0.5" y2="0.65" option="lp" />
/// \endcode
///
/// \warning For the moment this entry is common to all the plots, althought
/// it would be desirable that in the future we are allowed to define the legend
/// properties for each single plot independently. The positions are defined with
/// coordinates relative to the pad using the ROOT object TLegend.
///
/// ### Plot properties
///
/// Inside a TRestMetadataPlot RML section we are allowed to define any number of
/// plots, and inside a graph we are allowed to define any number of graphs.
///
/// The only mandatory field inside the plot description is the `xVariable` to be
/// used on the x-axis of the resulting plot.
///
/// The parameters that can be defined (see also TRestMetadataPlot::Plot_Info_Set)
/// inside each `<plot` section are:
///
/// * **name**: The default name that will be used for the TGraph object if no name
/// is explicitly given.
/// * **title**: An optional title that will be shown on the top of the plot.
/// * **xVariable**: A mandatory field specifying the variable, extracted from a
/// metadata member to be used for the x-axis. The keyword "timestamp" can be
/// used instead of the data member so that the run middle time is considered.
/// * **logscale (or logY)**: It defines a logarithmic scale on the y-axis.
/// * **logX**: It defines a logarithmic scale on the x-axis.
/// * **xlabel**: The label or title to be given to the x-axis.
/// * **ylabel**: The label or title to be given to the y-axis.
/// * **legend**: If it is "true" or "on" the legend of the plot will be displayed.
/// * **timeDisplay**: If it is "true" or "on" the x-axis will be translated to a
/// date/time format. The value given at `xVariable` should be a valid UNIX
/// timestamp.
/// * **save**: A filename specifying the output where the current plot will be
/// saved. Any image/document format supported by ROOT will be accepted.
/// * **xRange**: It allows to define manually the x-range to be used in the plot.
/// If not given, ROOT will automatically define the range.
/// * **yRange**: We can provide optionally the range to be used in the plot. If not,
/// it will be automatically calculated as 80%-120% of the minimum-maximum values.
///
/// \note As previously mentioned, the `xVariable` must be a valid metadata member.
/// However, there is one exception where we are allowed to use the `timestamp`
/// keyword so that the middle time of each run is used on the x-axis.
///
/// ### Graph properties
///
/// Inside each plot we can define any number of graphs to be plotted together.
/// Each graph must define a unique `name` and the corresponding `yVariable` to be
/// used on the graph data. Those fields are mandatory.
///
/// The parameters that can be defined inside each `<graph` section are:
///
/// * **name**: The default name that will be used for the TGraph object if no name
/// is explicitely given.
/// * **title**: An optional title that will be shown on the top of the plot.
/// * **yVariable**: A mandatory field specifying the variable, extracted from a
/// metadata member to be used for the y-axis.
/// * **option**: It defines the ROOT option to be used at the TGraph Draw method.
/// The available options are defined by the ROOT class TGraphPainter, please, check
/// the official ROOT documentation of
/// <a href="https://root.cern.ch/doc/master/classTGraphPainter.html">TGraphPainter</a>
/// to see all the available options.
///
/// ### Including metadata rules, or conditions
///
/// When we provide a set of files we may filter the input files passed to the class
/// to create a selection that will follow a certain condition or metadata rule. Only
/// the files fulfilling the given conditions will contribute to the TGraph.
///
/// In order to use this feature we need to define the `metadataRule` parameter
/// specifying the condition to be satisfied. Any numerical operator is allowed, as
/// well as direct string comparison.
///
/// The following line inside the <graph definition would force the corresponding
/// graph to include points extracted only from files where the fRunTag metadata
/// member is equal to `Calibration_BIPO`.
///
/// \code
///     <parameter name="metadataRule" value="TRestRun->fRunTag==Calibration_BIPO" />
/// \endcode
///
/// \warning for the moment the conditions can be applied to a single metadata member.
/// Or in other words, we cannot create a metadata rule based on the combination of
/// different conditions on different metadata parameters.
///
/// ### Adding panels
///
/// Additionally, we can add a pad to the canvas containning basic information. As it
/// is implemented, this panel will only be able to gather the run information from the
/// first file given in the filelist. Therefore it might be useful to show only
/// information common to all the files.
///
/// The panel is built defining labels where metadata members given between << >> are
/// replaced by the corresponding run values.
///
/// \code
///   <panel font_size="0.06">
///       <label value="Run number : <<TRestRun::fRunNumber>>" x="0.25" y="0.9" />
///       ...
/// \endcode
///
/// \htmlonly <style>div.image img[src="panel.png"]{width:200px;}</style> \endhtmlonly
///
/// The panel it is given at the same level as the <plot, as it will fill a given pad
/// region inside the canvas. Additional keywords are available, and calculated inside
/// the class with all the files passed. Those keywords are also enclosed between << >>,
/// and are the following ones:
///
/// * **startTime**: The earliest (or lowest) start time found between all the files
/// given.
/// * **endTime**: The latest (or highest) end time found between all the files given.
/// * **entries**: The integrated number of entries using all the filelist given to the
/// class.
/// * **runLength**: The integrated run length in hours using all the filelist given to
/// the class.
/// * **meanRate**: The rate calculated as the result of the total number of entries and
/// the run length, the resulting rate is expressed in Hz.
///
/// ![An example of panel information generated using the <panel section](panel.png)
///
/// \note All panels will be always placed at the first pads of the canvas, independently
/// of the order given inside the RML file.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2020-June: First concept and implementation
/// \author     Javier Galan
///
/// \class TRestMetadataPlot
///
/// <hr>
///

#include "TRestMetadataPlot.h"
#include "TRestManager.h"
#include "TRestTools.h"
using namespace std;

#include <TLegend.h>
#include <TStyle.h>

#include <TGraph.h>

#include <ctime>

ClassImp(TRestMetadataPlot);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestMetadataPlot::TRestMetadataPlot() { Initialize(); }

///////////////////////////////////////////////
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
/// corresponding TRestG4Metadata section inside the RML.
///
TRestMetadataPlot::TRestMetadataPlot(const char* cfgFileName, const char* name) : TRestMetadata(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestMetadataPlot::~TRestMetadataPlot() {
    if (fRun != NULL) delete fRun;
}

///////////////////////////////////////////////
/// \brief Initialization of TRestMetadataPlot data members
///
void TRestMetadataPlot::Initialize() {
    SetSectionName(this->ClassName());

    fRun = NULL;

    fNFiles = 0;

    fCombinedCanvas = NULL;

    fPlotNamesCheck.clear();
}

///////////////////////////////////////////////
/// \brief Initialization of TRestMetadataPlot members through a RML file
///
void TRestMetadataPlot::InitFromConfigFile() {
    size_t position = 0;
    if (fHostmgr->GetRunInfo() != NULL) {
        fRun = fHostmgr->GetRunInfo();
    }

#pragma region ReadLabels
    debug << "TRestMetadataPlot: Reading canvas settings" << endl;
    position = 0;
    string formatDefinition;
    if ((formatDefinition = GetKEYDefinition("labels", position)) != "") {
        if (GetVerboseLevel() >= REST_Debug) {
            cout << formatDefinition << endl;
            cout << "Reading format definition : " << endl;
            cout << "---------------------------" << endl;
        }

        fTicksScaleX = StringToDouble(GetFieldValue("ticksScaleX", formatDefinition));
        fTicksScaleY = StringToDouble(GetFieldValue("ticksScaleY", formatDefinition));

        fLabelScaleX = StringToDouble(GetFieldValue("labelScaleX", formatDefinition));
        fLabelScaleY = StringToDouble(GetFieldValue("labelScaleY", formatDefinition));

        fLabelOffsetX = StringToDouble(GetFieldValue("labelOffsetX", formatDefinition));
        fLabelOffsetY = StringToDouble(GetFieldValue("labelOffsetY", formatDefinition));

        if (fLabelOffsetX == -1) fLabelOffsetX = 1.1;
        if (fLabelOffsetY == -1) fLabelOffsetY = 1.3;

        if (fTicksScaleX == -1) fTicksScaleX = 1.5;
        if (fTicksScaleY == -1) fTicksScaleY = 1.5;

        if (fLabelScaleX == -1) fLabelScaleX = 1.3;
        if (fLabelScaleY == -1) fLabelScaleY = 1.3;

        if (GetVerboseLevel() >= REST_Debug) {
            cout << "ticks scale X : " << fTicksScaleX << endl;
            cout << "ticks scale Y : " << fTicksScaleY << endl;
            cout << "label scale X : " << fLabelScaleX << endl;
            cout << "label scale Y : " << fLabelScaleY << endl;
            cout << "label offset X : " << fLabelOffsetX << endl;
            cout << "label offset Y : " << fLabelOffsetY << endl;

            if (GetVerboseLevel() >= REST_Extreme) GetChar();
        }
    }
#pragma endregion

#pragma region ReadLegend
    position = 0;
    string legendDefinition;
    if ((legendDefinition = GetKEYDefinition("legendPosition", position)) != "") {
        if (GetVerboseLevel() >= REST_Debug) {
            cout << legendDefinition << endl;
            cout << "Reading legend definition : " << endl;
            cout << "---------------------------" << endl;
        }

        fLegendX1 = StringToDouble(GetFieldValue("x1", legendDefinition));
        fLegendY1 = StringToDouble(GetFieldValue("y1", legendDefinition));

        fLegendX2 = StringToDouble(GetFieldValue("x2", legendDefinition));
        fLegendY2 = StringToDouble(GetFieldValue("y2", legendDefinition));

        if (fLegendX1 == -1) fLegendX1 = 0.7;
        if (fLegendY1 == -1) fLegendY1 = 0.75;

        if (fLegendX2 == -1) fLegendX2 = 0.88;
        if (fLegendY2 == -1) fLegendY2 = 0.88;

        if (GetVerboseLevel() >= REST_Debug) {
            cout << "x1 : " << fLegendX1 << " y1 : " << fLegendY1 << endl;
            cout << "x2 : " << fLegendX2 << " y2 : " << fLegendY2 << endl;

            if (GetVerboseLevel() >= REST_Extreme) GetChar();
        }

        fLegendOption = GetFieldValue("option", legendDefinition);
        if (fLegendOption == "Not defined") fLegendOption = "lp";
    }
#pragma endregion

#pragma region ReadCanvas
    position = 0;
    string canvasDefinition;
    if ((canvasDefinition = GetKEYDefinition("canvas", position)) != "") {
        fCanvasSize = StringTo2DVector(GetFieldValue("size", canvasDefinition));
        fCanvasDivisions = StringTo2DVector(GetFieldValue("divide", canvasDefinition));
        fCanvasSave = GetFieldValue("save", canvasDefinition);
        if (fCanvasSave == "Not defined") {
            fCanvasSave = GetParameter("pdfFilename", "/tmp/restplot.pdf");
        }
    }
#pragma endregion

#pragma region ReadPlot
    debug << "TRestMetadataPlot: Reading plot sections" << endl;
    Int_t maxPlots = (Int_t)fCanvasDivisions.X() * (Int_t)fCanvasDivisions.Y();
    TiXmlElement* plotele = fElement->FirstChildElement("plot");
    while (plotele != NULL) {
        string active = GetParameter("value", plotele, "ON");
        if (ToUpper(active) == "ON") {
            int N = fPlots.size();
            if (N >= maxPlots) {
                ferr << "Your canvas divisions (" << fCanvasDivisions.X() << " , " << fCanvasDivisions.Y()
                     << ") are not enough to show " << N + 1 << " plots" << endl;
                exit(1);
            }

            Plot_Info_Set plot;
            plot.name = RemoveWhiteSpaces(GetParameter("name", plotele, "plot_" + ToString(N)));
            plot.title = GetParameter("title", plotele, "");
            plot.xVariable = RemoveWhiteSpaces(GetParameter("xVariable", plotele, "timestamp"));
            plot.logY = StringToBool(GetParameter("logscale", plotele, "false"));
            plot.logY = plot.logY ? plot.logY : StringToBool(GetParameter("logY", plotele, "false"));
            plot.logX = StringToBool(GetParameter("logX", plotele, "false"));
            plot.labelX = GetParameter("xlabel", plotele, "");
            plot.labelY = GetParameter("ylabel", plotele, "");
            plot.legendOn = StringToBool(GetParameter("legend", plotele, "OFF"));
            plot.timeDisplay = StringToBool(GetParameter("timeDisplay", plotele, "OFF"));
            plot.save = RemoveWhiteSpaces(GetParameter("save", plotele, ""));

            plot.yRange = StringTo2DVector(GetParameter("yRange", plotele, "(-1,-1)"));
            plot.xRange = StringTo2DVector(GetParameter("xRange", plotele, "(-1,-1)"));

            debug << " Plot parameters read from <plot section " << endl;
            debug << " --------------------------------------- " << endl;
            debug << "- name: " << plot.name << endl;
            debug << "- title: " << plot.title << endl;
            debug << "- xVariable: " << plot.xVariable << endl;
            debug << "- logX: " << plot.logX << endl;
            debug << "- logY: " << plot.logY << endl;
            debug << "- X-label : " << plot.labelX << endl;
            debug << "- Y-label : " << plot.labelY << endl;
            debug << "- legendOn: " << plot.legendOn << endl;
            debug << "- timeDisplay: " << plot.timeDisplay << endl;
            debug << "- save : " << plot.save << endl;

            TiXmlElement* graphele = plotele->FirstChildElement("graph");
            if (graphele == NULL) {
                // in case for single-graph plot, variables might be added directly inside the <plot section
                graphele = plotele;
            }

            while (graphele != NULL) {
                Graph_Info_Set graph = SetupGraphFromConfigFile(graphele, plot);

                debug << "Graph name : " << graph.name << endl;
                debug << "Graph variable : " << graph.yVariable << endl;

                plot.graphs.push_back(graph);

                graphele = graphele->NextSiblingElement("graph");
            }

            fPlots.push_back(plot);
            plotele = plotele->NextSiblingElement("plot");
        }
    }
#pragma endregion

#pragma region ReadPanel
    debug << "TRestMetadataPlot: Reading panel sections" << endl;
    maxPlots -= fPlots.size();  // remaining spaces on canvas
    TiXmlElement* panelele = fElement->FirstChildElement("panel");
    while (panelele != NULL) {
        string active = GetParameter("value", panelele, "ON");
        if (ToUpper(active) == "ON") {
            int N = fPanels.size();
            if (N >= maxPlots) {
                ferr << "Your canvas divisions (" << fCanvasDivisions.X() << " , " << fCanvasDivisions.Y()
                     << ") are not enough to show " << fPlots.size() << " plots, and " << N + 1
                     << " info panels" << endl;
                exit(1);
            }

            Panel_Info panel;
            panel.font_size = StringToDouble(GetParameter("font_size", panelele, "0.1"));

            TiXmlElement* labelele = panelele->FirstChildElement("label");
            while (labelele != NULL) {
                panel.label.push_back(GetParameter("value", labelele, "Error. Label value not defined"));
                panel.posX.push_back(StringToDouble(GetParameter("x", labelele, "0.1")));
                panel.posY.push_back(StringToDouble(GetParameter("y", labelele, "0.1")));

                labelele = labelele->NextSiblingElement("label");
            }

            fPanels.push_back(panel);
            panelele = panelele->NextSiblingElement("panel");
        }
    }

    for (int n = 0; n < fPanels.size(); n++) {
        extreme << "Panel " << n << " with font size : " << fPanels[n].font_size << endl;
        for (int m = 0; m < fPanels[n].posX.size(); m++) {
            extreme << "Label : " << fPanels[n].label[m] << endl;
            extreme << "Pos X : " << fPanels[n].posX[m] << endl;
            extreme << "Pos Y : " << fPanels[n].posY[m] << endl;
        }
    }
#pragma endregion
}

///////////////////////////////////////////////
/// \brief It fills the components of a Graph_Info_set object using the definition inside a <graph element.
///
TRestMetadataPlot::Graph_Info_Set TRestMetadataPlot::SetupGraphFromConfigFile(TiXmlElement* graphele,
                                                                              Plot_Info_Set plot) {
    Graph_Info_Set graph;
    graph.name = RemoveWhiteSpaces(GetParameter("name", graphele, plot.name));
    graph.title = GetParameter("title", graphele, plot.title);
    graph.yVariable = RemoveWhiteSpaces(GetParameter("yVariable", graphele, ""));
    graph.drawOption = RemoveWhiteSpaces(GetParameter("option", graphele, ""));
    graph.metadataRule = RemoveWhiteSpaces(GetParameter("metadataRule", graphele, ""));

    for (int n = 0; n < fPlotNamesCheck.size(); n++)
        if (graph.name == fPlotNamesCheck[n]) {
            ferr << "Repeated plot/graph names were found! Please, use different names for different plots!"
                 << endl;
            ferr << "<plot/graph name=\"" << graph.name << "\" already defined!" << endl;
            exit(1);
        }

    if (graph.yVariable == "") {
        ferr << "Problem reading yVariable from graph with name : " << graph.name << endl;
        exit(2);
    }

    fPlotNamesCheck.push_back(graph.name);

    // 5. read draw style(line color, width, marker style, size, etc.)
    graph.markerStyle = StringToInteger(GetParameter("markerStyle", graphele));
    graph.markerSize = StringToInteger(GetParameter("markerSize", graphele));
    graph.markerColor = StringToInteger(GetParameter("markerColor", graphele));
    graph.lineColor = StringToInteger(GetParameter("lineColor", graphele));
    graph.lineWidth = StringToInteger(GetParameter("lineWidth", graphele));
    graph.lineStyle = StringToInteger(GetParameter("lineStyle", graphele));

    return graph;
}

///////////////////////////////////////////////
/// \brief It creates a new TRestRun object extracted from the fileName, and it
/// adds it to the list of input files beloning to this class.
///
void TRestMetadataPlot::AddFile(TString fileName) {
    debug << "TRestMetadataPlot::AddFile. Adding file. " << endl;
    debug << "File name: " << fileName << endl;

    // TODO: How do we check here that the run is valid?
    fRunInputFileName.push_back((string)fileName);
    fNFiles++;
}

///////////////////////////////////////////////
/// \brief We can add input file from process's output file
///
void TRestMetadataPlot::AddFileFromExternalRun() {
    if (fRun != NULL) {
        if (fHostmgr->GetProcessRunner() != NULL && fRun->GetOutputFileName() != "") {
            // if we have a TRestProcessRunner before head, we use its output file
            AddFile(fRun->GetOutputFileName());
            return;
        } else if (fRun->GetInputFileNames().size() != 0) {
            // if we have only TRestRun, we ask for its input file list
            auto names = fRun->GetInputFileNames();
            for (int i = 0; i < names.size(); i++) {
                this->AddFile(names[i]);
            }
            return;
        }
    }
}

///////////////////////////////////////////////
/// \brief We can add input file from parameter "inputFile"
///
void TRestMetadataPlot::AddFileFromEnv() {
    string filepattern = GetParameter("inputFile", "");
    auto files = TRestTools::GetFilesMatchingPattern(filepattern);

    for (unsigned int n = 0; n < files.size(); n++) {
        essential << "Adding file : " << files[n] << endl;
        AddFile(files[n]);
    }
}

///////////////////////////////////////////////
/// \brief It returns the index of the array correspoding to the plot with the given plotName.
///
Int_t TRestMetadataPlot::GetPlotIndex(TString plotName) {
    for (unsigned int n = 0; n < fPlots.size(); n++)
        if (fPlots[n].name == plotName) return n;

    warning << "TRestMetadataPlot::GetPlotIndex. Plot name " << plotName << " not found" << endl;
    return -1;
}

///////////////////////////////////////////////
/// \brief The main method of this class, collecting the input files, producing the TGraphs
/// and generating the plots inside the main canvas.
///
void TRestMetadataPlot::GenerateCanvas() {
    info << "--------------------------" << endl;
    info << "Starting to GenerateCanvas" << endl;
    info << "--------------------------" << endl;
    // Add files, first use <addFile section definition
    TiXmlElement* ele = fElement->FirstChildElement("addFile");
    while (ele != NULL) {
        TString inputfile = GetParameter("name", ele);
        this->AddFile(inputfile);
        ele = ele->NextSiblingElement("addFile");
    }
    // try to add files from external TRestRun handler
    if (fNFiles == 0) AddFileFromExternalRun();
    // try to add files from env "inputFile", which is set by --i argument
    if (fNFiles == 0) AddFileFromEnv();

    if (fNFiles == 0) {
        ferr << "TRestMetadataPlot: No input files are added!" << endl;
        exit(1);
    }

    // initialize output root file if we have TRestRun running
    TFile* fOutputRootFile = NULL;
    if (fRun != NULL) {
        fOutputRootFile = fRun->GetOutputFile();
        if (fOutputRootFile == NULL) {
            fRun->SetHistoricMetadataSaving(false);
            fOutputRootFile = fRun->FormOutputFile();
        }
    }

    // Initializing canvas window
    if (fCombinedCanvas != NULL) {
        delete fCombinedCanvas;
        fCombinedCanvas = NULL;
    }
    fCombinedCanvas = new TCanvas("combined", "combined", 0, 0, fCanvasSize.X(), fCanvasSize.Y());
    fCombinedCanvas->Divide((Int_t)fCanvasDivisions.X(), (Int_t)fCanvasDivisions.Y());

    // Setting up TStyle
    TStyle* st = new TStyle();
    st->SetPalette(1);

    if (fPanels.size() > 0) {
        Double_t startTime = 0;
        Double_t endTime = 0;
        Double_t runLength = 0;
        Int_t totalEntries = 0;
        for (unsigned int n = 0; n < fRunInputFileName.size(); n++) {
            TRestRun* run = new TRestRun();
            run->SetHistoricMetadataSaving(false);
            run->OpenInputFile(fRunInputFileName[n]);

            info << "Loading timestamps from file : " << fRunInputFileName[n] << endl;

            Double_t endTimeStamp = run->GetEndTimestamp();
            Double_t startTimeStamp = run->GetStartTimestamp();

            // We get the lowest/highest run time stamps.
            if (!startTime || startTime > endTimeStamp) startTime = endTimeStamp;
            if (!startTime || startTime > startTimeStamp) startTime = startTimeStamp;

            if (!endTime || endTime < startTimeStamp) endTime = startTimeStamp;
            if (!endTime || endTime < endTimeStamp) endTime = endTimeStamp;

            if (endTimeStamp - startTimeStamp > 0) {
                runLength += endTimeStamp - startTimeStamp;
                totalEntries += run->GetEntries();
            }
            delete run;
        }

        Double_t meanRate = totalEntries / runLength;

        runLength /= 3600.;

        TRestRun* panelRun = new TRestRun();
        panelRun->SetHistoricMetadataSaving(false);
        panelRun->OpenInputFile(fRunInputFileName[0]);
        for (unsigned int n = 0; n < fPanels.size(); n++) {
            TPad* targetPad = (TPad*)fCombinedCanvas->cd(n + 1);
            for (unsigned int m = 0; m < fPanels[n].posX.size(); m++) {
                string label = fPanels[n].label[m];

                size_t pos = 0;
                label = Replace(label, "<<startTime>>", ToDateTimeString(startTime), pos);
                pos = 0;
                label = Replace(label, "<<endTime>>", ToDateTimeString(endTime), pos);
                pos = 0;
                label = Replace(label, "<<entries>>", Form("%d", totalEntries), pos);
                pos = 0;
                label = Replace(label, "<<runLength>>", Form("%5.2lf", runLength), pos);
                pos = 0;
                label = Replace(label, "<<meanRate>>", Form("%5.2lf", meanRate), pos);

                label = panelRun->ReplaceMetadataMembers(label);

                TLatex* texxt = new TLatex(fPanels[n].posX[m], fPanels[n].posY[m], label.c_str());
                texxt->SetTextColor(1);
                texxt->SetTextSize(fPanels[n].font_size);
                texxt->Draw("same");
            }
        }
        delete panelRun;
    }

    // start drawing plots
    vector<TGraph*> graphCollectionAll;
    for (unsigned int n = 0; n < fPlots.size(); n++) {
        Plot_Info_Set plot = fPlots[n];

        TPad* targetPad = (TPad*)fCombinedCanvas->cd(n + 1 + fPanels.size());
        targetPad->SetLogx(plot.logX);
        targetPad->SetLogy(plot.logY);
        targetPad->SetLeftMargin(0.18);
        targetPad->SetRightMargin(0.1);
        targetPad->SetBottomMargin(0.15);
        targetPad->SetTopMargin(0.07);

        // If the first graph does not contain the axis option we must add it manually
        if (plot.graphs[0].drawOption.find("A") == string::npos)
            plot.graphs[0].drawOption = plot.graphs[0].drawOption + "A";

        // draw to a new graph
        vector<TGraph*> graphCollectionPlot;
        for (unsigned int i = 0; i < plot.graphs.size(); i++) {
            Graph_Info_Set graph = plot.graphs[i];

            TString graphName = graph.name;
            TString graphVariable = graph.yVariable;

            if (GetVerboseLevel() >= REST_Debug) {
                cout << endl;
                cout << "--------------------------------------" << endl;
                cout << "Graph name : " << graphName << endl;
                cout << "Graph variable : " << graphVariable << endl;
                cout << "++++++++++++++++++++++++++++++++++++++" << endl;
            }

            // Perhaps an RML option?
            Bool_t skipZeroData = true;

            std::vector<Double_t> xData;
            std::vector<Double_t> yData;
            std::map<Double_t, Double_t> dataMap;
            std::map<Double_t, Int_t> dataMapN;

            // TODO Here we open the file for each graph construction. This might slowdown these
            // drawing routnes when we load thousands of files and we have many graphs. It could be
            // optimized for the case of many graphs by preloading TGraph data into a dedicated
            // structure at Graph_Info_Set

            // We build the corresponding TGraph extracting the points from each file
            for (unsigned int j = 0; j < fRunInputFileName.size(); j++) {
                info << "Loading file : " << fRunInputFileName[j] << endl;

                TRestRun* run = new TRestRun();
                run->SetHistoricMetadataSaving(false);
                run->OpenInputFile(fRunInputFileName[j]);

                // Checking if the run satisfies the metadata rule defined (if any)
                // If it doesnt we skip this run
                if (!run->EvaluateMetadataMember(graph.metadataRule)) {
                    delete run;
                    continue;
                }

                // We reject runs with unexpected zero y-data
                Double_t yVal = StringToDouble(run->GetMetadataMember(graph.yVariable));
                if (yVal == 0 && skipZeroData) {
                    delete run;
                    continue;
                }

                // Populating map/vector data
                if (plot.xVariable == "timestamp") {
                    Double_t startTimeStamp = run->GetStartTimestamp();
                    Double_t endTimeStamp = run->GetEndTimestamp();

                    Double_t meanTime = (endTimeStamp + startTimeStamp) / 2.;
                    xData.push_back(meanTime);
                    yData.push_back(yVal);
                } else {
                    Double_t xVal = StringToDouble(run->GetMetadataMember(plot.xVariable));
                    if (dataMap.find(xVal) == dataMap.end()) {
                        dataMap[xVal] = yVal;
                        dataMapN[xVal] = 1;
                    } else {
                        dataMap[xVal] += yVal;
                        dataMapN[xVal]++;
                    }
                }
                delete run;
            }

            // dataMap will contain data only in case is not "timestamp"
            for (auto const& x : dataMap) {
                xData.push_back(x.first);
                yData.push_back(dataMap[x.first] / dataMapN[x.first]);
            }

            // In case of problems we output this
            if (xData.size() == 0) {
                warning << "TRestMetadataPlot: no input file matches condition for graph: " << graph.name
                        << endl;
                warning << "This graph is empty and it will not be plotted" << endl;
                plot.graphs.erase(plot.graphs.begin() + i);
                i--;
            }

            if (xData.size() == 1) {
                warning << "TRestMetadataPlot: Only 1-point for graph: " << graph.name << endl;
                warning << "X: " << xData[0] << " Y: " << yData[0] << endl;
            }

            for (int nn = 0; nn < xData.size(); nn++)
                debug << "X: " << xData[nn] << " Y: " << yData[nn] << endl;

            // adjust the graph
            TGraph* gr_temp = new TGraph(xData.size(), &xData[0], &yData[0]);
            gr_temp->SetName(graph.name.c_str());
            gr_temp->SetTitle(plot.title.c_str());

            gr_temp->GetXaxis()->SetTitle(plot.labelX.c_str());
            gr_temp->GetYaxis()->SetTitle(plot.labelY.c_str());
            // Removed to avoid Gitlab SJTU pipeline failing.
            // Probably due to ROOT version?
            // gr_temp->GetXaxis()->SetMaxDigits(4);

            gr_temp->GetXaxis()->SetLabelSize(fTicksScaleX * gr_temp->GetXaxis()->GetLabelSize());
            gr_temp->GetYaxis()->SetLabelSize(fTicksScaleY * gr_temp->GetYaxis()->GetLabelSize());
            gr_temp->GetXaxis()->SetTitleSize(fLabelScaleX * gr_temp->GetXaxis()->GetTitleSize());
            gr_temp->GetYaxis()->SetTitleSize(fLabelScaleY * gr_temp->GetYaxis()->GetTitleSize());
            gr_temp->GetXaxis()->SetTitleOffset(fLabelOffsetX * gr_temp->GetXaxis()->GetTitleOffset());
            gr_temp->GetYaxis()->SetTitleOffset(fLabelOffsetY * gr_temp->GetYaxis()->GetTitleOffset());
            // gr_temp->GetXaxis()->SetNdivisions(-5);

            gr_temp->SetLineColor(graph.lineColor);
            gr_temp->SetLineWidth(graph.lineWidth);
            gr_temp->SetLineStyle(graph.lineStyle);
            gr_temp->SetFillColor(graph.fillColor);
            gr_temp->SetFillStyle(graph.fillStyle);

            gr_temp->SetDrawOption(graph.drawOption.c_str());
            gr_temp->SetMarkerStyle(graph.markerStyle);
            gr_temp->SetMarkerSize(graph.markerSize);
            gr_temp->SetMarkerColor(graph.markerColor);

            if (plot.timeDisplay) gr_temp->GetXaxis()->SetTimeDisplay(1);

            graphCollectionPlot.push_back(gr_temp);
            graphCollectionAll.push_back(gr_temp);
        }

        if (graphCollectionPlot.size() == 0) {
            warning << "TRestMetadataPlot: pad empty for the plot: " << plot.name << endl;
            continue;
        }

        // draw to the pad
        Double_t minValue_Pad = 0;
        Double_t maxValue_Pad = 0;
        for (unsigned int i = 0; i < graphCollectionPlot.size(); i++) {
            TGraph* gr = graphCollectionPlot[i];
            // need to draw the max graph first, in order to prevent data hidden problem
            Double_t maxValue = TMath::MaxElement(gr->GetN(), gr->GetY());
            Double_t minValue = TMath::MinElement(gr->GetN(), gr->GetY());
            if (i == 0) {
                maxValue_Pad = maxValue;
                minValue_Pad = minValue;
            } else {
                if (maxValue > maxValue_Pad) maxValue_Pad = maxValue;
                if (minValue < minValue_Pad) minValue_Pad = minValue;
            }
        }

        // We add some margin to the automatically identified ranges
        if (minValue_Pad > 0)
            minValue_Pad *= 0.8;
        else
            minValue_Pad *= 1.2;

        if (maxValue_Pad > 0)
            maxValue_Pad *= 1.2;
        else
            maxValue_Pad *= 0.8;

        // If ranges have beend defined we override the automatic ranges
        if (!(plot.yRange.X() == -1 && plot.yRange.Y() == -1)) {
            minValue_Pad = plot.yRange.X();
            maxValue_Pad = plot.yRange.Y();
        }

        for (unsigned int i = 0; i < graphCollectionPlot.size(); i++) {
            // draw the remaining histo
            if (!(plot.xRange.X() == -1 && plot.xRange.Y() == -1))
                graphCollectionPlot[i]->GetXaxis()->SetLimits(plot.xRange.X(), plot.xRange.Y());
            graphCollectionPlot[i]->GetHistogram()->SetMinimum(minValue_Pad);
            graphCollectionPlot[i]->GetHistogram()->SetMaximum(maxValue_Pad);

            graphCollectionPlot[i]->Draw(plot.graphs[i].drawOption.c_str());
        }

        // save histogram to root file
        for (unsigned int i = 0; i < graphCollectionPlot.size(); i++) {
            if (fRun != NULL) {
                fOutputRootFile->cd();
                graphCollectionPlot[i]->Write();
            }
        }

        // draw legend
        if (plot.legendOn) {
            TLegend* legend = new TLegend(fLegendX1, fLegendY1, fLegendX2, fLegendY2);
            for (unsigned int i = 0; i < graphCollectionPlot.size(); i++)
                legend->AddEntry(graphCollectionPlot[i], (TString)plot.graphs[i].title, fLegendOption);
            legend->Draw("same");
        }

        // save pad
        targetPad->Update();
        if (plot.save != "") {
            SavePlotToPDF(plot.save, n + 1);
        }

        fCombinedCanvas->Update();
    }

    // Preview plot. User can make some changes before saving
    if (StringToBool(GetParameter("previewPlot", "TRUE"))) {
        GetChar();
    }

    // Save canvas to a PDF file
    TRestRun* run = new TRestRun();
    run->SetHistoricMetadataSaving(false);
    run->OpenInputFile(fRunInputFileName[0]);

    fCanvasSave = run->FormFormat(fCanvasSave);
    delete run;
    if (fCanvasSave != "") fCombinedCanvas->Print(fCanvasSave);

    // If the extension of the canvas save file is ROOT we store also the histograms
    if (TRestTools::isRootFile((string)fCanvasSave)) {
        TFile* f = new TFile(fCanvasSave, "UPDATE");
        f->cd();
        for (unsigned int n = 0; n < graphCollectionAll.size(); n++) graphCollectionAll[n]->Write();
        f->Close();
    }

    // Save this class to the root file
    if (fRun != NULL && fOutputRootFile != NULL) {
        fOutputRootFile->cd();
        this->Write();
        fRun->CloseFile();
    }
}

///////////////////////////////////////////////
/// \brief A method to execute the generation of an output file with the contents of the TCanvas as it is.
/// The name of the output file is given through the argument
///
void TRestMetadataPlot::SaveCanvasToPDF(TString fileName) { fCombinedCanvas->Print(fileName); }

///////////////////////////////////////////////
/// \brief A method to execute the generation of a plot inside the canvas using the index of the plot.
/// The name of the output file is given through the argument
///
void TRestMetadataPlot::SavePlotToPDF(TString fileName, Int_t n) {
    // gErrorIgnoreLevel = 10;
    fCombinedCanvas->SetBatch(kTRUE);

    if (n == 0) {
        fCombinedCanvas->Print(fileName);
        fCombinedCanvas->SetBatch(kFALSE);
        return;
    }

    TPad* pad = (TPad*)fCombinedCanvas->GetPad(n);

    TCanvas* c = new TCanvas(fPlots[n - 1].name.c_str(), fPlots[n - 1].name.c_str(), 800, 600);
    pad->DrawClone();

    c->Print(fileName);

    delete c;

    fCombinedCanvas->SetBatch(kFALSE);

    return;
}

///////////////////////////////////////////////
/// \brief A method to execute the generation of a particular graph from a plot inside the canvas
/// using the index of the plot, and the index of the graph.
/// The name of the output file is given through the argument
///
void TRestMetadataPlot::SaveGraphToPDF(TString fileName, Int_t nPlot, Int_t nGraph) {
    string name = fPlots[nPlot].graphs[nGraph].name;
    TGraph* graph = (TGraph*)gPad->GetPrimitive(name.c_str());

    TCanvas* c = new TCanvas(name.c_str(), name.c_str(), 800, 600);

    graph->Draw();

    c->Print(fileName);

    delete c;
    return;
}
