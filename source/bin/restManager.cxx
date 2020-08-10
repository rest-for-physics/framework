#include <TApplication.h>
#include <TRestManager.h>
#include <TRestTools.h>
#include <TSystem.h>

#include "TRestStringOutput.h"
//#include <REST_General_CreateHisto.hh>

char cfgFileName[256];
char iFile[256];

#ifdef WIN32
void setenv(const char* __name, const char* __value, int __replace) {
    _putenv(((string)__name + "=" + (string)__value).c_str());
}
#endif

void PrintHelp() {
    TRestStringOutput fout(COLOR_BOLDYELLOW, "", kHeaderedLeft);
    fout << " " << endl;

    fout.setheader("Usage1 : ./restManager ");
    fout << "--c CONFIG_FILE [--i/f INPUT] [--o OUTPUT] [--j THREADS] [--e EVENTS_TO_PROCESS] [--v "
            "VERBOSELEVEL] [--d RUNID] [--p PDF_PLOTS.pdf]"
         << endl;
    fout.setheader("Usage2 : ./restManager ");
    fout << "TASK_NAME ARG1 ARG2 ARG3" << endl;

    fout.setcolor(COLOR_WHITE);
    fout.setheader("");
    fout << " " << endl;
    fout.setheader("CONFIG_FILE: ");
    fout << "-" << endl;
    fout << "The rml configuration file. It should contain a TRestManager section. This "
            "argument MUST be provided. The others can be also specified in the rml file."
         << endl;
    fout.setheader("INPUT      : ");
    fout << "-" << endl;
    fout << "Input file name. If not given it will be acquired from the rml file. If you want "
            "to use multiple input file, you can either specify the string of matching pattern with "
            "quotation marks surrounding it, or put the file names in a .list file."
         << endl;
    fout.setheader("OUTPUT     : ");
    fout << "-" << endl;
    fout << "Output file name. It can be given as a name string (abc.root), or as an expression "
            "with naming fields to be replaced (Run[RunNumber]_[Tag].root)."
         << endl;
    fout.setheader("THREADS    : ");
    fout << "-" << endl;
    fout << "Enable specific number of threads to run the jobs. In most time 3~6 threads are "
            "enough to make full use of computer power. Maximum is 15."
         << endl;
    fout.setheader("");
    fout << "=" << endl;
}

void ParseInputFileArgs(const char* argv) {
    if (argv == NULL) return;

    if (getenv("inputFile") != NULL) {
        string input_old = getenv("inputFile");
        input_old += "\n" + string(argv);

        setenv("inputFile", input_old.c_str(), 1);
        setenv("REST_INPUTFILE", input_old.c_str(), 1);
    } else {
        setenv("inputFile", argv, 1);
        setenv("REST_INPUTFILE", argv, 1);
    }
}

// Note!
// Don't use cout in the main function!
// This will make cout un-usable in the Macros!
int main(int argc, char* argv[]) {
    // global envs
    setenv("REST_VERSION", REST_RELEASE, 1);

    // preprocess arguments
    vector<string> args;
    for (int i = 0; i < argc; i++) args.push_back(argv[i]);

    // TApplication arguments
    int argCApp = 1;
    char* argVApp[3];
    char batch[64], appName[64];
    sprintf(appName, "restManager");
    sprintf(batch, "%s", "-b");
    argVApp[0] = appName;
    argVApp[1] = batch;
    {
        // handle special arguments like "--batch"
        for (int i = 1; i < args.size(); i++) {
            if (args[i] == "--batch") {
                fout << "you are in batch mode, all displays off" << endl;
                argCApp = 2;
                args.erase(args.begin() + i);
            }
        }
        if (fout.CompatibilityMode()) {
            fout << "you are in batch mode, all displays off" << endl;
            argCApp = 2;
        }
    }
    TApplication app("app", &argCApp, argVApp);

    // print help
    if (args.size() <= 1) {
        PrintHelp();
        exit(1);
    }

    // Load libraries
    TRestTools::LoadRESTLibrary(true);
    gInterpreter->ProcessLine("#define REST_MANAGER");

    // read arguments
    if (args.size() >= 2) {
        if (args[1][0] == '-') {  // usage1
            for (int i = 1; i < args.size(); i++) {
                char* c = &args[i][0];
                if (*c == '-') {
                    c++;
                    if (*c == '-') c++;
                    switch (*c) {
                        case 'c':
                            sprintf(cfgFileName, "%s", args[i + 1].c_str());
                            break;
                        case 'd':
                            setenv("runNumber", args[i + 1].c_str(), 1);
                            break;
                        case 'f':
                            ParseInputFileArgs(args[i + 1].c_str());
                            break;
                        case 'i':
                            ParseInputFileArgs(args[i + 1].c_str());
                            break;
                        case 'o':
                            setenv("outputFile", args[i + 1].c_str(), 1);
                            break;
                        case 'j':
                            setenv("threadNumber", args[i + 1].c_str(), 1);
                            break;
                        case 'e':
                            setenv("eventsToProcess", args[i + 1].c_str(), 1);
                            break;
                        case 'v':
                            // setenv("verboseLevel", args[i + 1].c_str(), 1);
                            gVerbose = StringToVerboseLevel(args[i + 1]);
                            break;
                        case 'p':
                            setenv("pdfFilename", args[i + 1].c_str(), 1);
                            break;
                        // case 'help': PrintHelp(); exit(0);
                        default:
                            fout << endl;
                            PrintHelp();
                            return 0;
                    }
                }
            }

            fout << endl;
            fout.setcolor(COLOR_BOLDBLUE);
            fout.setorientation(0);
            fout << "Launching TRestManager..." << endl;
            fout << endl;
            TRestManager* mgr = new TRestManager();

            auto path = TRestTools::SeparatePathAndName(cfgFileName).first;
            setenv("configPath", path.c_str(), 1);

            mgr->LoadConfigFromFile(cfgFileName);

            fout << "Done!" << endl;
            // a->GetChar();

            delete mgr;
            gSystem->Exit(0);
        } else  // usage2
        {
            vector<string> argumentlist;
            for (int i = 2; i < args.size(); i++) {
                argumentlist.push_back(args[i]);
            }
            string type = (args[1]);
            fout << "Initializing " << type << endl;
            TRestManager* a = new TRestManager();
            a->InitFromTask(type, argumentlist);
        }
    }

    return 0;
}
