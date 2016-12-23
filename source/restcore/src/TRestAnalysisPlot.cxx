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
using namespace std;



const int debug = 0;

ClassImp(TRestAnalysisPlot)
    //______________________________________________________________________________
TRestAnalysisPlot::TRestAnalysisPlot()
{
    Initialize();

}

TRestAnalysisPlot::TRestAnalysisPlot( const char *cfgFileName, const char *name) : TRestMetadata (cfgFileName)
{
    Initialize();

    LoadConfigFromFile( fConfigFileName, name );

}

void TRestAnalysisPlot::Initialize()
{
    SetSectionName( this->ClassName() );

    fRun = NULL;

    fNFiles = 0;

    fCombinedCanvas = NULL;
}


//______________________________________________________________________________
TRestAnalysisPlot::~TRestAnalysisPlot()
{
    if( fRun != NULL ) delete fRun;
}

//______________________________________________________________________________
void TRestAnalysisPlot::InitFromConfigFile()
{
    size_t position = 0;
    string addFileString;
    while( ( addFileString = GetKEYDefinition( "addFile", position ) ) != "" )
    {
        TString inputfile = GetFieldValue( "name", addFileString );

        this->AddFile( inputfile );
    }

    fPlotMode = GetParameter( "plotMode", "compare" );

    position = 0;
    string canvasDefinition;
    if( ( canvasDefinition = GetKEYDefinition( "canvas", position ) ) != "" )
    {
        fCanvasSize = StringTo2DVector ( GetFieldValue( "size", canvasDefinition ) );
        fCanvasDivisions = StringTo2DVector ( GetFieldValue( "divide", canvasDefinition ) );
        fCanvasSave = GetFieldValue( "save", canvasDefinition );
    }

    vector <TString> globalCuts;

    position = 0;
    string globalCutString;
    while( ( globalCutString = GetKEYDefinition( "globalCut", position ) ) != "" ) //general cuts 
    {  
        TString cutActive = GetFieldValue( "value", globalCutString );

        if( cutActive == "on" || cutActive == "ON" || cutActive == "On" || cutActive == "oN" )
        {
            TString obsName = GetFieldValue( "name", globalCutString );

            TString cutCondition = GetFieldValue( "condition", globalCutString );
            TString cutString = obsName + cutCondition;

            globalCuts.push_back( cutString );
        }
    }

    position = 0;
    string addPlotString;
    while( ( addPlotString = GetKEYStructure( "plot", position ) ) != "NotFound" ) //general cuts 
    {
        TString plotActive = GetFieldValue( "value", addPlotString );

        if( plotActive == "on" || plotActive == "ON" || plotActive == "On" || plotActive == "oN" )
        {     
            TString plotName = RemoveWhiteSpaces( GetFieldValue( "name", addPlotString ) );
            fPlotNames.push_back( plotName );

            TString saveName = RemoveWhiteSpaces( GetFieldValue( "save", addPlotString ) );
            fPlotSaveToFile.push_back( saveName );

            TString logScale = GetFieldValue( "logscale", addPlotString );

            if( logScale == "true" )
                fLogScale.push_back( true );
            else
                fLogScale.push_back( false );

            TString xLabel = GetFieldValue( "xlabel", addPlotString );
            fPlotXLabel.push_back( xLabel );

            TString yLabel = GetFieldValue( "ylabel", addPlotString );
            fPlotYLabel.push_back( yLabel );

            TString title = GetFieldValue( "title", addPlotString );
            fPlotTitle.push_back( title );

            // scale to be implemented

            vector <TString> varNames;
            vector <TVector2> ranges;
            vector <Int_t> bins;

            string variableDefinition;
            size_t pos = 0;
            while( (variableDefinition = GetKEYDefinition( "variable", pos, addPlotString ) ) != "" )
            {            

                varNames.push_back( GetFieldValue( "name", (string) variableDefinition ) );
                ranges.push_back( StringTo2DVector( GetFieldValue( "range", variableDefinition ) ) );
                bins.push_back( StringToInteger( GetFieldValue( "nbins", variableDefinition ) ) );
            }

            TString pltString = "";
            for( unsigned int i = 0; i < varNames.size(); i++ )
            {
                pltString += varNames[i]; 
                if( i < varNames.size()-1 ) pltString += ":";
            }

            pltString += " >>" + plotName;

            for( unsigned int i = 0; i < ranges.size(); i++ )
            {

                TString binsStr;
                binsStr.Form( "%d", bins[i] );

                TString rXStr;
                rXStr.Form( "%f", ranges[i].X() );

                TString rYStr;
                rYStr.Form( "%f", ranges[i].Y() );

                if( i == 0 ) pltString += "(";
                pltString += binsStr + " , " + rXStr + " , " + rYStr;
                if( i < ranges.size()-1 ) pltString += ",";
                if( i == ranges.size()-1 ) pltString += ")";
            }

            fPlotString.push_back ( pltString );

            pos = 0;
            string addCutString;

            TString cutString = "";
            Int_t n = 0;
            while( ( addCutString = GetKEYDefinition( "cut", pos, addPlotString ) ) != "" )
            {
                TString cutActive = GetFieldValue( "value", addCutString );

                if( cutActive == "on" || cutActive == "ON" || cutActive == "On" || cutActive == "oN" )
                {     
                    TString cutVariable = GetFieldValue( "variable", addCutString );
                    TString cutCondition = GetFieldValue( "condition", addCutString );

                    if( n > 0 ) cutString += " && ";

                    cutString += cutVariable + cutCondition;
                    n++;
                }
            }

            for( unsigned int i = 0; i < globalCuts.size(); i++ )
            {
                if( n > 0 ) cutString += " && ";
                cutString += globalCuts[i];
            }

            fCutString.push_back( cutString );
        }
    }

    Int_t maxPlots = (Int_t) fCanvasDivisions.X() * (Int_t) fCanvasDivisions.Y();

    Int_t nPlots = (Int_t) fPlotString.size();

    if( nPlots > maxPlots ) 
    {
        cout << "REST ERROR: Your canvas divisions (" << fCanvasDivisions.X() << " , " << fCanvasDivisions.Y();
        cout << ") are not enough to show " << nPlots << " plots" << endl;
        exit(1);
    }
}

