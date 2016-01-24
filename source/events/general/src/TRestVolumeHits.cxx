///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestVolumeHits.h
///
///             Event class to store signals form simulation and acquisition events 
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Gracia
///_______________________________________________________________________________


#include "TRestVolumeHits.h"

ClassImp(TRestVolumeHits)
//______________________________________________________________________________
    TRestVolumeHits::TRestVolumeHits()
{
   // TRestVolumeHits default constructor
}

//______________________________________________________________________________
TRestVolumeHits::~TRestVolumeHits()
{
   // TRestVolumeHits destructor
}


void TRestVolumeHits::AddHit( Double_t x, Double_t y, Double_t z, Double_t en,  Double_t sigmax, Double_t sigmay, Double_t sigmaz )
{
    TRestHits::AddHit( x, y, z, en);
    fSigmaX.push_back( (Int_t) (sigmax * 1000.));
    fSigmaY.push_back( (Int_t) (sigmay * 1000.));
    fSigmaZ.push_back( (Int_t) (sigmaz * 1000.));

}

void TRestVolumeHits::AddHit( TVector3 pos, Double_t en,  TVector3 sigma  )
{

    TRestHits::AddHit( pos , en);

    fSigmaX.push_back( (Int_t) (sigma.X() * 1000.) );
    fSigmaY.push_back( (Int_t) (sigma.Y() * 1000.) );
    fSigmaZ.push_back( (Int_t) (sigma.Z() * 1000.) );
}

void TRestVolumeHits::RemoveHits( )
{
    //cout << "Removing hits" << endl;
     TRestHits::RemoveHits();
     fSigmaX.clear();
     fSigmaY.clear();
     fSigmaZ.clear();
}

void TRestVolumeHits::MergeHits( Int_t n, Int_t m )
{
    Double_t totalEnergy = fEnergy[n] + fEnergy[m];

    fSigmaX[n] = (fSigmaX[n]*fEnergy[n] + fSigmaX[m]*fEnergy[m])/totalEnergy;
    fSigmaY[n] = (fSigmaY[n]*fEnergy[n] + fSigmaY[m]*fEnergy[m])/totalEnergy;
    fSigmaZ[n] = (fSigmaZ[n]*fEnergy[n] + fSigmaZ[m]*fEnergy[m])/totalEnergy;

    TRestHits::MergeHits( n, m );
}

void TRestVolumeHits::RemoveHit( int n )
{
    TRestHits::RemoveHit(n);

    fSigmaX.erase(fSigmaX.begin()+n);        ;
    fSigmaY.erase(fSigmaY.begin()+n);        ;
    fSigmaZ.erase(fSigmaZ.begin()+n);   ;
}

void TRestVolumeHits::PrintHits( )
{
	for( int n = 0; n < GetNumberOfHits(); n++ )
	{
		cout << "Hit " << n << " X: " << GetX(n) << " sY: " << GetY(n) << " sZ: " << GetZ(n) <<  " sX: " << GetSigmaX(n) << " sY: " << GetSigmaY(n) << " sZ: " << GetSigmaZ(n) <<  " Energy: " << GetEnergy(n) << endl;
	}

}
