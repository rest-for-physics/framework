
#include "TRestSystemOfUnits.h"

#include <iostream>
#include <limits> 
using namespace std;


namespace REST_Units
{

    Double_t GetEnergyInRESTUnits( Double_t energy, TString unitsStr )
    {
        if( unitsStr == "meV" ) return energy/meV; 
        if( unitsStr == "eV" ) return energy/eV; 
        if( unitsStr == "keV" ) return energy/keV; 
        if( unitsStr == "MeV" ) return energy/MeV; 
        if( unitsStr == "GeV" ) return energy/GeV; 

        return energy;
    }

    Bool_t isEnergy( TString unitsStr )
    {
        if( unitsStr == "meV" ) return true;
        if( unitsStr == "eV" ) return true;
        if( unitsStr == "keV" ) return true;
        if( unitsStr == "MeV" ) return true;
        if( unitsStr == "GeV" ) return true;

        return false;
    }

    Double_t GetDistanceInRESTUnits( Double_t distance, TString unitsStr )
    {
        if( unitsStr == "um" ) return distance/um; 
        if( unitsStr == "mm" ) return distance/mm; 
        if( unitsStr == "cm" ) return distance/cm; 
        if( unitsStr == "m" ) return distance/m; 

        return distance;
    }

    Bool_t isDistance( TString unitsStr )
    {
        if( unitsStr == "um" ) return true;
        if( unitsStr == "mm" ) return true;
        if( unitsStr == "cm" ) return true;
        if( unitsStr == "m" ) return true;

        return false;
    }

    Double_t GetFieldInRESTUnits( Double_t field, TString unitsStr )
    {
        if( unitsStr == "V/um" ) return field/Vum;
        if( unitsStr == "V/mm" ) return field/Vmm;
        if( unitsStr == "V/cm" ) return field/Vcm;
        if( unitsStr == "V/m" ) return field/Vm;

        if( unitsStr == "kV/um" ) return field/kVum;
        if( unitsStr == "kV/mm" ) return field/kVmm;
        if( unitsStr == "kV/cm" ) return field/kVcm;
        if( unitsStr == "kV/m" ) return field/kVm;

        return field;
    }

    Bool_t isField( TString unitsStr )
    {
        if( unitsStr == "V/um" ) return true;
        if( unitsStr == "V/mm" ) return true;
        if( unitsStr == "V/cm" ) return true;
        if( unitsStr == "V/m" ) return true;

        if( unitsStr == "kV/um" ) return true;
        if( unitsStr == "kV/mm" ) return true;
        if( unitsStr == "kV/cm" ) return true;
        if( unitsStr == "kV/m" ) return true;

        return false;
    }

    Double_t GetValueInRESTUnits( Double_t value, TString unitsStr )
    {
        if( isEnergy ( unitsStr ) ) return GetEnergyInRESTUnits( value, unitsStr );
        if( isDistance ( unitsStr ) ) return GetDistanceInRESTUnits( value, unitsStr );
        if( isField ( unitsStr ) ) return GetFieldInRESTUnits( value, unitsStr );

        cout << endl;
        cout << "REST WARNING (REST_Units)  Unit=[" << unitsStr << "] not recognized" << endl;

        return (Double_t ) numeric_limits<Double_t>::quiet_NaN();
    }
}

