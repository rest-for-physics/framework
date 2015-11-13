#include "TRestHits.h"

ClassImp(TRestHits)

TRestHits::TRestHits()
{
    fNHits = 0;
    fTotEnergy = 0;
}

TRestHits::~TRestHits()
{
}

void TRestHits::AddHit( Double_t x, Double_t y, Double_t z, Double_t en )
{
    fNHits++;
    fX.push_back( (Float_t) (x) );
    fY.push_back( (Float_t) (y ));
    fZ.push_back( (Float_t) ( z));
    fEnergy.push_back( (Float_t) ( en ) );

    fTotEnergy += en;
}

void TRestHits::AddHit( TVector3 pos, Double_t en )
{
    //cout << "Adding hit : " << fNHits << endl;
    fNHits++;

    fX.push_back( (Float_t) (pos.X()) );
    fY.push_back( (Float_t) (pos.Y() ));
    fZ.push_back( (Float_t) ( pos.Z()  ));
    fEnergy.push_back( (Float_t) ( en ) );

    fTotEnergy += en;
}

void TRestHits::RemoveHits( )
{
    //cout << "Removing hits" << endl;
    fNHits = 0;
    fX.clear();
    fY.clear();
    fZ.clear();
    fEnergy.clear();
    fTotEnergy = 0;
}


void TRestHits::ChangeOrigin(double origx, double origy, double origz)
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
