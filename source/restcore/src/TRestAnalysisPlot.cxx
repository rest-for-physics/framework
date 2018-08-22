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

#include <TStyle.h>
#include <TLegend.h>

#include <ctime>

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

    fStartTime = 0;
    fEndTime = 0;

    fClasifyBy = "runTag";

    fLabelOffsetX = 1.1;
    fLabelOffsetY = 1.3;

    fLabelScaleX = 1.2;
    fLabelScaleY = 1.3;

    fTicksScaleX = 1.5;
    fTicksScaleY = 1.5;
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

    position = 0;
    string styleString;
    while( ( styleString = GetKEYDefinition( "style", position ) ) != "" )
    {
        Int_t lineColor = StringToInteger ( GetFieldValue( "lineColor", styleString ) );
        if( lineColor != -1 )
            fLineColor.push_back( lineColor );

        Int_t lineWidth = StringToInteger ( GetFieldValue( "lineWidth", styleString ) );
        if( lineWidth != -1 )
            fLineWidth.push_back( lineWidth );

        Int_t lineStyle = StringToInteger ( GetFieldValue( "lineStyle", styleString ) );
        if( lineStyle != -1 )
            fLineStyle.push_back( lineStyle );

        Int_t fillStyle = StringToInteger ( GetFieldValue( "fillStyle", styleString ) );
        if( fillStyle != -1 )
            fFillStyle.push_back( fillStyle );

        Int_t fillColor = StringToInteger ( GetFieldValue( "fillColor", styleString ) );
        if( fillColor != -1 )
            fFillColor.push_back( fillColor );
    }

    position = 0;
    string formatDefinition;
    if( ( formatDefinition = GetKEYDefinition( "labels", position ) ) != "" )
    {
        if( GetVerboseLevel() >= REST_Debug )
        {
            cout << formatDefinition << endl;
            cout << "Reading format definition : " << endl;
            cout << "---------------------------" << endl;
        }

        fTicksScaleX = StringToDouble ( GetFieldValue( "ticksScaleX", formatDefinition ) );
        fTicksScaleY = StringToDouble ( GetFieldValue( "ticksScaleY", formatDefinition ) );
        
        fLabelScaleX = StringToDouble ( GetFieldValue( "labelScaleX", formatDefinition ) );
        fLabelScaleY = StringToDouble ( GetFieldValue( "labelScaleY", formatDefinition ) );
        
        fLabelOffsetX = StringToDouble ( GetFieldValue( "labelOffsetX", formatDefinition ) );
        fLabelOffsetY = StringToDouble ( GetFieldValue( "labelOffsetY", formatDefinition ) );

        if( fLabelOffsetX == -1 ) fLabelOffsetX = 1.1;
        if( fLabelOffsetY == -1 ) fLabelOffsetY = 1.3;

        if( fTicksScaleX == -1 ) fTicksScaleX = 1.5;
        if( fTicksScaleY == -1 ) fTicksScaleY = 1.5;

        if( fLabelScaleX == -1 ) fLabelScaleX = 1.3;
        if( fLabelScaleY == -1 ) fLabelScaleY = 1.3;

        if( GetVerboseLevel() >= REST_Debug )
        {
            cout << "ticks scale X : " << fTicksScaleX << endl;
            cout << "ticks scale Y : " << fTicksScaleY << endl;
            cout << "label scale X : " << fLabelScaleX << endl;
            cout << "label scale Y : " << fLabelScaleY << endl;
            cout << "label offset X : " << fLabelOffsetX << endl;
            cout << "label offset Y : " << fLabelOffsetY << endl;

            if( GetVerboseLevel() >= REST_Extreme )
                GetChar();
        }
    }

    position = 0;
    string legendDefinition;
    if( ( legendDefinition = GetKEYDefinition( "legendPosition", position ) ) != "" )
    {
        if( GetVerboseLevel() >= REST_Debug )
        {
            cout << legendDefinition << endl;
            cout << "Reading legend definition : " << endl;
            cout << "---------------------------" << endl;
        }

        fLegendX1 = StringToDouble ( GetFieldValue( "x1", legendDefinition ) );
        fLegendY1 = StringToDouble ( GetFieldValue( "y1", legendDefinition ) );

        fLegendX2 = StringToDouble ( GetFieldValue( "x2", legendDefinition ) );
        fLegendY2 = StringToDouble ( GetFieldValue( "y2", legendDefinition ) );
        
        if( fLegendX1 == -1 ) fLegendX1 = 0.7;
        if( fLegendY1 == -1 ) fLegendY1 = 0.75;
        
        if( fLegendX2 == -1 ) fLegendX2 = 0.88;
        if( fLegendY2 == -1 ) fLegendY2 = 0.88;

        if( GetVerboseLevel() >= REST_Debug )
        {
            cout << "x1 : " << fLegendX1 << " y1 : " << fLegendY1 << endl;
            cout << "x2 : " << fLegendX2 << " y2 : " << fLegendY2 << endl;

            if( GetVerboseLevel() >= REST_Extreme )
                GetChar();
        }
    }

    position = 0;
    string addFileString;
    while( ( addFileString = GetKEYDefinition( "addFile", position ) ) != "" )
    {
        TString inputfile = GetFieldValue( "name", addFileString );

        this->AddFile( inputfile );
    }

    fPlotMode = GetParameter( "plotMode", "compare" );
    fHistoOutputFile = GetParameter( "histoFilename", "/tmp/histos.root" );

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

            TString option = RemoveWhiteSpaces( GetFieldValue( "option", addPlotString ) );

            if( option == "Notdefined" )
                option = "colz";

            if( RemoveWhiteSpaces( GetFieldValue( "stats", addPlotString ) ) == "OFF" )
                fStats.push_back( kFALSE );
            else
                fStats.push_back( kTRUE );

            if( RemoveWhiteSpaces( GetFieldValue( "legend", addPlotString ) ) == "OFF" )
                fLegend.push_back( kFALSE );
            else
                fLegend.push_back( kTRUE );

            fPlotOption.push_back( option );

            // scale to be implemented

            vector <TString> varNames;
            vector <TVector2> ranges;
            vector <Int_t> bins;

            string variableDefinition;
            size_t pos = 0;
            while( (variableDefinition = GetKEYDefinition( "variable", pos, addPlotString ) ) != "" )
            {            
                varNames.push_back( GetFieldValue( "name", (string) variableDefinition ) );

                string rangeStr = GetFieldValue( "range", variableDefinition );
                rangeStr = Replace( rangeStr, "unixTime", std::to_string( std::time(nullptr) ) );
                rangeStr = Replace( rangeStr, "days", "24*3600" );

                ranges.push_back( StringTo2DVector( rangeStr ) );
                bins.push_back( StringToInteger( GetFieldValue( "nbins", variableDefinition ) ) );
            }

            TString pltString = "";
            for( unsigned int i = 0; i < varNames.size(); i++ )
            {
                pltString += varNames[i]; 
                if( i < varNames.size()-1 ) pltString += ":";
            }

            if( GetVerboseLevel() >= REST_Debug )
            {
                for( unsigned int n = 0; n < bins.size(); n++ )
                {
                    cout << "Variable " << varNames[n] << endl;
                    cout << "------------------------------------------" << endl;
                    cout << "Plot range : ( " << ranges.back().X() << " , " << ranges.back().Y() << " ) " << endl;
                    cout << "bins : " << bins.back() << endl;
                    cout << endl;
                }
            }

            pltString += " >>" + plotName;

            // The range definitions are in reversed ordered. Compared to ROOT variable definitions
            for( int i = ((int) bins.size()) - 1; i >= 0; i-- )
            {

                TString binsStr;
                binsStr.Form( "%d", bins[i] );
                if( bins[i] == -1 )
                    binsStr = " ";

                TString rXStr;
                rXStr.Form( "%f", ranges[i].X() );
                if( ranges[i].X() == -1 )
                    rXStr = " ";

                TString rYStr;
                rYStr.Form( "%f", ranges[i].Y() );
                if( ranges[i].Y() == -1 )
                    rYStr = " ";

                if( i == (int) bins.size()-1 ) pltString += "(";

                pltString += binsStr + " , " + rXStr + " , " + rYStr;
                if( i > 0 ) pltString += ",";
                if( i == 0 ) pltString += ")";
            }

            fPlotString.push_back ( pltString );

            pos = 0;
            string addCutString;

            if( GetVerboseLevel() >= REST_Debug )
            {
                cout << endl;
                cout << "Plot string : " << pltString << endl;
            }

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

                    if( GetVerboseLevel() >= REST_Debug )
                        cout << "Adding local cut : " << cutVariable << cutCondition << endl;

                    cutString += cutVariable + cutCondition;
                    n++;
                }
            }

            for( unsigned int i = 0; i < globalCuts.size(); i++ )
            {
                if( i > 0 || cutString != "" ) cutString += " && ";
                if( GetVerboseLevel() >= REST_Debug )
                    cout << "Adding global cut : " << globalCuts[i] << endl;
                cutString += globalCuts[i];
            }

            fCutString.push_back( cutString );

            if( GetVerboseLevel() >= REST_Debug )
            {
                cout << "-------------------------------" << endl;
            }
        }
    }

    position = 0;
    while( ( addPlotString = GetKEYStructure( "histo", position ) ) != "NotFound" ) //general cuts 
    {
        TString histoActive = GetFieldValue( "value", addPlotString );

        if( histoActive == "on" || histoActive == "ON" || histoActive == "On" || histoActive == "oN" )
        {     
            TString histoName = RemoveWhiteSpaces( GetFieldValue( "name", addPlotString ) );
            fHistoNames.push_back( histoName );

            TString xLabel = GetFieldValue( "xlabel", addPlotString );
            fHistoXLabel.push_back( xLabel );

            TString yLabel = GetFieldValue( "ylabel", addPlotString );
            fHistoYLabel.push_back( yLabel );

            TString title = GetFieldValue( "title", addPlotString );
            fHistoTitle.push_back( title );

            TString saveName = RemoveWhiteSpaces( GetFieldValue( "save", addPlotString ) );
            fHistoSaveToFile.push_back( saveName );
        }
    }

    Int_t maxPlots = (Int_t) fCanvasDivisions.X() * (Int_t) fCanvasDivisions.Y();

    Int_t nPlots = (Int_t) fPlotString.size() + (Int_t) fHistoNames.size();

    if( nPlots > maxPlots ) 
    {
        cout << "REST ERROR: Your canvas divisions (" << fCanvasDivisions.X() << " , " << fCanvasDivisions.Y();
        cout << ") are not enough to show " << nPlots << " plots" << endl;
        exit(1);
    }
}

