
#include "TRestSystemOfUnits.h"
#include <iostream>
#include <limits>
#include "TRestStringHelper.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////
/// TODO : Write a detailed documentation here
///
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2017-Nov:   First concept and implementation of REST_Units namespace.
/// \author     Javier Galan
///
/// \class REST_Units
///
/// <hr>
namespace REST_Units {

Double_t GetEnergyInRESTUnits(Double_t energy, TString unitsStr) {
    if (unitsStr == "meV") return energy / meV;
    if (unitsStr == "eV") return energy / eV;
    if (unitsStr == "keV") return energy / keV;
    if (unitsStr == "MeV") return energy / MeV;
    if (unitsStr == "GeV") return energy / GeV;

    return energy;
}

Bool_t isEnergy(TString unitsStr) {
    if (unitsStr == "meV") return true;
    if (unitsStr == "eV") return true;
    if (unitsStr == "keV") return true;
    if (unitsStr == "MeV") return true;
    if (unitsStr == "GeV") return true;

    return false;
}

Double_t GetTimeInRESTUnits(Double_t time, TString unitsStr) {
    if (unitsStr == "ns") return time / ns;
    if (unitsStr == "us") return time / us;
    if (unitsStr == "ms") return time / ms;
    if (unitsStr == "s") return time / s;

    return time;
}

Bool_t isTime(TString unitsStr) {
    if (unitsStr == "ns") return true;
    if (unitsStr == "us") return true;
    if (unitsStr == "ms") return true;
    if (unitsStr == "s") return true;

    return false;
}

Double_t GetDistanceInRESTUnits(Double_t distance, TString unitsStr) {
    if (unitsStr == "um") return distance / um;
    if (unitsStr == "mm") return distance / mm;
    if (unitsStr == "cm") return distance / cm;
    if (unitsStr == "m") return distance / m;

    return distance;
}

Bool_t isDistance(TString unitsStr) {
    if (unitsStr == "um") return true;
    if (unitsStr == "mm") return true;
    if (unitsStr == "cm") return true;
    if (unitsStr == "m") return true;

    return false;
}

Double_t GetFieldInRESTUnits(Double_t field, TString unitsStr) {
    if (unitsStr == "V/um") return field / Vum;
    if (unitsStr == "V/mm") return field / Vmm;
    if (unitsStr == "V/cm") return field / Vcm;
    if (unitsStr == "V/m") return field / Vm;

    if (unitsStr == "kV/um") return field / kVum;
    if (unitsStr == "kV/mm") return field / kVmm;
    if (unitsStr == "kV/cm") return field / kVcm;
    if (unitsStr == "kV/m") return field / kVm;

    return field;
}

Bool_t isField(TString unitsStr) {
    if (unitsStr == "V/um") return true;
    if (unitsStr == "V/mm") return true;
    if (unitsStr == "V/cm") return true;
    if (unitsStr == "V/m") return true;

    if (unitsStr == "kV/um") return true;
    if (unitsStr == "kV/mm") return true;
    if (unitsStr == "kV/cm") return true;
    if (unitsStr == "kV/m") return true;

    return false;
}

Double_t GetPotentialInRESTUnits(Double_t potential, TString unitsStr) {
    if (unitsStr == "mV") return potential / mV;
    if (unitsStr == "V") return potential / V;
    if (unitsStr == "kV") return potential / kV;

    return potential;
}

// Return the value in Teslas
Double_t GetMagneticFieldInRESTUnits(Double_t field, TString unitsStr) {
    if (unitsStr == "T") return field / T;
    if (unitsStr == "mT") return field / mT;
    if (unitsStr == "G") return field / G;

    return field;
}

Bool_t isPotential(TString unitsStr) {
    if (unitsStr == "mV") return true;
    if (unitsStr == "V") return true;
    if (unitsStr == "kV") return true;

    return false;
}

Bool_t isMagneticField(TString unitsStr) {
    if (unitsStr == "T") return true;
    if (unitsStr == "mT") return true;
    if (unitsStr == "G") return true;

    return false;
}

bool IsUnit(string unitsStr) {
    if (isEnergy(unitsStr) || isDistance(unitsStr) || isField(unitsStr) || isTime(unitsStr) ||
        isPotential(unitsStr)) {
        return true;
    }

    if (isMagneticField(unitsStr)) return true;

    return false;
}

string GetRESTUnitsInString(string s) {
    string unitsStr = "";
    {
        // the last of a number must be "1234567890(),"
        // we use this prepority to spilt the input string
        // into value part and unit part
        // e.g.
        // value="(1,-13)mm"
        // value="-3mm"
        // value="50,units=mm"
        // value="20 mm"
        // can both be recoginzed
        string unitDef = s.substr(s.find_last_of("1234567890(),") + 1, -1);

        if (unitDef.find("=") != -1) {
            string def = unitDef.substr(0, unitDef.find("="));
            if (def == "units" || def == "unit") {
                unitsStr = unitDef.substr(unitDef.find("=") + 1, -1);
            }
        } else {
            unitsStr = Replace(unitDef, " ", "", 0);
        }
    }
    if (IsUnit(unitsStr)) {
        return unitsStr;
    }
    return "";
}

Double_t GetValueInRESTUnits(Double_t value, TString unitsStr) {
    if (unitsStr == "") {
        return value;
    }
    if (isEnergy(unitsStr)) return GetEnergyInRESTUnits(value, unitsStr);
    if (isDistance(unitsStr)) return GetDistanceInRESTUnits(value, unitsStr);
    if (isField(unitsStr)) return GetFieldInRESTUnits(value, unitsStr);
    if (isTime(unitsStr)) return GetTimeInRESTUnits(value, unitsStr);
    if (isPotential(unitsStr)) return GetPotentialInRESTUnits(value, unitsStr);
    if (isMagneticField(unitsStr)) return GetMagneticFieldInRESTUnits(value, unitsStr);

    cout << "REST WARNING (REST_Units)  Unit=[" << unitsStr << "] is not recognized" << endl;
    cout << "returning NaN value" << endl;
    cout << endl;
    return (Double_t)numeric_limits<Double_t>::quiet_NaN();
}
}  // namespace REST_Units
