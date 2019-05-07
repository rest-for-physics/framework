#include <TApplication.h>
#include <TROOT.h>
#include <TRint.h>
#include <TSystem.h>

#include <TRestMetadata.h>
#include <TRestTools.h>

#include "TRestVersion.h"

bool silent = false;
bool debug = false;
int main(int argc, char* argv[]) {
    setenv("REST_VERSION", REST_RELEASE, 1);

    for (int i = 1; i < argc; i++) {
        if (ToUpper((string)argv[i]) == "--SILENT") {
            silent = true;
            break;
        }

        if (ToUpper((string)argv[i]) == "--DEBUG") {
            debug = true;
            break;
        }
    }
    TRint theApp("App", &argc, argv);

    TRestTools::LoadRESTLibrary(silent);

    auto a = ExecuteShellCommand(
        "find $REST_PATH/macros | grep REST_.*.C | grep -v \"swo\" | grep -v "
        "\"CMakeLists\" | grep -v \"swp\"  | grep -v \"svn\"");
    auto b = Spilt(a, "\n");
    for (auto c : b) {
        if (debug) printf("Loading macro : %s\n", c.c_str());

        gROOT->ProcessLine((".L " + c).c_str());
    }

    theApp.Run();
}