void TRestAnalysisPlot::AddFile( TString fileName ) 
{ 
    TRestRun *run = new TRestRun();
    run->OpenInputFile( fileName );

    if( fClasifyBy == "runTag" )
    {
        TString rTag = run->GetRunTag();

        Int_t index = GetRunTagIndex( run->GetRunTag() );

        fFileNames[index].push_back( fileName ); 
        fNFiles++; 
    }
    else if( fClasifyBy == "combineAll" )
    {
        fFileNames[0].push_back( fileName ); 
        fNFiles++;
    }
    else
    {
        cout << "REST Warning. TRestAnalysisPlot : fClassifyBy not recognized" << endl;

        fFileNames[0].push_back( fileName ); 
        fNFiles++;
    }

    run->CloseInputFile();
    delete run;
}

Int_t TRestAnalysisPlot::GetRunTagIndex( TString tag )
{
    Int_t index = 0;
    for( unsigned int n = 0; n < fLegendName.size(); n++ )
    {
        if( fLegendName[n] == tag )
            return index;
        index++;
    }

    fLegendName.push_back( tag );

    return index;
}

Int_t TRestAnalysisPlot::GetPlotIndex( TString plotName )
{

    for( unsigned int n = 0; n < fPlotNames.size(); n++ )
        if( fPlotNames[n] == plotName ) return n;

    cout << "REST WARNING : GetPlotIndex. Plot name " << plotName << " not found" << endl;
    return -1;
}

