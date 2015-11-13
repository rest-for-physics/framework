#include "TRestHitsEvent.h"

ClassImp(TRestHitsEvent)

TRestHitsEvent::TRestHitsEvent()
{

fHits = new TRestHits();
    
}

TRestHitsEvent::~TRestHitsEvent()
{
delete fHits;
}

void TRestHitsEvent::AddHit( Double_t x, Double_t y, Double_t z, Double_t en )
{
    fHits->AddHit(x,y,z,en);
}

void TRestHitsEvent::AddHit( TVector3 pos, Double_t en )
{
    fHits->AddHit(pos,en);
}

void TRestHitsEvent::Initialize()
{
    TRestEvent::Initialize();
    fEventClassName = "TRestHitsEvent";

    RemoveHits();

}

Double_t TRestHitsEvent::GetDistance2( int n, int m )
{
    return (GetX(n)-GetX(m))*(GetX(n)-GetX(m)) +  (GetY(n)-GetY(m))*(GetY(n)-GetY(m)) + (GetZ(n)-GetZ(m))*(GetZ(n)-GetZ(m));
}

void TRestHitsEvent::MergeHits( int n, int m )
{
    fHits->MergeHits( n, m );
}

void TRestHitsEvent::RemoveHit( int n )
{
    fHits->RemoveHit(n);
}

void TRestHitsEvent::RemoveHits( )
{
    fHits->RemoveHits( );
}


void TRestHitsEvent::ChangeOrigin(double origx, double origy, double origz)
{
    /*
	for(int i = 0; i < fHits; i++)
	{
		fX.fArray[i] += origx;
		fY.fArray[i] += origy;
		fZ.fArray[i] += origz;
	}
    */
}


void TRestHitsEvent::PrintEvent()
{
	//TRestEvent::PrintEvent();
	fHits->PrintEvent();


}
