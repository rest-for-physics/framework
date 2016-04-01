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

    Initialize( );
}

TRestAnalysisTree::TRestAnalysisTree( TString name, TString title ) : TTree( name, title )
{
    SetName( name );
    SetTitle( title );

    Initialize( );
}

void TRestAnalysisTree::Initialize( )
{

    fSubEventTag = new TString();

    fNObservables = 0;

    fConnected = false;
    fBranchesCreated = false;

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

void TRestAnalysisTree::ConnectObservables( )
{
    TBranch *branch[fNObservables];
    for( int i = 0; i < GetNumberOfObservables(); i++ )
    {
        Double_t x = 0;
        branch[i] = GetBranch( fObservableNames[i] );
        fObservableValues.push_back( x );
        branch[i]->SetAddress( &fObservableValues[i] );
    }
    fConnected = true;

}

Int_t TRestAnalysisTree::AddObservable( TString observableName )
{
    if( fBranchesCreated )
    {
        cout << "REST ERROR : Branches have been already created" << endl; 
        cout << "No more observables can be added" << endl;
        return -1;
    }
    Double_t x = 0;
    fObservableNames.push_back( observableName );
    fObservableValues.push_back( x );

    fNObservables++;
    return fNObservables-1;
}

Int_t TRestAnalysisTree::FillEvent( TRestEvent *evt )
{
    fEventID = evt->GetID( );
    fSubEventID = evt->GetSubID( );
    fTimeStamp = evt->GetTimeStamp( ).AsDouble();
    *fSubEventTag = evt->GetSubEventTag( );

    return this->Fill( );
}

void TRestAnalysisTree::CreateEventBranches( )
{
    Branch( "eventID", &fEventID );
    Branch( "subEventID", &fSubEventID );
    Branch( "timeStamp", &fTimeStamp );
    Branch( "subEventTag", fSubEventTag );
}

void TRestAnalysisTree::CreateObservableBranches( )
{
    if( fBranchesCreated ) 
    {
        cout << "REST ERROR : Branches have been already created" << endl; 
        exit(1); 
    }

    for( int n = 0; n < GetNumberOfObservables(); n++ )
        Branch( fObservableNames[n], &fObservableValues[n] );

    fBranchesCreated = true;
}


//______________________________________________________________________________
TRestAnalysisTree::~TRestAnalysisTree()
{
}

