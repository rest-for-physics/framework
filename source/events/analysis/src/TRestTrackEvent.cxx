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
///
///	       
///_______________________________________________________________________________


#include "TRestTrackEvent.h"
using namespace std;

ClassImp(TRestTrackEvent)
//______________________________________________________________________________
    TRestTrackEvent::TRestTrackEvent()
{
   // TRestTrackEvent default constructor
    TRestEvent::Initialize();
    fTrack.clear();
    fXYHit = NULL;
    fXZHit = NULL;
    fYZHit = NULL;
    fXYZHit = NULL;
    fXYTrack = NULL;
    fXZTrack = NULL;
    fYZTrack = NULL;
    fXYZTrack = NULL;
    fPad = NULL;
    fLevels = -1;

}

//______________________________________________________________________________
TRestTrackEvent::~TRestTrackEvent()
{
   // TRestTrackEvent destructor
}

void TRestTrackEvent::Initialize()
{
    fNtracks = 0;
    fNtracksX = 0;
    fNtracksY = 0;
    fTrack.clear();
    TRestEvent::Initialize();

}

TRestTrack *TRestTrackEvent::GetTrackById( Int_t id )
{
    for( int i = 0; i < GetNumberOfTracks(); i++ )
        if( GetTrack( i )->GetTrackID() == id ) return GetTrack( i );
    return NULL;
}

TRestTrack *TRestTrackEvent::GetMaxEnergyTrackInX( )
{
    Int_t track = -1;
    Double_t maxEnergy = 0;
    for( int tck = 0; tck < GetNumberOfTracks(); tck++ )
    {
        TRestTrack *t = GetTrack( tck );
        if( t->isXZ() )
        {
            if ( t->GetEnergy() > maxEnergy )
            {
                maxEnergy = t->GetEnergy();
                track = tck;
            }
        }
    }

    if ( track == -1 ) return NULL;

    return GetTrack( track );
}

TRestTrack *TRestTrackEvent::GetMaxEnergyTrackInY( )
{
    Int_t track = -1;
    Double_t maxEnergy = 0;
    for( int tck = 0; tck < GetNumberOfTracks(); tck++ )
    {
        TRestTrack *t = GetTrack( tck );
        if( t->isYZ() )
        {
            if ( t->GetEnergy() > maxEnergy )
            {
                maxEnergy = t->GetEnergy();
                track = tck;
            }
        }
    }

    if ( track == -1 ) return NULL;

    return GetTrack( track );
}

TRestTrack *TRestTrackEvent::GetLongestTopLevelTrack()
{
    Int_t found = 0;
    Double_t len = 0;
    Int_t theTrack = 0;

    for( int tck = 0; tck < GetNumberOfTracks(); tck++ )
    {
        if( this->isTopLevel( tck ) )
        {
            Double_t l = this->GetTrack(tck)->GetTrackLength();

            if( l > len )
            {
                len = l;
                theTrack = tck;
                found = 1;
            }
        }
    }

    if( found == 0 )
    {
        cout << "REST warning! A track was not found!" << endl;
        return NULL;
    }
    return GetTrack( theTrack );

}


Int_t TRestTrackEvent::GetTotalHits( )
{
    Int_t totHits = 0;
    for( int tck = 0; tck < GetNumberOfTracks(); tck++ )
        totHits += GetTrack( tck )->GetNumberOfHits( );
    return totHits;
}

Int_t TRestTrackEvent::GetLevel( Int_t tck )
{
    Int_t lvl = 1;
    Int_t parentTrackId = GetTrack( tck )->GetParentID();

    while( parentTrackId > 0 )
    {
        lvl++;
        parentTrackId = GetTrackById( parentTrackId )->GetParentID();
    }
    return lvl;
}

Bool_t TRestTrackEvent::isTopLevel( Int_t tck )
{
    if( GetLevels() == GetLevel( tck ) )
        return true;
    return false;
}

Int_t TRestTrackEvent::GetOriginTrackID( Int_t tck )
{
    Int_t originTrackID = tck;
    Int_t pID = GetTrack(tck)->GetParentID();

    while( pID != 0 )
    {
        originTrackID = pID;
        pID = GetTrackById(originTrackID)->GetParentID();
    }

    return originTrackID;
}

void TRestTrackEvent::SetLevels( )
{
    Int_t maxLevel = 0;

    for( int tck = 0; tck < GetNumberOfTracks(); tck++ )
    {
        Int_t lvl = GetLevel( tck );
        if( maxLevel < lvl ) maxLevel = lvl;
    }
    fLevels = maxLevel;
}



void TRestTrackEvent::PrintOnlyTracks()
{
    cout << "TrackEvent " << GetID() << endl;
    cout << "-----------------------" << endl;
    for( int i = 0; i < GetNumberOfTracks(); i++ )
    {
        cout << "Track " << i << " id : " << GetTrack(i)->GetTrackID() << " parent : " << GetTrack(i)->GetParentID() << endl;
    }
    cout << "-----------------------" << endl;
    cout << "Track levels : " << GetLevels() << endl;

}

