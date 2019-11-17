#include <TApplication.h>
#include <TSystem.h>

#include <TRestManager.h>
#include <TRestTools.h>
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

void ParseInputArgs(const char* argv) {
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
                                ParseInputArgs(argv[i + 1]);
                                break;
                            case 'i':
                                ParseInputArgs(argv[i + 1]);
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
