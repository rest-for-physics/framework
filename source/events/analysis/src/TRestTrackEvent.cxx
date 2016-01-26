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
    fXYHit = NULL;
    fXZHit = NULL;
    fYZHit = NULL;
    fXYTrack = NULL;
    fXZTrack = NULL;
    fYZTrack = NULL;
    fPad = NULL;
    fCanvas = NULL;

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

Int_t TRestTrackEvent::GetTotalHits( )
{
    Int_t totHits = 0;
    for( int tck = 0; tck < GetNumberOfTracks(); tck++ )
        totHits += GetTrack( tck )->GetNumberOfHits( );
    return totHits;
}

void TRestTrackEvent::PrintEvent()
{
    TRestEvent::PrintEvent();
    for( int i = 0; i < GetNumberOfTracks(); i++ )
    {
        this->GetTrack(i)->PrintTrack();
    }
}

//Draw current event in a Tpad
TPad *TRestTrackEvent::DrawEvent()
{
    if( fXYHit != NULL ) { delete[] fXYHit; fXYHit=NULL;}
    if( fXZHit != NULL ) { delete[] fXZHit; fXZHit=NULL;}
    if( fYZHit != NULL ) { delete[] fYZHit; fYZHit=NULL;}
    if( fXYTrack != NULL ) { delete[] fXYTrack; fXYTrack=NULL;}
    if( fXZTrack != NULL ) { delete[] fXZTrack; fXZTrack=NULL;}
    if( fYZTrack != NULL ) { delete[] fYZTrack; fYZTrack=NULL;}
    if( fPad != NULL ) { delete fPad; fPad=NULL;}

    int nTracks = this->GetNumberOfTracks();

    cout << "Number of tracks " << nTracks << endl;

    if( nTracks == 0 )
    {
        cout<<"Empty event "<<endl;
        return NULL;
    }


    double maxX = -1e10, minX = 1e10, maxZ = -1e10, minZ = 1e10, maxY = -1e10, minY = 1e10;

    Int_t nTotHits = GetTotalHits( );
    fXYHit = new TGraph[nTotHits];
    fXZHit = new TGraph[nTotHits];
    fYZHit = new TGraph[nTotHits];
    fXYTrack = new TGraph[nTracks];
    fXZTrack = new TGraph[nTracks];
    fYZTrack = new TGraph[nTracks];

    int countXY = 0, countYZ = 0, countXZ = 0;
    int nTckXY = 0, nTckXZ = 0, nTckYZ = 0;

    Double_t minRadiusSize = 0.1;
    Double_t maxRadiusSize = 2.;

    for (int tck = 0; tck < nTracks; tck++)
    {
        TRestVolumeHits *hits = fTrack[tck].GetVolumeHits( );
        Double_t maxHitEnergy = hits->GetMaximumHitEnergy();
        Double_t meanHitEnergy = hits->GetMeanHitEnergy();
        cout << "Number of hits " << hits->GetNumberOfHits( ) <<endl;

        int tckXY = 0, tckYZ = 0, tckXZ = 0;

        for( int nhit = 0; nhit < hits->GetNumberOfHits( ); nhit++ )
        {
            Double_t x = hits->GetX( nhit );
            Double_t y = hits->GetY( nhit );
            Double_t z = hits->GetZ( nhit );
            Double_t en = hits->GetEnergy( nhit );

            Double_t m = (maxRadiusSize)/(maxHitEnergy-meanHitEnergy);
            Double_t n = (maxRadiusSize-minRadiusSize) - m * meanHitEnergy;

            Double_t radius = m * en + n;
            if( radius < 0.1 ) radius = 0.1;
            if( nTotHits > 200. ) radius = 0.5;

            if( x != 0 && y != 0 )
            {
                if( tckXY == 0 ) nTckXY++;
                fXYTrack[nTckXY-1].SetPoint( tckXY , x, y);
                fXYHit[countXY].SetPoint( 0 , x, y);
                fXYHit[countXY].SetMarkerColor(1 + tck);
                fXYHit[countXY].SetMarkerSize(radius);
                fXYHit[countXY].SetMarkerStyle(20);
                tckXY++;
                countXY++;
            }

            if( x != 0 && z != 0 )
            {
                if( tckXZ == 0 ) nTckXZ++;
                fXZTrack[nTckXZ-1].SetPoint( tckXZ , x, z);
                fXZHit[countXZ].SetPoint( 0 , x, z);
                fXZHit[countXZ].SetMarkerColor(1 + tck);
                fXZHit[countXZ].SetMarkerSize(radius);
                fXZHit[countXZ].SetMarkerStyle(20);
                tckXZ++;
                countXZ++;
            }

            if( y!= 0 && z != 0 )
            {
                if( tckYZ == 0 ) nTckYZ++;
                fYZTrack[nTckYZ-1].SetPoint( tckYZ , y, z);
                fYZHit[countYZ].SetPoint( countYZ , y, z);
                fYZHit[countYZ].SetMarkerColor(1 + tck);
                fYZHit[countYZ].SetMarkerSize(radius);
                fYZHit[countYZ].SetMarkerStyle(20);
                tckYZ++;
                countYZ++;
            }

            if( x > maxX ) maxX = x;
            if( x < minX ) minX = x;
            if( y > maxY ) maxY = y;
            if( y < minY ) minY = y;
            if( z > maxZ ) maxZ = z;
            if( z < minZ ) minZ = z;
        }
    }


    fPad = new TPad(this->GetClassName().Data(), " ", 0, 0, 1, 1 );
    fPad->Divide( 3 , 1 );
    fPad->Draw( );

    fPad->cd(1)->DrawFrame( minX-10 , minY-10, maxX+10, maxY+10 );
    fPad->cd(2)->DrawFrame( minX-10 , minZ-10, maxX+10, maxZ+10 );
    fPad->cd(3)->DrawFrame( minY-10 , minZ-10, maxY+10, maxZ+10 );

    for( int i = 0; i < countXY; i++ )
    {
        fPad->cd(1); 
        fXYHit[i].Draw("P");
    }

    for( int i = 0; i < countXZ; i++ )
    {
        fPad->cd(2); 
        fXZHit[i].Draw("P");
    }

    for( int i = 0; i < countYZ; i++ )
    {
        fPad->cd(3); 
        fYZHit[i].Draw("P");
    }

    if( nTotHits < 50. )
    {
        for( int tck = 0; tck < nTckXY; tck++ )
        {
            fPad->cd(1);
            fXYTrack[tck].SetLineWidth(2.);
            fXYTrack[tck].Draw("L");
        }

        for( int tck = 0; tck < nTckXZ; tck++ )
        {
            fPad->cd(2);
            fXZTrack[tck].SetLineWidth(2.);
            fXZTrack[tck].Draw("L");
        }

        for( int tck = 0; tck < nTckYZ; tck++ )
        {
            fPad->cd(3);
            fYZTrack[tck].SetLineWidth(2.);
            fYZTrack[tck].Draw("L");
        }
    }

    return fPad;
}
