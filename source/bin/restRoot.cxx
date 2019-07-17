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

    TRestTools::LoadRESTLibrary(silent);

    auto a = TRestTools::Execute(
        "find $REST_PATH/macros | grep REST_.*.C | grep -v \"swo\" | grep -v "
        "\"CMakeLists\" | grep -v \"swp\"  | grep -v \"svn\"");
    auto b = Split(a, "\n");
    for (auto c : b) {
        if (debug) printf("Loading macro : %s\n", c.c_str());

        gROOT->ProcessLine((".L " + c).c_str());
    }

    int Nfile = 0;
    for (int i = 1; i < argc; i++) {
        string opt = (string)argv[i];
        if (TRestTools::fileExists(opt) && TRestTools::isRootFile(opt)) {
            printf("Attaching file %s as run%i...\n", opt.c_str(), Nfile);
            gROOT->ProcessLine(Form("TRestRun* run%i =new TRestRun(\"%s\")", Nfile, opt.c_str()));
            argv[i] = "";
            Nfile++;
        }
    }

    TRint theApp("App", &argc, argv);

    theApp.Run();
}
