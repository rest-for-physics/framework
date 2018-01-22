#include "TRestTask.h"
#include "TRestReadout.h"
#include "TRestBrowser.h"
#include <vector>
#include <iostream>
using namespace std;

#ifndef RestTask_ViewReadout
#define RestTask_ViewReadout

int REST_Viewer_Readout( TString rootFile, TString name, Int_t plane = 0 )
{

    TFile *f = new TFile( rootFile );
    TRestReadout *readout = (TRestReadout *) f->Get( name );
    readout->PrintMetadata();

    TRestReadoutPlane *readoutPlane = readout->GetReadoutPlane( plane );

    Int_t nModules = readoutPlane->GetNumberOfModules( );

    Int_t totalPixels = 0;
    Int_t totalChannels = 0;
    for( int mdID = 0; mdID < nModules; mdID++ )
    {
        TRestReadoutModule *module = readoutPlane->GetReadoutModule( mdID );
        Int_t nChannels = module->GetNumberOfChannels();
	totalChannels += nChannels;

        for( int ch = 0; ch < nChannels; ch++ )
        {
            TRestReadoutChannel *channel = module->GetChannel( ch );
            Int_t nPixels = channel->GetNumberOfPixels();
            totalPixels += nPixels;
        }
    }

    const Int_t nPixConst = totalPixels;
    const Int_t nModConst = nModules;
    const Int_t nChConst = totalChannels;

    TGraph *pixelGraph[nPixConst];
    TGraph *channelGraph[nChConst];
    TGraph *modGraph[nModConst];
    TLatex *channelIDLabel[nPixConst];
    
    double xmin=1E9,xmax=-1E9,ymin=1E9,ymax=-1E9;

    Int_t graph = 0;
    Int_t modGraphID = 0; 
    Int_t chGraph = 0;
    for( int mdID = 0; mdID < nModules; mdID++ )
    {
        TRestReadoutModule *module = readoutPlane->GetReadoutModule( mdID );
        Int_t nChannels = module->GetNumberOfChannels();

        Double_t x[5];
        Double_t y[5];

        for( int v = 0; v < 5; v++ )
        {
            x[v] = module->GetVertex( v ).X();
            y[v] = module->GetVertex( v ).Y();
            
            if(x[v]<xmin)xmin=x[v];
            if(y[v]<ymin)ymin=y[v];
            if(x[v]>xmax)xmax=x[v];
            if(y[v]>ymax)ymax=y[v];
            
        }
        modGraph[modGraphID] = new TGraph( 5, x, y );

        modGraph[modGraphID]->SetLineColor( kBlack );
        modGraph[modGraphID]->SetLineWidth(2);
        //modGraph[modGraphID]->SetMaximum( 200 );
        //modGraph[modGraphID]->SetMinimum( -200 );
        //modGraph[modGraphID]->GetXaxis()->SetLimits(-200,200);

        modGraphID++;

        for( int ch = 0; ch < nChannels; ch++ )
        {
            TRestReadoutChannel *channel = module->GetChannel( ch );


            Int_t nPixels = channel->GetNumberOfPixels();

	    Double_t xCH[1000], yCH[1000];
            for( int px = 0; px < nPixels; px++ )
            {
		xCH[px] = module->GetPixelCenter( ch, px ).X();
		yCH[px] = module->GetPixelCenter( ch, px ).Y();

                for( int v = 0; v < 5; v++ )
                {
                    x[v] = module->GetPixelVertex( ch, px, v ).X();
                    y[v] = module->GetPixelVertex( ch, px, v ).Y();
                }
                pixelGraph[graph] = new TGraph( 5, x, y );

                pixelGraph[graph]->SetLineColor( kRed );
                pixelGraph[graph]->SetLineWidth(2);
                //pixelGraph[graph]->SetMaximum( 200 );
                //pixelGraph[graph]->SetMinimum( -200 );
                //pixelGraph[graph]->GetXaxis()->SetLimits(-200,200);

                Double_t xMin = 1e10;
                Double_t yMin = 1e10;
                Double_t xMax = -1e10;
                for( int l = 0; l < 5; l++ )
                {
                    if( x[l] < xMin ) xMin = x[l];
                    if( y[l] < yMin ) yMin = y[l];
                    if( x[l] > xMax ) xMax = x[l];

                }
                channelIDLabel[graph] = new TLatex( xMin+(xMax-xMin)/2, yMin, Form("%d", channel->GetID() ) );
                channelIDLabel[graph]->SetTextSize( 0.01 );
                channelIDLabel[graph]->SetTextFont( 12 );
                channelIDLabel[graph]->SetTextAlign( 21 );
                //           channelIDLable[graph]->SetTextColor( kBlue+4*mdID );


                //pixelGraph[graph]->Draw("same");
                graph++;
            }
                channelGraph[chGraph] = new TGraph( nPixels, xCH, yCH );

                channelGraph[chGraph]->SetLineColor( kBlue );
                channelGraph[chGraph]->SetLineWidth(2);
		//channelGraph[chGraph]->SetMarkerStyle(20);
		//channelGraph[chGraph]->SetMarkerSize(10);
                //channelGraph[chGraph]->SetMaximum( 200 );
                //channelGraph[chGraph]->SetMinimum( -200 );
                //channelGraph[chGraph]->GetXaxis()->SetLimits(-200,200);
		chGraph++;
        }
    }

    TCanvas* c = new TCanvas("ReadoutGraphViewer", "  ", 900, 900);
    c->DrawFrame(xmin,ymin,xmax,ymax);
    c->SetTicks();
    
    for( int i = 0; i < modGraphID; i++ )modGraph[i]->Draw("same");
   

    
    for( int i = 0; i < graph; i++ )
    {
	     pixelGraph[i] ->Draw("same");
	     //channelIDLabel[i]->Draw("same"); //If commented the graphics are faster
    }

    for( int i = 0; i < chGraph; i++ )channelGraph[i]->Draw("same");
    
    return 0;
}


class REST_ViewReadout :public TRestTask {
public:
	ClassDef(REST_ViewReadout, 1);

	REST_ViewReadout() { fNRequiredArgument = 2; }
	~REST_ViewReadout() {}

	TString filename = " ";
	TString eventype = "";
	int planeId = 0;

	void RunTask(TRestManager*mgr)
	{
		REST_Viewer_Readout(filename, eventype,planeId);
		gApplication->Run();
	}

};

#endif

