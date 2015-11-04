#include "TRestHitsEvent.h"

ClassImp(TRestHitsEvent)

TRestHitsEvent::TRestHitsEvent()
{
    fNHits = 0;
    fTotEnergy = 0;
}

TRestHitsEvent::~TRestHitsEvent()
{
}

void TRestHitsEvent::AddHit( Double_t x, Double_t y, Double_t z, Double_t en )
{
    fNHits++;
    fX.push_back( (Int_t) (x*1000) );
    fY.push_back( (Int_t) (y * 1000.) );
    fZ.push_back( (Int_t) ( z * 1000. ));
    fEnergy.push_back( (Int_t) ( en * 1000. ) );

    fTotEnergy += en;
}

void TRestHitsEvent::AddHit( TVector3 pos, Double_t en )
{
    //cout << "Adding hit : " << fNHits << endl;
    fNHits++;

    fX.push_back( (Int_t) (pos.X()*1000) );
    fY.push_back( (Int_t) (pos.Y() * 1000.) );
    fZ.push_back( (Int_t) ( pos.Z() * 1000. ));
    fEnergy.push_back( (Int_t) ( en * 1000. ) );

    fTotEnergy += en;
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
    fX[n] = fX[n]*.5 + fX[m]*.5;
    fY[n] = fY[n]*.5 + fY[m]*.5;
    fZ[n] = fZ[n]*.5 + fZ[m]*.5;

    fEnergy[n] += fEnergy[m];

    RemoveHit( m );
}

void TRestHitsEvent::RemoveHit( int n )
{
    fNHits--;
    fX.erase(fX.begin()+n);
    fY.erase(fY.begin()+n);
    fZ.erase(fZ.begin()+n);
    fEnergy.erase(fEnergy.begin()+n);
}

void TRestHitsEvent::RemoveHits( )
{
    fNHits = 0;
    fX.clear();
    fY.clear();
    fZ.clear();
    fEnergy.clear();
    fTotEnergy = 0;
}


void TRestHitsEvent::ChangeOrigin(double origx, double origy, double origz)
{
    /*
	for(int i = 0; i < fNHits; i++)
	{
		fX.fArray[i] += origx;
		fY.fArray[i] += origy;
		fZ.fArray[i] += origz;
	}
    */
}
