/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/


//////////////////////////////////////////////////////////////////////////
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// \class      TRestEventProcess
/// A base class for any REST event process
///
/// One of the core classes of REST. Absract class
/// from which all REST "event process classes" must derive.
/// An event process in REST is an atomic operation on one single
/// (input) event, potentially producing a different (output) event.
/// Which type of (input/output) events will depent on the type of
/// process. Processes can be tailored via metadata classes.
/// All this needs to be defined in the inherited class.
/// TRestEventProcess provides the basic structure (virtual functions)
///
///
/// History of developments:
///
/// 2014-june: First concept. As part of conceptualization of previous REST
///       code (REST v2) 
///       Igor G. Irastorza
///
//////////////////////////////////////////////////////////////////////////


#include "TRestEventProcess.h"
using namespace std;

ClassImp(TRestEventProcess)

//////////////////////////////////////////////////////////////////////////
/// TRestEventProcess default constructor

TRestEventProcess::TRestEventProcess()
{
}

//////////////////////////////////////////////////////////////////////////
/// TRestEventProcess destructor

TRestEventProcess::~TRestEventProcess()
{
}

void TRestEventProcess::LoadConfig( std::string cfgFilename, std::string cfgName )
{
    if( LoadConfigFromFile( cfgFilename, cfgName ) == -1 ) LoadDefaultConfig( );
}

void TRestEventProcess::LoadDefaultConfig() 
{
    SetName( "cfgDefault" ); 
    SetTitle( "Default config" ); 
}

vector <string> TRestEventProcess::ReadObservables( )
{
    vector <string> obsList = GetObservablesList( );

    for( unsigned int n = 0; n < obsList.size(); n++ )
    {
        fAnalysisTree->AddObservable( this->GetName() + (TString) "." + (TString) obsList[n] );
        fObservableNames.push_back ( this->GetName() + (string) "." + obsList[n] );
    }
    return obsList;
}

TString TRestEventProcess::GetProcessName()
{
    TString sectionName = this->GetSectionName();
    TString processName = sectionName;
    if( sectionName.Contains( "TRest" ) )
    {
        TString pcsName ( sectionName( 5, sectionName.Length() ) );
        TString firstChar( sectionName( 0, 1 ) ); 
        TString remainingString( sectionName( 1, sectionName.Length() ) ); 

        firstChar.ToLower();
        processName = firstChar + remainingString;
    }

    return processName;
}

//////////////////////////////////////////////////////////////////////////
/// Returns pointer to gas (TRestGas) metadata
///
/// TODO : For the moment this function will return the first occurence of TRestGas.
/// What happens if there are several? And if I want to use one gas from the config file? 
/// We need to introduce an option somewhere.
/// For the moment I know there will be an existing gas file since the hits come from electronDiffusion.

TRestMetadata *TRestEventProcess::GetGasMetadata( )
{

    for( size_t i = 0; i < fRunMetadata.size(); i++ )
        if ( fRunMetadata[i]->ClassName() == (TString) "TRestGas" )
            return fRunMetadata[i];

    return NULL;
}

//////////////////////////////////////////////////////////////////////////
/// Returns pointer to readout metadata
///
/// TODO : For the moment this function will return the first occurence of TRestReadout.
/// What happens if there are several? And if I want to use one gas from the config file? 
/// We need to introduce an option somewhere.
/// For the moment I know there will be an existing gas file since the hits come from electronDiffusion.

TRestMetadata *TRestEventProcess::GetReadoutMetadata( )
{
    for( size_t i = 0; i < fRunMetadata.size(); i++ )
        if ( fRunMetadata[i]->ClassName() == (TString) "TRestReadout" )
            return fRunMetadata[i];

    return NULL;
}

//////////////////////////////////////////////////////////////////////////
/// Returns pointer to G4 metadata
///
/// TODO : For the moment this function will return the first occurence of TRestG4Metadata.
/// What happens if there are several? And if I want to use one gas from the config file? 
/// We need to introduce an option somewhere.
/// For the moment I know there will be an existing gas file since the hits come from electronDiffusion.

TRestMetadata *TRestEventProcess::GetGeant4Metadata( )
{
    for( size_t i = 0; i < fRunMetadata.size(); i++ )
        if ( fRunMetadata[i]->ClassName() == (TString) "TRestG4Metadata" )
            return fRunMetadata[i];

    return NULL;
}

//////////////////////////////////////////////////////////////////////////
/// Returns pointer to G4 metadata
///
/// TODO : For the moment this function will return the first occurence of TRestDetectorSetup.
/// What happens if there are several? And if I want to use one gas from the config file? 
/// We need to introduce an option somewhere.
/// For the moment I know there will be an existing gas file since the hits come from electronDiffusion.

TRestMetadata *TRestEventProcess::GetDetectorSetup( )
{
    for( size_t i = 0; i < fRunMetadata.size(); i++ )
        if ( fRunMetadata[i]->ClassName() == (TString) "TRestDetectorSetup" )
            return fRunMetadata[i];

    return NULL;
}

//////////////////////////////////////////////////////////////////////////
/// Retrieve parameter with name parName from metadata className
///
/// \param className string with name of metadata class to access
/// \param parName  string with name of parameter to retrieve
///

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

//////////////////////////////////////////////////////////////////////////
/// Retrieve parameter with name parName from metadata className
///
/// \param className string with name of metadata class to access
/// \param parName  string with name of parameter to retrieve
///

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

//////////////////////////////////////////////////////////////////////////
/// Show default console output at starting of process
///

void TRestEventProcess::BeginPrintProcess()
{
	cout << endl;
	cout << "--------------------------------------------------------------------------------------------------" << endl;
	cout << "-- Process : " << GetProcessName() << " Name : " << GetName() << " Title : " << GetTitle() << endl;
	cout << "--------------------------------------------------------------------------------------------------" << endl;

	cout << endl;
	if( fObservableNames.size() > 0 )
	{
		cout << " Analysis tree observables added by this process " << endl;
		cout << " +++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
	}

	for( unsigned int i = 0; i < fObservableNames.size(); i++ )
	{
		cout << " ++ " << fObservableNames[i] << endl;
	}

	if( fObservableNames.size() > 0 )
	{
		cout << " +++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
		cout << endl;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Show default console output at finish of process
///

void TRestEventProcess::EndPrintProcess()
{
   cout << endl;
   cout << "--------------------------------------------------------------------------------------------------" << endl;
   cout << endl;
}
