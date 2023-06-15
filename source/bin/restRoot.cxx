#include <TApplication.h>
#include <TROOT.h>
#include <TRint.h>
#include <TSystem.h>

#include "TRestMetadata.h"
#include "TRestRun.h"
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

    Int_t loadMacros = 0;
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
                    loadMacros = StringToInteger(argv[i + 1]);
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

    // load input root file with TRestRun, initialize input event, analysis tree and metadata structures
    int nFile = 0;
    int nDataSet = 0;
    for (int i = 1; i < argc; i++) {
        const string opt = (string)argv[i];
        if (opt.at(0) == ('-')) continue;
        printf("\nAttaching file %s\n", opt.c_str());

        if (opt.find("http") == string::npos && !TRestTools::fileExists(opt)) {
            printf("\nFile %s not compatible ... !!\n", opt.c_str());
            continue;
        }

        if (TRestTools::isRunFile(opt)) {
            printf("\n%s\n", "REST processed file identified. It contains a valid TRestRun.");
            printf("\nAttaching TRestRun %s as run%i...\n", opt.c_str(), nFile);
            string runcmd = Form("TRestRun* run%i = new TRestRun (\"%s\");", nFile, opt.c_str());
            if (debug) printf("%s\n", runcmd.c_str());
            gROOT->ProcessLine(runcmd.c_str());
            argv[i] = (char*)"";
            nFile++;
        } else if (TRestTools::isDataSet(opt)) {
            printf("\n%s\n", "REST dataset file identified. It contains a valid TRestDataSet.");
            printf("\nImporting dataset as `dSet%i`\n", nDataSet);
            printf("\n%s\n", "The dataset is ready. You may now access the dataset using:");
            printf("\n%s\n", " - dSet0->PrintMetadata()");
            printf("%s\n", " - dSet0->GetDataFrame().GetColumnNames()");
            printf("%s\n\n", " - dSet0->GetTree()->GetEntries()");
            string runcmd = "TRestDataSet *dSet" + to_string(nDataSet) + " = new TRestDataSet();";
            gROOT->ProcessLine(runcmd.c_str());
            runcmd = Form("dSet%i->Import(\"%s\");", nDataSet, opt.c_str());
            gROOT->ProcessLine(runcmd.c_str());
            argv[i] = (char*)"";
            nDataSet++;
        }
    }

    // display root's command line
    TRint theApp("App", &argc, argv);
    theApp.Run();

    return 0;
}
