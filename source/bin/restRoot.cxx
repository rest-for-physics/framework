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
            gROOT->ProcessLine(Form("TRestRun* run%i = (TRestRun*)0x%x;", Nfile, runTmp));

			if (runTmp->GetEventTree() != NULL) {
                TString eventType = runTmp->GetEventTree()->GetTitle();
                // Removing the Tree ending
                eventType = eventType(0, eventType.Length() - 4);

                printf("Attaching eventTree %s as ev%i...\n", eventType.Data(), Nfile);
                gROOT->ProcessLine(Form("%s* ev%i =new %s();", eventType.Data(), Nfile, eventType.Data()));
                gROOT->ProcessLine(Form("run%i->SetInputEvent( ev%i );", Nfile, Nfile));
            }

            cout << endl;
            cout << "Attaching metadata structures..." << endl;
            Int_t Nmetadata = runTmp->GetNumberOfMetadataStructures();
            for (int n = 0; n < Nmetadata; n++) {
                TString* myOutput = (TString*)gROOT->ProcessLine(
                    Form("new TString(run%d->GetMetadataStructureNames()[%i]);", Nfile, n));
                TString metaName(myOutput->Data());

                myOutput = (TString*)gROOT->ProcessLine(
                    Form("new TString(run%d->GetMetadata(\"%s\")->ClassName());", Nfile, metaName.Data()));
                TString metaType(myOutput->Data());

				if (metaName.Contains("Historic")) {
                    continue;
				}

				TString metaFixed = Replace( (string) metaName, "-", "_" );
				metaFixed = Replace( (string) metaFixed, " ", "" );
				metaFixed = Replace( (string) metaFixed, ".", "_" );

                cout << "- md" << Nfile << "_" << metaFixed << " (" << metaType << ")" << endl;

				if( debug )
					cout << Form("%s *md%i_%s = (%s *) run%d->GetMetadata(\"%s\");", metaType.Data(),
                                        Nfile, metaFixed.Data(), metaType.Data(), Nfile, metaName.Data()) << endl;

                gROOT->ProcessLine(Form("%s *md%i_%s = (%s *) run%d->GetMetadata(\"%s\");", metaType.Data(),
                                        Nfile, metaFixed.Data(), metaType.Data(), Nfile, metaName.Data()));
            }

            argv[i] = (char*)"";
            Nfile++;
        }
    }

    TRint theApp("App", &argc, argv);

    theApp.Run();
}
