///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutEventViewer.cxx
///
///             dec 2015:   First concept
///                 Viewer class for a TRestSignalEvent and TRestReadout
///                 JuanAn Garcia
///_______________________________________________________________________________


#include "TRestReadoutEventViewer.h"

ClassImp(TRestReadoutEventViewer)

TRestReadoutEventViewer::TRestReadoutEventViewer(char *cfgFilename)
{
  fReadout = new TRestReadout(cfgFilename);
  fDecoding = new TRestDecoding(cfgFilename);
  
  Initialize();
    
}


//______________________________________________________________________________
TRestReadoutEventViewer::~TRestReadoutEventViewer()
{
   
}

//______________________________________________________________________________
void TRestReadoutEventViewer::Initialize()
{
TRestSignalEventViewer::Initialize();

fCanvasXY = new TCanvas("ReadoutMap","ReadoutMap");
fCanvasXZYZ = new TCanvas("XZYZ","XZYZ");
fCanvasXZYZ->Divide(2,1);

fSignalEvent = new TRestSignalEvent( );
fEvent = fSignalEvent;

fHistoXY= fReadout->GetReadoutHistogram();
fReadout->GetBoundaries(xmin,xmax,ymin,ymax);


}

void TRestReadoutEventViewer::AddEvent( TRestEvent *ev ){

TRestSignalEventViewer::AddEvent(ev );

if(fPad==NULL)return;

fHistoXY->Reset(0);
delete fHistoXZ;
delete fHistoYZ;

DrawReadoutPulses( );
fCanvasXY->cd();
fHistoXY->Draw("COLZ");
fCanvasXY->Update();

fCanvasXZYZ->cd(1);
fHistoXZ->Draw("CONTZ");
fCanvasXZYZ->cd(2);
fHistoYZ->Draw("CONTZ");
fCanvasXZYZ->Update();

}

void TRestReadoutEventViewer::DrawReadoutPulses( ){

int readoutChannel,daqChannel;
double charge,x,y;

TRestReadoutModule *module;
TRestReadoutChannel *channel;

int maxIndex;

zmin=1E9;
zmax=1E-9;

double z;
	for(int i=0;i<fSignalEvent->GetNumberOfSignals();i++)
	for(int j=0;j<fSignalEvent->GetSignal(i)->GetNumberOfPoints();j++){
	z=fSignalEvent->GetSignal(i)->GetTime(j);
	if(z<zmin)zmin=z;
	if(z>zmax)zmax=z;
	}

zmax++;
zmin--;

fHistoXZ = new TH2D("XZ","XZ",100,xmin,xmax,100,zmin,zmax);
fHistoYZ = new TH2D("YZ","YZ",100,ymin,ymax,100,zmin,zmax);

	for(int i=0;i<fSignalEvent->GetNumberOfSignals();i++){
	
	daqChannel=fSignalEvent->GetSignal(i)->GetSignalID();
	readoutChannel = fDecoding->GetReadoutChannel(daqChannel);
	cout<<"daqChannel "<<daqChannel<<" readoutChannel "<<readoutChannel<<endl;
	if((module = GetModule(readoutChannel))==NULL)continue;
	if((channel = GetChannel(readoutChannel))==NULL)continue;
	
	int nPixels=channel->GetNumberOfPixels();
	
	//Pixel readout
	if(nPixels==1){
	x=module->GetPixelCenter( readoutChannel, 0 ).X();
	y=module->GetPixelCenter( readoutChannel, 0 ).Y();
	for(int j=0;j<fSignalEvent->GetSignal(i)->GetNumberOfPoints();j++)
	    fHistoXZ->Fill(module->GetPixelCenter( readoutChannel, 0 ).X(),fSignalEvent->GetSignal(i)->GetTime(j),fSignalEvent->GetSignal(i)->GetData(j));
	for(int j=0;j<fSignalEvent->GetSignal(i)->GetNumberOfPoints();j++)
	    fHistoYZ->Fill(module->GetPixelCenter( readoutChannel, 0 ).Y(),fSignalEvent->GetSignal(i)->GetTime(j),fSignalEvent->GetSignal(i)->GetData(j));
	}
	//Strips readout
	else{
	
	    if((x=channel->GetPixel(0)->GetOriginX())==channel->GetPixel(1)->GetOriginX()){
	    for(int j=0;j<fSignalEvent->GetSignal(i)->GetNumberOfPoints();j++)
	    fHistoXZ->Fill(module->GetPixelCenter( readoutChannel, 0 ).X(),fSignalEvent->GetSignal(i)->GetTime(j),fSignalEvent->GetSignal(i)->GetData(j));
	    }
	    else if((y=channel->GetPixel(0)->GetOriginY())==channel->GetPixel(1)->GetOriginY()){
	    for(int j=0;j<fSignalEvent->GetSignal(i)->GetNumberOfPoints();j++)
	    fHistoYZ->Fill(module->GetPixelCenter( readoutChannel, 0 ).Y(),fSignalEvent->GetSignal(i)->GetTime(j),fSignalEvent->GetSignal(i)->GetData(j));
	    }
	    		
	}
	
	//Only the maximum is plotted for the XY readout
	maxIndex = fSignalEvent->GetSignal(i)->GetMaxIndex( );
	charge= fSignalEvent->GetSignal(i)->GetData(maxIndex);
		
	   for( int px = 0; px < nPixels; px++ ){
           //cout<<module->GetPixelCenter( readoutChannel, px ).X()<<" "<<module->GetPixelCenter( readoutChannel, px ).Y()<<" "<<charge<<endl;
           fHistoXY->Fill(module->GetPixelCenter( readoutChannel, px ).X(),module->GetPixelCenter( readoutChannel, px ).Y(),charge);
            	}
		
	}

}


TRestReadoutChannel *TRestReadoutEventViewer::GetChannel( int readoutChannel){


	    for( int n = 0; n < fReadout->GetNumberOfModules( ); n++ ){
            
            if (fReadout->GetReadoutModule(n)->GetChannelByID(readoutChannel)==NULL)continue;
            return fReadout->GetReadoutModule(n)->GetChannelByID(readoutChannel);
            	    	    
	    }

cout<<"Readout channel "<<readoutChannel<<" not found"<<endl;
return NULL;

}

TRestReadoutModule *TRestReadoutEventViewer::GetModule( int readoutChannel){

	    for( int n = 0; n < fReadout->GetNumberOfModules( ); n++ ){
            
            if (fReadout->GetReadoutModule(n)->GetChannelByID(readoutChannel)==NULL)continue;
            return fReadout->GetReadoutModule(n);
            	    	    
	    }

cout<<"Readout channel "<<readoutChannel<<" not found"<<endl;
return NULL;

}



