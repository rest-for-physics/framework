#include "TRestDataBase.h"
#include "TRestDetector.h"
#include "TRestReflector.h"
#include "TRestStringHelper.h"
#include "TRestStringOutput.h"
#include "TRestSystemOfUnits.h"
#include "TRestTools.h"

// initialize REST constants
string REST_COMMIT;
string REST_PATH;
string REST_USER;
string REST_USER_PATH;
map<string, string> REST_ARGS = {};
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

/// formatted message output, used for print metadata
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