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
    if( !IsNaN( GetX(0) ) && !IsNaN( GetY(0) ) && IsNaN( GetZ(0) ) ) return true;
    return false;
}

Bool_t TRestHits::areXZ()
{
    if( IsNaN( GetY(0) ) && !IsNaN( GetX(0) ) && !IsNaN( GetZ(0) ) ) return true;
    return false;
}

Bool_t TRestHits::areYZ()
{
    if( IsNaN( GetX(0) ) && !IsNaN( GetY(0) ) && !IsNaN( GetZ(0) ) ) return true;
    return false;
}

Bool_t TRestHits::areXYZ()
{
    if( !isNaN(0) && (IsNaN ( GetX(0) ) || IsNaN( GetY(0) ) || IsNaN( GetZ(0) ) ) )  return false;
    return true;
}

Bool_t TRestHits::isNaN( Int_t n )
{
    if( IsNaN ( GetX(n) ) && IsNaN( GetY(n) ) && IsNaN( GetZ(n) ) )  return true;
    return false;
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


Bool_t TRestHits::isHitNInsidePrism( Int_t n, TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY)
{

    TVector3 axis = x1 - x0;

    Double_t prismLength = axis.Mag();

    TVector3 hitPos = this->GetPosition( n ) - x0;

    Double_t l = axis.Dot( hitPos )/prismLength;

    if( ( l > 0 ) && ( l < prismLength ) )
       if( ( TMath::Abs( hitPos.X() ) < sizeX/2 ) && ( TMath::Abs( hitPos.Y( ) ) < sizeY/2 ) )
             return true;

    return false;
}


Double_t TRestHits::GetEnergyInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY)
{
    Double_t energy = 0.;

    for( int n = 0; n < GetNumberOfHits(); n++ )
        if( isHitNInsidePrism( n, x0, x1, sizeX, sizeY ) )
            energy += this->GetEnergy( n );

    return energy;
}

Int_t TRestHits::GetNumberOfHitsInsidePrism( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY)
{
    Int_t hits = 0;

    for( int n = 0; n < GetNumberOfHits(); n++ )
        if( isHitNInsidePrism( n, x0, x1, sizeX,sizeY ) )
            hits++;

    return hits;
}

Bool_t TRestHits::isHitNInsideCylinder( Int_t n, TVector3 x0, TVector3 x1, Double_t radius )
{
   /* cout << "TRestHits::isHitNInsideCylinder has not been validated." << endl;
    cout << "After validation this output should be removed" << endl;*/

    TVector3 axis = x1 - x0;

    Double_t cylLength = axis.Mag();

   /* cout << "X0 : " << endl;
    x0.Print();
    cout << "Y0 : " << endl;
    x1.Print();
    cout << "Radius : " << radius << endl;

    cout << "Absolute position" << endl;
    this->GetPosition( n ).Print();*/

    TVector3 hitPos = this->GetPosition( n ) - x0;
   // cout << "HitPos" << endl;
  //  hitPos.Print();

    Double_t l = axis.Dot( hitPos ) / cylLength;

    if( l > 0 && l < cylLength )
    {
       // cout << "Is inside length" << endl;
        Double_t hitPosNorm2 = hitPos.Mag2();
        Double_t r = TMath::Sqrt( hitPosNorm2 - l*l );

        if( r < radius ) return true;
    }

    return false;
}

Double_t TRestHits::GetEnergyInCylinder( TVector3 x0, TVector3 x1, Double_t radius )
{
    Double_t energy = 0.;
    for( int n = 0; n < GetNumberOfHits(); n++ )
    {
        if( isHitNInsideCylinder( n, x0, x1, radius ) )
            energy += this->GetEnergy( n );
    }

    return energy;
}

Int_t TRestHits::GetNumberOfHitsInsideCylinder( TVector3 x0, TVector3 x1, Double_t radius )
{
    Int_t hits = 0;
    for( int n = 0; n < GetNumberOfHits(); n++ )
        if( isHitNInsideCylinder( n, x0, x1, radius ) ) hits++;

    return hits;
}

