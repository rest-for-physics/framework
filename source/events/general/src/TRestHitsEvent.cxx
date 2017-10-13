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

