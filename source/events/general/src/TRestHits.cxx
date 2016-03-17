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
using namespace std;
using namespace TMath;

ClassImp(TRestHits)

TRestHits::TRestHits()
{
    fNHits = 0;
    fTotEnergy = 0;
}

TRestHits::~TRestHits()
{
}

Bool_t TRestHits::areXY()
{
    //for( int i = 0; i < GetNumberOfHits(); i++ )
    if( IsNaN( GetZ(0) ) ) return true;
    return false;
}

Bool_t TRestHits::areXZ()
{
    if( IsNaN( GetY(0) ) ) return true;
    return false;
}

Bool_t TRestHits::areYZ()
{
    if( IsNaN( GetX(0) ) ) return true;
    return false;
}

Bool_t TRestHits::areXYZ()
{
    if( IsNaN ( GetX(0) ) || IsNaN( GetY(0) ) || IsNaN( GetZ(0) ) )  return false;
    return true;
}

void TRestHits::GetXArray( Float_t *x )
{
    if( areYZ() )
    {
        for( int i = 0; i < GetNumberOfHits(); i++ )
            x[i] = 0;
    }
    else
    {
        for( int i = 0; i < GetNumberOfHits(); i++ )
            x[i] = GetX(i);
    }
}

void TRestHits::GetYArray( Float_t *y )
{
    if( areXZ() )
    {
        for( int i = 0; i < GetNumberOfHits(); i++ )
            y[i] = 0;
    }
    else
    {
        for( int i = 0; i < GetNumberOfHits(); i++ )
            y[i] = GetY(i);
    }
}

void TRestHits::GetZArray( Float_t *z )
{
    if( areXY() )
    {
        for( int i = 0; i < GetNumberOfHits(); i++ )
            z[i] = 0;
    }
    else
    {
        for( int i = 0; i < GetNumberOfHits(); i++ )
            z[i] = GetZ(i);
    }
}

Double_t TRestHits::GetEnergyIntegral()
{
    Double_t sum = 0;
    for( int i = 0; i < GetNumberOfHits(); i++ )
        sum += GetEnergy(i);
    return sum;
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
    fNHits++;

    fX.push_back( (Float_t) (pos.X()) );
    fY.push_back( (Float_t) (pos.Y() ));
    fZ.push_back( (Float_t) ( pos.Z()  ));
    fEnergy.push_back( (Float_t) ( en ) );

    fTotEnergy += en;
}

void TRestHits::RemoveHits( )
{
    fNHits = 0;
    fX.clear();
    fY.clear();
    fZ.clear();
    fEnergy.clear();
    fTotEnergy = 0;
}


void TRestHits::Traslate(Int_t n, double deltax, double deltay, double deltaz)
{
	fX[n] += deltax;
	fY[n] += deltay;
	fZ[n] += deltaz;
}



void TRestHits::RotateIn3D(Int_t n, Double_t alpha, Double_t beta, Double_t gamma, TVector3 vMean)
{

    TVector3 vHit;

    vHit[0] = fX[n] - vMean[0]; 
    vHit[1] = fY[n] - vMean[1]; 
    vHit[2] = fZ[n] - vMean[2]; 

    vHit.RotateZ( gamma ); 
    vHit.RotateY( beta ); 
    vHit.RotateX( alpha ); 

    fX[n] = vHit[0] + vMean[0]; 
    fY[n] = vHit[1] + vMean[1]; 
    fZ[n] = vHit[2] + vMean[2];

}

void TRestHits::Rotate(Int_t n, Double_t alpha, TVector3 vAxis, TVector3 vMean)
{

  TVector3 vHit;

  vHit[0] = fX[n] - vMean[0];
  vHit[1] = fY[n] - vMean[1];
  vHit[2] = fZ[n] - vMean[2];

  vHit.Rotate( alpha, vAxis);

  fX[n] = vHit[0] + vMean[0];
  fY[n] = vHit[1] + vMean[1];
  fZ[n] = vHit[2] + vMean[2];

}


Double_t TRestHits::GetMaximumHitEnergy( )
{
    Double_t energy = 0;
    for ( int i = 0; i < GetNumberOfHits( ); i++ )
        if( GetEnergy(i) > energy ) energy = GetEnergy(i);
    return energy;
}

Double_t TRestHits::GetMinimumHitEnergy( )
{
    Double_t energy = GetMaximumHitEnergy( );
    for ( int i = 0; i < GetNumberOfHits( ); i++ )
        if( GetEnergy(i) < energy ) energy = GetEnergy(i);
    return energy;
}

Double_t TRestHits::GetMeanHitEnergy( )
{
    return GetTotalEnergy( )/GetNumberOfHits( );
}

