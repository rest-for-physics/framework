#include <TApplication.h>
#include <TROOT.h>
#include <TRint.h>
#include <TSystem.h>

#include <TRestMetadata.h>
#include <TRestRun.h>
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
            printf("\nAttaching file %s as run%i...\n", opt.c_str(), Nfile);

            TRestRun* runTmp = new TRestRun(opt);
            string runcmd = Form("TRestRun* run%i = (TRestRun*)%s;", Nfile, ToString(runTmp).c_str());
            if (debug) cout << runcmd << endl;
            gROOT->ProcessLine(runcmd.c_str());
            if (runTmp->GetInputEvent() != NULL) {
                string eventType = runTmp->GetInputEvent()->ClassName();

                printf("Attaching event %s as ev%i...\n", eventType.c_str(), Nfile);
                string evcmd = Form("%s* ev%i = (%s*)%s;", eventType.c_str(), Nfile, eventType.c_str(),
                                    ToString(runTmp->GetInputEvent()).c_str());
                if (debug) cout << evcmd << endl;
                gROOT->ProcessLine(evcmd.c_str());
                runTmp->GetEntry(0);
            }

            cout << endl;
            cout << "Attaching metadata structures..." << endl;
            Int_t Nmetadata = runTmp->GetNumberOfMetadataStructures();
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
                cout << "- " << metaFixed << " (" << metaType << ")" << endl;

                string mdcmd = Form("%s* %s = (%s*)%s;", metaType.c_str(), metaFixed.c_str(),
                                    metaType.c_str(), ToString(md).c_str());

                if (debug) cout << mdcmd << endl;

                gROOT->ProcessLine(mdcmd.c_str());
            }

            argv[i] = (char*)"";
            Nfile++;
        }
    }

    TRint theApp("App", &argc, argv);

    theApp.Run();
}
