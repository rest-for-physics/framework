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
/// This process is internal to the library. That means the input and
/// output event is the event type corresponding to the library, i.e.
/// TRestDetectorHitsEvent.
///
/// ### Observables
///
/// Position statistics
///
/// * **xSigmaGaus**: Standard deviation (sigma) in coordinate X in the hits of an event, obtained from a
/// gaussian fit applied to the hits.
/// * **ySigmaGaus**: Standard deviation (sigma) in coordinate Y in the hits of an event, obtained from a
/// gaussian fit applied to the hits.
/// * **xy2SigmaGaus**: Sum of the variance (sigma^2) in coordinates X and Y in the hits of an event.
/// s_xy^2=s_x^2+s_y^2.
/// * **xySigmaBalanceGaus**: (s_x-s_y)/(s_x+s_y).
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2020-Sep:  First implementation
///             Cristina Margalejo
///
/// \class      TRestDetectorHitsGaussAnalysisProcess
/// \author     Cristina Margalejo
///
/// <hr>
///
#include "TRestDetectorHitsGaussAnalysisProcess.h"
using namespace std;

ClassImp(TRestDetectorHitsGaussAnalysisProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDetectorHitsGaussAnalysisProcess::TRestDetectorHitsGaussAnalysisProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// The path to the config file can be specified using full path,
/// absolute or relative.
///
/// If the file is not found then REST will try to find the file on
/// the default paths defined in REST Framework, usually at the
/// REST_PATH installation directory. Additional search paths may be
/// defined using the parameter `searchPath` in globals section. See
/// TRestMetadata description.
///
/// \param cfgFileName A const char* giving the path to an RML file.
///
TRestDetectorHitsGaussAnalysisProcess::TRestDetectorHitsGaussAnalysisProcess(char* cfgFileName) {
    Initialize();

    LoadConfig(cfgFileName);
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDetectorHitsGaussAnalysisProcess::~TRestDetectorHitsGaussAnalysisProcess() {
    delete fInputHitsEvent;
    delete fOutputHitsEvent;
}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestDetectorHitsGaussAnalysisProcess::LoadDefaultConfig() {
    SetName(this->ClassName());
    SetTitle("Default config");
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
/// correspondig TRestDetectorHitsGaussAnalysisProcess section inside the RML.
///
void TRestDetectorHitsGaussAnalysisProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Function to use in initialization of process members before starting
/// to process the event
///
void TRestDetectorHitsGaussAnalysisProcess::InitProcess() {
    // For example, try to initialize a pointer to existing metadata
    // accessible from TRestRun
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name and library version
///
void TRestDetectorHitsGaussAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());
    // SetLibraryVersion(LIBRARY_VERSION);

    // fHitsEvent = new TRestDetectorHitsEvent();
    fInputHitsEvent = NULL;
    fOutputHitsEvent = new TRestDetectorHitsEvent();
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestDetectorHitsGaussAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fInputHitsEvent = (TRestDetectorHitsEvent*)evInput;

    TString obsName;

    // cout << "Event id : " << fInputHitsEvent->GetID() << endl;
    TRestHits* hits = fInputHitsEvent->GetHits();

    for (int n = 0; n < hits->GetNumberOfHits(); n++) {
        Double_t eDep = hits->GetEnergy(n);

        Double_t x = hits->GetX(n);
        Double_t y = hits->GetY(n);
        Double_t z = hits->GetZ(n);

        auto time = hits->GetTime(n);
        auto type = hits->GetType(n);

        fOutputHitsEvent->AddHit(x, y, z, eDep, time, type);
    }

    Double_t gausSigmaX = fOutputHitsEvent->GetGaussSigmaX(fReadoutChannelsX, fStartChannelPosition, fEndChannelPosition, fPitch);
    Double_t gausSigmaY = fOutputHitsEvent->GetGaussSigmaY(fReadoutChannelsX, fStartChannelPosition, fEndChannelPosition, fPitch);

    SetObservableValue("xSigmaGaus", gausSigmaX);
    SetObservableValue("ySigmaGaus", gausSigmaY);
    SetObservableValue("xy2SigmaGaus", (gausSigmaX * gausSigmaX) + (gausSigmaY * gausSigmaY));
    SetObservableValue("xySigmaBalanceGaus", (gausSigmaX - gausSigmaY) / (gausSigmaX + gausSigmaY));

    // We transform here fHitsEvent if necessary

    if (GetVerboseLevel() >= REST_Debug) {
        fOutputHitsEvent->PrintEvent();

        if (GetVerboseLevel() >= REST_Extreme) GetChar();
    }

    return fOutputHitsEvent;
}

///////////////////////////////////////////////
/// \brief Function reading input parameters from the RML
/// TRestDetectorHitsGaussAnalysisProcess section
///
void TRestDetectorHitsGaussAnalysisProcess::InitFromConfigFile() {
    fReadoutChannelsX = StringToInteger(GetParameter("readoutChannelsX", "120"));
    fReadoutChannelsY = StringToInteger(GetParameter("readoutChannelsY", "120"));
    fStartChannelPosition = StringToInteger(GetParameter("StartChPos", "-30"));
    fEndChannelPosition = StringToInteger(GetParameter("EndChPos", "30"));

    fPitch = StringToDouble(GetParameter("Pitch", "0.5"));

}

///////////////////////////////////////////////
/// \brief It prints out the process parameters stored in the
/// metadata structure
///
void TRestDetectorHitsGaussAnalysisProcess::PrintMetadata() {
    BeginPrintProcess();

    // Print output metadata using, metadata << endl;

    metadata << "Number of X readout channels : " << fReadoutChannelsX << endl;
    metadata << "Number of Y readout channels : " << fReadoutChannelsY << endl;
    metadata << "Start channel position (mm) : " << fStartChannelPosition << endl;
    metadata << "End channel position (mm) : " << fEndChannelPosition << endl;
    metadata << "Pitch (mm) : " << fPitch << endl;

    EndPrintProcess();
}