void TRestTrackEvent::PrintEvent( Bool_t fullInfo )
{
    TRestEvent::PrintEvent();

    cout << "Number of tracks : " << GetNumberOfTracks() << endl;
    cout << "Number of tracks XZ " << fNtracksX << endl;
    cout << "Number of tracks YZ " << fNtracksY << endl;
    cout << "Track levels : " << GetLevels() << endl;
    cout << "+++++++++++++++++++++++++++++++++++" << endl;
    for( int i = 0; i < GetNumberOfTracks(); i++ )
        this->GetTrack(i)->PrintTrack( fullInfo );
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

    /*
    PrintOnlyTracks();
    PrintEvent();
    getchar();
    */

    if( nTracks == 0 )
    {
        cout<<"Empty event "<<endl;
        return NULL;
    }

    this->PrintEvent( false );

    double maxX = -1e10, minX = 1e10, maxZ = -1e10, minZ = 1e10, maxY = -1e10, minY = 1e10;

    Int_t nTotHits = GetTotalHits( );
    fXYHit = new TGraph[nTotHits];
    fXZHit = new TGraph[nTotHits];
    fYZHit = new TGraph[nTotHits];
    fXYZHit = new TGraph2D[nTotHits];
    fXYTrack = new TGraph[nTracks];
    fXZTrack = new TGraph[nTracks];
    fYZTrack = new TGraph[nTracks];
    fXYZTrack = new TGraph2D[nTracks];

    Int_t drawLinesXY[nTracks];
    Int_t drawLinesXZ[nTracks];
    Int_t drawLinesYZ[nTracks];
    Int_t drawLinesXYZ[nTracks];

    for( int i = 0; i < nTracks; i++ )
    {
        drawLinesXY[i] = 0;
        drawLinesXZ[i] = 0;
        drawLinesYZ[i] = 0;
    }

    int countXY = 0, countYZ = 0, countXZ = 0, countXYZ = 0;
    int nTckXY = 0, nTckXZ = 0, nTckYZ = 0, nTckXYZ = 0;

    Double_t minRadiusSize = 0.4;
    Double_t maxRadiusSize = 2.;

    Int_t maxTrackHits = 0;

    Int_t tckColor = 1;

    for (int tck = 0; tck < nTracks; tck++)
    {
        TRestVolumeHits *hits = fTrack[tck].GetVolumeHits( );


        Double_t maxHitEnergy = hits->GetMaximumHitEnergy();
        Double_t meanHitEnergy = hits->GetMeanHitEnergy();

        /*
        cout << "Max hit energy : " << maxHitEnergy << endl;
        cout << "Mean hit energy : " << meanHitEnergy << endl;
        cout << "Number of hits " << hits->GetNumberOfHits( ) <<endl;
        */


        Bool_t isTopLevel = this->isTopLevel( tck );
        if( isTopLevel ) tckColor++;
        Int_t level = this->GetLevel( tck );

        int tckXY = 0, tckYZ = 0, tckXZ = 0, tckXYZ = 0;
        Double_t radius;

        for( int nhit = 0; nhit < hits->GetNumberOfHits( ); nhit++ )
        {
            if( hits->GetNumberOfHits() > maxTrackHits ) maxTrackHits = hits->GetNumberOfHits();

            Double_t x = hits->GetX( nhit );
            Double_t y = hits->GetY( nhit );
            Double_t z = hits->GetZ( nhit );
            Double_t en = hits->GetEnergy( nhit );

            /* {{{ Hit size definition (radius) */
            Double_t m = (maxRadiusSize)/(maxHitEnergy-meanHitEnergy);
            Double_t n = (maxRadiusSize-minRadiusSize) - m * meanHitEnergy;

            if( isTopLevel )
            {
                radius = m * en + n;
                if( radius < 0.1 ) radius = 0.1;
                if( hits->GetNumberOfHits() == 1 ) radius = 2;
                if( hits->GetNumberOfHits() > 100 ) radius = 0.8;

            }
            else
            {
                radius = 1.5 * minRadiusSize * level;
            }
            /* }}} */

            if( this->isXYZ() && nhit > 1 )
            {
                if( tckXYZ == 0 ) nTckXYZ++;
                fXYZTrack[nTckXYZ-1].SetPoint( tckXYZ , x, y, z);

                if( isTopLevel ) drawLinesXYZ[nTckXYZ-1] = 1;

                if( isTopLevel )
                {
                    fXYZHit[countXYZ].SetPoint( 0 , x, y, z);
                    // If there is only one-point the TGraph2D does NOT draw the point!
                    fXYZHit[countXYZ].SetPoint( 1, x+0.001, y+0.001, z+0.001 );

                    if( !isTopLevel ) fXYZHit[countXYZ].SetMarkerColor( level + 11 );
                    else fXYZHit[countXYZ].SetMarkerColor( tckColor );

                    fXYZHit[countXYZ].SetMarkerSize(radius);
                    fXYZHit[countXYZ].SetMarkerStyle(20);
                    countXYZ++;
                }
                tckXYZ++;
            }

            if( x != 0 && y != 0 )
            {
                if( tckXY == 0 ) nTckXY++;
                fXYTrack[nTckXY-1].SetPoint( tckXY , x, y);
                if( isTopLevel ) drawLinesXY[nTckXY-1] = 1;
                fXYHit[countXY].SetPoint( 0 , x, y);

                if( !isTopLevel ) fXYHit[countXY].SetMarkerColor( level + 11 );
                else fXYHit[countXY].SetMarkerColor( tckColor );
                
                fXYHit[countXY].SetMarkerSize(radius);
                fXYHit[countXY].SetMarkerStyle(20);
                tckXY++;
                countXY++;
            }

            if( x != 0 && z != 0 )
            {
                if( tckXZ == 0 ) nTckXZ++;
                fXZTrack[nTckXZ-1].SetPoint( tckXZ , x, z);
                if( isTopLevel ) drawLinesXZ[nTckXZ-1] = 1;
                fXZHit[countXZ].SetPoint( 0 , x, z);

                if( !isTopLevel ) fXZHit[countXZ].SetMarkerColor( level + 11 );
                else fXZHit[countXZ].SetMarkerColor( tckColor );
                
                fXZHit[countXZ].SetMarkerSize(radius);
                fXZHit[countXZ].SetMarkerStyle(20);
                tckXZ++;
                countXZ++;
            }

            if( y!= 0 && z != 0 )
            {
                if( tckYZ == 0 ) nTckYZ++;
                fYZTrack[nTckYZ-1].SetPoint( tckYZ , y, z);
                if( isTopLevel ) drawLinesYZ[nTckYZ-1] = 1;
                fYZHit[countYZ].SetPoint( countYZ , y, z);

                if( !isTopLevel ) fYZHit[countYZ].SetMarkerColor( level + 11 );
                else fYZHit[countYZ].SetMarkerColor( tckColor );

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

    fPad = new TPad(this->GetName(), " ", 0, 0, 1, 1 );
    if( this->isXYZ() )
        fPad->Divide( 2 , 2 );
    else
        fPad->Divide( 2 , 1 );
    fPad->Draw( );

    char title[256];
    sprintf(title, "Event ID %d", this->GetID());

    TMultiGraph *mgXY = new TMultiGraph();
    TMultiGraph *mgXZ = new TMultiGraph();
    TMultiGraph *mgYZ = new TMultiGraph();

    fPad->cd(1)->DrawFrame( minX-10 , minZ-10, maxX+10, maxZ+10, title );
    fPad->cd(2)->DrawFrame( minY-10 , minZ-10, maxY+10, maxZ+10, title );

    if( this->isXYZ() )
        fPad->cd(3)->DrawFrame( minX-10 , minY-10, maxX+10, maxY+10, title);

    for( int i = 0; i < countXZ; i++ )
        mgXZ->Add(&fXZHit[i]);

    fPad->cd(1); 
    mgXZ->GetXaxis()->SetTitle("X-axis (mm)");
    mgXZ->GetYaxis()->SetTitle("Z-axis (mm)");
    mgXZ->Draw("P");


    for( int i = 0; i < countYZ; i++ )
        mgYZ->Add(&fYZHit[i]);

    fPad->cd(2); 
    mgYZ->GetXaxis()->SetTitle("Y-axis (mm)");
    mgYZ->GetYaxis()->SetTitle("Z-axis (mm)");
    mgYZ->Draw("P");

    if( this->isXYZ() )
    {
        for( int i = 0; i < countXY; i++ )
            mgXY->Add(&fXYHit[i]);

        fPad->cd(3); 
        mgXY->GetXaxis()->SetTitle("X-axis (mm)");
        mgXY->GetYaxis()->SetTitle("Y-axis (mm)");
        mgXY->Draw("P");
    }

    for( int tck = 0; tck < nTckXZ; tck++ )
    {
        fPad->cd(1);
        fXZTrack[tck].SetLineWidth(2.);
        if( fXZTrack[tck].GetN() < 100 && drawLinesXZ[tck] == 1 )
            fXZTrack[tck].Draw("L");
    }

    for( int tck = 0; tck < nTckYZ; tck++ )
    {
        fPad->cd(2);
        fYZTrack[tck].SetLineWidth(2.);
        if( fYZTrack[tck].GetN() < 100 && drawLinesYZ[tck] == 1 )
            fYZTrack[tck].Draw("L");
    }

    if( this->isXYZ() )
    {
        for( int tck = 0; tck < nTckXY; tck++ )
        {
            fPad->cd(3);
            fXYTrack[tck].SetLineWidth(2.);
            if( fXYTrack[tck].GetN() < 100 && drawLinesXY[tck] == 1 )
                fXYTrack[tck].Draw("L");
        }

        fPad->cd(4);

        TString option = "P";
        for( int tck = 0; tck < nTckXYZ; tck++ )
        {
            fXYZTrack[tck].SetLineWidth(2.);
            if(fXZTrack[tck].GetN() < 100 && drawLinesXYZ[tck] == 1 )
            {
                fXYZTrack[tck].Draw("LINE");
                option = "same P";
            }
        }

        for( int i = 0; i < countXYZ; i++ )
        {
            if( i > 0 ) option = "same P";
            fXYZHit[i].Draw( option );
        }
    }

    return fPad;
}

