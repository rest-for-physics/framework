///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAnalysisPlot.cxx
///
///             june 2016    Gloria Luzón/Javier Galan
///_______________________________________________________________________________

#include "TRestAnalysisPlot.h"

#include "TRestManager.h"
#include "TRestTools.h"
using namespace std;

#include <TLegend.h>
#include <TStyle.h>

#include <ctime>

ClassImp(TRestAnalysisPlot);
//______________________________________________________________________________
TRestAnalysisPlot::TRestAnalysisPlot() { Initialize(); }

TRestAnalysisPlot::TRestAnalysisPlot(const char* cfgFileName, const char* name) : TRestMetadata(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);
}

void TRestAnalysisPlot::Initialize() {
    SetSectionName(this->ClassName());

    fRun = NULL;
    fNFiles = 0;
    fCombinedCanvas = NULL;
    fPlotNamesCheck.clear();
    fDrawNEntries = TTree::kMaxEntries;
    fDrawFirstEntry = 0;
}

//______________________________________________________________________________
TRestAnalysisPlot::~TRestAnalysisPlot() {
    if (fRun != NULL) delete fRun;
}

//______________________________________________________________________________
void TRestAnalysisPlot::InitFromConfigFile() {
    size_t position = 0;
    if (fHostmgr->GetRunInfo() != NULL) {
        fRun = fHostmgr->GetRunInfo();
    }
    if (fRun == NULL) {
        fRun = new TRestRun();
        fRun->SetHistoricMetadataSaving(false);
        string defaultFileName = "/tmp/restplot_" + REST_USER + ".root";
        string outputname = GetParameter("outputFile", defaultFileName);
        fRun->SetOutputFileName(outputname);
        fRun->FormOutputFile();
    }

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
        ferr << "TRestAnalysisPlot: No input files are added!" << endl;
        exit(1);
    }

#pragma region ReadLabels
    debug << "TRestAnalysisPlot: Reading canvas settings" << endl;
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
    }
#pragma endregion

#pragma region ReadCanvas
    position = 0;
    TiXmlElement* canvasdef = fElement->FirstChildElement("canvas");
    if (canvasdef == NULL) {
        canvasdef = fElement;
    }

    fCanvasSize = StringTo2DVector(GetParameter("size", canvasdef, "(800,600)"));
    fCanvasDivisions = StringTo2DVector(GetParameter("divide", canvasdef, "(1,1)"));
    fCanvasDivisionMargins = StringTo2DVector(GetParameter("divideMargin", canvasdef, "(0.01, 0.01)"));
    fCanvasSave = GetParameter("save", canvasdef, "/tmp/restplot.pdf");
    fPaletteStyle = StringToInteger(GetParameter("paletteStyle", canvasdef, "57"));
#pragma endregion

#pragma region ReadGlobalCuts
    debug << "TRestAnalysisPlot: Reading global cuts" << endl;
    vector<string> globalCuts;
    TiXmlElement* gCutele = fElement->FirstChildElement("globalCut");
    while (gCutele != NULL)  // general cuts
    {
        string cutActive = GetParameter("value", gCutele, "ON");

        if (ToUpper(cutActive) == "ON") {
            string obsName = GetParameter("variable", gCutele, "");
            if (obsName == "") {
                obsName = GetParameter("name", gCutele, "");
                if (obsName != "") {
                    warning << "<globalCut name=\"var\" is now obsolete." << endl;
                    warning << "Please, replace by : <globalCut variable=\"var\" " << endl;
                    cout << endl;
                }
            }

            if (obsName == "") continue;

            string cutCondition = GetParameter("condition", gCutele);
            string cutString = obsName + cutCondition;

            globalCuts.push_back(cutString);
        }

        gCutele = gCutele->NextSiblingElement("globalCut");
    }
#pragma endregion

#pragma region ReadGlobalCutStrings
    debug << "TRestAnalysisPlot: Reading global cut strings" << endl;
    TiXmlElement* gCutStrele = fElement->FirstChildElement("globalCutString");
    while (gCutStrele != NULL)  // general cuts
    {
        string cutActive = GetParameter("value", gCutStrele, "ON");

        if (ToUpper(cutActive) == "ON") {
            string cutString = GetParameter("string", gCutStrele, "");
            cutString = "(" + cutString + ")";
            if (cutString == "") continue;

            globalCuts.push_back(cutString);
        }

        gCutStrele = gCutStrele->NextSiblingElement("globalCutString");
    }
