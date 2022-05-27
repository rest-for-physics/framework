#include <TApplication.h>
#include <TRestManager.h>
#include <TRestTools.h>
#include <TSystem.h>

#include "TRestStringOutput.h"

using namespace std;

char configFilename[256];
char iFile[256];

const int maxForksAllowed = 32;


int fork_n_execute(string command) {
#ifdef WIN32
    return -1;
#else
    int status;
    pid_t pid;

    pid = fork();

    if (pid == 0) {
        /* This is the child process */
        system(command.c_str());  // execute the command
        // we call exit() when system() returns to complete child process
        exit(EXIT_SUCCESS);
    } else if (pid < 0) {
        /* The fork failed */
        printf("Failed to fork(): %s ", command.c_str());
        status = -1;
    }

    /* This is the parent process
     * incase you want to do something
     * like wait for the child process to finish
     */
    /*
       else
       if(waitpid(pid, &status, 0) != pid)
       status = -1;
       */
    return status;
#endif  // !WIN32
}

void PrintHelp() {
    TRestStringOutput fout(COLOR_BOLDYELLOW, "", TRestStringOutput::REST_Display_Orientation::kLeft);
    RESTcout << " " << RESTendl;

    RESTcout.setheader("Usage1 : ./restManager ");
    RESTcout << "--c CONFIG_FILE [--i/f INPUT] [--o OUTPUT] [--j THREADS] [--e EVENTS_TO_PROCESS] [--v "
                "VERBOSELEVEL] [--d RUNID] [--p PDF_PLOTS.pdf]"
             << RESTendl;
    RESTcout.setheader("Usage2 : ./restManager ");
    RESTcout << "TASK_NAME ARG1 ARG2 ARG3" << RESTendl;

    RESTcout.setcolor(COLOR_WHITE);
    RESTcout.setheader("");
    RESTcout << " " << RESTendl;
    RESTcout.setheader("CONFIG_FILE: ");
    RESTcout << "-" << RESTendl;
    RESTcout << "The rml configuration file. It should contain a TRestManager section. This "
                "argument MUST be provided. The others can be also specified in the rml file."
             << RESTendl;
    RESTcout.setheader("INPUT      : ");
    RESTcout << "-" << RESTendl;
    RESTcout << "Input file name. If not given it will be acquired from the rml file. If you want "
                "to use multiple input file, you can either specify the string of matching pattern with "
                "quotation marks surrounding it, or put the file names in a .list file."
             << RESTendl;
    RESTcout.setheader("OUTPUT     : ");
    RESTcout << "-" << RESTendl;
    RESTcout << "Output file name. It can be given as a name string (abc.root), or as an expression "
                "with naming fields to be replaced (Run[RunNumber]_[Tag].root)."
             << RESTendl;
    RESTcout.setheader("THREADS    : ");
    RESTcout << "-" << RESTendl;
    RESTcout << "Enable specific number of threads to run the jobs. In most time 3~6 threads are "
                "enough to make full use of computer power. Maximum is 15."
             << RESTendl;
    RESTcout.setheader("");
    RESTcout << "=" << RESTendl;
}

Bool_t doFork = false;
std::vector<std::string> input_files;

void ParseInputFileArgs(const char* argv) {
    if (argv == nullptr) return;

    if (REST_ARGS.count("inputFileName") > 0) {
        string input_old = REST_ARGS["inputFileName"];
        input_old += "\n" + string(argv);

        REST_ARGS["inputFileName"] = input_old;
        setenv("REST_INPUTFILE", input_old.c_str(), 1);
    } else {
        REST_ARGS["inputFileName"] = argv;
        setenv("REST_INPUTFILE", argv, 1);
    }
}

// Note!
// Don't use cout in the main function!
// This will make cout un-usable in the Macros!
int main(int argc, char* argv[]) {
    // global envs
    printf("Starting pid: %d\n", getpid());
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
                RESTcout << "you are in batch mode, all graphical displays off" << RESTendl;
                argCApp = 2;
                args.erase(args.begin() + i);
            }
            if (args[i] == "--fork") {
                RESTcout << "Fork is enabled!" << RESTendl;
                argCApp = 2;
                args.erase(args.begin() + i);
                doFork = true;
            }
        }
        if (REST_Display_CompatibilityMode) {
            RESTcout << "you are in compatibility mode, all graphical displays off" << RESTendl;
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
                            REST_ARGS["configFile"] = args[i + 1];
                            sprintf(configFilename, "%s", args[i + 1].c_str());
                            break;
                        case 'd':
                            REST_ARGS["runNumber"] = args[i + 1];
                            break;
                        case 'f':
                            if (doFork)
                                input_files = TRestTools::GetFilesMatchingPattern(args[i + 1].c_str());
                            else
                                ParseInputFileArgs(args[i + 1].c_str());
                            break;
                        case 'i':
                            if (doFork)
                                input_files = TRestTools::GetFilesMatchingPattern(args[i + 1].c_str());
                            else
                                ParseInputFileArgs(args[i + 1].c_str());
                            break;
                        case 'o':
                            REST_ARGS["outputFileName"] = args[i + 1];
                            break;
                        case 'j':
                            REST_ARGS["threadNumber"] = args[i + 1];
                            break;
                        case 'e':
                            REST_ARGS["eventsToProcess"] = args[i + 1];
                            break;
                        case 'v':
                            REST_ARGS["verboseLevel"] = args[i + 1];
                            gVerbose = StringToVerboseLevel(args[i + 1]);
                            break;
                        case 'p':
                            REST_ARGS["pdfFilename"] = args[i + 1];
                            break;
                        default:
                            RESTcout << RESTendl;
                            PrintHelp();
                            return 0;
                    }
                }
            }

            RESTcout << RESTendl;
            RESTcout.setcolor(COLOR_BOLDBLUE);
            RESTcout.setorientation(TRestStringOutput::REST_Display_Orientation::kMiddle);
            RESTcout << "Launching TRestManager..." << RESTendl;
            RESTcout << RESTendl;

            int pid = 0;
            if (doFork && input_files.size() > maxForksAllowed) {
                RESTError << "Fork list is larger than " << maxForksAllowed
                          << " files. Please, use a glob pattern producing a shorter list" << RESTendl;
            } else if (doFork) {
                for (unsigned int n = 0; n < input_files.size(); n++) {
                    string command = "restManager";
                    for (unsigned int x = 1; x < args.size(); x++) {
                        if (args[x] != "--f" && args[x - 1] != "--f" && args[x] != "--fork")
                            command += " " + args[x];
                    }
                    command +=
                        " --f " + input_files[n] + " >> /tmp/" + getenv("USER") + "_out." + ToString(n);
                    RESTcout << "Executing : " << command << RESTendl;
                    fork_n_execute(command);
                }
                exit(0);
            } else {
                RESTcout << "Creating TRestManager" << RESTendl;
                TRestManager* mgr = new TRestManager();

                auto path = TRestTools::SeparatePathAndName(configFilename).first;
                RESTcout << "path:" << path << RESTendl;

                setenv("configPath", path.c_str(), 1);

                mgr->LoadConfigFromFile(configFilename);

                RESTcout << "Done!" << RESTendl;
                // a->GetChar();

                delete mgr;
                gSystem->Exit(0);
            }
        } else  // usage2
        {
            vector<string> argumentlist;
            for (int i = 2; i < args.size(); i++) {
                argumentlist.push_back(args[i]);
            }
            string type = (args[1]);
            RESTcout << "Initializing " << type << RESTendl;
            TRestManager* a = new TRestManager();
            a->InitFromTask(type, argumentlist);
        }
    }

    return 0;
}
