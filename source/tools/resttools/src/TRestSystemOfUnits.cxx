#include <iostream>
#include <limits>
#include "TRestStringHelper.h"
using namespace std;

namespace REST_Units {
map<string, pair<int, double>> __ListOfRESTUnits;
}

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
/// \class REST_Units
///
/// <hr>
namespace REST_Units {

bool IsUnit(string unitsStr) { return !TRestSystemOfUnits(unitsStr).IsZombie(); }
bool IsBasicUnit(string unitsStr) { return (__ListOfRESTUnits.count(unitsStr) == 1); }

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
    double a = TRestSystemOfUnits((string)unitsStr).ToRESTUnits(value);
    return a;
}

double _AddUnit(string name, int type, double scale) {
    __ListOfRESTUnits[name] = {type, scale};
    return scale;
}

////////////////////////////////////////////////////////////////
///
/// Wrapper class for custom composite unit, e.g. mm/us, kg-yr, V/cm
///
/// Usage: TRestSystemOfUnits("kg-yr").ToRESTUnits(300)
/// Note: It doesn't support unit with numbers, e.g. m/s^2
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

Double_t TRestSystemOfUnits::ToRESTUnits(Double_t value) {
    if (IsZombie()) {
        return value;
    }
    return value / fScaleCombined;
}

void TRestSystemOfUnits::Print() {}
}  // namespace REST_Units