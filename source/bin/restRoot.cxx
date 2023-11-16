#include <TApplication.h>
#include <TROOT.h>
#include <TRint.h>
#include <TSystem.h>

#include "TRestStringHelper.h"
#include "TRestStringOutput.h"
#include "TRestTools.h"
#include "TRestVersion.h"

using namespace std;

#ifdef WIN32
// in windows the pointer address from string conversion is without "0x", we must add
// the prefix so that ROOT can correctly initialize run/metadata objects
#define PTR_ADDR_PREFIX "0x"
#else
#define PTR_ADDR_PREFIX ""
#endif  // WIN32

// Note!
// Don't use cout in the main function!
// This will make cout un-usable in the command line!
int main(int argc, char* argv[]) {
    // set the env and debug status
    setenv("REST_VERSION", REST_RELEASE, 1);

    printf("Setting verbose level to info. You may change level using `restRoot -v N`.\n");
    printf("Use `restRoot --help` for additional info.\n");
    gVerbose = StringToVerboseLevel("2");

    Bool_t loadMacros = false;
    for (int i = 1; i < argc; i++) {
        char* c = &argv[i][0];
        if (*c == '-') {
            c++;
            if (*c == '-') c++;
            switch (*c) {
                case 'v':
                    gVerbose = StringToVerboseLevel(argv[i + 1]);
                    break;
                case 'm':
                    loadMacros = true;
                    break;
                case 'h':
                    // We use cout here since we will just exit afterwards
                    printf("\n");
                    printf("-----------------------------------\n");
                    printf("restRoot basic options description.\n");
                    printf("-----------------------------------\n");
                    printf("\n");
                    printf(" In order to define the verbosity of restRoot you may use:\n");
                    printf("\n");
                    printf(" restRoot --v [VERBOSE_LEVEL]\n");
                    printf("\n");
                    printf(" Where VERBOSE_LEVEL=0,1,2,3 is equivalent to silent, warning, info, debug\n");
                    printf("\n");
                    printf("-----\n");
                    printf("\n");
                    printf(" In order to decide if REST macros should be loadedd you may use:\n");
                    printf("\n");
                    printf(" restRoot --m [0,1]\n");
                    printf("\n");
                    printf(" Option 0 will disable macro loading. Option 1 is the default.\n");
                    printf("\n");
                    exit(0);
            }
        }
    }

    bool silent = false;
    if (gVerbose == TRestStringOutput::REST_Verbose_Level::REST_Silent) silent = true;

    bool debug = false;
    if (gVerbose >= TRestStringOutput::REST_Verbose_Level::REST_Debug) debug = true;

    // load rest library and macros
    TRestTools::LoadRESTLibrary(silent);

    gROOT->ProcessLine("#include <TRestStringHelper.h>");
    gROOT->ProcessLine("#include <TRestPhysics.h>");
    gROOT->ProcessLine("#include <TRestSystemOfUnits.h>");
    if (loadMacros) {
        if (!silent) printf("= Loading macros ...\n");
        vector<string> macroFiles;
        const vector<string> patterns = {
            REST_PATH + "/macros/REST_*.C",   // framework
            REST_PATH + "/macros/*/REST_*.C"  // libraries
        };
        for (const auto& pattern : patterns) {
            for (const auto& macroFile : TRestTools::GetFilesMatchingPattern(pattern)) {
                macroFiles.push_back(macroFile);
            }
        }

        for (const auto& macroFile : macroFiles) {
            if (debug) printf("Loading macro : %s\n", macroFile.c_str());
            gROOT->ProcessLine((".L " + macroFile).c_str());
        }
    }

    std::string runName = "";
    std::string dSName = "";

    for (int i = 1; i < argc; i++) {
        const string opt = (string)argv[i];
        if (opt.at(0) == ('-') && opt.length() > 1 && opt.at(1) == ('-')) {
            i++;
            continue;
        }
        if (opt.at(0) == ('-')) continue;

        if (opt.find("http") == string::npos && !TRestTools::fileExists(opt)) {
            printf("\nFile %s not compatible ... !!\n", opt.c_str());
            continue;
        }
        if (TRestTools::isRunFile(opt) && runName.empty()) {
            runName = opt;
            string cmd = ".L " + REST_PATH + "/macros/REST_OpenInputFile.C";
            if (!loadMacros && dSName.empty()) gROOT->ProcessLine(cmd.c_str());
            cmd = "REST_OpenInputFile(\"" + runName + "\")";
            gROOT->ProcessLine(cmd.c_str());
            argv[i] = (char*)"";
        } else if (TRestTools::isDataSet(opt) && dSName.empty()) {
            dSName = opt;
            string cmd = ".L " + REST_PATH + "/macros/REST_OpenInputFile.C";
            if (!loadMacros && runName.empty()) gROOT->ProcessLine(cmd.c_str());
            cmd = "REST_OpenInputFile(\"" + dSName + "\")";
            gROOT->ProcessLine(cmd.c_str());
            argv[i] = (char*)"";
        }
    }

    // display root's command line
    TRint theApp("App", &argc, argv);
    theApp.Run();

    return 0;
}
