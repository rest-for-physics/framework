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
using namespace std;

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
    fTrack.clear();
    TRestEvent::Initialize();
    fEventClassName = "TRestTrackEvent";

}

TRestTrack *TRestTrackEvent::GetTrackById( Int_t id )
{
    for( int i = 0; i < GetNumberOfTracks(); i++ )
        if( GetTrack( i )->GetTrackID() == id ) return GetTrack( i );
    return NULL;
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
    cout << "TrackEvent " << GetEventID() << endl;
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
    fXYTrack = new TGraph[nTracks];
    fXZTrack = new TGraph[nTracks];
    fYZTrack = new TGraph[nTracks];

    Int_t drawLinesXY[nTracks];
    Int_t drawLinesXZ[nTracks];
    Int_t drawLinesYZ[nTracks];

    for( int i = 0; i < nTracks; i++ )
    {
        drawLinesXY[i] = 0;
        drawLinesXZ[i] = 0;
        drawLinesYZ[i] = 0;
    }

    int countXY = 0, countYZ = 0, countXZ = 0;
    int nTckXY = 0, nTckXZ = 0, nTckYZ = 0;

    Double_t minRadiusSize = 0.4;
    Double_t maxRadiusSize = 2.;

    Int_t maxTrackHits = 0;
    Int_t trackLevels = this->GetLevels();

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

        int tckXY = 0, tckYZ = 0, tckXZ = 0;
        Double_t radius;

        for( int nhit = 0; nhit < hits->GetNumberOfHits( ); nhit++ )
        {
            if( hits->GetNumberOfHits() > maxTrackHits ) maxTrackHits = hits->GetNumberOfHits();
            Double_t x = hits->GetX( nhit );
            Double_t y = hits->GetY( nhit );
            Double_t z = hits->GetZ( nhit );
            Double_t en = hits->GetEnergy( nhit );

            Double_t m = (maxRadiusSize)/(maxHitEnergy-meanHitEnergy);
            Double_t n = (maxRadiusSize-minRadiusSize) - m * meanHitEnergy;

            if( isTopLevel )
            {
                radius = m * en + n;
                if( radius < 0.1 ) radius = 0.1;
                if( hits->GetNumberOfHits() == 1 ) radius = 2;
                if( trackLevels == 1 ) radius = 0.8;

            }
            else
            {
                radius = 1.5 * minRadiusSize * level;
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

    for( int tck = 0; tck < nTckXY; tck++ )
    {
        fPad->cd(1);
        fXYTrack[tck].SetLineWidth(2.);
        if( fXYTrack[tck].GetN() < 50 && drawLinesXY[tck] == 1 )
            fXYTrack[tck].Draw("L");
    }

    for( int tck = 0; tck < nTckXZ; tck++ )
    {
        fPad->cd(2);
        fXZTrack[tck].SetLineWidth(2.);
        if( fXZTrack[tck].GetN() < 50 && drawLinesXZ[tck] == 1 )
            fXZTrack[tck].Draw("L");
    }

    for( int tck = 0; tck < nTckYZ; tck++ )
    {
        fPad->cd(3);
        fYZTrack[tck].SetLineWidth(2.);
        if( fYZTrack[tck].GetN() < 50 && drawLinesYZ[tck] == 1 )
            fYZTrack[tck].Draw("L");
    }

    return fPad;
}
