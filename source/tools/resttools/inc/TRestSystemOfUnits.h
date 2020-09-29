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
#include <map>
#include <string>
using namespace std;

#include <TString.h>
#include <TVector2.h>
#include <TVector3.h>

#ifdef REST_UnitsAdd_Caller
#define AddUnit(name, type, scale) const double name = _AddUnit(#name, type, scale)
#else
#define AddUnit(name, type, scale) const double name = scale
#endif

/// This namespace defines the unit conversion for different units which are understood by REST.
namespace REST_Units {

class TRestSystemOfUnits {
   private:
    vector<string> component;
    vector<int> type;
    vector<double> scale;
    vector<double> order;

    Bool_t fZombie;

    double fScaleCombined;

    /// Get the type of the units
    int GetUnitType(string singleUnit);
    /// Get the scale of the unit to convert to the REST standard units
    double GetUnitScale(string singleUnit);

   public:
    // We use more common high energy physics unit instead of SI unit
    enum Physical_Unit {
        Energy,
        Time,
        Distance,
        Mass,
        Electric,
        Magnetic,
        Pressure,
        Volume,
        NOT_A_UNIT = -1
    };

    /// Constructor from a unit string
    TRestSystemOfUnits(string unitsStr);
    /// Whether this unit is zombie(invalid)
    bool IsZombie() const { return fZombie; }

    friend Double_t operator*(const Double_t& val, const TRestSystemOfUnits& units) {
        if (units.fZombie) return val;
        return val * units.fScaleCombined;
    }

    friend Double_t operator/(const Double_t& val, const TRestSystemOfUnits& units) {
        if (units.fZombie) return val;
        return val / units.fScaleCombined;
    }
};

bool IsBasicUnit(string in);
bool IsUnit(string in);

string FindRESTUnitsInString(string InString);
string RemoveUnitsFromString(string s);
Double_t ConvertValueToRESTUnits(Double_t value, string unitsStr);
Double_t ConvertRESTUnitsValueToCustomUnits(Double_t value, string unitsStr);
Double_t GetDblValueInString(string in);
TVector2 Get2DVectorValueInString(string in);
TVector3 Get3DVectorValueInString(string in);

extern map<string, pair<int, double>> __ListOfRESTUnits;  // name, [unit type id, scale]

double _AddUnit(string name, int type, double scale);

// energy unit multiplier
AddUnit(meV, TRestSystemOfUnits::Energy, 1e6);
AddUnit(eV, TRestSystemOfUnits::Energy, 1e3);
AddUnit(keV, TRestSystemOfUnits::Energy, 1);
AddUnit(MeV, TRestSystemOfUnits::Energy, 1e-3);
AddUnit(GeV, TRestSystemOfUnits::Energy, 1e-6);
AddUnit(J, TRestSystemOfUnits::Energy, 1.60e-19);
AddUnit(kJ, TRestSystemOfUnits::Energy, 1.60e-22);

// time unit multiplier
AddUnit(ns, TRestSystemOfUnits::Time, 1.e3);
AddUnit(us, TRestSystemOfUnits::Time, 1.);
AddUnit(ms, TRestSystemOfUnits::Time, 1.e-3);
AddUnit(s, TRestSystemOfUnits::Time, 1.e-6);
AddUnit(minu, TRestSystemOfUnits::Time, 1.67e-8);
AddUnit(hr, TRestSystemOfUnits::Time, 2.78e-10);
AddUnit(day, TRestSystemOfUnits::Time, 1.16e-11);
AddUnit(mon, TRestSystemOfUnits::Time, 3.85e-13);
AddUnit(yr, TRestSystemOfUnits::Time, 3.17e-14);

// distance unit multiplier
AddUnit(um, TRestSystemOfUnits::Distance, 1e3);
AddUnit(mm, TRestSystemOfUnits::Distance, 1.);
AddUnit(cm, TRestSystemOfUnits::Distance, 1e-1);
AddUnit(m, TRestSystemOfUnits::Distance, 1e-3);

// volume unit multiplier
AddUnit(mm3, TRestSystemOfUnits::Volume, 1.);
AddUnit(cm3, TRestSystemOfUnits::Volume, 1e-3);
AddUnit(dm3, TRestSystemOfUnits::Volume, 1e-6);
AddUnit(m3, TRestSystemOfUnits::Volume, 1e-9);

// mass unit multiplier
AddUnit(mg, TRestSystemOfUnits::Mass, 1e6);
AddUnit(gram, TRestSystemOfUnits::Mass, 1e3);
AddUnit(g, TRestSystemOfUnits::Mass, 1e3);
AddUnit(kg, TRestSystemOfUnits::Mass, 1.);
AddUnit(ton, TRestSystemOfUnits::Mass, 1e-3);

// e-potential unit multiplier
AddUnit(mV, TRestSystemOfUnits::Electric, 1.e3);
AddUnit(V, TRestSystemOfUnits::Electric, 1.);
AddUnit(kV, TRestSystemOfUnits::Electric, 1.e-3);

// magnetic field unit multiplier
AddUnit(mT, TRestSystemOfUnits::Magnetic, 1.e3);
AddUnit(T, TRestSystemOfUnits::Magnetic, 1.);
AddUnit(G, TRestSystemOfUnits::Magnetic, 1.e4);

// pressure field unit multiplier
AddUnit(bar, TRestSystemOfUnits::Pressure, 1.);
AddUnit(mbar, TRestSystemOfUnits::Pressure, 1.e3);
AddUnit(atm, TRestSystemOfUnits::Pressure, 1.);
AddUnit(torr, TRestSystemOfUnits::Pressure, 760);
AddUnit(MPa, TRestSystemOfUnits::Pressure, 0.101325);
AddUnit(kPa, TRestSystemOfUnits::Pressure, 101.325);
AddUnit(Pa, TRestSystemOfUnits::Pressure, 101325);
AddUnit(mPa, TRestSystemOfUnits::Pressure, 10132500);

}  // namespace REST_Units

typedef REST_Units::TRestSystemOfUnits units;

#endif
