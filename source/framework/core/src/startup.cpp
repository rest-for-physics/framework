#include <filesystem>

#ifdef WIN32
#include <Windows.h>
#endif  // WIN32

#ifdef __APPLE__
#include <unistd.h>
#endif

#include "RVersion.h"
#include "TEnv.h"
#include "TRestDataBase.h"
#include "TRestReflector.h"
#include "TRestStringHelper.h"
#include "TRestStringOutput.h"
#include "TRestSystemOfUnits.h"
#include "TRestTools.h"
#include "TRestVersion.h"
//////////////////////////////////////////////////////////////////////////
/// This script initializes REST global variables in sequence to clearify
/// their dependency, therefore avoiding seg.fault during startup. All
/// global variables in libRestTools, if depend on other global variable,
/// should be placed here for initialization.

using namespace std;

EXTERN_IMP string REST_COMMIT;
EXTERN_IMP string REST_PATH;
EXTERN_IMP string REST_USER;
EXTERN_IMP string REST_USER_PATH;
EXTERN_IMP map<string, string> REST_ARGS = {};

#ifdef WIN32
EXTERN_IMP string REST_TMP_PATH = std::filesystem::temp_directory_path().string();
EXTERN_IMP int COLOR_RESET = 7;
#else
string REST_TMP_PATH = "/tmp/";
#endif  // WIN32

EXTERN_IMP bool REST_Display_CompatibilityMode = false;

namespace REST_Reflection {
EXTERN_IMP map<void*, TClass*> RESTListOfClasses_typeid = {};
EXTERN_IMP map<string, TClass*> RESTListOfClasses_typename = {};
}  // namespace REST_Reflection
EXTERN_IMP map<size_t, RESTVirtualConverter*> RESTConverterMethodBase = {};

