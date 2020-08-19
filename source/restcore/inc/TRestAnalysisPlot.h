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

#include <TLatex.h>
#include "TCanvas.h"
#include "TH3D.h"

#include <TRestRun.h>
#include "TRestAnalysisTree.h"

const int REST_MAX_TAGS = 15;

class TRestAnalysisPlot : public TRestMetadata {
   public:
    struct Histo_Info_Set {
        string name;   // will be shown in the legend
        string range;  // output histo string for TTree::Draw(), e.g. name+range = htemp(100,0,1000)
        Bool_t status;

        string plotString;                // draw string for TTree::Draw()
        string cutString;                 // cut string for TTree::Draw()
        map<string, string> classifyMap;  // select the input files to draw the histogram, if their
                                          // TRestRun::Get() returns the assumed string
        string drawOption;                // draw option for TTree::Draw()

        Int_t lineColor;
        Int_t lineWidth;
        Int_t lineStyle;
        Int_t fillColor;
        Int_t fillStyle;

        TH3F* ptr = NULL;  //!
        TH3F* operator->() { return ptr; }
    };

    struct Plot_Info_Set {
        string name;
        string title;

        Double_t normalize;
        Bool_t logX;
        Bool_t logY;
        Bool_t logZ;
        Bool_t gridX;
        Bool_t gridY;
        Bool_t staticsOn;
        Bool_t legendOn;
        Bool_t annotationOn;

        Double_t xOffset;
        Double_t yOffset;

        Bool_t timeDisplay;

        string labelX;
        string labelY;

        TVector2 rangeX;  // absolute x range(e.g. 1000~3000 keV), same as SetRangeUser()
        TVector2 rangeY;  // absolute y range(e.g. 1000~3000 keV), same as SetRangeUser()

        Double_t marginLeft;
        Double_t marginRight;
        Double_t marginTop;
        Double_t marginBottom;

        Int_t ticksX;
        Int_t ticksY;

        string save;

        vector<Histo_Info_Set> histos;
    };

    struct Panel_Info {
        Float_t font_size;

        vector<Float_t> posX;
        vector<Float_t> posY;

        vector<string> label;
    };

   private:
    void InitFromConfigFile() override;
    Histo_Info_Set SetupHistogramFromConfigFile(TiXmlElement* ele, Plot_Info_Set info);

    Int_t fNFiles;
    // canvas option
    TVector2 fCanvasSize;
    TVector2 fCanvasDivisions;
    TVector2 fCanvasDivisionMargins;
    TString fCanvasSave;
    Int_t fPaletteStyle;

    Int_t fLabelFont;
    Double_t fLabelOffsetX = 1.1;
    Double_t fLabelOffsetY = 1.3;
    Double_t fLabelScaleX = 1.2;
    Double_t fLabelScaleY = 1.3;
    Double_t fTicksScaleX = 1.5;
    Double_t fTicksScaleY = 1.5;

    Double_t fLegendX1 = 0.7;
    Double_t fLegendY1 = 0.75;
    Double_t fLegendX2 = 0.88;
    Double_t fLegendY2 = 0.88;

    // plots information
    vector<Plot_Info_Set> fPlots;
    vector<Panel_Info> fPanels;

    vector<string> fPlotNamesCheck;  //!

#ifndef __CINT__
    TRestRun* fRun;                          //! TRestRun to handle output file
    std::vector<TString> fRunInputFileName;  //! TRestRun to handle input file
    TCanvas* fCombinedCanvas;                //! Output canvas
    Long64_t fDrawNEntries;                  //!
    Long64_t fDrawFirstEntry;                //!
#endif


    void AddFileFromExternalRun();
    void AddFileFromEnv();

    TRestAnalysisTree* GetTreeFromFile(TString fileName);
    TRestRun* GetInfoFromFile(TString fileName);
    bool IsDynamicRange(TString rangeString);

   protected:
   public:
    void Initialize() override;

    void PrintMetadata() override {}

        void AddFile(TString fileName);
    void SetFile(TString fileName);

    void SaveCanvasToPDF(TString fileName);
    void SavePlotToPDF(TString fileName, Int_t n = 0);
    void SaveHistoToPDF(TString fileName, Int_t nPlot = 0, Int_t nHisto = 0);

    void SetOutputPlotsFilename(TString fname) { fCanvasSave = fname; }

    Int_t GetPlotIndex(TString plotName);
    TVector2 GetCanvasSize() { return fCanvasSize; }
    TVector2 GetCanvasDivisions() { return fCanvasDivisions; }

    void SetTreeEntryRange(Long64_t NEntries = TTree::kMaxEntries, Long64_t FirstEntry = 0) {
        fDrawNEntries = NEntries;
        fDrawFirstEntry = FirstEntry;
    }
    void PlotCombinedCanvas();

    // Construtor
    TRestAnalysisPlot();
    TRestAnalysisPlot(const char* cfgFileName, const char* name = "");
    // Destructor
    virtual ~TRestAnalysisPlot();

    ClassDefOverride(TRestAnalysisPlot, 4);
};
#endif