Int_t TRestAnalysisPlot::GetPlotIndex( TString plotName )
{

    for( unsigned int n = 0; n < fPlotNames.size(); n++ )
        if( fPlotNames[n] == plotName ) return n;

    cout << "REST WARNING : GetPlotIndex. Plot name " << plotName << " not found" << endl;
    return -1;
}

void TRestAnalysisPlot::PlotCombinedCanvas( )
{
    // We should check that all the variables defined in the plots exist in our files
    // TOBE implemented and return a REST error in case
    if( fPlotMode == "compare" )
    {
        PlotCombinedCanvasCompare();
        return;
    }

    if( fPlotMode == "add" )
    {
        PlotCombinedCanvasAdd();
        return;
    }

    cout << "REST WARNING: TRestAnalysisPlot::PlotCombinedCanvas(). Plot mode (" << fPlotMode << ") not found" << endl;

}

void TRestAnalysisPlot::PlotCombinedCanvasAdd( )
{

    vector <TRestRun *> runs;
    vector <TRestAnalysisTree *> trees;

    TRestRun *r;
    TRestAnalysisTree *anT;
    for( int n = 0; n < fNFiles; n++ )
    {
        r = new TRestRun();
        runs.push_back( r );
        r->OpenInputFile( fFileNames[n] );
        anT = r->GetAnalysisTree();
        trees.push_back( anT );
    }

    fCanvasSave = ReplaceFilenameTags( fCanvasSave, runs[0] );

    if( fCombinedCanvas != NULL ) 
    {
        delete fCombinedCanvas;
        fCombinedCanvas = NULL;
    }

    fCombinedCanvas = new TCanvas( "combined", "combined", 0, 0, fCanvasSize.X(), fCanvasSize.Y() );

    fCombinedCanvas->Divide( (Int_t) fCanvasDivisions.X(), (Int_t) fCanvasDivisions.Y() );

    for( unsigned int n = 0; n < fPlotString.size(); n++ )
    {
        fCombinedCanvas->cd(n+1);
        if( fLogScale[n] ) 
            fCombinedCanvas->cd(n+1)->SetLogy();
        
        for( int m = 0; m < fNFiles; m++ )
        {

            TString plotString = fPlotString[n];

            if( m > 0 )
            {
                plotString = Replace( (string) fPlotString[n], ">>", ">>+", 0 );
                plotString = plotString( 0, fPlotString[n].First(">>+") + 3 ) + fPlotNames[n];
            }

            trees[m]->Draw( plotString, fCutString[n], "" );
        }

        TH3F *htemp = (TH3F*)gPad->GetPrimitive( fPlotNames[n] );
        htemp->SetTitle( fPlotTitle[n] );
        htemp->GetXaxis()->SetTitle( fPlotXLabel[n] );
        htemp->GetYaxis()->SetTitle( fPlotYLabel[n] );

        if( fPlotSaveToFile[n] != "Notdefined" && fPlotSaveToFile[n] != "" )
            SavePlotToPDF( fPlotNames[n], fPlotSaveToFile[n] );
        fCombinedCanvas->Update();
    }

    if( fCanvasSave != "" )
        fCombinedCanvas->Print( fCanvasSave );
}

