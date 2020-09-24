#include <iostream>
#include <limits>
#include "TRestStringHelper.h"
using namespace std;

namespace REST_Units {
map<string, pair<int, double>> __ListOfRESTUnits;
}
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
string RemoveUnitsFromString(string s) {
    string value = "";

    // ss will be the string after we clean all units
    string ss = s;
    map<string, pair<int, double>>::iterator it;
    for (it = __ListOfRESTUnits.begin(); it != __ListOfRESTUnits.end(); ++it) {
        ss = ss.substr(0, ss.find(it->first));
    }

    return s.substr(0, ss.find_last_of("1234567890(),") + 1);
}

///////////////////////////////////////////////
/// \brief It scales a physics measurement with its units into a REST default units value.
///
/// For example, string in = "35.V/cm", return 3.5 (V/mm)
Double_t GetDblValueInString(string in) {
    string units = FindRESTUnitsInString(in);
    double val = StringToDouble(RemoveUnitsFromString(in));
    return ConvertValueToRESTUnits(val, units);
}

///////////////////////////////////////////////
/// \brief It scales a physics measurement with its units into a REST default units value.
///
/// For example, string in = "(0,5)MeV", return (0,5000) (keV)
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
/// For example, string in = "35.V/cm", return 3.5 (V/mm)
/// string in = "(0,5,6)cm", return (0,50,60) mm
TVector3 Get3DVectorValueInString(string in) {
    string unit = FindRESTUnitsInString(in);
    TVector3 value = StringTo3DVector(RemoveUnitsFromString(in));
    Double_t valueX = REST_Units::ConvertValueToRESTUnits(value.X(), unit);
    Double_t valueY = REST_Units::ConvertValueToRESTUnits(value.Y(), unit);
    Double_t valueZ = REST_Units::ConvertValueToRESTUnits(value.Z(), unit);
    return TVector3(valueX, valueY, valueZ);
}

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
/// \brief Private method of this namespace, called during __static_initialization_and_destruction_0()
///
double _AddUnit(string name, int type, double scale) {
    __ListOfRESTUnits[name] = {type, scale};
    return scale;
}

////////////////////////////////////////////////////////////////
///
/// Wrapper class for custom composite unit, e.g. mm/us, kg-yr, V/cm
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
/// **Note**: It doesn't support unit with numbers, e.g. m/s^2
///
/// \class TRestSystemOfUnits
///
TRestSystemOfUnits::TRestSystemOfUnits(string unitsStr) {
    if (unitsStr == "") {
        fZombie = true;
        return;
    }

    if (unitsStr.find_first_of("/-*", 0) == -1) {
        // single unit
        if (!IsBasicUnit(unitsStr)) {
            fZombie = true;
            return;
        }
        component.push_back(unitsStr);
        type.push_back(GetUnitType(unitsStr));
        order.push_back(1);
        scale.push_back(GetUnitScale(unitsStr));
    } else {
        int pos = -1;
        int front = 0;
        double nextorder = 1;
        int lasttype = -1;
        while (1) {
            pos = unitsStr.find_first_of("/-*", pos + 1);
            string sub = unitsStr.substr(front, pos - front);

            if (sub != "") {
                if (!IsBasicUnit(sub)) {
                    fZombie = true;
                    return;
                }
                auto _type = GetUnitType(sub);
                auto _scale = GetUnitScale(sub);
                if (_type == lasttype) {
                    // we cannnot have units like keV/GeV
                    fZombie = true;
                    return;
                }

                component.push_back(sub);
                type.push_back(_type);
                order.push_back(nextorder);
                scale.push_back(_scale);

                if (pos != -1) {
                    char mark = unitsStr[pos];
                    if (mark == '-' || mark == '*') {
                        nextorder = 1;
                    } else if (mark == '/') {
                        nextorder = -1;
                    }
                }
                lasttype = _type;
            }
            front = pos + 1;
            if (pos == -1) break;
        }
    }

    fScaleCombined = 1;
    for (int i = 0; i < component.size(); i++) {
        if (order[i] == 1)
            fScaleCombined *= scale[i];
        else if (order[i] == -1)
            fScaleCombined /= scale[i];
    }
    fZombie = false;
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

}  // namespace REST_Units
