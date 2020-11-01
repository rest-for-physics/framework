#include "TRestDataBase.h"
#include "TRestDetector.h"
#include "TRestReflector.h"
#include "TRestStringHelper.h"
#include "TRestStringOutput.h"
#include "TRestSystemOfUnits.h"
#include "TRestTools.h"
//////////////////////////////////////////////////////////////////////////
/// This script initializes REST global variables in sequence to clearify
/// their dependency, therefore avoiding seg.fault during startup. All 
/// global variables in libRestTools, if depend on other global variable,
/// should be placed here for initialization. 

string REST_COMMIT;
string REST_PATH;
string REST_USER;
string REST_USER_PATH;
map<string, string> REST_ARGS = {};
namespace REST_Reflection {
map<string, TDataType*> RESTListOfDataTypes = {};
}
map<string, RESTVirtualConverter*> RESTConverterMethodBase = {};

// initialize REST constants
struct __REST_CONST_INIT {
   public:
    __REST_CONST_INIT() {
        REST_COMMIT = TRestTools::Execute("rest-config --commit");

        char* _REST_PATH = getenv("REST_PATH");
        char* _REST_USER = getenv("USER");
        char* _REST_USERHOME = getenv("HOME");

        if (_REST_PATH == nullptr) {
            cout << "REST ERROR!! Lacking system env \"REST_PATH\"! Cannot start!" << endl;
            cout << "You need to source \"thisREST.sh\" first" << endl;
            abort();
        }
        REST_PATH = _REST_PATH;

        if (_REST_USER == nullptr) {
            cout << "REST WARNING!! Lacking system env \"USER\"!" << endl;
            cout << "Setting user name to : \"defaultUser\"" << endl;
            REST_USER = "defaultUser";
            setenv("USER", REST_USER.c_str(), true);

        } else {
            REST_USER = _REST_USER;
        }

        if (_REST_USERHOME == nullptr) {
            cout << "REST WARNING!! Lacking system env \"HOME\"!" << endl;
            cout << "Setting REST temp path to : " << REST_PATH + "/data" << endl;
            REST_USER_PATH = REST_PATH + "/data";
        } else {
            string restUserPath = (string)_REST_USERHOME + "/.rest";
            // check the directory exists
            if (!TRestTools::fileExists(restUserPath)) {
                mkdir(restUserPath.c_str(), S_IRWXU);
            }
            // check the runNumber file
            if (!TRestTools::fileExists(restUserPath + "/runNumber")) {
                TRestTools::Execute("echo 1 > " + restUserPath + "/runNumber");
            }
            // check the dataURL file
            if (!TRestTools::fileExists(restUserPath + "/dataURL")) {
                TRestTools::Execute("cp " + REST_PATH + "/data/dataURL " + restUserPath + "/");
            }
            // check the download directory
            if (!TRestTools::fileExists(restUserPath + "/download")) {
                mkdir((restUserPath + "/download").c_str(), S_IRWXU);
            }
            // check the gdml directory
            if (!TRestTools::fileExists(restUserPath + "/gdml")) {
                mkdir((restUserPath + "/gdml").c_str(), S_IRWXU);
            }

            // now we don't need to check write accessibility in other methods in REST
            REST_USER_PATH = restUserPath;
        }
    }
};
const __REST_CONST_INIT REST_CONST_INIT;

// initialize gDataBase
TRestDataBase* gDataBase = NULL;
MakeGlobal(TRestDataBase, gDataBase, 1);
TRestDetector* gDetector = NULL;
MakeGlobal(TRestDetector, gDetector, 1);

// initialize formatted message output tool
TRestStringOutput fout(REST_Silent, COLOR_BOLDBLUE, "[==", kBorderedMiddle);
TRestStringOutput ferr(REST_Silent, COLOR_BOLDRED, "-- Error : ", kHeaderedLeft);
TRestStringOutput warning(REST_Warning, COLOR_BOLDYELLOW, "-- Warning : ", kHeaderedLeft);
TRestStringOutput essential(REST_Essential, COLOR_BOLDGREEN, "", kHeaderedMiddle);
TRestStringOutput metadata(REST_Essential, COLOR_BOLDGREEN, "||", kBorderedMiddle);
TRestStringOutput info(REST_Info, COLOR_BLUE, "-- Info : ", kHeaderedLeft);
TRestStringOutput success(REST_Info, COLOR_GREEN, "-- Success : ", kHeaderedLeft);
TRestStringOutput debug(REST_Debug, COLOR_RESET, "-- Debug : ", kHeaderedLeft);
TRestStringOutput extreme(REST_Extreme, COLOR_RESET, "-- Extreme : ", kHeaderedLeft);