void TRestHits::MergeHits( int n, int m )
{
    Double_t totalEnergy = fEnergy[n] + fEnergy[m];
    fX[n] = (fX[n]*fEnergy[n] + fX[m]*fEnergy[m])/totalEnergy;
    fY[n] = (fY[n]*fEnergy[n] + fY[m]*fEnergy[m])/totalEnergy;
    fZ[n] = (fZ[n]*fEnergy[n] + fZ[m]*fEnergy[m])/totalEnergy;

    fEnergy[n] += fEnergy[m];

    fNHits--;
    fX.erase( fX.begin() + m);
    fY.erase( fY.begin() + m);
    fZ.erase( fZ.begin() + m);
    fEnergy.erase( fEnergy.begin() + m );
}

void TRestHits::SwapHits( Int_t i, Int_t j )
{
    iter_swap(fX.begin() + i, fX.begin() + j);
    iter_swap(fY.begin() + i, fY.begin() + j);
    iter_swap(fZ.begin() + i, fZ.begin() + j);
    iter_swap(fEnergy.begin() + i, fEnergy.begin() + j);
}

Bool_t TRestHits::isSortedByEnergy( )
{
    for( int i = 0; i < GetNumberOfHits()-1; i++ )
    {
        if( GetEnergy( i+1 ) > GetEnergy( i ) ) return false;
    }
    return true;
}

void TRestHits::RemoveHit( int n )
{
    fNHits--;
    fTotEnergy -= GetEnergy( n );
    fX.erase(fX.begin()+n);
    fY.erase(fY.begin()+n);
    fZ.erase(fZ.begin()+n);
    fEnergy.erase(fEnergy.begin()+n);
}

TVector3 TRestHits::GetMeanPosition( )
{
    Double_t meanX = 0;
    Double_t meanY = 0;
    Double_t meanZ = 0;
    Double_t totalEnergy = 0;
    for( int n = 0; n < GetNumberOfHits(); n++ )
    {
        meanX += fX[n] * fEnergy[n];
        meanY += fY[n] * fEnergy[n];
        meanZ += fZ[n] * fEnergy[n];

        totalEnergy += fEnergy[n];
    }

    if( totalEnergy != GetTotalEnergy() )
        cout << "REST WARNING (TRestHits) : Energies do not match. " << totalEnergy << " != " << GetTotalEnergy() << endl;

    meanX /= totalEnergy;
    meanY /= totalEnergy;
    meanZ /= totalEnergy;

    return TVector3( meanX, meanY, meanZ );
}

Double_t TRestHits::GetTotalDistance()
{
    Double_t distance = 0;
    for( int i = 0; i < GetNumberOfHits()-1; i++ )
        distance += TMath::Sqrt( GetDistance2( i, i+1 ) );
    return distance;
}

Double_t TRestHits::GetDistance2( int n, int m )
{
    Double_t dx = GetX(n) - GetX(m);
    Double_t dy = GetY(n) - GetY(m);
    Double_t dz = GetZ(n) - GetZ(m);

    if( areXY() ) return dx*dx + dy*dy;
    if( areXZ() ) return dx*dx + dz*dz;
    if( areYZ() ) return dy*dy + dz*dz;

    return dx*dx + dy*dy + dz*dz;
}

Double_t TRestHits::GetDistanceToNode( Int_t n )
{
    Double_t distance = 0;
    if( n > GetNumberOfHits()-1 ) n = GetNumberOfHits()-1;

    for( int hit = 0; hit < n; hit++ )
        distance += GetVector( hit+1, hit ).Mag();

    return distance;
}

Int_t TRestHits::GetClosestHit( TVector3 position )
{
    Int_t closestHit = 0;

    Double_t minDistance = 1.e30;
    for( int nHit = 0; nHit < GetNumberOfHits(); nHit++ )
    {
        TVector3 vector = position - GetPosition( nHit );

        Double_t distance = vector.Mag2();
        if( distance < minDistance )
        {
            closestHit = nHit;
            minDistance = distance;
        }
    }

    return closestHit;
}

TVector2 TRestHits::GetProjection( Int_t n, Int_t m, TVector3 position )
{
    TVector3 nodesSegment = this->GetVector( n, m );

    TVector3 origin = position - this->GetPosition( m );

    if( origin == TVector3( 0,0,0 ) ) return TVector2( 0, 0 );

    Double_t longitudinal = nodesSegment.Unit().Dot( origin );
    if( origin == nodesSegment ) return TVector2( longitudinal, 0 );

    Double_t transversal = TMath::Sqrt( origin.Mag2() - longitudinal*longitudinal );

    return TVector2( longitudinal, transversal );
}


void TRestHits::PrintHits( Int_t nHits )
{
    Int_t N = nHits;

    if( N == -1 ) N = GetNumberOfHits();

	for( int n = 0; n < N; n++ )
		cout << "Hit " << n << " X: " << GetX(n) << " Y: " << GetY(n) << " Z: " << GetZ(n) <<  " Energy: " << GetEnergy(n) << endl;
}