void TRestAnalysisPlot::PlotCombinedCanvasCompare( )
{
    vector <TRestRun *> runs;
    vector <TRestAnalysisTree *> trees;

    TRestRun *r;
    TRestAnalysisTree *anT;
    for( int n = 0; n < fNFiles; n++ )
    {
        r = new TRestRun();
        runs.push_back( r );
        r->OpenInputFile( fFileNames[n] );
        anT = r->GetAnalysisTree();
        trees.push_back( anT );
    }

    fCanvasSave = ReplaceFilenameTags( fCanvasSave, runs[0] );

    if( fCombinedCanvas != NULL ) 
    {
        delete fCombinedCanvas;
        fCombinedCanvas = NULL;
    }

    fCombinedCanvas = new TCanvas( "combined", "combined", 0, 0, fCanvasSize.X(), fCanvasSize.Y() );

    fCombinedCanvas->Divide( (Int_t) fCanvasDivisions.X(), (Int_t) fCanvasDivisions.Y() );

    for( unsigned int n = 0; n < fPlotString.size(); n++ )
    {
        fCombinedCanvas->cd(n+1);
        if( fLogScale[n] ) 
            fCombinedCanvas->cd(n+1)->SetLogy();
        
        for( int m = 0; m < fNFiles; m++ )
        {

            TString plotString = fPlotString[n];

            if( m > 0 )
                plotString = fPlotString[n]( 0, fPlotString[n].First(">>") );

            trees[m]->SetLineColor(1+m);
            if( m == 0 )
                trees[m]->Draw( plotString, fCutString[n], "" );
            else 
                trees[m]->Draw( plotString, fCutString[n], "same" );
        }

        TH3F *htemp = (TH3F*)gPad->GetPrimitive( fPlotNames[n] );
        htemp->SetTitle( fPlotTitle[n] );
        htemp->GetXaxis()->SetTitle( fPlotXLabel[n] );
        htemp->GetYaxis()->SetTitle( fPlotYLabel[n] );

        if( fPlotSaveToFile[n] != "Notdefined" && fPlotSaveToFile[n] != "" )
            SavePlotToPDF( fPlotNames[n], fPlotSaveToFile[n] );
        fCombinedCanvas->Update();
    }

    if( fCanvasSave != "" )
        fCombinedCanvas->Print( fCanvasSave );
}

void TRestAnalysisPlot::SavePlotToPDF( TString plotName, TString fileName )
{
    Int_t index = GetPlotIndex( plotName );
    if ( index >= 0 )
        SavePlotToPDF( index, fileName );
    else
        cout << "Save to plot failed. Plot name " << plotName << " not found" << endl;

}

void TRestAnalysisPlot::SavePlotToPDF( Int_t n, TString fileName )
{
    gErrorIgnoreLevel=10;

    TCanvas *c = new TCanvas( fPlotNames[n], fPlotNames[n], 800, 600 );

    TRestRun *run = new TRestRun();
    run->OpenInputFile( fFileNames[0] );

    TRestAnalysisTree *anTree = run->GetAnalysisTree();
    anTree->Draw( fPlotString[n], fCutString[n], "", anTree->GetEntries(), 0 );

    TH3F *htemp = (TH3F*)gPad->GetPrimitive( fPlotNames[n] );
    htemp->SetTitle( fPlotTitle[n] );
    htemp->GetXaxis()->SetTitle( fPlotXLabel[n] );
    htemp->GetYaxis()->SetTitle( fPlotYLabel[n] );

    c->Print( fileName );

    delete c;
}

TString TRestAnalysisPlot::ReplaceFilenameTags( TString filename, TRestRun *run )
{
    return run->ConstructFilename( filename );
}

