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

#ifndef RestCore_TRestGas
#define RestCore_TRestGas
#include <stdlib.h>
#include <fstream>
#include <iostream>

#include <TROOT.h>
#include "TApplication.h"
#include "TArrayI.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TNamed.h"
#include "TString.h"
#include "TSystem.h"
#include "TVector3.h"

#include "TRestMetadata.h"

//#define USE_Garfield
#if defined USE_Garfield
#include "ComponentConstant.hh"
#include "GeometrySimple.hh"
#include "MediumMagboltz.hh"
#include "Sensor.hh"
#include "SolidBox.hh"
#include "TrackHeed.hh"
using namespace Garfield;
#else
class MediumMagboltz;
#endif

const int RESTGAS_ERROR = -1;
const int RESTGAS_INTITIALIZED = 0;
const int RESTGAS_CFG_LOADED = 1;
const int RESTGAS_GASFILE_LOADED = 2;

//! A specific metadata class to generate and read gas files using Magboltz
//! interface
class TRestGas : public TRestMetadata {
 private:
  MediumMagboltz* fGasMedium;  //! Pointer to Garfield::MediumMagboltz class
                               //! giving access to gas properties

  Int_t fStatus;  // Used to define the status of the gas : RESTGAS_ERROR,
                  // RESTGAS_INTITIALIZED, RESTGAS_CFG_LOADED,
                  // RESTGAS_GASFILE_LOADED
  TString fGasFilename;  // The filename of the Magboltz gas file.
  Int_t fNofGases;  // Number of different elements composing the gas mixture

  Int_t fNCollisions;  // Number of collisions used in the Magboltz calculation.
  Double_t fMaxElectronEnergy;  // Maximum electron energy, in eV, used in
                                // Magboltz gas calculation.
  Double_t
      fW;  // Work function for electron extraction. This is defined by REST.

  std::vector<TString> fGasComponentName;  // A string vector storing the names
                                           // of each of the gas components
  std::vector<Double_t>
      fGasComponentFraction;  // A double vector storing the fraction values of
                              // each of the gas components

  Double_t fPressureInAtm;   // Pressure of the gas in atm.
  Double_t fTemperatureInK;  // Temperature of the gas in K.

  Int_t fEnodes;  // Number of electric field nodes used in the gas calculation.
  Double_t fEmax;  // Minimum value of the electric field used for the gas
                   // calculation.
  Double_t fEmin;  // Maximum value of the electric field used for the gas
                   // calculation.

  std::vector<Double_t> fEFields;  // The electric field nodes as calculated by
                                   // Garfield::MediumMagboltz.
  std::vector<Double_t> fBFields;  // The magnetic field nodes as calculated by
                                   // Garfield::MediumMagboltz.
  std::vector<Double_t>
      fAngles;  // The field angles as calculated by Garfield::MediumMagboltz.

  TString fGDMLMaterialRef;  // The corresponding material reference name in
                             // GDML description

  bool fGasGeneration;  //!             If true, and the pre-generated Magboltz
                        //!             gas file is not found, it will allow to
                        //!             launch the gas generation.

  TString fGasOutputPath;  //!          A string to store the output path where
                           //!          a new generated gas file will be written

  TString fGasServer;  //!              The remote server from where we retrieve
                       //!              the gasFiles. If it is set to *none*
                       //!              local files will be used.

  TString fGasFileContent;  // Used for saving the gasFile into a root file

  void InitFromConfigFile();
  string ConstructFilename();

  void AddGasComponent(std::string gasName, Double_t fraction);

  void GenerateGasFile();

  void UploadGasToServer(string gasFilename);

 public:
  TRestGas();
  TRestGas(const char* cfgFileName, string name = "",
           bool gasGeneration = false);
  ~TRestGas();

  /// This enables the generation of the gas file if a non existing gas file is
  /// found.
  void EnableGasGeneration() { fGasGeneration = true; }

  /// Returns true if the file generation is enabled. False otherwise.
  bool GasFileGenerationEnabled() { return fGasGeneration; }

  /// Returns true if the gas file has been properly loaded. False otherwise.
  bool GasFileLoaded() { return fStatus == RESTGAS_GASFILE_LOADED; }

  void Initialize();

  void LoadGasFile();

  string FindGasFile(string name);

  void CalcGarField(double Emin, double Emax, int n);

  Int_t Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0);

  void InitFromRootFile();

  Double_t GetW() { return fW; }

  /// Returns the maximum electron energy used by Magboltz for the gas
  /// properties calculation
  Double_t GetMaxElectronEnergy() { return fMaxElectronEnergy; }

  /// Returns the number of gas elements/compounds present in the gas mixture.
  Int_t GetNofGases() { return fNofGases; }

  /// Returns the gas component *n*.
  TString GetGasComponentName(Int_t n) {
    if (n >= GetNofGases()) {
      cout << "REST WARNING. Gas name component n=" << n
           << " requested. But only " << GetNofGases()
           << " component(s) in the mixture." << endl;
      return "";
    }
    return fGasComponentName[n];
  }

  TString GetGasMixture();

  Double_t GetDriftVelocity(Double_t E);
  Double_t GetLongitudinalDiffusion(Double_t E);
  Double_t GetTransversalDiffusion(Double_t E);
  Double_t GetTownsendCoefficient(Double_t E);
  Double_t GetAttachmentCoefficient(Double_t E);

  void GetGasWorkFunction();

  /// Returns the gas fraction in volume for component *n*.
  Double_t GetGasComponentFraction(Int_t n) {
    if (n >= GetNofGases()) {
      cout << "REST WARNING. Gas fraction for component n=" << n
           << " requested. But only " << GetNofGases()
           << " component(s) in the mixture." << endl;
      return 0.;
    }

    return fGasComponentFraction[n];
  }

  /// Returns the gas pressure in atm.
  Double_t GetPressure() { return fPressureInAtm; };

  /// Returns the gas temperature in K.
  Double_t GetTemperature() { return fTemperatureInK; };

  /// Returns the gas work function in eV.
  Double_t GetWvalue() { return fW; }

#ifndef __CINT__
  /// Return pointer to Garfield::MediumGas for gas properties
  MediumMagboltz* GetGasMedium() { return fGasMedium; };
#endif

  /// Return reference name of the corresponding material in GDML file
  TString GetGDMLMaterialRef() { return fGDMLMaterialRef; };

  void SetPressure(Double_t pressure);

  /// Sets the maximum electron energy to be used in gas generation.
  void SetMaxElectronEnergy(Double_t energy) { fMaxElectronEnergy = energy; }

  /// Sets the value of the work funtion for the gas mixture.
  void SetWvalue(Double_t iP) { fW = iP; }

  void PlotDriftVelocity(Double_t eMin, Double_t eMax, Int_t nSteps);
  void PlotLongitudinalDiffusion(Double_t eMin, Double_t eMax, Int_t nSteps);
  void PlotTransversalDiffusion(Double_t eMin, Double_t eMax, Int_t nSteps);
  void PlotTownsendCoefficient(Double_t eMin, Double_t eMax, Int_t nSteps);
  void PrintGasInfo();
  void PrintGasFileContent() { cout << fGasFileContent << endl; };

  /// Prints the metadata information from the gas
  void PrintMetadata() { PrintGasInfo(); }

  ClassDef(TRestGas, 2);  // Gas Parameters
};

#endif
