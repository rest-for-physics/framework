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
/// BASIC DOCUMENTATION
///
//////////////////////////////////////////////////////////////////////////

#include "TRestRawCommonNoiseReductionProcess.h"
using namespace std;
#include <algorithm>
#include <iostream>     // std::cout
#include <vector>       // std::vector

ClassImp(TRestRawCommonNoiseReductionProcess)

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestRawCommonNoiseReductionProcess::TRestRawCommonNoiseReductionProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
///
TRestRawCommonNoiseReductionProcess::TRestRawCommonNoiseReductionProcess( char *cfgFileName ) {
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestRawCommonNoiseReductionProcess::~TRestRawCommonNoiseReductionProcess() {
    delete fInputEvent;
    delete fOutputEvent;
}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestRawCommonNoiseReductionProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestRawCommonNoiseReductionProcess::Initialize() {
    SetSectionName(this->ClassName());

    fOutputEvent = new TRestRawSignalEvent();
    fInputEvent  = new TRestRawSignalEvent();
}

///////////////////////////////////////////////
/// \brief Function to load the configuration from an external configuration
/// file.
///
/// If no configuration path is defined in TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// correspondig TRestGeant4AnalysisProcess section inside the RML.
///
void TRestRawCommonNoiseReductionProcess::LoadConfig( std::string cfgFilename, std::string name ) {
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

///////////////////////////////////////////////
/// \brief Process initialization.
///
void TRestRawCommonNoiseReductionProcess::InitProcess() {
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestRawCommonNoiseReductionProcess::ProcessEvent( TRestEvent *evInput ) {

    fInputEvent = (TRestRawSignalEvent *) evInput;

    // Copying the input event to the output event
    fOutputEvent->SetID( fInputEvent->GetID() );
    fOutputEvent->SetSubID( fInputEvent->GetSubID() );
    fOutputEvent->SetTimeStamp( fInputEvent->GetTimeStamp() );
    fOutputEvent->SetSubEventTag( fInputEvent->GetSubEventTag() );

    Int_t N = fInputEvent->GetNumberOfSignals();

    if( GetVerboseLevel() >= REST_Debug ) N = 1;
    for( int sgnl = 0; sgnl < N; sgnl++ )
    {   
        fOutputEvent->AddSignal( *fInputEvent->GetSignal( sgnl ) );  
    }

    /////////////////////////////////////////////////
    Int_t  nBins = fInputEvent-> GetSignal(0)->GetNumberOfPoints();
    Int_t begin, end;
    Double_t norm = 1.0;
    vector<Double_t> sgnlValues(N, 0.0);


    for( Int_t bin = 0 ; bin < nBins ; bin++ ) { 
        // For each time bin we find the common Noise 
        for( Int_t sgnl = 0 ; sgnl < N ; sgnl++ ){ 
            // Getting all the corresponding time bin for all TRestSignals
            sgnlValues[sgnl] = fOutputEvent->GetSignal( sgnl )->GetData(bin);
        }

        // Sorting the signal from lowest to highest
        std::sort(sgnlValues.begin(), sgnlValues.end()); 

        // Sorting the different methods
        Int_t begin, middle ,end;
        middle = (Int_t) nBins / 2;
        Double_t norm = 1.0;

        if(fMode == 0){
            // We take only the middle one
            begin = (Int_t) ( (double_t) nBins / 2.0 );
            end   = begin;
            norm  = 1.;
        }
        else if( fMode == 1 ){
            // We take the average of the TRestSignals at the Center
            begin = middle - (Int_t) (nBins * fcenterWidth * 0.01) ;
            end   = middle + (Int_t) (nBins * fcenterWidth * 0.01) ;
            norm = (Double_t) end - begin;
        }

        // Calculation of the correction to be made to each TRestRawSignal
        Double_t binCorrection = 0.0;
        for( Int_t i = begin; i <= end; i++){
            binCorrection += sgnlValues[i] ;
        }
        binCorrection = binCorrection / norm;

        // Application of the correction.
        for( Int_t sgnl = 0 ; sgnl < N ; sgnl++ ){ 
            fOutputEvent->GetSignal( sgnl )->IncreaseBinBy( bin , - binCorrection ) ;
        }
    }

    /////////////////////////////////////////////////
    return fOutputEvent;
}

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been
/// processed. This method will write the channels histogram.
///
void TRestRawCommonNoiseReductionProcess::EndProcess() {
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}


///////////////////////////////////////////////
/// \brief Function to read input parameters.
///
void TRestRawCommonNoiseReductionProcess::InitFromConfigFile( ) {
    fMode        = StringToInteger( GetParameter( "mode", "0" ) );
    fcenterWidth = StringToInteger( GetParameter( "centerWidth", "10" ) );
}

