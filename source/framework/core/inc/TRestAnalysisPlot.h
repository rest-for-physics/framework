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
const std::map<std::string, int> ColorIdMap{
    {"white", kWhite},   {"black", kBlack},   {"gray", kGray},       {"red", kRed},       {"green", kGreen},
    {"blue", kBlue},     {"yellow", kYellow}, {"magenta", kMagenta}, {"cyan", kCyan},     {"orange", kOrange},
    {"spring", kSpring}, {"teal", kTeal},     {"azure", kAzure},     {"violet", kViolet}, {"pink", kPink}};
const std::map<std::string, int> LineStyleMap{
    {"solid", kSolid}, {"dashed", kDashed}, {"dotted", kDotted}, {"dashDotted", kDashDotted}};
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

class TRestAnalysisPlot : public TRestMetadata {
   public:
    struct HistoInfoSet {
        std::string name;   // will be shown in the legend
        std::string range;  // output histo std::string for TTree::Draw(), e.g. name+range = htemp(100,0,1000)
        Bool_t status;

        std::string plotString;  // draw std::string for TTree::Draw()
        std::string cutString;   // cut std::string for TTree::Draw()
        std::string weight;
        std::map<std::string, std::string>
            classifyMap;         // select the input files to draw the histogram, if their
                                 // TRestRun::Get() returns the assumed std::string
        std::string drawOption;  // draw option for TTree::Draw()

        Int_t lineColor;
        Int_t lineWidth;
        Int_t lineStyle;
        Int_t fillColor;
        Int_t fillStyle;

        TH3F* ptr = nullptr;  //!
        TH3F* operator->() { return ptr; }
    };

    struct PlotInfoSet {
        std::string name;
        std::string title;

        Double_t normalize;
        std::string scale;
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

        std::string labelX;
        std::string labelY;

        TVector2 rangeX;  // absolute x range(e.g. 1000~3000 keV), same as SetRangeUser()
        TVector2 rangeY;  // absolute y range(e.g. 1000~3000 keV), same as SetRangeUser()

        Double_t marginLeft;
        Double_t marginRight;
        Double_t marginTop;
        Double_t marginBottom;

        Int_t ticksX;
        Int_t ticksY;

        std::string save;

        std::vector<HistoInfoSet> histos;
    };

    struct PanelInfo {
        Float_t font_size;
        Int_t precision;

        std::vector<Float_t> posX;
        std::vector<Float_t> posY;

        std::vector<std::string> label;
    };

   private:
    void InitFromConfigFile() override;
    HistoInfoSet SetupHistogramFromConfigFile(TiXmlElement* ele, PlotInfoSet info);

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
    std::vector<PlotInfoSet> fPlots;
    std::vector<PanelInfo> fPanels;

    std::vector<std::string> fPlotNamesCheck;  //!

    TRestRun* fRun;                          //! TRestRun to handle output file
    std::vector<TString> fRunInputFileName;  //! TRestRun to handle input file
    TCanvas* fCombinedCanvas;                //! Output canvas
    Long64_t fDrawNEntries;                  //!
    Long64_t fDrawFirstEntry;                //!

    void AddFileFromExternalRun();
    void AddFileFromEnv();

    TRestAnalysisTree* GetTree(TString fileName);
    TRestRun* GetRunInfo(TString fileName);
    bool IsDynamicRange(TString rangeString);
    Int_t GetColorIDFromString(std::string in);
    Int_t GetFillStyleIDFromString(std::string in);
    Int_t GetLineStyleIDFromString(std::string in);

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
    inline TVector2 GetCanvasSize() const { return fCanvasSize; }
    inline TVector2 GetCanvasDivisions() const { return fCanvasDivisions; }

    void SetTreeEntryRange(Long64_t NEntries = TTree::kMaxEntries, Long64_t FirstEntry = 0) {
        fDrawNEntries = NEntries;
        fDrawFirstEntry = FirstEntry;
    }
    void PlotCombinedCanvas();

    // Constructor
    TRestAnalysisPlot();
    TRestAnalysisPlot(const char* configFilename, const char* name = "");
    // Destructor
    virtual ~TRestAnalysisPlot();
};
#endif
