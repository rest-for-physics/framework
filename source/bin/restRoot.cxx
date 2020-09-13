#include <TApplication.h>
#include <TROOT.h>
#include <TRint.h>
#include <TSystem.h>

#include <TRestMetadata.h>
#include <TRestRun.h>
#include <TRestTools.h>

#include "TRestVersion.h"

// Note!
// Don't use cout in the main function!
// This will make cout un-usable in the command line!
int main(int argc, char* argv[]) {
    // set the env and debug status
    setenv("REST_VERSION", REST_RELEASE, 1);
    for (int i = 1; i < argc; i++) {
        char* c = &argv[i][0];
        if (*c == '-') {
            c++;
            if (*c == '-') c++;
            switch (*c) {
                case 'v':
                    gVerbose = StringToVerboseLevel(argv[i + 1]);
                    break;
            }
        }
    }

    bool silent = false;
    if (gVerbose == REST_Silent) silent = true;

    bool debug = false;
    if (gVerbose >= REST_Debug) debug = true;

    // load rest library and macros
    TRestTools::LoadRESTLibrary(silent);
    auto a = TRestTools::Execute(
        "find $REST_PATH/macros | grep REST_[^/]*.C | grep -v \"swo\" | grep -v "
        "\"CMakeLists\" | grep -v \"swp\"  | grep -v \"svn\"");
    auto b = Split(a, "\n");
    for (auto c : b) {
        if (debug) printf("Loading macro : %s\n", c.c_str());

        gROOT->ProcessLine((".L " + c).c_str());
    }

    // load input root file with TRestRun, initialize input event, analysis tree and metadata structures
    int Nfile = 0;
    for (int i = 1; i < argc; i++) {
        string opt = (string)argv[i];
        if (opt.find("http") != string::npos ||
            (TRestTools::fileExists(opt) && TRestTools::isRootFile(opt))) {
            printf("\nAttaching file %s as run%i...\n", opt.c_str(), Nfile);

            TRestRun* runTmp = new TRestRun(opt);
            string runcmd = Form("TRestRun* run%i = (TRestRun*)%s;", Nfile, ToString(runTmp).c_str());
            if (debug) printf("%s\n", runcmd.c_str());
            gROOT->ProcessLine(runcmd.c_str());
            if (runTmp->GetInputEvent() != NULL) {
                string eventType = runTmp->GetInputEvent()->ClassName();

                printf("Attaching event %s as ev%i...\n", eventType.c_str(), Nfile);
                string evcmd = Form("%s* ev%i = (%s*)%s;", eventType.c_str(), Nfile, eventType.c_str(),
                                    ToString(runTmp->GetInputEvent()).c_str());
                if (debug) printf("%s\n", evcmd.c_str());
                gROOT->ProcessLine(evcmd.c_str());
                runTmp->GetEntry(0);
            }

            printf("\n%s\n", "Attaching metadata structures...");
            Int_t Nmetadata = runTmp->GetNumberOfMetadataStructures();
            map<string, int> metanames;
            for (int n = 0; n < Nmetadata; n++) {
                string metaName = runTmp->GetMetadataStructureNames()[n];
                if (metaName.find("Historic") != -1) {
                    continue;
                }

                TRestMetadata* md = runTmp->GetMetadata(metaName);
                string metaType = md->ClassName();

                string metaFixed = Replace(metaName, "-", "_");
                metaFixed = Replace(metaFixed, " ", "");
                metaFixed = Replace(metaFixed, ".", "_");
                metaFixed = "md" + ToString(Nfile) + "_" + metaFixed;
                if (metanames.count(metaFixed) != 0) continue;
                metanames[metaFixed] = 0;
                printf("- %s (%s)\n", metaFixed.c_str(), metaType.c_str());

                string mdcmd = Form("%s* %s = (%s*)%s;", metaType.c_str(), metaFixed.c_str(),
                                    metaType.c_str(), ToString(md).c_str());

                if (debug) printf("%s\n", mdcmd.c_str());

                gROOT->ProcessLine(mdcmd.c_str());
            }

            argv[i] = (char*)"";
            Nfile++;
        }
    }

    // display root's command line
    TRint theApp("App", &argc, argv);
    theApp.Run();

    return 0;
}
