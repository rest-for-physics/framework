///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEventProcess.cxx
///
///             Base class from which to inherit all REST process classes inherit
///             A process class in REST deals with one input, ouput (or both) 
///             single REST events. RestMetadata controls the access to config 
///             files.
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________


#include "TRestEventProcess.h"
using namespace std;

ClassImp(TRestEventProcess)
//______________________________________________________________________________
    TRestEventProcess::TRestEventProcess()
{
   // TRestEventProcess default constructor
}


//______________________________________________________________________________
TRestEventProcess::~TRestEventProcess()
{
   // TRestEventProcess destructor
}

TRestMetadata *TRestEventProcess::GetGasMetadata( )
{
    // TODO : For the moment this function will return the first occurence of TRestGas.
    // What happens if there are several? And if I want to use one gas from the config file? 
    // We need to introduce an option somewhere.
    // For the moment I know there will be an existing gas file since the hits come from electronDiffusion.

    for( size_t i = 0; i < fRunMetadata.size(); i++ )
        if ( fRunMetadata[i]->ClassName() == (TString) "TRestGas" )
        {
            return fRunMetadata[i];
        }

    return NULL;
}

TRestMetadata *TRestEventProcess::GetReadoutMetadata( )
{
    // TODO : For the moment this function will return the first occurence of TRestReadout.
    // What happens if there are several? And if I want to use one gas from the config file? 
    // We need to introduce an option somewhere.
    // For the moment I know there will be an existing gas file since the hits come from electronDiffusion.

    for( size_t i = 0; i < fRunMetadata.size(); i++ )
        if ( fRunMetadata[i]->ClassName() == (TString) "TRestReadout" )
        {
            return fRunMetadata[i];
        }

    return NULL;
}

Double_t TRestEventProcess::GetDoubleParameterFromClass( TString className, TString parName )
{
    for( size_t i = 0; i < fRunMetadata.size(); i++ )
        if ( fRunMetadata[i]->ClassName() == className )
        {
 //           cout << "String parameter : " << fRunMetadata[i]->GetParameter( (string) parName ) << endl;
 //           fRunMetadata[i]->PrintConfigBuffer( );
            return StringToDouble( fRunMetadata[i]->GetParameter( (string) parName ) );
        }

    return PARAMETER_NOT_FOUND_DBL;
}

Double_t TRestEventProcess::GetDoubleParameterFromClassWithUnits( TString className, TString parName )
{
    for( size_t i = 0; i < fRunMetadata.size(); i++ )
        if ( fRunMetadata[i]->ClassName() == className )
        {
 //           cout << "String parameter : " << fRunMetadata[i]->GetParameter( (string) parName ) << endl;
 //           fRunMetadata[i]->PrintConfigBuffer( );
            return fRunMetadata[i]->GetDblParameterWithUnits( (string) parName );
        }

    return PARAMETER_NOT_FOUND_DBL;
}

/*
//______________________________________________________________________________
void TRestEventProcess::InitProcess()
{
   // virtual function to be executed once at the beginning of process
   // (before starting the process of the events)
   cout << GetName() << ": Process initialization..." << endl;
}

//______________________________________________________________________________
void TRestEventProcess::ProcessEvent( TRestEvent *eventInput )
{
   // virtual function to be executed for every event to be processed
}

//______________________________________________________________________________
void TRestEventProcess::EndProcess()
{
   // virtual function to be executed once at the end of the process 
   // (after all events have been processed)
   cout << GetName() << ": Process ending..." << endl;
}
*/
void TRestEventProcess::BeginPrintProcess()
{
   cout << "--------------------------------------------------------------------------------------------------" << endl;
   cout << "-- Process :" << GetProcessName() << " ## " << GetName() << " ## " << GetTitle() << endl;
   cout << "--------------------------------------------------------------------------------------------------" << endl;
}

void TRestEventProcess::EndPrintProcess()
{
   cout << "--------------------------------------------------------------------------------------------------" << endl;
}