void TRestAnalysisPlot::AddMissingStyles( )
{
    if( fLegendName.size() > fLineStyle.size() )
    {
        for( unsigned int n = fLineStyle.size(); n < fLegendName.size(); n++ )
            fLineStyle.push_back( 1 );
    }

    if( fLegendName.size() > fLineColor.size() )
    {
        for( unsigned int n = fLineColor.size(); n < fLegendName.size(); n++ )
            fLineColor.push_back( 1 );
    }

    if( fLegendName.size() > fLineWidth.size() )
    {
        for( unsigned int n = fLineWidth.size(); n < fLegendName.size(); n++ )
            fLineWidth.push_back( 1 );
    }

    if( fLegendName.size() > fFillColor.size() )
    {
        for( unsigned int n = fFillColor.size(); n < fLegendName.size(); n++ )
            fFillColor.push_back( 1 );
    }

    if( fLegendName.size() > fFillStyle.size() )
    {
        for( unsigned int n = fFillStyle.size(); n < fLegendName.size(); n++ )
            fFillStyle.push_back( 0 );
    }
}

void TRestAnalysisPlot::PlotCombinedCanvas( )
{
    AddMissingStyles();

    vector <TRestRun *> runs[REST_MAX_TAGS];
    vector <TRestAnalysisTree *> trees[REST_MAX_TAGS];
    vector <TH3F *> histCollection;

    fStartTime = 0;
    fEndTime = 0;

    /* {{{ We create a list of analysis trees in each run, and define start/end times */
    TRestRun *r;
    TRestAnalysisTree *anT;
    for( unsigned int i = 0; i < fLegendName.size(); i++ )
        for( unsigned int n = 0; n < fFileNames[i].size(); n++ )
        {
            r = new TRestRun();
            runs[i].push_back( r );
            r->OpenInputFile( fFileNames[i][n] );
            anT = r->GetAnalysisTree();

            trees[i].push_back( anT );

            r->SkipEventTree();

            if( r->GetEntries() < 3 )
                continue;

            r->GetEntry(1);
            if( fStartTime == 0 || anT->GetTimeStamp() < fStartTime ) fStartTime = anT->GetTimeStamp();

            r->GetEntry( r->GetEntries() - 1);
            if( fEndTime == 0 || anT->GetTimeStamp() > fEndTime ) fEndTime = anT->GetTimeStamp();
        }
    /* }}} */

    fHistoOutputFile = ReplaceFilenameTags( fHistoOutputFile, runs[0][0] );
    TFile *f = new TFile( fHistoOutputFile, "RECREATE");

    cout << "Saving histograms to ROOT file : " << fHistoOutputFile << endl;

    /* {{{ Initializing canvas window */
    if( fCombinedCanvas != NULL ) 
    {
        delete fCombinedCanvas;
        fCombinedCanvas = NULL;
    }

    fCombinedCanvas = new TCanvas( "combined", "combined", 0, 0, fCanvasSize.X(), fCanvasSize.Y() );

    fCombinedCanvas->Divide( (Int_t) fCanvasDivisions.X(), (Int_t) fCanvasDivisions.Y() );
    /* }}} */

    TStyle *st = new TStyle();
    st->SetPalette(1);

    for( unsigned int n = 0; n < fPlotString.size(); n++ )
    {
        fCombinedCanvas->cd(n+1);
        if( fLogScale[n] ) 
            fCombinedCanvas->cd(n+1)->SetLogy();

        fCombinedCanvas->cd(n+1)->SetLeftMargin(0.15);
        fCombinedCanvas->cd(n+1)->SetBottomMargin(0.15);

        histCollection.clear();

        std::vector <TString> hName;
        for( unsigned int i = 0; i < fLegendName.size(); i++ )
        {
            TString plotString = fPlotString[n];

            size_t varStart = FindNthStringPosition( (string) fPlotString[n], 0, ">>", 0 );
            size_t varEnd = FindNthStringPosition( (string) fPlotString[n], varStart, "(", 0 );

            TString varName = fPlotString[n]( varStart , varEnd - varStart );

            hName.push_back( varName + "_" + fLegendName[i] );
            plotString = Replace( (string) plotString, (string) varName, (string) hName[i], 0 , 1 );
            hName[i].Remove(0,2);

            for( unsigned int m = 0; m < fFileNames[i].size(); m++ )
            {
                /* {{{ Constructing plotString for time plots (TO BE REIMPLEMENTED!!)
                if( fPlotXLabel[n].Contains("Time") ||  fPlotXLabel[n].Contains("time") )
                {
                    size_t first = FindNthStringPosition( (string) fPlotString[n], 0, ",", 0 );
                    size_t second = FindNthStringPosition( (string) fPlotString[n], 0, ",", 1 );
                    size_t third = FindNthStringPosition( (string) fPlotString[n], 0, ",", 2 );

                    TString xStr = fPlotString[n]( first + 1 ,second - first - 1 );
                    string xstr = trim( (string) xStr );

                    if( xstr == "" )
                    {
                        string startTimeStr = std::to_string( fStartTime);
                        plotString.Insert( second -1, startTimeStr );
                    }

                    TString yStr = fPlotString[n]( second + 1 ,third- second - 1 );
                    string ystr = trim( (string) yStr );

                    if( ystr == "" )
                    {
                        string endTimeStr = std::to_string( fEndTime);
                        third = FindNthStringPosition( (string) plotString, 0, ",", 2 );
                        plotString.Insert( third -1, endTimeStr );
                    }
                }
                 }}} */

                if( m == 1 )
                {
                    plotString = Replace( (string) plotString, ">>", ">>+", 0 , 1 );
                    plotString = plotString ( 0, FindNthStringPosition( (string) plotString, 0, "(", 0 ) );
                }

                if( GetVerboseLevel() >= REST_Debug )
                {
                    cout << endl;
                    cout << "--------------------------------------" << endl;
                    cout << "Plot name : " << fPlotNames[n] << endl;
                    cout << "Plot string : " << plotString << endl;
                    cout << "Cut string : " << fCutString[n] << endl;
                    cout << "Plot option : " << fPlotOption[n] << endl;
                    cout << "++++++++++++++++++++++++++++++++++++++" << endl;

                }

                if ( trees[i][m]->Draw( plotString, fCutString[n], fPlotOption[n] ) == -1 )
                {
                    cout << endl;
                    cout << "REST ERROR. TRestAnalysisPlot::PlotCombinedCanvas." << endl;
                    cout << "Plot string not properly constructed. Does the analysis observable exist inside the file?" << endl;
                    cout << "Use \"restPrintAnaTreeVariables FILE.ROOT\" to get a list of existing observables." << endl;
                    cout << endl;
                    exit(1);
                }
            }

            histCollection.push_back ( ( TH3F* ) gPad->GetPrimitive( hName[i] ) );
        }

        Double_t maxValue = 0;
        for( unsigned int i = 0; i < fLegendName.size(); i++ )
        {

            Double_t value = histCollection[i]->GetBinContent( histCollection[i]->GetMaximumBin() );
            if( i == 0 ) maxValue = value;
            else if( value > maxValue )
                maxValue = value;

            TH3F *htemp = histCollection[i];
            htemp->SetStats( fStats[n] );

            htemp->SetTitle( fPlotTitle[n] );
            htemp->GetXaxis()->SetTitle( fPlotXLabel[n] );
            htemp->GetYaxis()->SetTitle( fPlotYLabel[n] );

            htemp->GetXaxis()->SetLabelSize( fTicksScaleX * htemp->GetXaxis()->GetLabelSize( ) );
            htemp->GetYaxis()->SetLabelSize( fTicksScaleY * htemp->GetYaxis()->GetLabelSize( ) );

            htemp->GetXaxis()->SetTitleSize( fLabelScaleX * htemp->GetXaxis()->GetTitleSize( ) );
            htemp->GetYaxis()->SetTitleSize( fLabelScaleY * htemp->GetYaxis()->GetTitleSize( ) );

            htemp->GetXaxis()->SetTitleOffset( fLabelOffsetX * htemp->GetXaxis()->GetTitleOffset( ) );
            htemp->GetYaxis()->SetTitleOffset( fLabelOffsetY * htemp->GetYaxis()->GetTitleOffset( ) );

            htemp->SetLineColor( fLineColor[i] );
            htemp->SetLineWidth( fLineWidth[i] );
            htemp->SetLineStyle( fLineStyle[i] );
            htemp->SetFillColor( fFillColor[i] );
            htemp->SetFillStyle( fFillStyle[i] );

            /*
            if( fPlotXLabel[n].Contains("Time") ||  fPlotXLabel[n].Contains("time") )
            {
                Double_t hours = (fEndTime-fStartTime)/3600.;
                htemp->GetXaxis()->SetTimeOffset( 0, "gmt" );
                if( hours < 24 )
                    htemp->GetXaxis()->SetTimeFormat("%Hh %Mm");
                else
                    htemp->GetXaxis()->SetTimeFormat("%Hh %d/%b");
                htemp->GetXaxis()->SetTimeDisplay(1);
            }
            */

        }

        TLegend *legend = new TLegend( fLegendX1, fLegendY1, fLegendX2, fLegendY2 );
        for( unsigned int i = 0; i < fLegendName.size(); i++ )
        {
            legend->AddEntry( histCollection[i], fLegendName[i], "lf" );

            histCollection[i]->SetStats( fStats[n] );

            histCollection[i]->GetYaxis()->SetRangeUser(0.1, 1.1 * maxValue );
            if( i == 0 )
                histCollection[i]->Draw();
            else
                histCollection[i]->Draw("same");

            f->cd();
            histCollection[i]->Write( hName[i] );
        }
        if( fLegend[n] )
            legend->Draw("same");

        if( fPlotSaveToFile[n] != "Notdefined" && fPlotSaveToFile[n] != "" )
            SavePlotToPDF( fPlotNames[n], fPlotSaveToFile[n] );
        fCombinedCanvas->Update();
    }

    /* {{{ Acumulating and plotting histograms present in the file */
    ////// TODO : Needs to be reviewed to WORK with the new version of runTag classification (20180620).
    for( unsigned int n = 0; n < fHistoNames.size(); n++ )
    {
        cout << "Histo names : " << fHistoNames[n] << endl;
        fCombinedCanvas->cd( (Int_t) fPlotString.size() + n + 1 );

        runs[0][0]->GetInputFile()->cd();

        TH1D *h = (TH1D *) runs[0][0]->GetInputFile()->Get( fHistoNames[n] );

        if( !h )
        {
            cout << "REST ERROR. TRestAnalysisPlot. A histogram with name : " << fHistoNames[n] << " does not exist in input file" << endl;
            exit(1);
        }

        Int_t nB = h->GetNbinsX();
        Int_t bX = h->GetXaxis()->GetBinCenter( 1 ) - 0.5;
        Int_t bY = h->GetXaxis()->GetBinCenter( h->GetNbinsX() ) + 0.5;

        TH1D *hNew = new TH1D( "New_" + (TString) fHistoNames[n], fHistoNames[n], nB, bX, bY );

        for( unsigned int m = 0; m < fFileNames[0].size(); m++ )
        {
            TH1D *aHist = (TH1D *) runs[0][m]->GetInputFile()->Get( fHistoNames[n] );
            runs[0][m]->GetInputFile()->cd();
            hNew->Add( aHist );
        }

        if( fStats[n] == kFALSE )
            hNew->SetStats(kFALSE);

        hNew->SetTitle( fHistoTitle[n] );
        hNew->GetXaxis()->SetTitle( fHistoXLabel[n] );
        hNew->GetYaxis()->SetTitle( fHistoYLabel[n] );

        hNew->Draw();

        f->cd();
        hNew->Write( fHistoNames[n] );

        if( fHistoSaveToFile[n] != "Notdefined" && fHistoSaveToFile[n] != "" )
            SaveHistoToPDF( hNew, n, fHistoSaveToFile[n] );
        fCombinedCanvas->Update();
    }
    /* }}} */

    // Saving to a PDF file
    fCanvasSave = ReplaceFilenameTags( fCanvasSave, runs[0][0] );
    if( fCanvasSave != "" )
        fCombinedCanvas->Print( fCanvasSave );

    f->Close();
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
    run->OpenInputFile( fFileNames[0][0] );

    TRestAnalysisTree *anTree = run->GetAnalysisTree();
    anTree->Draw( fPlotString[n], fCutString[n], "", anTree->GetEntries(), 0 );

    TH3F *htemp = (TH3F*)gPad->GetPrimitive( fPlotNames[n] );
    htemp->SetTitle( fPlotTitle[n] );
    htemp->GetXaxis()->SetTitle( fPlotXLabel[n] );
    htemp->GetYaxis()->SetTitle( fPlotYLabel[n] );

    c->Print( fileName );

    delete c;
}

void TRestAnalysisPlot::SaveHistoToPDF( TH1D *h, Int_t n, TString fileName )
{
    gErrorIgnoreLevel=10;

    TCanvas *c = new TCanvas( h->GetName(), h->GetTitle(), 800, 600 );

    h->Draw( );

    h->SetTitle( fHistoTitle[n] );
    h->GetXaxis()->SetTitle( fHistoXLabel[n] );
    h->GetYaxis()->SetTitle( fHistoYLabel[n] );

    c->Print( fileName );

    delete c;
}

TString TRestAnalysisPlot::ReplaceFilenameTags( TString filename, TRestRun *run )
{
    return run->ConstructFilename( filename );
}

