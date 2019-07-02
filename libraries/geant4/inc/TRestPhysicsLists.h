//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestPhysicsLists.h
///
///             G4 class to be used to design REST metadata classes to be
///             inherited from TRestMetadata
///             How to use: replace TRestPhysicsLists by your class name,
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             Apr 2017:   First concept. Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestPhysicsLists
#define RestCore_TRestPhysicsLists

#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include <TString.h>

#include <TRestMetadata.h>

class TRestPhysicsLists : public TRestMetadata {
   private:
    void Initialize();

    void InitFromConfigFile();

    TString GetPhysicsListOptionString(TString phName);

    std::vector<TString> fPhysicsLists;
    std::vector<TString> fPhysicsListOptions;

    Double_t fCutForElectron;
    Double_t fCutForGamma;
    Double_t fCutForPositron;
    Double_t fCutForMuon;
    Double_t fCutForNeutron;
    Double_t fMinEnergyRangeProductionCuts;
    Double_t fMaxEnergyRangeProductionCuts;

   public:
    Double_t GetCutForGamma() { return fCutForGamma; }
    Double_t GetCutForElectron() { return fCutForElectron; }
    Double_t GetCutForPositron() { return fCutForPositron; }
    Double_t GetCutForMuon() { return fCutForMuon; }
    Double_t GetCutForNeutron() { return fCutForNeutron; }

    Double_t GetMinimumEnergyProductionCuts() { return fMinEnergyRangeProductionCuts; }
    Double_t GetMaximumEnergyProductionCuts() { return fMaxEnergyRangeProductionCuts; }

    Int_t FindPhysicsList(TString phName);
    Bool_t PhysicsListExists(TString phName);

    TString GetPhysicsListOptionValue(TString phName, TString option);

    void PrintMetadata();

    // Constructors
    TRestPhysicsLists();
    TRestPhysicsLists(char* cfgFileName, std::string name = "");
    // Destructor
    ~TRestPhysicsLists();

    ClassDef(TRestPhysicsLists, 1);
};
#endif
