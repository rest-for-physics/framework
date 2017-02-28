///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSystemOfUnits.h
///
///             Feb 2016:   First concept
///                 author : Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestSystemOfUnits
#define RestCore_TRestSystemOfUnits

#include <iostream>
using namespace std;

#include <TString.h>

//! This namespace defines the unit conversion for different units which are understood by REST

//! TODO: Write a detailed description HERE
namespace REST_Units {

    const double meV = 1e6;
    const double eV = 1e3;
    const double keV = 1;
    const double MeV = 1e-3;
    const double GeV = 1e-6;


    Bool_t isEnergy( TString unitsStr );
    Double_t GetEnergyInRESTUnits( Double_t energy, TString unitsStr );

    const double ns = 1.e3;
    const double us = 1.;
    const double ms = 1.e-3;
    const double s = 1.e-6;

    Double_t GetTimeInRESTUnits( Double_t time, TString unitsStr );
    Bool_t isTime( TString unitsStr );

    const double um = 1e3;
    const double mm = 1.;
    const double cm = 1e-1;
    const double m = 1e-3;

    Double_t GetDistanceInRESTUnits( Double_t distance, TString unitsStr );
    Bool_t isDistance( TString unitsStr );

    const double Vum = 1.e-4;
    const double Vmm = 1.e-1;
    const double Vcm = 1.;
    const double Vm = 1e2;

    const double kVum = 1.e-7;
    const double kVmm = 1.e-4;
    const double kVcm = 1.e-3;
    const double kVm = 1e-1;

    Double_t GetFieldInRESTUnits( Double_t field, TString unitsStr );
    Bool_t isField( TString unitsStr );

    Double_t GetValueInRESTUnits( Double_t value, TString unitsStr );
}
#endif