Double_t TRestHits::GetEnergyInSphere( Double_t x0, Double_t y0, Double_t z0, Double_t radius )
{
    Double_t sum = 0;
    for( int i = 0; i < GetNumberOfHits(); i++ )
    {
        Double_t x = this->GetX(i);
        Double_t y = this->GetY(i);
        Double_t z = this->GetZ(i);

		if (TMath::IsNaN(x) + TMath::IsNaN(y) + TMath::IsNaN(z) > 1) {
			cout << "REST Warning (TRestHits::GetEnergyInSphere) : more than two coordinates missing in the hit!" << endl;
			return 0;
		}

		x = TMath::IsNaN(x) ? 0 : x;
		y = TMath::IsNaN(y) ? 0 : y;
		z = TMath::IsNaN(z) ? 0 : z;

		if (TMath::IsNaN(x0) + TMath::IsNaN(y0) + TMath::IsNaN(z0) > 1) {
			cout << "REST Warning (TRestHits::GetEnergyInSphere) : more than two coordinates missing in the hit!" << endl;
			return 0;
		}

		x0 = TMath::IsNaN(x0) ? 0 : x0;
		y0 = TMath::IsNaN(y0) ? 0 : y0;
		z0 = TMath::IsNaN(z0) ? 0 : z0;

        Double_t dist = (x-x0) * (x-x0) + (y-y0) * (y-y0) + (z-z0) * (z-z0);

        if( dist < radius * radius )
            sum += GetEnergy(i);
    }
    return sum;
}

void TRestHits::AddHit( Double_t x, Double_t y, Double_t z, Double_t en, Double_t t )
{
    fNHits++;
    fX.push_back( (Float_t) (x) );
    fY.push_back( (Float_t) (y ));
    fZ.push_back( (Float_t) ( z));
    fT.push_back( (Float_t) t );
    fEnergy.push_back( (Float_t) ( en ) );

    fTotEnergy += en;
}


void TRestHits::AddHit( TVector3 pos, Double_t en, Double_t t )
{
    fNHits++;

    fX.push_back( (Float_t) (pos.X()) );
    fY.push_back( (Float_t) (pos.Y() ));
    fZ.push_back( (Float_t) ( pos.Z()  ));
    fT.push_back( (Float_t) t );
    fEnergy.push_back( (Float_t) ( en ) );

    fTotEnergy += en;
}

void TRestHits::AddHit( TRestHits &hits, Int_t n )
{
    Double_t x =  hits.GetX( n );
    Double_t y =  hits.GetY( n );
    Double_t z =  hits.GetZ( n );
    Double_t en = hits.GetEnergy( n );
    Double_t t =  hits.GetTime( n );

    AddHit( x, y, z, en, t );
}


void TRestHits::RemoveHits( )
{
    fNHits = 0;
    fX.clear();
    fY.clear();
    fZ.clear();
    fT.clear();
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
    fT[n] = ( fT[n]*fEnergy[n] + fT[m]*fEnergy[m] )/totalEnergy;
    fEnergy[n] += fEnergy[m];

    fX.erase( fX.begin() + m);
    fY.erase( fY.begin() + m);
    fZ.erase( fZ.begin() + m);
    fT.erase( fT.begin() + m );
    fEnergy.erase( fEnergy.begin() + m );
    fNHits--;
}

void TRestHits::SwapHits( Int_t i, Int_t j )
{
    iter_swap(fX.begin() + i, fX.begin() + j);
    iter_swap(fY.begin() + i, fY.begin() + j);
    iter_swap(fZ.begin() + i, fZ.begin() + j);
    iter_swap(fEnergy.begin() + i, fEnergy.begin() + j);
    iter_swap(fT.begin() + i, fT.begin() + j);
}

Bool_t TRestHits::isSortedByEnergy( )
{
    for( int i = 0; i < GetNumberOfHits()-1; i++ )
        if( GetEnergy( i+1 ) > GetEnergy( i ) ) return false;

    return true;
}

void TRestHits::RemoveHit( int n )
{
    fTotEnergy -= GetEnergy( n );
    fX.erase(fX.begin()+n);
    fY.erase(fY.begin()+n);
    fZ.erase(fZ.begin()+n);
    fT.erase(fT.begin()+n);
    fEnergy.erase(fEnergy.begin()+n);
    fNHits--;
}

