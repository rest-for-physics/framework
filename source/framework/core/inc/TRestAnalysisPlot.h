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
#include <TRestRun.h>

#include "TCanvas.h"
#include "TH3D.h"
#include "TRestAnalysisTree.h"

const int REST_MAX_TAGS = 15;

// as enum "EColor" defined in Rtypes.h
// as enum "ELineStyle" defined in TAttLine.h
// as enum "EFillStyle" defined in TAttFill.h
const map<string, int> ColorIdMap{
    {"white", kWhite},   {"black", kBlack},   {"gray", kGray},       {"red", kRed},       {"green", kGreen},
    {"blue", kBlue},     {"yellow", kYellow}, {"magenta", kMagenta}, {"cyan", kCyan},     {"orange", kOrange},
    {"spring", kSpring}, {"teal", kTeal},     {"azure", kAzure},     {"violet", kViolet}, {"pink", kPink}};
const map<string, int> LineStyleMap{
    {"solid", kSolid}, {"dashed", kDashed}, {"dotted", kDotted}, {"dashDotted", kDashDotted}};
const map<string, int> FillStyleMap{
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

        TH3F* ptr = nullptr;  //!
        TH3F* operator->() { return ptr; }
    };

    struct Plot_Info_Set {
        string name;
        string title;

        Double_t normalize;
        string scale;
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

    TRestAnalysisTree* GetTree(TString fileName);
    TRestRun* GetRunInfo(TString fileName);
    bool IsDynamicRange(TString rangeString);
    Int_t GetColorIDFromString(string in);
    Int_t GetFillStyleIDFromString(string in);
    Int_t GetLineStyleIDFromString(string in);

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
