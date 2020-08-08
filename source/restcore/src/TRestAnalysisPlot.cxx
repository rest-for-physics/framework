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
            plot.normalize = StringToDouble(GetParameter("norm", plotele, ""));
            plot.labelX = GetParameter("xlabel", plotele, "");
            plot.labelY = GetParameter("ylabel", plotele, "");
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
    hist.lineColor = StringToInteger(GetParameter("lineColor", histele));
    hist.lineWidth = StringToInteger(GetParameter("lineWidth", histele));
    hist.lineStyle = StringToInteger(GetParameter("lineStyle", histele));
    hist.fillStyle = StringToInteger(GetParameter("fillStyle", histele));
    hist.fillColor = StringToInteger(GetParameter("fillColor", histele));

    return hist;
}

void TRestAnalysisPlot::AddFile(TString fileName) {
    debug << "TRestAnalysisPlot::AddFile. Adding file. " << endl;
    debug << "File name: " << fileName << endl;

    TRestRun* run = new TRestRun();
    run->SetHistoricMetadataSaving(false);
    run->OpenInputFile((string)fileName);
    if (run->GetAnalysisTree() != NULL) {
        fRunInputFileName.push_back((string)fileName);
        fNFiles++;
    }
    delete run;

    // TFile* f = new TFile(fileName);
    // TIter nextkey(f->GetListOfKeys());
    // TKey* key;
    // TString rTag = "notFound";
    // while ((key = (TKey*)nextkey())) {
    //    string kName = key->GetClassName();
    //    if (kName == "TRestRun") {
    //        rTag = ((TRestRun*)f->Get(key->GetName()))->GetRunTag();
    //        break;
    //    }
    //}
    // f->Close();

    // if (fClasifyBy == "runTag") {
    //    debug << "TRestAnalysisPlot::AddFile. Calling GetRunTagIndex. Tag = " << rTag << endl;
    //    Int_t index = GetRunTagIndex(rTag);
    //    debug << "Index. = " << index << endl;

    //    if (index < REST_MAX_TAGS) {
    //        fFileNames[index].push_back(fileName);
    //        fNFiles++;
    //    } else {
    //        ferr << "TRestAnalysisPlot::AddFile. Maximum number of tags per plot is : " << REST_MAX_TAGS
    //             << endl;
    //    }
    //} else if (fClasifyBy == "combineAll") {
    //    fFileNames[0].push_back(fileName);
    //    fNFiles++;
    //} else {
    //    warning << "TRestAnalysisPlot : fClassifyBy not recognized" << endl;

    //    fFileNames[0].push_back(fileName);
    //    fNFiles++;
    //}
}

