///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackEvent.h
///
///             Event class to store DAQ events either from simulation and acquisition 
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Gracia
///_______________________________________________________________________________


#include "TRestTrackEvent.h"

ClassImp(TRestTrackEvent)
//______________________________________________________________________________
    TRestTrackEvent::TRestTrackEvent()
{
   // TRestTrackEvent default constructor
    TRestEvent::Initialize();
    fEventClassName = "TRestTrackEvent";
    fTrack.clear();
    fXY=NULL;
    fXZ=NULL;
    fYZ=NULL;
    fPad=NULL;

}

//______________________________________________________________________________
TRestTrackEvent::~TRestTrackEvent()
{
   // TRestTrackEvent destructor
}

void TRestTrackEvent::Initialize()
{
    fNtracks = 0;
    fTrack.clear();
    TRestEvent::Initialize();
    fEventClassName = "TRestTrackEvent";

}

void TRestTrackEvent::PrintEvent()
{
    TRestEvent::PrintEvent();
}


//Draw current event in a Tpad
TPad *TRestTrackEvent::DrawEvent()
{

    cout<<"Drawing event"<<endl;

    if( fXY != NULL ) { delete[] fXY; fXY=NULL;}
    if( fXZ != NULL ) { delete[] fXZ; fXZ=NULL;}
    if( fYZ != NULL ) { delete[] fYZ; fYZ=NULL;}
    if( fPad != NULL ) { delete fPad; fPad=NULL;}

    int nTracks = this->GetNumberOfTracks();

    cout << "Number of tracks " << nTracks << endl;

    if( nTracks == 0 )
    {
        cout<<"Empty event "<<endl;
        return NULL;
    }

    TRestVolumeHits hits;

    double maxX=-1e10, minX = 1e10, maxZ=-1e10, minZ=1e10, maxY=-1e10, minY=1e10 ;
    int count;

    fXY = new TGraph[nTracks];
    fXZ = new TGraph[nTracks];
    fYZ = new TGraph[nTracks];

    for (int j = 0; j< nTracks; j++)
    {
        hits = fTrack[j].GetVolumeHits( );
        cout << "Number of hits " << hits.GetNumberOfHits( ) <<endl;

        count = 0;

        for( int nhit = 0; nhit < hits.GetNumberOfHits( ); nhit++ )
        {
            Double_t x = hits.GetX( nhit );
            Double_t y = hits.GetY( nhit );
            Double_t z = hits.GetZ( nhit );
            Double_t en = hits.GetEnergy( nhit );

            cout << x << " " << y << " " << z << ": energy " << en << endl;

            if( x != 0 && y != 0 )
                fXY[j].SetPoint(count, x, y);
            if( x != 0 )
                fXZ[j].SetPoint(count, x, z);
            if( y!= 0 )
                fYZ[j].SetPoint(count, y, z);

            if( x > maxX ) maxX = x;
            if( x < minX ) minX = x;
            if( y > maxY ) maxY = y;
            if( y < minY ) minY = y;
            if( z > maxZ ) maxZ = z;
            if( z < minZ ) minZ = z;

            count++;
        }
    }


    fPad = new TPad(this->GetClassName().Data()," ",0,0,1,1);
    fPad->Divide(3,1);
    fPad->Draw();

    fPad->cd(1)->DrawFrame(minX-10,minY-10,maxX+10,maxY+10);
    fPad->cd(2)->DrawFrame(minX-10,minZ-10,maxX+10,maxZ+10);
    fPad->cd(3)->DrawFrame(minY-10,minZ-10,maxY+10,maxZ+10);


    for(int ntr=0;ntr<nTracks;ntr++)
    {
        fXY[ntr].SetMarkerColor(1+ntr);
        fXY[ntr].SetMarkerSize(1.);
        fXY[ntr].SetMarkerStyle(21);
        fXZ[ntr].SetMarkerColor(1+ntr);
        fXZ[ntr].SetMarkerSize(1.);
        fXZ[ntr].SetMarkerStyle(21);
        fYZ[ntr].SetMarkerColor(1+ntr);
        fYZ[ntr].SetMarkerSize(1.);
        fYZ[ntr].SetMarkerStyle(21);

        fPad->cd(1); 
        fXY[ntr].Draw("P");

        fPad->cd(2); 
        fXZ[ntr].Draw("P");

        fPad->cd(3); 
        fYZ[ntr].Draw("P");

    }

    return fPad;
}
