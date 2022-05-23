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

#include <TMath.h>
#include <TString.h>
#include <TVector2.h>
#include <TVector3.h>

#include <iostream>
#include <map>
#include <string>

#ifdef REST_UnitsAdd_Caller
#define AddUnit(name, type, scale) constexpr double name = _AddUnit(#name, type, scale)
#else
#define AddUnit(name, type, scale) constexpr double name = scale
#endif

/// This namespace defines the unit conversion for different units which are understood by REST.
namespace REST_Units {

// We use more common physics units instead of SI unit
enum Physical_Unit {
    Energy,
    Time,
    Length,
    Mass,
    ElectricField,
    MagneticField,
    Pressure,
    Angle,
    NOT_A_UNIT = -1
};

class TRestSystemOfUnits {
   private:
    // stores a list of base units for composite units
    std::vector<int> fComponents;
    // stores a list of units order for composite units
    std::vector<double> fComponentOrder;

    Bool_t fZombie;

    double fScaleCombined;

    /// Get the type of the units
    int GetUnitType(std::string singleUnit);
    /// Get the scale of the unit to convert to the REST standard units
    double GetUnitScale(std::string singleUnit);

   public:
    /// Constructor from a unit std::string
    TRestSystemOfUnits(std::string unitsStr);
    /// Whether this unit is zombie(invalid)
    bool IsZombie() const { return fZombie; }
    std::string ToStandardDefinition();

    friend Double_t operator*(const Double_t& val, const TRestSystemOfUnits& units) {
        if (units.fZombie) return val;
        return val * units.fScaleCombined;
    }

    friend Double_t operator/(const Double_t& val, const TRestSystemOfUnits& units) {
        if (units.fZombie) return val;
        return val / units.fScaleCombined;
    }
};

bool IsBasicUnit(std::string in);
bool IsUnit(std::string in);

double GetScaleToStandardUnit(std::string unitsdef);
std::string GetStandardUnitDefinition(std::string unitsdef);
std::string FindRESTUnitsInString(std::string InString);
std::string RemoveUnitsFromString(std::string s);
Double_t ConvertValueToRESTUnits(Double_t value, std::string unitsStr);
Double_t ConvertRESTUnitsValueToCustomUnits(Double_t value, std::string unitsStr);

Double_t GetDblValueInString(std::string in);
TVector2 Get2DVectorValueInString(std::string in);
TVector3 Get3DVectorValueInString(std::string in);

Double_t GetValueInRESTUnits(std::string in);
Double_t GetDblValueInRESTUnits(std::string in);
TVector2 Get2DVectorInRESTUnits(std::string in);
TVector3 Get3DVectorInRESTUnits(std::string in);

double _AddUnit(std::string name, int type, double scale);

// energy unit multiplier
AddUnit(meV, REST_Units::Energy, 1e6);
AddUnit(eV, REST_Units::Energy, 1e3);
AddUnit(keV, REST_Units::Energy, 1);
AddUnit(MeV, REST_Units::Energy, 1e-3);
AddUnit(GeV, REST_Units::Energy, 1e-6);
AddUnit(J, REST_Units::Energy, 1.60e-19);
AddUnit(kJ, REST_Units::Energy, 1.60e-22);

// time unit multiplier
AddUnit(ns, REST_Units::Time, 1.e3);
AddUnit(us, REST_Units::Time, 1.);
AddUnit(ms, REST_Units::Time, 1.e-3);
AddUnit(s, REST_Units::Time, 1.e-6);
AddUnit(Hz, REST_Units::Time, 1.e6);
AddUnit(minu, REST_Units::Time, 1.67e-8);
AddUnit(hr, REST_Units::Time, 2.78e-10);
AddUnit(day, REST_Units::Time, 1.16e-11);
AddUnit(mon, REST_Units::Time, 3.85e-13);
AddUnit(yr, REST_Units::Time, 3.17e-14);

// distance unit multiplier
AddUnit(um, REST_Units::Length, 1e3);
AddUnit(mm, REST_Units::Length, 1.);
AddUnit(cm, REST_Units::Length, 1e-1);
AddUnit(m, REST_Units::Length, 1e-3);

// mass unit multiplier
AddUnit(mg, REST_Units::Mass, 1e6);
AddUnit(gram, REST_Units::Mass, 1e3);
AddUnit(g, REST_Units::Mass, 1e3);
AddUnit(kg, REST_Units::Mass, 1.);
AddUnit(ton, REST_Units::Mass, 1e-3);

// e-potential unit multiplier
AddUnit(mV, REST_Units::ElectricField, 1.e3);
AddUnit(V, REST_Units::ElectricField, 1.);
AddUnit(kV, REST_Units::ElectricField, 1.e-3);

// magnetic field unit multiplier
AddUnit(mT, REST_Units::MagneticField, 1.e3);
AddUnit(T, REST_Units::MagneticField, 1.);
AddUnit(G, REST_Units::MagneticField, 1.e4);

// pressure field unit multiplier
AddUnit(bar, REST_Units::Pressure, 1.);
AddUnit(mbar, REST_Units::Pressure, 1.e3);
AddUnit(atm, REST_Units::Pressure, 1.013);
AddUnit(torr, REST_Units::Pressure, 760);
AddUnit(MPa, REST_Units::Pressure, 0.101325);
AddUnit(kPa, REST_Units::Pressure, 101.325);
AddUnit(Pa, REST_Units::Pressure, 101325);
AddUnit(mPa, REST_Units::Pressure, 10132500);

// angle field unit multiplier
AddUnit(rad, REST_Units::Angle, 1.);
AddUnit(radian, REST_Units::Angle, 1.);
AddUnit(radians, REST_Units::Angle, 1.);
AddUnit(deg, REST_Units::Angle, TMath::DegToRad());
AddUnit(degree, REST_Units::Angle, TMath::DegToRad());
AddUnit(degrees, REST_Units::Angle, TMath::DegToRad());
}  // namespace REST_Units

typedef REST_Units::TRestSystemOfUnits units;

#endif