#pragma endregion

#pragma region ReadPlot
    debug << "TRestAnalysisPlot: Reading plot sections" << endl;
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
            plot.title = GetParameter("title", plotele, plot.name);
            plot.logY = StringToBool(GetParameter("logscale", plotele, "false"));
            plot.logY = plot.logY ? plot.logY : StringToBool(GetParameter("logY", plotele, "false"));
            plot.logX = StringToBool(GetParameter("logX", plotele, "false"));
            plot.logZ = StringToBool(GetParameter("logZ", plotele, "false"));
            plot.gridY = StringToBool(GetParameter("gridY", plotele, "false"));
            plot.gridX = StringToBool(GetParameter("gridX", plotele, "false"));
            plot.normalize = StringToDouble(GetParameter("norm", plotele, ""));
            plot.labelX = GetParameter("xlabel", plotele, "");
            plot.labelY = GetParameter("ylabel", plotele, "");
            plot.ticksX = StringToInteger(GetParameter("xticks", plotele, "510"));
            plot.ticksY = StringToInteger(GetParameter("yticks", plotele, "510"));
            plot.marginBottom = StringToDouble(GetParameter("marginBottom", plotele, "0.15"));
            plot.marginTop = StringToDouble(GetParameter("marginTop", plotele, "0.07"));
            plot.marginLeft = StringToDouble(GetParameter("marginLeft", plotele, "0.18"));
            plot.marginRight = StringToDouble(GetParameter("marginRight", plotele, "0.1"));
            plot.legendOn = StringToBool(GetParameter("legend", plotele, "OFF"));
            plot.staticsOn = StringToBool(GetParameter("stats", plotele, "OFF"));
            plot.annotationOn = StringToBool(GetParameter("annotation", plotele, "OFF"));
            plot.xOffset = StringToDouble(GetParameter("xOffset", plotele, "0"));
            plot.yOffset = StringToDouble(GetParameter("yOffset", plotele, "0"));
            plot.timeDisplay = StringToBool(GetParameter("timeDisplay", plotele, "OFF"));
            plot.save = RemoveWhiteSpaces(GetParameter("save", plotele, ""));

            TiXmlElement* histele = plotele->FirstChildElement("histo");
            if (histele == NULL) {
                // in case for single-hist plot, variables are added directly inside the <plot section
                histele = plotele;
            }
            while (histele != NULL) {
                Histo_Info_Set hist = SetupHistogramFromConfigFile(histele, plot);
                // add global cut
                for (unsigned int i = 0; i < globalCuts.size(); i++) {
                    if (i > 0 || hist.cutString != "") hist.cutString += " && ";
                    if (GetVerboseLevel() >= REST_Debug)
                        cout << "Adding global cut : " << globalCuts[i] << endl;
                    hist.cutString += globalCuts[i];
                }
                //// add "SAME" option
                // if (plot.histos.size() > 0) {
                //    hist.drawOption += " SAME";
                //}

                if (hist.plotString == "") {
                    warning << "No variables or histograms defined in the plot, skipping!" << endl;
                } else {
                    plot.histos.push_back(hist);
                }

                if (histele == plotele) {
                    break;
                }
                histele = histele->NextSiblingElement("histo");
            }

            fPlots.push_back(plot);
            plotele = plotele->NextSiblingElement("plot");
        }
    }
#pragma endregion

#pragma region ReadPanel
    debug << "TRestAnalysisPlot: Reading panel sections" << endl;
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
}
#pragma endregion

