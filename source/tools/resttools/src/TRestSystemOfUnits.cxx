#include <bits/stdc++.h>

#include <iostream>
#include <limits>

#include "TRestStringHelper.h"
using namespace std;

namespace REST_Units {
struct UnitsStruct {
    UnitsStruct() {
        name = "";
        type = -1;
        scale = 1;
    }
    UnitsStruct(string _name, int _type, double _scale) {
        name = _name;
        type = _type;
        scale = _scale;
    }

    string name = "";
    int type = -1;
    double scale = 1;

    bool operator>(const UnitsStruct& compare) const { return name > compare.name; }

    bool operator<(const UnitsStruct& compare) const { return name < compare.name; }

    bool operator==(const UnitsStruct& compare) const { return name == compare.name; }
};

map<string, pair<int, double>> __ListOfRESTUnits;  // name, {type, scale}
}  // namespace REST_Units
#define REST_UnitsAdd_Caller

#include "TRestSystemOfUnits.h"

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
/// \namespace REST_Units
///
/// <hr>
namespace REST_Units {

///////////////////////////////////////////////
/// \brief Checks if the string is a REST supported unit
///
///	REST supports several basic units and kinds of their combinations(multiply, divide)
/// e.g.
/// cm, ns, mV, kg, keV
/// V/cm, kg-yr
///
/// Note: REST doesn't support units combination with numbers, e.g. m/s^2
bool IsUnit(string unitsStr) { return !TRestSystemOfUnits(unitsStr).IsZombie(); }

///////////////////////////////////////////////
/// \brief Checks if the string is a REST basic unit
///
bool IsBasicUnit(string unitsStr) { return (__ListOfRESTUnits.count(unitsStr) == 1); }

///////////////////////////////////////////////
/// \brief Get the scale to REST standard unit. scale (unitsdef) = 1 (standard unit)
///
/// e.g. 0.001(m) = 1(mm). Where "mm" is REST standard unit.
double GetScaleToStandardUnit(string unitsdef) { return 1 * TRestSystemOfUnits(unitsdef); }

///////////////////////////////////////////////
/// \brief Get standard form of this unit definition
///
/// e.g. m/s --> mm/us
string GetStandardUnitDefinition(string unitsdef) { return units(unitsdef).ToStandardDefinition(); }

///////////////////////////////////////////////
/// \brief Find and return the units definition in a string
///
/// We suppose the last of **value** before **units** must be "1234567890(),".
/// Hence this prepority can be used to spilt the input string into value part and unit part
/// e.g.
/// value="(1,-13)mm"
/// value="-3mm"
/// value="50,units=mm"
/// value="20 mm"
/// can both be recognized
///
string FindRESTUnitsInString(string s) {
    string unitsStr = "";

    size_t l = RemoveUnitsFromString(s).length();
    string unitDef = s.substr(l, -1);

    if (unitDef.find("=") != -1) {
        string def = unitDef.substr(0, unitDef.find("="));
        if (def == "units" || def == "unit") {
            unitsStr = unitDef.substr(unitDef.find("=") + 1, -1);
        }
    } else {
        unitsStr = Replace(unitDef, " ", "", 0);
    }

    if (IsUnit(unitsStr)) {
        return unitsStr;
    }
    return "";
}

///////////////////////////////////////////////
/// \brief It should remove all units found inside the input string
///
string RemoveUnitsFromString(string s) { return s.substr(0, s.find_first_not_of("1234567890(),.-")); }

///////////////////////////////////////////////
/// \brief It scales a physics measurement with its units into a REST default units value.
///
/// For example, string in = "35.V/cm", return 3.5 (V/mm)
///
Double_t GetDblValueInString(string in) {
    string units = FindRESTUnitsInString(in);
    double val = StringToDouble(RemoveUnitsFromString(in));
    return ConvertValueToRESTUnits(val, units);
}

///////////////////////////////////////////////
/// \brief It scales a physics measurement with its units into a REST default units value.
///
/// For example, string in = "35.V/cm", return 3.5 (V/mm)
///
Double_t GetValueInRESTUnits(string in) { return GetDblValueInString(in); }

///////////////////////////////////////////////
/// \brief It scales a physics measurement with its units into a REST default units value.
///
/// For example, string in = "35.V/cm", return 3.5 (V/mm)
///
Double_t GetDblValueInRESTUnits(string in) { return GetDblValueInString(in); }

///////////////////////////////////////////////
/// \brief It scales a physics measurement with its units into a REST default units value.
///
/// For example, string in = "(0,5)MeV", return (0,5000) (keV)
///
TVector2 Get2DVectorValueInString(string in) {
    string unit = FindRESTUnitsInString(in);
    TVector2 value = StringTo2DVector(RemoveUnitsFromString(in));
    Double_t valueX = REST_Units::ConvertValueToRESTUnits(value.X(), unit);
    Double_t valueY = REST_Units::ConvertValueToRESTUnits(value.Y(), unit);
    return TVector2(valueX, valueY);
}

///////////////////////////////////////////////
/// \brief It scales a physics measurement with its units into a REST default units value.
///
/// For example, string in = "(0,5)MeV", return (0,5000) (keV)
///
TVector2 Get2DVectorInRESTUnits(string in) { return Get2DVectorValueInString(in); }

///////////////////////////////////////////////
/// \brief It scales a physics measurement with its units into a REST default units value.
///
/// For example, string in = "(0,5,6)cm", return (0,50,60) mm
///
TVector3 Get3DVectorValueInString(string in) {
    string unit = FindRESTUnitsInString(in);
    TVector3 value = StringTo3DVector(RemoveUnitsFromString(in));
    Double_t valueX = REST_Units::ConvertValueToRESTUnits(value.X(), unit);
    Double_t valueY = REST_Units::ConvertValueToRESTUnits(value.Y(), unit);
    Double_t valueZ = REST_Units::ConvertValueToRESTUnits(value.Z(), unit);
    return TVector3(valueX, valueY, valueZ);
}

///////////////////////////////////////////////
/// \brief It scales a physics measurement with its units into a REST default units value.
///
/// For example, string in = "(0,5,6)cm", return (0,50,60) mm
///
TVector3 Get3DVectorInRESTUnits(string in) { return Get3DVectorValueInString(in); }

///////////////////////////////////////////////
/// \brief Convert value into REST units
///
/// For a given value with custom units, REST will first find its equivalent REST units and
/// calculate the scaling factor. Then it will strip off the custom units by dividing the
/// value with the scaling factor.
///
/// e.g. REST standard unit for dimension time*mass is kg*us. When we call:
/// `double a = ConvertValueToRESTUnits(8, "kg-yr"); // a = 2.5236593e+14`
/// The returned value is in unit "kg-us".
///
/// The returned the value can be thought as "unitless". This means we don't need to care
/// about it when saving it. When we are going to use it, we just add the unit back. For example:
/// `SetExposure(a*units("ton-day"));`
/// This explictily adds the unit "ton-day" to the "unitless" value a.
Double_t ConvertValueToRESTUnits(Double_t value, string unitsStr) {
    return value / TRestSystemOfUnits((string)unitsStr);
}

///////////////////////////////////////////////
/// \brief Convert value with REST units into the given custom units
///
Double_t ConvertRESTUnitsValueToCustomUnits(Double_t value, string unitsStr) {
    return value * TRestSystemOfUnits((string)unitsStr);
}

///////////////////////////////////////////////
/// \brief Add a unit with given name, type and scale.
///
/// Helper method, called during __static_initialization_and_destruction_0()
/// to fill __ListOfRESTUnits
double _AddUnit(string name, int type, double scale) {
    __ListOfRESTUnits[name] = {type, scale};
    return scale;
}

////////////////////////////////////////////////////////////////
///
/// Wrapper class for custom composite unit, e.g. mm/us, kg-yr, kg-m/s^2, cm^3
///
/// Implemented operator `/` and `*`, meaning strip-off/adds the unit for a unit-embeded/unitless value
///
/// Example 1: convert exposure "24(kg-yr)" to unit "ton-day"
/// `SetExposure(24/units("kg-yr")*units("ton-day"));`
///
/// Example 2: save a "unitless" value, then assign a concrete unit when using it
/// `double field = GetDblParameterWithUnits("electricField");`
/// `fGas->GetDriftVelocity(field*units("V/cm"));`
///
/// **Note**: If the unit definition is not recognized, the object will be zombie,
/// and the value will not be converted.
/// **Note**: Single unit must be pure alpha. It cannot contain numbers or symbols.
/// Do not use cm3, we shal use cm^3 instead.
///
/// \class TRestSystemOfUnits
///
TRestSystemOfUnits::TRestSystemOfUnits(string unitsStr) {
    if (unitsStr == "") {
        fZombie = true;
        return;
    }
    fScaleCombined = 1;

    for (int pos = 0; pos >= 0 && pos < unitsStr.size();) {
        if (isalpha(unitsStr[pos])) {
            int pos1 = pos;
            while (pos < unitsStr.size() && isalpha(unitsStr[pos])) {
                pos++;
            }
            int pos2 = pos;
            string singleunit = unitsStr.substr(pos1, pos2 - pos1);

            if (IsBasicUnit(singleunit)) {
                int orderprefix = 1;
                if (pos1 > 0) {
                    if (unitsStr[pos1 - 1] == '/') {
                        orderprefix = -1;
                    } else if (unitsStr[pos1 - 1] == '-' || unitsStr[pos1 - 1] == '*') {
                    } else {
                        warning << "illegeal unit combiner \"" << unitsStr[pos1 - 1] << "\"" << endl;
                    }
                }

                double ordernum = 1;
                if (pos2 < unitsStr.size() - 1) {
                    if (unitsStr[pos2] == '^') {
                        int pos3 = unitsStr.find_first_not_of("-0123456789.", pos2 + 1);
                        string orderstr = unitsStr.substr(pos2 + 1, pos3 - pos2 - 1);
                        ordernum = StringToDouble(orderstr);
                        pos = pos3;
                    }
                }

                int _type = GetUnitType(singleunit);
                double _scale = GetUnitScale(singleunit);
                double _order = ordernum * orderprefix;

                fComponents.push_back(_type);
                fComponentOrder.push_back(_order);

                fScaleCombined *= pow(_scale, _order);
            } else {
                warning << "not a unit \"" << singleunit << "\"" << endl;
            }

        } else {
            if (pos == 0 && unitsStr[pos] != '/') {
                cout << unitsStr << endl;
                warning << "first character \"" << unitsStr[pos] << "\" unrecognized in unit definition!"
                        << endl;
            }
            if (pos == unitsStr.size() - 1) {
                warning << "last character \"" << unitsStr[pos] << "\" unrecognized in unit definition!"
                        << endl;
            }

            pos++;
        }
    }
    if (fComponents.size() == 0) {
        fZombie = true;
    } else {
        fZombie = false;
    }
}

int TRestSystemOfUnits::GetUnitType(string singleUnit) {
    if (IsBasicUnit(singleUnit)) {
        return __ListOfRESTUnits[singleUnit].first;
    }
    return -1;
}

double TRestSystemOfUnits::GetUnitScale(string singleUnit) {
    if (IsBasicUnit(singleUnit)) {
        return __ListOfRESTUnits[singleUnit].second;
    }
    return 1;
}

string TRestSystemOfUnits::ToStandardDefinition() {
    string result = "";
    for (int i = 0; i < fComponents.size(); i++) {
        if (fComponentOrder[i] < 0) {
            result += "/";
        } else if (i > 0) {
            result += "-";
        }

        for (auto iter : __ListOfRESTUnits) {
            if (iter.second.first == fComponents[i] && iter.second.second == 1) {
                result += iter.first;
            }
        }

        if (abs(fComponentOrder[i]) != 1) {
            result += "^" + ToString(abs(fComponentOrder[i]));
        }
    }

    return result;
}

}  // namespace REST_Units