// we can add input file from process's output file
void TRestAnalysisPlot::AddFileFromExternalRun() {
    if (fRun != NULL && fNFiles == 0) {
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

void TRestAnalysisPlot::PlotCombinedCanvas() {
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
        ferr << "TRestAnalysisPlot: No input files are added!" << endl;
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

    Double_t startTime = 0;
    Double_t endTime = 0;
    Double_t runLength = 0;
    Int_t totalEntries = 0;
    for (unsigned int n = 0; n < fRunInputFileName.size(); n++) {
        TRestRun* run = new TRestRun();
        run->SetHistoricMetadataSaving(false);
        run->OpenInputFile(fRunInputFileName[n]);

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

            TRestRun* run = new TRestRun();
            run->SetHistoricMetadataSaving(false);
            run->OpenInputFile(fRunInputFileName[0]);
            label = run->ReplaceMetadataMembers(label);
            delete run;

            TLatex* texxt = new TLatex(fPanels[n].posX[m], fPanels[n].posY[m], label.c_str());
            texxt->SetTextColor(1);
            texxt->SetTextSize(fPanels[n].font_size);
            texxt->Draw("same");
        }
    }

    // start drawing plots
    vector<TH3F*> histCollectionAll;
    for (unsigned int n = 0; n < fPlots.size(); n++) {
        Plot_Info_Set plot = fPlots[n];

        TPad* targetPad = (TPad*)fCombinedCanvas->cd(n + 1 + fPanels.size());
        targetPad->SetLogy(plot.logY);
        targetPad->SetLogz(plot.logZ);
        targetPad->SetLeftMargin(0.18);
        targetPad->SetRightMargin(0.1);
        targetPad->SetBottomMargin(0.15);
        targetPad->SetTopMargin(0.07);

        // draw to a new histogram
        vector<TH3F*> histCollectionPlot;
        for (unsigned int i = 0; i < plot.histos.size(); i++) {
            Histo_Info_Set hist = plot.histos[i];

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
            bool drawn = false;

            TH3F* hTotal;
            for (unsigned int j = 0; j < fRunInputFileName.size(); j++) {
                TRestRun* run = new TRestRun();
                run->SetHistoricMetadataSaving(false);
                run->OpenInputFile(fRunInputFileName[j]);
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

                TTree* tree = run->GetAnalysisTree();
                int outVal;

                TString histoName = nameString + "_" + std::to_string(j) + rangeString;
                if (!drawn) {
                    outVal = tree->Draw(plotString + ">>" + histoName, cutString, optString);
                    drawn = true;
                    hTotal = (TH3F*)gPad->GetPrimitive(nameString + "_" + j)->Clone(nameString);

                    // This is important so that the histogram is not erased when we delete TRestRun!
                    hTotal->SetDirectory(0);
                } else {
                    outVal = tree->Draw(plotString + ">>" + histoName, cutString, optString);
                    TString reducedHistoName = nameString + "_" + std::to_string(j);
                    TH3F* hh = (TH3F*)gPad->GetPrimitive(reducedHistoName);

                    hTotal->Add(hh);
                }

                if (outVal == -1) {
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
                delete run;
            }

            if (drawn == false) {
                warning << "TRestAnalysisPlot: no input file matches condition for histogram: " << hist.name
                        << ", this histogram is empty" << endl;
                plot.histos.erase(plot.histos.begin() + i);
                i--;
            } else {
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
                hTotal->GetXaxis()->SetNdivisions(-5);

                hTotal->SetLineColor(hist.lineColor);
                hTotal->SetLineWidth(hist.lineWidth);
                hTotal->SetLineStyle(hist.lineStyle);
                hTotal->SetFillColor(hist.fillColor);
                hTotal->SetFillStyle(hist.fillStyle);

                hTotal->SetDrawOption(hist.drawOption.c_str());

                if (plot.timeDisplay) hTotal->GetXaxis()->SetTimeDisplay(1);

                histCollectionPlot.push_back(hTotal);
                histCollectionAll.push_back(hTotal);
            }
        }

        if (histCollectionPlot.size() == 0) {
            warning << "TRestAnalysisPlot: pad empty for the plot: " << plot.name << endl;
            continue;
        }

        // scale the histograms
        if (plot.normalize > 0) {
            for (unsigned int i = 0; i < histCollectionPlot.size(); i++) {
                Double_t scale = 1.;
                if (histCollectionPlot[i]->Integral() > 0) {
                    scale = plot.normalize / histCollectionPlot[i]->Integral();
                    histCollectionPlot[i]->Scale(scale);
                }
            }
        }

        // draw to the pad
        Double_t maxValue_Pad = 0;
        int maxID = 0;
        for (unsigned int i = 0; i < histCollectionPlot.size(); i++) {
            // need to draw the max histogram first, in order to prevent peak hidden problem
            Double_t value = histCollectionPlot[i]->GetBinContent(histCollectionPlot[i]->GetMaximumBin());
            if (i == 0) {
                maxValue_Pad = value;
            } else if (value > maxValue_Pad) {
                maxValue_Pad = value;
                maxID = i;
            }
        }
        histCollectionPlot[maxID]->Draw(plot.histos[maxID].drawOption.c_str());
        if (((string)histCollectionPlot[maxID]->ClassName()).find("TH1") != -1) {
            histCollectionPlot[maxID]->GetYaxis()->SetRangeUser(plot.logY, maxValue_Pad * 1.2);
        }

        for (unsigned int i = 0; i < histCollectionPlot.size(); i++) {
            // draw the remaining histo
            if (i != maxID) {
                histCollectionPlot[i]->Draw((plot.histos[maxID].drawOption + "same").c_str());
            }
        }

        // save histogram to root file
        for (unsigned int i = 0; i < histCollectionPlot.size(); i++) {
            if (fRun != NULL) {
                fOutputRootFile->cd();
                histCollectionPlot[i]->Write();
            }
        }

        // draw legend
        if (plot.legendOn) {
            TLegend* legend = new TLegend(fLegendX1, fLegendY1, fLegendX2, fLegendY2);
            for (unsigned int i = 0; i < histCollectionPlot.size(); i++) {
                legend->AddEntry(histCollectionPlot[i], histCollectionPlot[i]->GetName(), "lf");
            }
            legend->Draw("same");
        }

        // draw annotation, mainly the cut string
        vector<pair<double, double>> plotted_text_Y;
        if (plot.annotationOn) {
            for (int i = 0; i < histCollectionPlot.size(); i++) {
                // place annotation only for 1D histograms
                TH3F* htemp = histCollectionPlot[i];
                if (((string)htemp->ClassName()).find("TH1") != -1) {
                    // annotation for the cut, it will be like: sAna_NumberOfGoodSignals>30 && ...
                    string cutannotation = plot.histos[i].cutString;
                    // annotation for file classifying, it will be like: FileName: run123.root
                    string classifyannotation = "";
                    auto iter = plot.histos[i].classifyMap.begin();
                    while (iter != plot.histos[i].classifyMap.end()) {
                        classifyannotation += iter->first + ": " + iter->second;
                        iter++;
                        if (iter != plot.histos[i].classifyMap.end()) classifyannotation += ", ";
                    }

                    string annotation = cutannotation != "" ? cutannotation : classifyannotation;
                    if (annotation == "") annotation = "no cut";

                    // calculate x start position of the annotation, according to the max bin of the histogram
                    int maxbin = htemp->GetMaximumBin();
                    int Nbins = htemp->GetNbinsX();
                    double maxval_hist = htemp->GetBinContent(maxbin);

                    double size = 0.04;
                    double x_pos_relative = plot.logX ? log(maxbin) / log(Nbins) : maxbin / (double)Nbins;
                    double y_pos_relative = plot.logY ? log(maxval_hist) / log(maxValue_Pad * 1.2)
                                                      : maxval_hist / (maxValue_Pad * 1.2);

                    int align = 0;
                    int nCharacterPreLine;
                    if (x_pos_relative > 0.5) {
                        align = kHAlignRight + kVAlignCenter;
                        x_pos_relative -= 0.01;
                        nCharacterPreLine =
                            x_pos_relative * fCanvasSize.X() / fCanvasDivisions.X() / size / 400;
                    } else {
                        align = kHAlignLeft + kVAlignCenter;
                        x_pos_relative += 0.01;
                        nCharacterPreLine =
                            (1 - x_pos_relative) * fCanvasSize.X() / fCanvasDivisions.X() / size / 400;
                    }

                    double xpos = plot.logX ? htemp->GetBinCenter(exp(x_pos_relative * log(Nbins)))
                                            : x_pos_relative * htemp->GetBinCenter(Nbins);

                    // split the long annotation
                    vector<string> annotation_multi_text;
                    for (int j = 0; j < annotation.size(); j += nCharacterPreLine) {
                        annotation_multi_text.push_back(annotation.substr(j, nCharacterPreLine));
                    }

                    // adjust y position to avoid overlap
                    double yup = y_pos_relative + size;
                    double ydown = yup - size * annotation_multi_text.size();

                    double shift_value;
                    for (double shift = 0; shift < 0.5; shift += size) {
                        bool overlap = false;
                        shift_value = yup > 0.5 ? -shift : shift;
                        for (int j = 0; j < plotted_text_Y.size(); j++) {
                            if (yup + shift > plotted_text_Y[j].second &&
                                yup + shift < plotted_text_Y[j].first) {
                                overlap = true;
                                break;
                            }
                            if (ydown + shift > plotted_text_Y[j].second &&
                                ydown + shift < plotted_text_Y[j].first) {
                                overlap = true;
                                break;
                            }
                            if (ydown + shift == plotted_text_Y[j].second &&
                                yup + shift == plotted_text_Y[j].first) {
                                overlap = true;
                                break;
                            }
                        }
                        if (!overlap) {
                            break;
                        }
                    }
                    yup += shift_value;
                    ydown += shift_value;
                    plotted_text_Y.push_back({yup, ydown});

                    // draw
                    for (int j = 0; j < annotation_multi_text.size(); j++) {
                        double yy = plot.logY ? exp((yup - (j + 1) * size) * log(maxValue_Pad * 1.2))
                                              : (yup - (j + 1) * size) * maxValue_Pad * 1.2;

                        TLatex* text = new TLatex(xpos + plot.xOffset, yy + plot.yOffset,
                                                  annotation_multi_text[j].c_str());
                        text->SetTextAlign(align);
                        text->SetTextSize(size);
                        text->SetTextColor(plot.histos[i].lineColor);
                        text->Draw("same");
                    }
                }
            }
        }

        // save pad
        targetPad->Update();
        if (plot.save != "") SavePlotToPDF(plot.save, n + 1);

        fCombinedCanvas->Update();
    }

    // Preview plot. User can make some changed before saving
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
        for (unsigned int n = 0; n < histCollectionAll.size(); n++) histCollectionAll[n]->Write();
        f->Close();
    }

    // Save this class to the root file
    if (fRun != NULL && fOutputRootFile != NULL) {
        fOutputRootFile->cd();
        this->Write();
        fRun->CloseFile();
    }
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