REST_Verbose_Level gVerbose = REST_Warning;

// initialize converter methods
template <class T>
string ToStringSimple(T source) {
    return ToString(source);
}
AddConverter(ToStringSimple, StringToInteger, int);
AddConverter(ToStringSimple, StringToDouble, double);
AddConverter(ToStringSimple, StringToBool, bool);
AddConverter(ToStringSimple, StringToFloat, float);
AddConverter(ToStringSimple, StringToLong, Long64_t);

char StringToChar(string in) { return in.size() > 0 ? (char)in[0] : 0; }
AddConverter(ToStringSimple, StringToChar, char);
short StringToShort(string in) { return StringToInteger(in); }
AddConverter(ToStringSimple, StringToShort, short);

long StringToLong32(string in) { return StringToDouble(in); }
AddConverter(ToStringSimple, StringToLong32, long);
unsigned char StringToUChar(string in) { return in.size() > 0 ? (unsigned char)in[0] : 0; }
AddConverter(ToStringSimple, StringToUChar, unsigned char);
unsigned short StringToUShort(string in) { return StringToInteger(in); }
AddConverter(ToStringSimple, StringToUShort, unsigned short);
unsigned int StringToUInt(string in) { return StringToInteger(in); }
AddConverter(ToStringSimple, StringToUInt, unsigned int);
unsigned long StringToULong(string in) { return StringToInteger(in); }
AddConverter(ToStringSimple, StringToULong, unsigned long);
unsigned long long StringToULL(string in) { return StringToInteger(in); }
AddConverter(ToStringSimple, StringToULL, unsigned long long);
TString StringToTString(string in) { return (TString)in; }
AddConverter(ToStringSimple, StringToTString, TString);

string TVector2ToString(TVector2 vec) { return Form("(%g,%g)", vec.X(), vec.Y()); }
AddConverter(TVector2ToString, StringTo2DVector, TVector2);
string TVector3ToString(TVector3 vec) { return Form("(%g,%g,%g)", vec.X(), vec.Y(), vec.Z()); }
AddConverter(TVector3ToString, StringTo3DVector, TVector3);

string StringToString(string in) { return in; }
AddConverter(StringToString, StringToString, string);

template <class T>
string VectorToString(vector<T> vec) {
    stringstream ss;
    ss << "{";
    for (int i = 0; i < vec.size(); i++) {
        ss << Converter<T>::thisptr->ToStringFunc(vec.at(i));
        if (i < vec.size() - 1) {
            ss << ",";
        }
    }
    ss << "}";
    return ss.str();
}
template <class T>
vector<T> StringToVector(string vec) {
    vector<T> result;
    if (vec[0] == '{' && vec[vec.size() - 1] == '}') {
        vec.erase(vec.begin());
        vec.erase(vec.end() - 1);
        vector<string> parts = Split(vec, ",");

        for (string part : parts) {
            while (part[0] == ' ') {
                part.erase(part.begin());
            }
            while (part[part.size() - 1] == ' ') {
                part.erase(part.end() - 1);
            }
            result.push_back(Converter<T>::thisptr->ParseStringFunc(part));
        }

    } else {
        cout << "illegal format!" << endl;
        return vector<T>{};
    }

    return result;
}
AddConverter(VectorToString, StringToVector, vector<int>);
AddConverter(VectorToString, StringToVector, vector<float>);
AddConverter(VectorToString, StringToVector, vector<double>);
AddConverter(VectorToString, StringToVector, vector<string>);
AddConverter(VectorToString, StringToVector, vector<TString>);

