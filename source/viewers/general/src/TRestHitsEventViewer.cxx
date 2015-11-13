///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsEventViewer.cxx
///
///             nov 2015:   First concept
///                 Viewer class for a TRestHitEvent
///                 JuanAn Garcia
///_______________________________________________________________________________



#include "TRestHitsEventViewer.h"

ClassImp(TRestHitsEventViewer)
//______________________________________________________________________________
TRestHitsEventViewer::TRestHitsEventViewer()
{
  Initialize();
}


//______________________________________________________________________________
TRestHitsEventViewer::~TRestHitsEventViewer()
{
   // TRestHitsEventViewer destructor
   
}

//______________________________________________________________________________
void TRestHitsEventViewer::Initialize()
{
   fHitsEvent = new TRestHitsEvent( );
   fEvent = fHitsEvent;
         
}


void TRestHitsEventViewer::DeleteCurrentEvent(  ){
   
   TRestEveEventViewer::DeleteCurrentEvent(  );
   cout<<"Removing event"<<endl;
         
}


void TRestHitsEventViewer::AddEvent( TRestEvent *ev )
{

DeleteCurrentEvent( );

fHitsEvent=(TRestHitsEvent *)ev;

Double_t eDepMin = 1.e6;
Double_t eDepMax = 0;
Double_t totalEDep =0;

     for( int hit = 0; hit < fHitsEvent->GetNumberOfHits(); hit++ ){
	Double_t eDep = fHitsEvent->GetEnergy(hit);
	if( eDep > eDepMax ) eDepMax = eDep;
	if( eDep < eDepMin ) eDepMin = eDep;
	totalEDep+=eDep;
      }
      
Double_t slope = (fMaxRadius-fMinRadius)/(eDepMax-eDepMin);
Double_t bias = fMinRadius - slope * eDepMin;


for( int hit = 0; hit < fHitsEvent->GetNumberOfHits(); hit++ )
    {
        Double_t x = fHitsEvent->GetX( hit );
        Double_t y = fHitsEvent->GetY( hit );
        Double_t energy = fHitsEvent->GetEnergy( hit );
        Double_t z = fHitsEvent->GetZ( hit );
	
	Double_t radius = slope * energy + bias;
	
	AddSphericalHit( x, y, z, radius, energy );
    }


Update( );

}