TRestAnalysisPlot::Histo_Info_Set TRestAnalysisPlot::SetupHistogramFromConfigFile(TiXmlElement* histele,
                                                                                  Plot_Info_Set plot) {
    Histo_Info_Set hist;
    hist.name = RemoveWhiteSpaces(GetParameter("name", histele, plot.name));
    hist.drawOption = GetParameter("option", histele, "colz");

    for (int n = 0; n < fPlotNamesCheck.size(); n++)
        if (hist.name == fPlotNamesCheck[n]) {
            ferr << "Repeated plot/histo names were found! Please, use different names for different plots!"
                 << endl;
            ferr << "<plot/histo name=\"" << hist.name << "\" already defined!" << endl;
            exit(1);
        }

    fPlotNamesCheck.push_back(hist.name);

    // 1. construct plot variables for the hist
    // read variables
    vector<string> varNames;
    vector<TVector2> ranges;
    vector<Int_t> bins;
    TiXmlElement* varele = histele->FirstChildElement("variable");

    while (varele != NULL) {
        varNames.push_back(GetParameter("name", varele));

        string rangeStr = GetParameter("range", varele);
        rangeStr = Replace(rangeStr, "unixTime", std::to_string(std::time(nullptr)));
        rangeStr = Replace(rangeStr, "days", "24*3600");
        ranges.push_back(StringTo2DVector(rangeStr));

        bins.push_back(StringToInteger(GetParameter("nbins", varele)));

        varele = varele->NextSiblingElement("variable");
    }
    if (GetVerboseLevel() >= REST_Debug) {
        for (unsigned int n = 0; n < bins.size(); n++) {
            cout << "Variable " << varNames[n] << endl;
            cout << "------------------------------------------" << endl;
            cout << "Plot range : ( " << ranges.back().X() << " , " << ranges.back().Y() << " ) " << endl;
            cout << "bins : " << bins.back() << endl;
            cout << endl;
        }
    }

    string pltString = "";
    for (int i = varNames.size() - 1; i >= 0; i--) {
        // The draw branches are in reversed ordered in TTree::Draw()
        pltString += varNames[i];
        if (i > 0) pltString += ":";
    }
    hist.plotString = pltString;

    // 2. construct plot name for the hist
    string rangestr = "";
    for (int i = 0; i < bins.size(); i++) {
        string binsStr = ToString(bins[i]);
        if (bins[i] == -1) binsStr = " ";

        string rXStr = ToString(ranges[i].X());
        if (ranges[i].X() == -1) {
            rXStr = " ";
            if (varNames[i] == "timeStamp" || plot.timeDisplay) rXStr = "MIN_TIME";
        }

        string rYStr = ToString(ranges[i].Y());
        if (ranges[i].Y() == -1) {
            rYStr = " ";
            if (varNames[i] == "timeStamp" || plot.timeDisplay) rYStr = "MAX_TIME";
        }

        if (i == 0) rangestr += "(";
        rangestr += binsStr + " , " + rXStr + " , " + rYStr;
        if (i < bins.size() - 1) rangestr += ",";
        if (i == bins.size() - 1) rangestr += ")";
    }
    hist.range = rangestr;

    // 3. read cuts
    string cutString = "";

    TiXmlElement* cutele = histele->FirstChildElement("cut");
    while (cutele != NULL) {
        string cutActive = GetParameter("value", cutele, "ON");
        if (ToUpper(cutActive) == "ON") {
            string cutVariable = GetParameter("variable", cutele);
            if (cutVariable == "NO_SUCH_PARA") {
                ferr << "Variable was not found! There is a problem inside <cut definition. Check it."
                     << endl;
                cout << "Contents of entire <histo definition : " << ElementToString(histele) << endl;
                cout << endl;
            }

            string cutCondition = GetParameter("condition", cutele);
            if (cutCondition == "NO_SUCH_PARA") {
                ferr << "Condition was not found! There is a problem inside <cut definition. Check it."
                     << endl;
                cout << "Contents of entire <histo definition : " << ElementToString(histele) << endl;
                cout << endl;
            }

            if (cutString.length() > 0) cutString += " && ";
            debug << "Adding local cut : " << cutVariable << cutCondition << endl;

            cutString += cutVariable + cutCondition;
        }
        cutele = cutele->NextSiblingElement("cut");
    }

    TiXmlElement* cutstrele = histele->FirstChildElement("cutString");
    while (cutstrele != NULL) {
        string cutActive = GetParameter("value", cutstrele, "ON");
        if (ToUpper(cutActive) == "ON") {
            string cutStr = GetParameter("string", cutstrele);
            if (cutStr == "NO_SUCH_PARA") {
                ferr << "Cut string was not found! There is a problem inside <cutString definition. Check it."
                     << endl;
                cout << "Contents of entire <histo definition : " << ElementToString(histele) << endl;
                cout << endl;
            }

            if (cutString.length() > 0) cutString += " && ";
            debug << "Adding local cut : " << cutStr << endl;

            cutString += "(" + cutStr + ")";
        }
        cutstrele = cutstrele->NextSiblingElement("cutString");
    }
    hist.cutString = cutString;

    // 4. read classify condition
    hist.classifyMap.clear();
    TiXmlElement* classifyele = histele->FirstChildElement("classify");
    while (classifyele != NULL) {
        string Active = GetParameter("value", classifyele, "ON");
        if (ToUpper(Active) == "ON") {
            TiXmlAttribute* attr = classifyele->FirstAttribute();
            while (attr != NULL) {
                if (attr->Value() != NULL && string(attr->Value()) != "") {
                    hist.classifyMap[attr->Name()] = attr->Value();
                }
                attr = attr->Next();
            }
        }
        classifyele = classifyele->NextSiblingElement("classify");
    }

    // 5. read draw style(line color, width, fill style, etc.)
    hist.lineColor = StringToInteger(GetParameter("lineColor", histele, "602"));
    hist.lineWidth = StringToInteger(GetParameter("lineWidth", histele, "1"));
    hist.lineStyle = StringToInteger(GetParameter("lineStyle", histele, "1"));
    hist.fillStyle = StringToInteger(GetParameter("fillStyle", histele, "1001"));
    hist.fillColor = StringToInteger(GetParameter("fillColor", histele, "0"));

    return hist;
}

