///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAnalysisPlot.h
///
///             june 2016    Gloria Luzón
///_______________________________________________________________________________

#ifndef RestCore_TRestAnalysisPlot
#define RestCore_TRestAnalysisPlot

#include <TRestRun.h>
#include "TCanvas.h"
#include "TH3D.h"
#include "TRestAnalysisTree.h"

const int REST_MAX_TAGS = 6;

class TRestAnalysisPlot : public TRestMetadata {
   private:
    void InitFromConfigFile();

    Int_t GetRunTagIndex(TString tag);

    Int_t fNFiles;
    std::vector<TString> fFileNames[REST_MAX_TAGS];

    TString fClasifyBy;

    std::vector<TString> fLegendName;

#ifndef __CINT__
    TRestRun* fRun;

    TCanvas* fCombinedCanvas;

    TString fPlotMode;
    TString fHistoOutputFile;
    TVector2 fCanvasSize;
    TVector2 fCanvasDivisions;
    TString fCanvasSave;
    std::vector<TString> fPlotNames;
    std::vector<TString> fHistoNames;
    std::vector<TString> fHistoXLabel;
    std::vector<TString> fHistoYLabel;
    std::vector<TString> fHistoTitle;
    std::vector<TString> fHistoSaveToFile;
    std::vector<TString> fPlotSaveToFile;
    std::vector<TString> fPlotXLabel;
    std::vector<TString> fPlotYLabel;
    std::vector<Bool_t> fLogScale;
    std::vector<Double_t> fNormalize;
    std::vector<TString> fPlotTitle;
    std::vector<TString> fPlotOption;
    std::vector<TString> fPlotString;
    std::vector<TString> fCutString;

    std::vector<Int_t> fLineColor;
    std::vector<Int_t> fLineWidth;
    std::vector<Int_t> fLineStyle;
    std::vector<Int_t> fFillColor;
    std::vector<Int_t> fFillStyle;

    std::vector<TVector2> fYRangeUser;

    Double_t fLabelOffsetX;
    Double_t fLabelOffsetY;

    Double_t fLabelScaleX;
    Double_t fLabelScaleY;

    Double_t fTicksScaleX;
    Double_t fTicksScaleY;

    Double_t fLegendX1;
    Double_t fLegendY1;
    Double_t fLegendX2;
    Double_t fLegendY2;

    Double_t fStartTime;
    Double_t fEndTime;

    std::vector<Bool_t> fStats;
    std::vector<Bool_t> fLegend;

#endif

    TString ReplaceFilenameTags(TString filename, TRestRun* run);
    void AddMissingStyles();
    void AddFileFromExternalRun();
    void AddFileFromEnv();

   protected:
   public:
    void Initialize();

    void PrintMetadata() {}

    void SetOutputPlotsFilename(TString fname) { fCanvasSave = fname; }
    void SetOutputHistosFilename(TString fname) { fHistoOutputFile = fname; }

    void AddFile(TString fileName);

    void PrintFiles() {
        cout << "++++++++++++++++++++++++++++++++++++++" << endl;
        cout << "Relation of files included in the plot" << endl;
        cout << "++++++++++++++++++++++++++++++++++++++" << endl;
        for (unsigned int i = 0; i < fLegendName.size(); i++) {
            cout << "Legend : " << fLegendName[i] << endl;
            cout << "---------------------------" << endl;
            for (unsigned int n = 0; n < fFileNames[i].size(); n++) {
                cout << fFileNames[i][n] << endl;
            }
        }

        cout << "++++++++++++++++++++++++++++++++++++++" << endl;
    }

    void SavePlotToPDF(TString plotName, TString fileName);
    void SavePlotToPDF(Int_t n, TString fileName);
    void SaveHistoToPDF(TH1D* h, Int_t n, TString fileName);

    Int_t GetPlotIndex(TString plotName);

    void PlotCombinedCanvas();

    // Construtor
    TRestAnalysisPlot();
    TRestAnalysisPlot(const char* cfgFileName, const char* name = "");
    // Destructor
    virtual ~TRestAnalysisPlot();

    ClassDef(TRestAnalysisPlot, 1);
};
#endif
