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
    //cout << "Getting XZ Hits" << endl;
    TRestHits *xzHits = new TRestHits();
    for( int i = 0; i < this->GetNumberOfHits(); i++ )
    {
        if( this->GetY(i) == 0 )
        {
            xzHits->AddHit( this->GetX(i), 0, this->GetZ(i), this->GetEnergy(i) );
        }
    }
    //cout << "Number of XZ Hits : " << xzHits->GetNumberOfHits() << endl;
    return xzHits;
}

TRestHits *TRestHitsEvent::GetYZHits() 
{ 
    //cout << "Getting YZ Hits" << endl;
    TRestHits *yzHits = new TRestHits();
    for( int i = 0; i < this->GetNumberOfHits(); i++ )
    {
        if( this->GetX(i) == 0 )
        {
            yzHits->AddHit( 0, this->GetY(i), this->GetZ(i), this->GetEnergy(i) );
        }
    }
    //cout << "Number of YZ Hits : " << yzHits->GetNumberOfHits() << endl;
    return yzHits;
}

TRestHits *TRestHitsEvent::GetXYZHits() 
{ 
    //cout << "Getting XYZ Hits" << endl;
    TRestHits *xyzHits = new TRestHits();
    for( int i = 0; i < this->GetNumberOfHits(); i++ )
    {
        if( this->GetX(i) != 0 &&  this->GetY(i) != 0  &&  this->GetZ(i) != 0)
        {
            xyzHits->AddHit( 0, this->GetY(i), this->GetZ(i), this->GetEnergy(i) );
        }
    }
    //cout << "Number of XYZ Hits : " << xyzHits->GetNumberOfHits() << endl;
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


void TRestHitsEvent::PrintEvent()
{
	//TRestEvent::PrintEvent();
	fHits->PrintEvent();


}