template <class T>
string SetToString(set<T> set) {
    string result = "{";
    for (auto val : set) {
        result += Converter<T>::thisptr->ToStringFunc(val) + ",";
    }
    if (result[result.size() - 1] == ',') result.erase(result.end() - 1);
    result += "}";
    return result;
}
template <class T>
set<T> StringToSet(string vec) {
    set<T> result;
    if (vec[0] == '{' && vec[vec.size() - 1] == '}') {
        vec.erase(vec.begin());
        vec.erase(vec.end() - 1);
        vector<string> parts = Split(vec, ",");

        for (string part : parts) {
            while (part[0] == ' ') {
                part.erase(part.begin());
            }
            while (part[part.size() - 1] == ' ') {
                part.erase(part.end() - 1);
            }
            result.insert(Converter<T>::thisptr->ParseStringFunc(part));
        }

    } else {
        cout << "illegal format!" << endl;
        return set<T>{};
    }
    return result;
}
AddConverter(SetToString, StringToSet, set<int>);
AddConverter(SetToString, StringToSet, set<float>);
AddConverter(SetToString, StringToSet, set<double>);
AddConverter(SetToString, StringToSet, set<string>);
AddConverter(SetToString, StringToSet, set<TString>);

template <class T1, class T2>
string MapToString(map<T1, T2> vec) {
    stringstream ss;
    ss << "{";
    int cont = 0;
    for (auto const& x : vec) {
        if (cont > 0) ss << ",";
        cont++;

        ss << "[";
        ss << Converter<T1>::thisptr->ToStringFunc(x.first);
        ss << ":";
        ss << Converter<T2>::thisptr->ToStringFunc(x.second);
        ss << "]";
    }
    ss << "}";
    return ss.str();
}
template <class T1, class T2>
map<T1, T2> StringToMap(string vec) {
    map<T1, T2> result;
    // input string format: {[dd:7],[aa:8],[ss:9]}
    if (vec[0] == '{' && vec[vec.size() - 1] == '}') {
        vec.erase(vec.begin());
        vec.erase(vec.end() - 1);
        vector<string> parts = Split(vec, ",");

        for (string part : parts) {
            while (part[0] == ' ') {
                part.erase(part.begin());
            }
            while (part[part.size() - 1] == ' ') {
                part.erase(part.end() - 1);
            }

            if (part[0] == '[' && part[part.size() - 1] == ']') {
                part.erase(part.begin());
                part.erase(part.end() - 1);
                vector<string> key_value = Split(part, ":");
                if (key_value.size() == 2) {
                    T1 key = Converter<T1>::thisptr->ParseStringFunc(key_value[0]);
                    T2 value = Converter<T2>::thisptr->ParseStringFunc(key_value[1]);
                    result[key] = value;
                } else {
                    cout << "illegal format!" << endl;
                    return map<T1, T2>{};
                }
            } else {
                cout << "illegal format!" << endl;
                return map<T1, T2>{};
            }
        }

    } else {
        cout << "illegal format!" << endl;
        return map<T1, T2>{};
    }

    return result;
}

#define comma ,
AddConverter(MapToString, StringToMap, map<int comma int>);
AddConverter(MapToString, StringToMap, map<int comma float>);
AddConverter(MapToString, StringToMap, map<int comma double>);
AddConverter(MapToString, StringToMap, map<int comma string>);
AddConverter(MapToString, StringToMap, map<int comma TString>);

AddConverter(MapToString, StringToMap, map<float comma int>);
AddConverter(MapToString, StringToMap, map<float comma float>);
AddConverter(MapToString, StringToMap, map<float comma double>);
AddConverter(MapToString, StringToMap, map<float comma string>);
AddConverter(MapToString, StringToMap, map<float comma TString>);

AddConverter(MapToString, StringToMap, map<double comma int>);
AddConverter(MapToString, StringToMap, map<double comma float>);
AddConverter(MapToString, StringToMap, map<double comma double>);
AddConverter(MapToString, StringToMap, map<double comma string>);
AddConverter(MapToString, StringToMap, map<double comma TString>);

AddConverter(MapToString, StringToMap, map<string comma int>);
AddConverter(MapToString, StringToMap, map<string comma float>);
AddConverter(MapToString, StringToMap, map<string comma double>);
AddConverter(MapToString, StringToMap, map<string comma string>);
AddConverter(MapToString, StringToMap, map<string comma TString>);

AddConverter(MapToString, StringToMap, map<TString comma int>);
AddConverter(MapToString, StringToMap, map<TString comma float>);
AddConverter(MapToString, StringToMap, map<TString comma double>);
AddConverter(MapToString, StringToMap, map<TString comma TString>);
AddConverter(MapToString, StringToMap, map<TString comma string>);

AddConverter(MapToString, StringToMap, map<TString comma TVector2>);