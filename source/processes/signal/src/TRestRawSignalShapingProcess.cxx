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
/// The TRestRawSignalShapingProcess allows to convolute a given time signal
/// response with the TRestRawSignal input signals found inside the input
/// TRestRawSignalEvent. This process may serve for conditioning the signals
/// produced in a MonteCarlo simulation and emulate those from real
/// electronics.
///
/// For the moment, only gaussian response has been implemented.
///
/// The different parameters allowed in this process are:
///
/// * **shapingType**: It defines the type of convolution to be performed.
///     - gaus : It produces a gausian convolution.
///     - shaper : It produces a shaping following traditional shaper
///               waveforms.
///     - shaperSin : It produces a shaping following traditional shaper
///               waveforms, it includes a sinusoidal effect.
///     - responseFile : A file providing a user provided response (TODO).
///
/// * **shapingTime** : The standard deviation of the gaussian convolution,
///                     or the shaping time on shaper models. Defined in
///                     samples unit.
/// * **gain** : A factor to amplify or attenuate the signal.
/// * **responseFile** : A response file to be used in case the shapingType
/// is defined to use a response file.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-March: First implementation
///             Xinglong
///
/// 2018-March: Transfered to TRestRawSignal
///             Javier Galan
///
/// \class      TRestRawSignalShapingProcess
/// \author     Xinglong
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestRawSignalShapingProcess.h"
using namespace std;

#include <TFile.h>
#include <TMath.h>

