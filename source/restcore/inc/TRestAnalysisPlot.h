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
#include "TRestAnalysisTree.h"
#include "TCanvas.h"
#include "TH3D.h"

class TRestAnalysisPlot:public TRestMetadata {
    private:
        void InitFromConfigFile( );

        Int_t fNFiles;
        std::vector <TString> fFileNames;

#ifndef __CINT__
        TRestRun *fRun;

        TCanvas *fCombinedCanvas;

        TString fPlotMode;
        TString fHistoOutputFile;
        TVector2 fCanvasSize;
        TVector2 fCanvasDivisions;
        TString fCanvasSave;
        std::vector <TString> fPlotNames;
        std::vector <TString> fPlotSaveToFile;
        std::vector <TString> fPlotXLabel;
        std::vector <TString> fPlotYLabel;
        std::vector <Bool_t> fLogScale;
        std::vector <TString> fPlotTitle;
        std::vector <TString> fPlotOption;
        std::vector <TString> fPlotString;
        std::vector <TString> fCutString;

        Double_t fStartTime;
        Double_t fEndTime;

        Bool_t fStats;


#endif

        TString ReplaceFilenameTags( TString filename, TRestRun *run );

    protected:

    public:

        void Initialize( );

        void PrintMetadata( ) { }

        void SetOutputPlotsFilename( TString fname ) { fCanvasSave = fname; }
        void SetOutputHistosFilename( TString fname ) { fHistoOutputFile = fname; }

        void AddFile( TString fileName ) { fFileNames.push_back( fileName ); fNFiles++; }

        void SavePlotToPDF( TString plotName, TString fileName );
        void SavePlotToPDF( Int_t n, TString fileName );

        Int_t GetPlotIndex( TString plotName );

        void PlotCombinedCanvas( );
        void PlotCombinedCanvasCompare( );
        void PlotCombinedCanvasAdd( );

        //Construtor
        TRestAnalysisPlot();
        TRestAnalysisPlot( const char *cfgFileName, const char *name = "" );
        //Destructor
        virtual ~ TRestAnalysisPlot();


        ClassDef(TRestAnalysisPlot, 1);     
};
#endif