void TRestAnalysisPlot::AddFile(TString fileName) {
    debug << "TRestAnalysisPlot::AddFile. Adding file. " << endl;
    debug << "File name: " << fileName << endl;
    fRunInputFileName.push_back((string)fileName);
    fNFiles++;
}

// we can add input file from process's output file
void TRestAnalysisPlot::AddFileFromExternalRun() {
    if (fRun != NULL && fNFiles == 0) {
        if (fHostmgr->GetProcessRunner() != NULL && fRun->GetOutputFileName() != "") {
            // if we have a TRestProcessRunner before head, we use its output file
            AddFile(fRun->GetOutputFileName());
            return;
        }
    }
}

// we can add input file from parameter "inputFile"
void TRestAnalysisPlot::AddFileFromEnv() {
    if (fNFiles == 0) {
        string filepattern = GetParameter("inputFile", "");
        auto files = TRestTools::GetFilesMatchingPattern(filepattern);

        for (unsigned int n = 0; n < files.size(); n++) {
            essential << "Adding file : " << files[n] << endl;
            AddFile(files[n]);
        }
    }
}

Int_t TRestAnalysisPlot::GetPlotIndex(TString plotName) {
    for (unsigned int n = 0; n < fPlots.size(); n++)
        if (fPlots[n].name == plotName) return n;

    warning << "TRestAnalysisPlot::GetPlotIndex. Plot name " << plotName << " not found" << endl;
    return -1;
}

TRestAnalysisTree* TRestAnalysisPlot::GetTreeFromFile(TString fileName) {
    if (fileName == fRun->GetInputFileName(0)) {
        // this means the file is already opened by TRestRun
        return fRun->GetAnalysisTree();
    }
    if (fileName == fRun->GetOutputFileName() && fRun->GetOutputFile() != NULL) {
        // this means the process is finished and the chain's output file is transferred to TRestRun
        return (TRestAnalysisTree*)fRun->GetOutputFile()->Get("AnalysisTree");
    }
    if (fHostmgr != NULL && fHostmgr->GetProcessRunner() != NULL &&
        fHostmgr->GetProcessRunner()->GetTempOutputDataFile() != NULL &&
        fHostmgr->GetProcessRunner()->GetTempOutputDataFile()->GetName() == fileName) {
        // this means the process is still ongoing
        return fHostmgr->GetProcessRunner()->GetOutputAnalysisTree();
    }
    fRun->OpenInputFile(fileName);
    return fRun->GetAnalysisTree();
}

TRestRun* TRestAnalysisPlot::GetInfoFromFile(TString fileName) {
    // in any case we directly return fRun. No need to reopen the given file
    if (fileName == fRun->GetInputFileName(0)) {
        return fRun;
    }
    if (fileName == fRun->GetOutputFileName() && fRun->GetOutputFile() != NULL) {
        return fRun;
    }
    if (fHostmgr != NULL && fHostmgr->GetProcessRunner() != NULL &&
        fHostmgr->GetProcessRunner()->GetTempOutputDataFile() != NULL &&
        fHostmgr->GetProcessRunner()->GetTempOutputDataFile()->GetName() == fileName) {
        return fRun;
    }
    fRun->OpenInputFile(fileName);
    return fRun;
}

