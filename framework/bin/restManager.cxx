#include <TApplication.h>
#include <TSystem.h>

#include <TRestManager.h>
#include <TRestTools.h>
#include "TRestStringOutput.h"
//#include <REST_General_CreateHisto.hh>

char cfgFileName[256];
char iFile[256];
TRestStringOutput fout;

#ifdef WIN32
void setenv(const char* __name, const char* __value, int __replace) {
    _putenv(((string)__name + "=" + (string)__value).c_str());
}
#endif

void PrintHelp() {
    fout.resetcolor();
    fout << " " << endl;
    fout << "Usage1 : ./restManager --c CONFIG_FILE [--d RUNID] [--i/f INPUT] "
            "[--o OUTPUT]        "
         << endl;
    fout << "                       [--j THREADS] [--e EVENTS_TO_PROCESS] [--v "
            "VERBOSELEVEL]      "
         << endl;
    fout << "                       [--r HISTOS_FILE.root] [--p PDF_PLOTS.pdf] "
            "[--n PLOT_NAME]    "
         << endl;
    fout << "Usage2 : ./restManager TASK_NAME ARG1 ARG2 ARG3                     "
            "                 "
         << endl;
    fout << " " << endl;
    fout << "-" << endl;
    fout << "CONFIG_FILE: The rml configuration file. It should contain a "
            "TRestManager section.   "
         << endl;
    fout << "This argument MUST be provided. The others can be also specified in "
            "the rml file.    "
         << endl;
    fout << "-" << endl;
    fout << "RUNID      : Input run number. REST will automatically find the "
            "input file with run  "
         << endl;
    fout << "number given and INPUT_FILE not specified. Subrun number can also "
            "be specified with  "
         << endl;
    fout << "a dot after runnumber ,e.g. \"--r 910.3\". It is by default 0 "
            "meaning data takaing run "
         << endl;
    fout << "-" << endl;
    fout << "INPUT      : Input file name. If not given it will be acquired from "
            "the rml file.    "
         << endl;
    fout << "If you want to use multiple input file, you can either specify the "
            "string of matching"
         << endl;
    fout << "pattern with quotation marks surrounding it, or put the file names "
            "in a .list file   "
         << endl;
    fout << "-" << endl;
    fout << "OUTPUT     : Output file name. It can be given as a name string "
            "(abc.root), or as an "
         << endl;
    fout << "expression for the program to replace parameters in it.             "
            "                 "
         << endl;
    fout << "-" << endl;
    fout << "THREADS    : Request specific number of threads to run the jobs. In "
            "most time 3~6    "
         << endl;
    fout << "threads can squeeze out the full potential of the computer. More "
            "may be negative     "
         << endl;
    fout << " " << endl;
    fout << "=" << endl;
}

int main(int argc, char* argv[]) {
    setenv("REST_VERSION", REST_RELEASE, 1);
    TApplication app("app", NULL, NULL);
    TRestTools::LoadRESTLibrary(true);
    gInterpreter->ProcessLine("#define REST_MANAGER");

    if (argc <= 1) {
        PrintHelp();
        exit(1);
    }

    if (argc >= 2) {
        if (*argv[1] == '-') {  // usage1
            for (int i = 1; i < argc; i++) {
                if (*argv[i] == '-') {
                    argv[i]++;
                    if (*argv[i] == '-') argv[i]++;
                    {
                        switch (*argv[i]) {
                            case 'c':
                                sprintf(cfgFileName, "%s", argv[i + 1]);
                                break;
                            case 'd':
                                setenv("runNumber", argv[i + 1], 1);
                                break;
                            case 'f':
                                setenv("inputFile", argv[i + 1], 1);
                                setenv("REST_INPUTFILE", argv[i + 1], 1);
                                break;
                            case 'i':
                                setenv("inputFile", argv[i + 1], 1);
                                setenv("REST_INPUTFILE", argv[i + 1], 1);
                                break;
                            case 'o':
                                setenv("outputFile", argv[i + 1], 1);
                                break;
                            case 'j':
                                setenv("threadNumber", argv[i + 1], 1);
                                break;
                            case 'e':
                                setenv("eventsToProcess", argv[i + 1], 1);
                                break;
                            case 'v':
                                setenv("verboseLevel", argv[i + 1], 1);
                                break;
                            case 'p':
                                setenv("pdfFilename", argv[i + 1], 1);
                                break;
                            case 'r':
                                setenv("histoFilename", argv[i + 1], 1);
                                break;
                            case 'n':
                                setenv("plotsectionname", argv[i + 1], 1);
                                break;
                            // case 'help': PrintHelp(); exit(0);
                            default:
                                fout << endl;
                                PrintHelp();
                                return 0;
                        }
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
            for (int i = 2; i < argc; i++) {
                string a = argv[i];
                argumentlist.push_back(a);
            }
            string type = (argv[1]);
            fout << "Initializing " << type << endl;
            TRestManager* a = new TRestManager();
            a->InitFromTask(type, argumentlist);
        }
    }

    return 0;
}