TVector3 TRestHits::GetPosition( int n )
{
    if( areXY() )  return TVector3 ( ( (Double_t) fX[n]), ((Double_t) fY[n]), 0 );
    if( areXZ() )  return TVector3 ( ( (Double_t) fX[n]), 0, ((Double_t) fZ[n]) );
    if( areYZ() )  return TVector3 ( 0, ((Double_t) fY[n]), ((Double_t) fZ[n]) );
    return TVector3 ( ( (Double_t) fX[n]), ((Double_t) fY[n]), ((Double_t) fZ[n]) );
}

TVector3 TRestHits::GetVector( int i, int j )
{
    TVector3 vector = GetPosition(i) - GetPosition(j);
    return vector;
}

Int_t TRestHits::GetNumberOfHitsX( )
{
    Int_t nHitsX= 0;

    for( int n = 0; n < GetNumberOfHits(); n++ )
        if( !IsNaN( fX[n] ) ) 
            nHitsX++;

    return nHitsX;
}

Int_t TRestHits::GetNumberOfHitsY( )
{
    Int_t nHitsY= 0;

    for( int n = 0; n < GetNumberOfHits(); n++ )
        if( !IsNaN( fY[n] ) ) 
            nHitsY++;

    return nHitsY;
}


Double_t TRestHits::GetMeanPositionX( )
{
    Double_t meanX = 0;
    Double_t totalEnergy = 0;
    for( int n = 0; n < GetNumberOfHits(); n++ )
    {
	if( !IsNaN( fX[n] ) )
	{
		meanX += fX[n] * fEnergy[n];
		totalEnergy += fEnergy[n];
	}
    }

    meanX /= totalEnergy;

    return meanX;
}

Double_t TRestHits::GetMeanPositionY( )
{
    Double_t meanY = 0;
    Double_t totalEnergy = 0;
    for( int n = 0; n < GetNumberOfHits(); n++ )
    {
	if( !IsNaN( fY[n] ) )
	{
		meanY += fY[n] * fEnergy[n];
		totalEnergy += fEnergy[n];
	}
    }

    meanY /= totalEnergy;

    return meanY;
}

TVector3 TRestHits::GetMeanPosition( )
{
    TVector3 mean( GetMeanPositionX(), GetMeanPositionY(), GetMeanPositionZ() );
    return mean;
}

Double_t TRestHits::GetMeanPositionZ( )
{
    Double_t meanZ = 0;
    Double_t totalEnergy = 0;
    for( int n = 0; n < GetNumberOfHits(); n++ )
    {
	if( !IsNaN( fZ[n] ) )
	{
		meanZ += fZ[n] * fEnergy[n];
		totalEnergy += fEnergy[n];
	}
    }

    meanZ /= totalEnergy;

    return meanZ;
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

Int_t TRestHits::GetMostEnergeticHitInRange( Int_t n, Int_t m )
{
    Int_t maxEn = 0;
    Int_t hit = -1;
    for( int i = n; i < m; i++ )
    {
        if( this->GetEnergy( i ) > maxEn )
        {
            maxEn = this->GetEnergy( i );
            hit = i;
        }
    }
    if( hit == -1 ) cout << "REST warning : No largest hit found! No hits?" << endl;
    return hit;
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

Double_t TRestHits::GetMaximumHitDistance( )
{
    Double_t maxDistance = 0;
    for( int n = 0; n < this->GetNumberOfHits(); n++ )
        for( int m = n+1; m < this->GetNumberOfHits(); m++ )
        {
            Double_t d = this->GetDistance(n, m);
            if( d > maxDistance ) maxDistance = d;
        }

    return maxDistance;
}

Double_t TRestHits::GetMaximumHitDistance2( )
{
    Double_t maxDistance = 0;
    for( int n = 0; n < this->GetNumberOfHits(); n++ )
        for( int m = n+1; m < this->GetNumberOfHits(); m++ )
        {
            Double_t d = this->GetDistance2(n, m);
            if( d > maxDistance ) maxDistance = d;
        }

    return maxDistance;
}

void TRestHits::PrintHits( Int_t nHits )
{
    Int_t N = nHits;

    if( N == -1 ) N = GetNumberOfHits();
    if( N > GetNumberOfHits() ) N = GetNumberOfHits();

    for( int n = 0; n < N; n++ )
      cout << "Hit " << n << " X: " << GetX(n) << " Y: " << GetY(n) << " Z: " << GetZ(n) 
          <<  " Energy: " << GetEnergy(n) << " T: " << GetTime(n) << endl;
}

