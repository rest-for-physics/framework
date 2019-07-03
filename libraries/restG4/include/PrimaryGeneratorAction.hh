//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file radioactivedecay/rdecay01/include/PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class
//
//
// $Id: PrimaryGeneratorAction.hh 68017 2013-03-13 13:29:53Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "DetectorConstruction.hh"
#include "G4IonTable.hh"
#include "G4ParticleGun.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

#include <TH1D.h>

#include <fstream>
#include <iostream>
using namespace std;

const int nSpct = 3000;

class G4Event;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
   public:
    PrimaryGeneratorAction(DetectorConstruction* pDetector);
    ~PrimaryGeneratorAction();

   public:
    virtual void GeneratePrimaries(G4Event*);
    G4ParticleGun* GetParticleGun() { return fParticleGun; };

    void SetSpectrum(TH1D* spt, double eMin = 0, double eMax = 0) {
        TString xLabel = (TString)spt->GetXaxis()->GetTitle();

        if (xLabel.Contains("MeV")) {
            energyFactor = 1.e3;
        } else if (xLabel.Contains("GeV")) {
            energyFactor = 1.e6;
        } else {
            energyFactor = 1.;
        }

        fSpectrum = spt;
        fSpectrumIntegral = fSpectrum->Integral();

        startEnergyBin = 1;
        endEnergyBin = fSpectrum->GetNbinsX();

        if (eMin > 0) {
            for (int i = startEnergyBin; i <= endEnergyBin; i++) {
                if (fSpectrum->GetBinCenter(i) > eMin) {
                    startEnergyBin = i;
                    break;
                }
            }
        }

        if (eMax > 0) {
            for (int i = startEnergyBin; i <= endEnergyBin; i++) {
                if (fSpectrum->GetBinCenter(i) > eMax) {
                    endEnergyBin = i;
                    break;
                }
            }
        }

        fSpectrumIntegral = fSpectrum->Integral(startEnergyBin, endEnergyBin);
    }

    void SetAngularDistribution(TH1D* ang) { fAngularDistribution = ang; }

   private:
    G4ParticleGun* fParticleGun;
    DetectorConstruction* fDetector;

    TH1D* fSpectrum;
    TH1D* fAngularDistribution;

    Int_t startEnergyBin;
    Int_t endEnergyBin;
    Double_t fSpectrumIntegral;

    Int_t nBiasingVolumes;

    Double_t energyFactor;

    void SetParticlePosition();
    void SetParticlePosition(int n);
    G4ParticleDefinition* SetParticleDefinition(int n);
    void SetParticleEnergy(int n);
    void SetParticleDirection(int n);

    G4ThreeVector GetIsotropicVector();
    Double_t GetAngle(G4ThreeVector x, G4ThreeVector y);
    Double_t GetCosineLowRandomThetaAngle();

    G4String fParType;
    G4String fGenType;
    G4double fParEnergy;
    G4double fParGenerator;

    G4String fSpctFilename;

    G4int gammaSpectrum[nSpct];

    G4int nCollections;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
