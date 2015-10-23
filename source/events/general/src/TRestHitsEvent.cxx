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

void TRestHitsEvent::RemoveHits( )
{
    //cout << "Removing hits" << endl;
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