void TRestAnalysisPlot::PlotCombinedCanvas() {
    // Initializing canvas window
    if (fCombinedCanvas != NULL) {
        delete fCombinedCanvas;
        fCombinedCanvas = NULL;
    }
    fCombinedCanvas = new TCanvas(this->GetName(), this->GetName(), 0, 0, fCanvasSize.X(), fCanvasSize.Y());
    fCombinedCanvas->Divide((Int_t)fCanvasDivisions.X(), (Int_t)fCanvasDivisions.Y(),
                            fCanvasDivisionMargins.X(), fCanvasDivisionMargins.Y());

    // Setting up TStyle
    TStyle* st = new TStyle();
    st->SetPalette(fPaletteStyle);

    Double_t startTime = 0;
    Double_t endTime = 0;
    Double_t runLength = 0;
    Int_t totalEntries = 0;
    for (unsigned int n = 0; n < fRunInputFileName.size(); n++) {
        auto run = GetInfoFromFile(fRunInputFileName[n]);

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
    }

    Double_t meanRate = totalEntries / runLength;

    runLength /= 3600.;

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

            auto run = GetInfoFromFile(fRunInputFileName[0]);
            label = run->ReplaceMetadataMembers(label);

            TLatex* texxt = new TLatex(fPanels[n].posX[m], fPanels[n].posY[m], label.c_str());
            texxt->SetTextColor(1);
            texxt->SetTextSize(fPanels[n].font_size);
            texxt->Draw("same");
        }
    }

    // start drawing plots
    vector<TH3F*> histCollectionAll;
    for (unsigned int n = 0; n < fPlots.size(); n++) {
        Plot_Info_Set& plot = fPlots[n];

        TPad* targetPad = (TPad*)fCombinedCanvas->cd(n + 1 + fPanels.size());
        targetPad->SetLogx(plot.logX);
        targetPad->SetLogy(plot.logY);
        targetPad->SetLogz(plot.logZ);
        targetPad->SetGridx(plot.gridX);
        targetPad->SetGridy(plot.gridY);
        targetPad->SetLeftMargin(plot.marginLeft);
        targetPad->SetRightMargin(plot.marginRight);
        targetPad->SetBottomMargin(plot.marginBottom);
        targetPad->SetTopMargin(plot.marginTop);

        // draw each histogram in the pad
        for (unsigned int i = 0; i < plot.histos.size(); i++) {
            Histo_Info_Set& hist = plot.histos[i];

            TString plotString = hist.plotString;
            TString nameString = hist.name;
            TString rangeString = hist.range;
            TString cutString = hist.cutString;
            TString optString = hist.drawOption;

            size_t pos = 0;
            rangeString = Replace((string)rangeString, "MIN_TIME", (string)Form("%9f", startTime), pos);
            rangeString = Replace((string)rangeString, "MAX_TIME", (string)Form("%9f", endTime), pos);

            if (GetVerboseLevel() >= REST_Debug) {
                cout << endl;
                cout << "--------------------------------------" << endl;
                cout << "Plot string : " << plotString << endl;
                cout << "Plot name : " << nameString << endl;
                cout << "Plot range : " << rangeString << endl;
                cout << "Cut : " << cutString << endl;
                cout << "Plot option : " << optString << endl;
                cout << "++++++++++++++++++++++++++++++++++++++" << endl;
            }

            // draw single histo from different file
            bool firstdraw = false;
            TH3F* hTotal = hist.ptr;
            for (unsigned int j = 0; j < fRunInputFileName.size(); j++) {
                auto run = GetInfoFromFile(fRunInputFileName[j]);
                // apply "classify" condition
                bool flag = true;
                auto iter = hist.classifyMap.begin();
                while (iter != hist.classifyMap.end()) {
                    if (run->GetInfo(iter->first) != iter->second) {
                        flag = false;
                        break;
                    }
                    iter++;
                }
                if (!flag) continue;

                TTree* tree = GetTreeFromFile(fRunInputFileName[j]);

                // call Draw() from analysis tree
                int outVal;
                TString reducedHistoName = nameString + "_" + std::to_string(j);
                TString histoName = nameString + "_" + std::to_string(j) + rangeString;
                info << "AnalysisTree->Draw(\"" << plotString << ">>" << histoName << "\", \"" << cutString
                     << "\", \"" << optString << "\", " << fDrawNEntries << ", " << fDrawFirstEntry << ")"
                     << endl;
                outVal = tree->Draw(plotString + ">>" + histoName, cutString, optString, fDrawNEntries,
                                    fDrawFirstEntry);
                TH3F* hh = (TH3F*)gPad->GetPrimitive(reducedHistoName);
                if (outVal == 0) {
                    info << "File: " << fRunInputFileName[j] << ": No entries are drawn" << endl;
                    info << "AnalysisTree is empty? cut is too hard?" << endl;
                } else if (outVal == -1) {
                    ferr << endl;
                    ferr << "TRestAnalysisPlot::PlotCombinedCanvas. Plot string not properly constructed. "
                            "Does the analysis observable exist inside the file?"
                         << endl;
                    ferr << "Use \" restManager PrintTrees FILE.ROOT\" to get a list of "
                            "existing observables."
                         << endl;
                    ferr << endl;
                    exit(1);
                }

                // add to histogram
                if (hTotal == NULL) {
                    if (hh != NULL) {
                        hTotal = (TH3F*)hh->Clone(nameString);
                        hist.ptr = hTotal;
                        firstdraw = true;
                        // This is important so that the histogram is not erased when we delete TRestRun!
                        hTotal->SetDirectory(0);
                    }
                } else {
                    if (outVal > 0) {
                        hTotal->Add(hh);
                    }
                }
            }

            if (hTotal == NULL) {
                warning << "Histogram \"" << nameString << "\" is NULL" << endl;
            } else if (firstdraw) {
                // adjust the histogram
                hTotal->SetTitle(plot.title.c_str());
                hTotal->SetStats(plot.staticsOn);

                hTotal->GetXaxis()->SetTitle(plot.labelX.c_str());
                hTotal->GetYaxis()->SetTitle(plot.labelY.c_str());

                hTotal->GetXaxis()->SetLabelSize(fTicksScaleX * hTotal->GetXaxis()->GetLabelSize());
                hTotal->GetYaxis()->SetLabelSize(fTicksScaleY * hTotal->GetYaxis()->GetLabelSize());
                hTotal->GetXaxis()->SetTitleSize(fLabelScaleX * hTotal->GetXaxis()->GetTitleSize());
                hTotal->GetYaxis()->SetTitleSize(fLabelScaleY * hTotal->GetYaxis()->GetTitleSize());
                hTotal->GetXaxis()->SetTitleOffset(fLabelOffsetX * hTotal->GetXaxis()->GetTitleOffset());
                hTotal->GetYaxis()->SetTitleOffset(fLabelOffsetY * hTotal->GetYaxis()->GetTitleOffset());
                hTotal->GetXaxis()->SetNdivisions(plot.ticksX);
                hTotal->GetYaxis()->SetNdivisions(plot.ticksY);

                hTotal->SetLineColor(hist.lineColor);
                hTotal->SetLineWidth(hist.lineWidth);
                hTotal->SetLineStyle(hist.lineStyle);
                hTotal->SetFillColor(hist.fillColor);
                hTotal->SetFillStyle(hist.fillStyle);

                hTotal->SetDrawOption(hist.drawOption.c_str());

                if (plot.timeDisplay) hTotal->GetXaxis()->SetTimeDisplay(1);

                histCollectionAll.push_back(hTotal);
            }
        }

        bool allempty = true;
        for (unsigned int i = 0; i < plot.histos.size(); i++) {
            if (plot.histos[i].ptr == NULL)
                continue;
            else {
                allempty = false;
                break;
            }
        }
        if (allempty) {
            warning << "TRestAnalysisPlot: pad empty for the plot: " << plot.name << endl;
            continue;
        }

        // scale the histograms
        if (plot.normalize > 0) {
            for (unsigned int i = 0; i < plot.histos.size(); i++) {
                if (plot.histos[i].ptr == NULL) continue;
                Double_t scale = 1.;
                if (plot.histos[i].ptr->Integral() > 0) {
                    scale = plot.normalize / plot.histos[i].ptr->Integral();
                    plot.histos[i].ptr->Scale(scale);
                }
            }
        }

        // draw to the pad
        targetPad = (TPad*)fCombinedCanvas->cd(n + 1 + fPanels.size());
        Double_t maxValue_Pad = 0;
        int maxID = 0;
        for (unsigned int i = 0; i < plot.histos.size(); i++) {
            // need to draw the max histogram first, in order to prevent peak hidden problem
            if (plot.histos[i].ptr == NULL) continue;
            Double_t value = plot.histos[i]->GetBinContent(plot.histos[i].ptr->GetMaximumBin());
            if (i == 0) {
                maxValue_Pad = value;
            } else if (value > maxValue_Pad) {
                maxValue_Pad = value;
                maxID = i;
            }
        }
        plot.histos[maxID]->Draw(plot.histos[maxID].drawOption.c_str());
        if (((string)plot.histos[maxID]->ClassName()).find("TH1") != -1) {
            plot.histos[maxID]->GetYaxis()->SetRangeUser(plot.logY, maxValue_Pad * 1.2);
        }

        for (unsigned int i = 0; i < plot.histos.size(); i++) {
            // draw the remaining histo
            if (plot.histos[i].ptr == NULL) continue;
            if (i != maxID) {
                plot.histos[i]->Draw((plot.histos[maxID].drawOption + "same").c_str());
            }
        }

        // save histogram to root file
        if (fRun->GetOutputFile() != NULL) {
            fRun->GetOutputFile()->cd();
            for (unsigned int i = 0; i < plot.histos.size(); i++) {
                if (plot.histos[i].ptr == NULL) continue;
                plot.histos[i]->Write();
            }
        }

        // draw legend
        if (plot.legendOn) {
            TLegend* legend = new TLegend(fLegendX1, fLegendY1, fLegendX2, fLegendY2);
            for (unsigned int i = 0; i < plot.histos.size(); i++) {
                if (plot.histos[i].ptr == NULL) continue;
                legend->AddEntry(plot.histos[i].ptr, plot.histos[i]->GetName(), "lf");
            }
            legend->Draw("same");
        }

        // save pad
        targetPad->Update();
        if (plot.save != "") targetPad->Print(plot.save.c_str());

        fCombinedCanvas->Update();
    }

    // Preview plot. User can make some changed before saving
    if (StringToBool(GetParameter("previewPlot", "TRUE"))) {
        GetChar();
    }

    // Save canvas to a PDF file
    fCanvasSave = fRun->FormFormat(fCanvasSave);
    if (fCanvasSave != "") fCombinedCanvas->Print(fCanvasSave);

    // If the extension of the canvas save file is ROOT we store also the histograms
    if (TRestTools::isRootFile((string)fCanvasSave)) {
        TRestRun* run = new TRestRun();
        run->SetOutputFileName((string)fCanvasSave);
        run->FormOutputFile();
        for (unsigned int n = 0; n < histCollectionAll.size(); n++) {
            histCollectionAll[n]->Write();
        }
        delete run;
    }

    // Save this class to the root file
    if (fRun->GetOutputFile() != NULL) {
        fRun->GetOutputFile()->cd();
        this->Write();
    }

    delete st;
}

