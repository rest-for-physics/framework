#include "TRestHitsEvent.h"

using namespace std;
using namespace TMath;

ClassImp(TRestHitsEvent)

TRestHitsEvent::TRestHitsEvent()
{

fHits = new TRestHits();
    
}

TRestHitsEvent::~TRestHitsEvent()
{
delete fHits;
}

void TRestHitsEvent::AddHit( Double_t x, Double_t y, Double_t z, Double_t en, Double_t t )
{
    fHits->AddHit(x, y, z, en, t);
}

void TRestHitsEvent::AddHit( TVector3 pos, Double_t en, Double_t t )
{
    fHits->AddHit(pos, en, t );
}

void TRestHitsEvent::Initialize()
{
    TRestEvent::Initialize();

    RemoveHits();

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

TRestHits *TRestHitsEvent::GetXZHits() 
{ 
    TRestHits *xzHits = new TRestHits();
    for( int i = 0; i < this->GetNumberOfHits(); i++ )
    {
        if( IsNaN ( this->GetY(i) ) )
        {
            xzHits->AddHit( this->GetX(i), this->GetY(i), this->GetZ(i), this->GetEnergy(i) );
        }
    }
    return xzHits;
}

TRestHits *TRestHitsEvent::GetYZHits() 
{ 
    TRestHits *yzHits = new TRestHits();
    for( int i = 0; i < this->GetNumberOfHits(); i++ )
    {
        if( IsNaN( this->GetX(i) ) )
        {
            yzHits->AddHit( this->GetX(i), this->GetY(i), this->GetZ(i), this->GetEnergy(i) );
        }
    }
    return yzHits;
}

TRestHits *TRestHitsEvent::GetXYZHits() 
{ 
    TRestHits *xyzHits = new TRestHits();
    for( int i = 0; i < this->GetNumberOfHits(); i++ )
    {
        if( !IsNaN( this->GetX(i) ) && !IsNaN( this->GetY(i) ) && !IsNaN( this->GetZ(i) ) )
        {
            xyzHits->AddHit( this->GetX(i), this->GetY(i), this->GetZ(i), this->GetEnergy(i) );
        }
    }
    return xyzHits;
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

Bool_t TRestHitsEvent::isHitsEventInsideCylinder( TVector3 x0, TVector3 x1, Double_t radius )
{
    if ( fHits->GetNumberOfHitsInsideCylinder(x0, x1, radius) == GetNumberOfHits( ) )
        return true;

    return false;
}

Bool_t TRestHitsEvent::isHitsEventInsidePrism( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY )
{
    if ( fHits->GetNumberOfHitsInsidePrism(x0, x1, sizeX, sizeY) == GetNumberOfHits( ) ) 
        return true;

    return false;
}

Double_t TRestHitsEvent::GetClosestHitInsideDistanceToCylinderWall( TVector3 x0, TVector3 x1, Double_t radius )
{
    Double_t rad2 = radius * radius;
    Double_t hitDistance = rad2;

    Double_t d2, l;

    TVector3 axis = x1 - x0;
    Double_t cylLength = axis.Mag();

    Int_t nhits = 0;
    for( int n = 0; n < GetNumberOfHits(); n++ )
    { 
        if( fHits->isHitNInsideCylinder( n, x0, x1, radius ) )
        {   
            l = axis.Dot( this->GetPosition( n ) - x0 ) / cylLength;

            d2 = rad2 - ( this->GetPosition( n ) - x0 ).Mag2() - l * l;

            if( d2 < hitDistance )
                hitDistance = d2;

            nhits++;
        }
    }

    if( nhits == 0 )
        return -1;

    return TMath::Sqrt( hitDistance );
}

Double_t TRestHitsEvent::GetClosestHitInsideDistanceToCylinderTop( TVector3 x0, TVector3 x1, Double_t radius )
{  
    TVector3 axis = x1 - x0;
    Double_t cylLength = axis.Mag();

    Double_t hitDistance = cylLength;
    Double_t d = cylLength;

    Int_t nhits = 0;
    for( int n = 0; n < GetNumberOfHits(); n++ )
    { 
        if( fHits->isHitNInsideCylinder( n, x0, x1, radius ) )
        {

            d = cylLength - axis.Dot( this->GetPosition( n ) - x0 ) / cylLength;

            if( d < hitDistance )
                hitDistance = d;

            nhits++; 
        }
    }

    if( nhits == 0 )
        return -1;

    return hitDistance;
}

Double_t TRestHitsEvent::GetClosestHitInsideDistanceToCylinderBottom( TVector3 x0, TVector3 x1, Double_t radius )
{  
    TVector3 axis = x1 - x0;
    Double_t cylLength = axis.Mag();

    Double_t hitDistance = cylLength;
    Double_t d = cylLength;

    Int_t nhits = 0;
    for( int n = 0; n < GetNumberOfHits(); n++ )
    { 
        if( fHits->isHitNInsideCylinder( n, x0, x1, radius ) )
        {

            d = cylLength - axis.Dot( this->GetPosition( n ) - x0 ) / cylLength;

            if( d < hitDistance )
                hitDistance = d;

            nhits++; 
        }
    }

    if( nhits == 0 )
        return -1;

    return hitDistance;
}

Double_t TRestHitsEvent::GetClosestHitInsideDistanceToPrismWall( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY )
{

    Double_t dX = sizeX / 2;
    Double_t dY = sizeY / 2;

    Double_t hitDistance = max( dX, dY);

    Double_t d;
    Int_t nhits = 0;
    for( int n = 0; n < GetNumberOfHits(); n++ )
    { 
        if( fHits->isHitNInsidePrism( n, x0, x1, sizeX, sizeY ) )
        {   
            dX = sizeX / 2 - TMath::Abs( ( this->GetPosition( n ) - x0 ).X() );
            dY = sizeY / 2 - TMath::Abs( ( this->GetPosition( n ) - x0 ).Y() ) ;

            d = min( dX, dY );

            if( d < hitDistance )
                hitDistance = d;

            nhits++; 
        }
    }

    if( nhits == 0)
        return -1;

    return hitDistance;
}

Double_t TRestHitsEvent::GetClosestHitInsideDistanceToPrismTop( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY )
{

    TVector3 axis = x1 - x0;
    Double_t prismLength = axis.Mag();

    Double_t hitDistance = prismLength;

    Double_t d;
    Int_t nhits = 0;
    for( int n = 0; n < GetNumberOfHits(); n++ )
    {
        if( fHits->isHitNInsidePrism( n, x0, x1, sizeX,sizeY ) )
        {  

            d = prismLength - axis.Dot( this->GetPosition( n ) - x0 ) / prismLength;

            if( d < hitDistance ) 
                hitDistance = d;

            nhits++;
        }
    }

    if( nhits == 0 )
        return -1;

    return hitDistance;
}

Double_t TRestHitsEvent::GetClosestHitInsideDistanceToPrismBottom( TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY )
{

    TVector3 axis = x1 - x0;
    Double_t prismLength = axis.Mag();

    Double_t hitDistance = prismLength;

    Double_t d;
    Int_t nhits = 0;
    for( int n = 0; n < GetNumberOfHits(); n++ )
    {
        if( fHits->isHitNInsidePrism( n, x0, x1, sizeX,sizeY ) )
        {  

            d = axis.Dot( this->GetPosition( n ) - x0 ) / prismLength;

            if( d < hitDistance ) 
                hitDistance = d;

            nhits++;
        }
    }

    if( nhits == 0 )
        return -1;

    return hitDistance;
}

void TRestHitsEvent::PrintEvent( Int_t nHits )
{
	TRestEvent::PrintEvent();

    cout << "Total energy : " << GetEnergy() << endl;
    cout << "Mean position : ( " << GetMeanPositionX() << " , " << GetMeanPositionY() << " , " << GetMeanPositionZ() << " ) " << endl;
    cout << "Number of hits : " << fHits->GetNumberOfHits() << endl;
    if( nHits != -1 )
    {
        cout << "+++++++++++++++++++++++" << endl;
        cout << "Printing only the first " << nHits << " hits" << endl;
    }

	fHits->PrintHits( nHits );
}