ClassImp(TRestRawSignalShapingProcess)

    ///////////////////////////////////////////////
    /// \brief Default constructor
    ///
    TRestRawSignalShapingProcess::TRestRawSignalShapingProcess() {
  Initialize();
}

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
TRestRawSignalShapingProcess::TRestRawSignalShapingProcess(char* cfgFileName) {
  Initialize();

  if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();

  PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestRawSignalShapingProcess::~TRestRawSignalShapingProcess() {
  delete fOutputSignalEvent;
  delete fInputSignalEvent;
}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestRawSignalShapingProcess::LoadDefaultConfig() {
  SetName("rawSignalShapingProcess-Default");
  SetTitle("Default config");
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestRawSignalShapingProcess::Initialize() {
  SetSectionName(this->ClassName());

  fInputSignalEvent = new TRestRawSignalEvent();
  fOutputSignalEvent = new TRestRawSignalEvent();

  fInputEvent = fInputSignalEvent;
  fOutputEvent = fOutputSignalEvent;
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
void TRestRawSignalShapingProcess::LoadConfig(string cfgFilename, string name) {
  if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Process initialization. Observable names are interpreted and auxiliar
/// observable members, related to VolumeEdep, MeanPos, TracksCounter, TrackEDep
/// observables defined in TRestGeant4AnalysisProcess are filled at this stage.
///
void TRestRawSignalShapingProcess::InitProcess() {
  /*
   * NOT IMPLEMENTED. TODO To use a generic response from a
   * predefined TRestSignal
   *
   * For the moment we do only a gausian shaping"
   * /

  responseSignal = new TRestRawSignal();

  if( fShapingType == "responseFile" )
  {
      TString fullPathName = (TString) getenv("REST_PATH") + "/data/signal/" +
  fResponseFilename; TFile *f = new TFile(fullPathName); responseSignal =
  (TRestRawSignal *) f->Get("signal Response"); f->Close();
  }

  if( GetVerboseLevel() >= REST_Debug )
  {
      CreateCanvas();
      fCanvas->Draw();
  }

  if( fShapingType == "gaus" )
  {
      responseSignal->InitGaussian( 100, 100, 30, 200 );

      if( GetVerboseLevel() >= REST_Debug )
      {
          responseSignal->GetGraph()->Draw();
          fCanvas->Update();
          GetChar();
      }
  }
  */
}

///////////////////////////////////////////////
/// \brief Function to include required initialization before each event starts
/// to process.
///
void TRestRawSignalShapingProcess::BeginOfEventProcess() {
  fOutputSignalEvent->Initialize();
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestRawSignalShapingProcess::ProcessEvent(TRestEvent* evInput) {
  fInputSignalEvent = (TRestRawSignalEvent*)evInput;

  if (fInputSignalEvent->GetNumberOfSignals() <= 0) return NULL;

  double* rsp;
  Int_t Nr;

  if (fShapingType == "gaus") {
    Double_t amp = fShapingGain;
    Int_t cBin = (Int_t)(fShapingTime * 3.5);
    Nr = 2 * cBin;
    Double_t sigma = fShapingTime;

    rsp = new double[Nr];
    for (int i = 0; i < Nr; i++)
      rsp[i] =
          (amp * TMath::Exp(-0.5 * (i - cBin) * (i - cBin) / sigma / sigma));
  } else if (fShapingType == "shaper") {
    Nr = (Int_t)(5 * fShapingTime);

    rsp = new double[Nr];
    for (int i = 0; i < Nr; i++) {
      Double_t coeff = ((Double_t)i) / fShapingTime;
      rsp[i] = (fShapingGain * TMath::Exp(-3. * coeff) * coeff * coeff * coeff);
    }
  } else if (fShapingType == "shaperSin") {
    Nr = (Int_t)(5 * fShapingTime);

    rsp = new double[Nr];
    for (int i = 0; i < Nr; i++) {
      Double_t coeff = ((Double_t)i) / fShapingTime;
      rsp[i] = (fShapingGain * TMath::Exp(-3. * coeff) * coeff * coeff * coeff *
                sin(coeff));
    }
  } else {
    if (GetVerboseLevel() >= REST_Warning)
      cout << "REST WARNING. Shaping type : " << fShapingType
           << " is not defined!!" << endl;
    return NULL;
  }

  for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++) {
    TRestRawSignal* shapingSignal = new TRestRawSignal();

    TRestRawSignal* inSignal = fInputSignalEvent->GetSignal(n);

    Int_t nBins = inSignal->GetNumberOfPoints();

    double* in = new double[nBins];
    for (int i = 0; i < nBins; i++) in[i] = inSignal->GetData(i);

    double* out = new double[nBins];
    for (int i = 0; i < nBins; i++) out[i] = 0;

    for (int m = 0; m < nBins; m++) {
      if (in[m] > 0) {
        for (int n = 0; n < Nr && m + n < nBins; n++)
          out[m + n] += rsp[n] * in[m];
      }
    }

    for (int i = 0; i < nBins; i++) shapingSignal->AddPoint((Short_t)out[i]);

    shapingSignal->SetSignalID(inSignal->GetSignalID());

    fOutputSignalEvent->AddSignal(*shapingSignal);

    delete shapingSignal;
    delete[] in;
    delete[] out;
  }

  delete[] rsp;

  return fOutputSignalEvent;
}

///////////////////////////////////////////////
/// \brief Function to include required actions after each event has been
/// processed.
///
void TRestRawSignalShapingProcess::EndOfEventProcess() {}

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been
/// processed.
///
void TRestRawSignalShapingProcess::EndProcess() {
  // Function to be executed once at the end of the process
  // (after all events have been processed)

  // Start by calling the EndProcess function of the abstract class.
  // Comment this if you don't want it.
  // TRestEventProcess::EndProcess();
}

///////////////////////////////////////////////
/// \brief Function to read input parameters from the RML
/// TRestGeant4AnalysisProcess metadata section
///
void TRestRawSignalShapingProcess::InitFromConfigFile() {
  // It is not used for the moment
  fResponseFilename = GetParameter("responseFile");

  // gaus, responseFile, etc
  fShapingType = GetParameter("shapingType", "gaus");

  fShapingTime = StringToDouble(GetParameter("shapingTime", "10"));

  fShapingGain = StringToDouble(GetParameter("gain", "1"));
}