// initialize REST constants
struct __REST_CONST_INIT {
   public:
    __REST_CONST_INIT() {
#ifdef WIN32
        REST_COMMIT = REST_GIT_COMMIT;
#else
        REST_COMMIT = TRestTools::Execute("rest-config --commit");
#endif

#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 26, 0)
        // we are not ready to use the new web-browser
        gEnv->SetValue("Browser.Name", "TRootBrowser");
#endif

#ifdef WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CHAR_INFO info[1];
        SMALL_RECT rect;
        rect.Bottom = 1;
        rect.Top = 0;
        rect.Left = 0;
        rect.Right = 1;

        ReadConsoleOutput(hConsole, info, {1, 1}, {0, 0}, &rect);

        COLOR_RESET = info[0].Attributes;
#endif  // WIN32

         char* _REST_PATH = getenv("REST_PATH");
         char* _REST_USER = getenv("USER");
         char* _REST_USERHOME = getenv("HOME");

        //char* _REST_PATH = 0;
        //char* _REST_USER = 0;
        //char* _REST_USERHOME = 0;

#ifdef WIN32
        if (_REST_PATH == nullptr) {
            TCHAR ProgramDir[MAX_PATH + 1];
            GetModuleFileName(0, ProgramDir, MAX_PATH);
            std::filesystem::path path(ProgramDir);
            if (exists(path)) {
                REST_PATH = path.parent_path().parent_path().string();
                RESTEssential << "Setting REST path to: " << REST_PATH << RESTendl;
            } else {
                RESTError << "Lacking system env \"REST_PATH\"! Cannot start!" << RESTendl;
                abort();
            }
        } else {
            REST_PATH = _REST_PATH;
        }

        if (_REST_USERHOME == nullptr || _REST_USERHOME == nullptr) {
            char* systemdir = getenv("SystemDrive");
            char* homepath = getenv("HOMEPATH");
            if (systemdir != nullptr && homepath != nullptr) {
                string userhome = string(systemdir) + string(homepath);

                std::filesystem::path path(userhome);
                if (exists(path)) {
                    REST_USER_PATH = userhome + "\\.rest";
                    REST_USER = path.stem().string();
                    RESTEssential << "Setting user name to : \"" << REST_USER << "\"" << RESTendl;
                    RESTEssential << "Setting REST temp path to: " << REST_USER_PATH << RESTendl;
                }
            } else {
                RESTWarning << "Lacking system env \"SystemDrive\" and \"HOMEPATH\"!" << RESTendl;
            }
        }
#else
        if (_REST_PATH == nullptr) {
            // use some other sources of information that indicates REST_PATH
            // /proc/3102456/exe -> /home/nkx/REST_v2/bin/restRoot
            int pid = getpid();
            string lsresult = TRestTools::Execute("ls /proc/" + ToString(pid) + "/exe -l");
            auto lsresolve = Split(lsresult, "->", true, true);
            if (lsresolve.size() == 2) {
                if (lsresolve[1].find("bin")) {
                    std::filesystem::path path(lsresolve[1]);
                    REST_PATH = path.parent_path().parent_path().string();
                    RESTWarning << "Lacking system env \"REST_PATH\"!" << RESTendl;
                    RESTWarning << "You need to source \"thisREST.sh\" first!" << RESTendl;
                    RESTWarning << "Setting REST path to the executable path: " << REST_PATH << RESTendl;
                } else {
                    std::filesystem::path path(lsresolve[1]);
                    REST_PATH = path.parent_path().string();
                    RESTWarning << "Lacking system env \"REST_PATH\"!" << RESTendl;
                    RESTWarning << "REST not installed? Setting to path: " << REST_PATH << RESTendl;
                }

                // set also REST_USER and REST_USER_PATH in this case
                REST_USER_PATH = REST_TMP_PATH + "/rest_PID" + getpid() + "/";
                RESTWarning << "Setting REST temp path to : " << REST_USER_PATH << RESTendl;

                const string systemUsername = TRestTools::Execute("whoami");
                if (!systemUsername.empty()) {
                    REST_USER = systemUsername;
                } else {
                    RESTWarning << R"(Cannot find username with "whoami" utility)" << RESTendl;
                    REST_USER = "defaultUser";
                }
                RESTWarning << "Setting user name to : \"" << REST_USER << "\"" << RESTendl;
            } else {
                RESTError << "Lacking system env \"REST_PATH\"! Cannot start!" << RESTendl;
                abort();
            }
        } else {
            REST_PATH = _REST_PATH;

            if (_REST_USER == nullptr) {
                RESTWarning << "Lacking system env \"USER\"!" << RESTendl;
                const string systemUsername = TRestTools::Execute("whoami");
                if (!systemUsername.empty()) {
                    REST_USER = systemUsername;
                } else {
                    RESTWarning << R"(Cannot find username with "whoami" utility)" << RESTendl;
                    REST_USER = "defaultUser";
                }
                RESTWarning << "Setting user name to : \"" << REST_USER << "\"" << RESTendl;
                setenv("USER", REST_USER.c_str(), true);

            } else {
                REST_USER = _REST_USER;
            }

            if (_REST_USERHOME == nullptr) {
                RESTWarning << "Lacking system env \"HOME\"!" << RESTendl;
                REST_USER_PATH = REST_TMP_PATH + "/rest_PID" + getpid() + "/";
                RESTWarning << "Setting REST temp path to : " << REST_USER_PATH << RESTendl;
            } else {
                string restUserPath = (string)_REST_USERHOME + "/.rest";
                REST_USER_PATH = restUserPath;
            }
        }
#endif
        if (REST_USER_PATH != "") {
            // check the data directories
            if (!TRestTools::fileExists(REST_USER_PATH)) {
                std::filesystem::create_directory(REST_USER_PATH);
            }
            // check the runNumber file
            if (!TRestTools::fileExists(REST_USER_PATH + "/runNumber")) {
                TRestTools::Execute("echo 1 > " + REST_USER_PATH + "/runNumber");
            }
            // check the dataURL file
            // if (!TRestTools::fileExists(restUserPath + "/dataURL")) {
            //    TRestTools::Execute("cp " + REST_PATH + "/data/dataURL " + restUserPath + "/");
            //}
            // check the download directory
            if (!TRestTools::fileExists(REST_USER_PATH + "/download")) {
                std::filesystem::create_directory(REST_USER_PATH + "/download");
            }
            // check the gdml directory
            if (!TRestTools::fileExists(REST_USER_PATH + "/gdml")) {
                std::filesystem::create_directory(REST_USER_PATH + "/gdml");
            }
        }
    }
};
const __REST_CONST_INIT REST_CONST_INIT;

// initialize gDataBase
EXTERN_IMP TRestDataBase* gDataBase = nullptr;
MakeGlobal(TRestDataBase, gDataBase, 1);

TRestStringOutput::REST_Verbose_Level gVerbose = TRestStringOutput::REST_Verbose_Level::REST_Warning;

// initialize converter methods
template <class T>
string ToStringSimple(T source) {
    ostringstream ss1;
    ss1.precision(cout.precision());
    ss1 << source;
    return ss1.str();
}
AddConverter(ToStringSimple, StringToInteger, int);
AddConverter(ToStringSimple, StringToDouble, double);
AddConverter(ToStringSimple, StringToBool, bool);
AddConverter(ToStringSimple, StringToFloat, float);
AddConverter(ToStringSimple, StringToLong, long long);

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
