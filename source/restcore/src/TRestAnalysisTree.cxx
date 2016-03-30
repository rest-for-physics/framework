///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAnalysisTree.cxx
///
///             Base class for managing a basic analysis tree. 
///
///             feb 2016:   First concept
///              author :   Javier Galan
///_______________________________________________________________________________


#include "TRestAnalysisTree.h"

#include "TObjArray.h"
using namespace std;


ClassImp(TRestAnalysisTree)
 
//______________________________________________________________________________
TRestAnalysisTree::TRestAnalysisTree() : TTree( )
{
    SetName( "TRestAnalysisTree" );
    SetTitle( "Analysis tree" );

    fSubEventTag = new TString();

    fNObservables = 0;
}

TRestAnalysisTree::TRestAnalysisTree( TString name, TString title ) : TTree( name, title )
{
    SetName( name );
    SetTitle( title );

    fSubEventTag = new TString();

    fNObservables = 0;
}

void TRestAnalysisTree::ConnectEventBranches( )
{
    TBranch *br1 = GetBranch( "eventID" );
    br1->SetAddress( &fEventID );

    TBranch *br2 = GetBranch( "subEventID" );
    br2->SetAddress( &fSubEventID );

    TBranch *br3 = GetBranch( "timeStamp" );
    br3->SetAddress( &fTimeStamp );

    TBranch *br4 = GetBranch( "subEventTag" );
    br4->SetAddress( &fSubEventTag );
}

Int_t TRestAnalysisTree::FillEvent( TRestEvent *evt )
{
    fEventID = evt->GetID( );
    fSubEventID = evt->GetSubID( );
    fTimeStamp = evt->GetTimeStamp( ).AsDouble();
    *fSubEventTag = evt->GetSubEventTag( );

    return this->Fill( );
}

Int_t TRestAnalysisTree::AddObservable( TString observableName )
{
    Double_t x = 0;
    fObservableNames.push_back( observableName );
    fObservableValues.push_back( x );
    Branch( observableName, &fObservableValues[fNObservables] );
    fNObservables++;
    return fNObservables-1;
}

void TRestAnalysisTree::CreateEventBranches( )
{
    Branch( "eventID", &fEventID );
    Branch( "subEventID", &fSubEventID );
    Branch( "timeStamp", &fTimeStamp );
    Branch( "subEventTag", fSubEventTag );
}

//______________________________________________________________________________
TRestAnalysisTree::~TRestAnalysisTree()
{
}

