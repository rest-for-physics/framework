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
/// \file radioactivedecay/rdecay01/include/EventAction.hh
/// \brief Definition of the EventAction class
//
// $Id: EventAction.hh 68017 2013-03-13 13:29:53Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef EventAction_h
#define EventAction_h 1

using namespace std;

#include "G4UserEventAction.hh"
#include "globals.hh"

#include "TH1D.h"

#include <TRestG4Metadata.h>
#include <TRestG4Event.h>
#include <TRestG4Track.h>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EventAction : public G4UserEventAction
{
    public:
        EventAction();
        ~EventAction();

    public:
        virtual void BeginOfEventAction(const G4Event*);
        virtual void   EndOfEventAction(const G4Event*);

        //  void AddDecayChain(G4String val) { fDecayChain += val;};
        //  void AddOutputString(G4String val) { fOutputString += val;};

        //   void SetPrevTime( G4double val ) { fPrevTime = val; } 
        //   G4double GetPrevTime( ) { return fPrevTime; }
        void SetDepositSpectrum( TH1D *dSpectrum ) { fDepositSpectrum = dSpectrum; }
        void SetDepositSpectrum_ROI( TH1D *dSpectrum ) { fDepositSpectrum_ROI = dSpectrum; }
        //       TH1D *GetDepositSpectrum() { return fDepositSpectrum; }
        //       TH1D *GetDepositSpectrum_ROI() { return fDepositSpectrum_ROI; }

    private:
        G4bool fDecayChain;
        //  G4String        fDecayChain;                   
        //  G4String        fOutputString;                   
        //  G4double 	    fPrevTime;
        //   EventMessenger* fEventMessenger;

        TH1D *fDepositSpectrum;
        TH1D *fDepositSpectrum_ROI;

        void SetSubeventIDs();

        vector <Double_t> fTrackTimestampList;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