void TRestAnalysisPlot::SaveCanvasToPDF(TString fileName) { fCombinedCanvas->Print(fileName); }

void TRestAnalysisPlot::SavePlotToPDF(TString fileName, Int_t n) {
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

void TRestAnalysisPlot::SaveHistoToPDF(TString fileName, Int_t nPlot, Int_t nHisto) {
    string name = fPlots[nPlot].histos[nHisto].name;
    TH3F* hist = (TH3F*)gPad->GetPrimitive(name.c_str());

    TCanvas* c = new TCanvas(name.c_str(), name.c_str(), 800, 600);

    hist->Draw();

    c->Print(fileName);

    delete c;
    return;
}

//
// void TRestAnalysisPlot::SaveHistoToPDF(TH1D* h, Int_t n, TString fileName) {
//    gErrorIgnoreLevel = 10;
//
//    TCanvas* c = new TCanvas(h->GetName(), h->GetTitle(), 800, 600);
//
//    h->Draw("colz");
//
//    h->SetTitle(fHistoTitle[n]);
//    h->GetXaxis()->SetTitle(fHistoXLabel[n]);
//    h->GetYaxis()->SetTitle(fHistoYLabel[n]);
//
//    h->Draw("colz");
//    c->Print(fileName);
//
//    delete c;
//}
