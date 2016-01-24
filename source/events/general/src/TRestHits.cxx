///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHits.cxx
///
///             Event class to store hits 
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///		nov 2015:
///		    Changed vectors fX fY fZ and fEnergy from <Int_t> to <Float_t>
///	            JuanAn Garcia
///_______________________________________________________________________________


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

void TRestHits::MergeHits( int n, int m )
{
    Double_t totalEnergy = fEnergy[n] + fEnergy[m];
    fX[n] = (fX[n]*fEnergy[n] + fX[m]*fEnergy[m])/totalEnergy;
    fY[n] = (fY[n]*fEnergy[n] + fY[m]*fEnergy[m])/totalEnergy;
    fZ[n] = (fZ[n]*fEnergy[n] + fZ[m]*fEnergy[m])/totalEnergy;

    fEnergy[n] += fEnergy[m];

    RemoveHit( m );
}

void TRestHits::RemoveHit( int n )
{
    fNHits--;
    fX.erase(fX.begin()+n);
    fY.erase(fY.begin()+n);
    fZ.erase(fZ.begin()+n);
    fEnergy.erase(fEnergy.begin()+n);
}

Double_t TRestHits::GetDistance2( int n, int m )
{
    return (GetX(n)-GetX(m))*(GetX(n)-GetX(m)) +  (GetY(n)-GetY(m))*(GetY(n)-GetY(m)) + (GetZ(n)-GetZ(m))*(GetZ(n)-GetZ(m));
}

void TRestHits::PrintEvent()
{
	//TRestEvent::PrintEvent();
	for( int n = 0; n < GetNumberOfHits(); n++ )
	{
		cout << "Hit " << n << " X: " << GetX(n) << " Y: " << GetY(n) << " Z: " << GetZ(n) <<  " Energy: " << GetEnergy(n) << endl;
	}


}

