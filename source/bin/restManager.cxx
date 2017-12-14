#include <TSystem.h>
#include <TApplication.h>

#include <TRestTools.h>
#include "TRestStringOutput.h"
#include <TRestManager.h>
//#include <REST_General_CreateHisto.hh>

char cfgFileName[256];
char iFile[256];
TRestStringOutput fout;

void PrintHelp()
{
	fout.resetcolor();
	fout << " " << endl;
	fout << "Usage1 : ./restManager --c CONFIG_FILE --i INPUT_FILE --o OUTPUT_FILE --j THREADS    " << endl;
	fout << "Usage2 : ./restManager TASK_NAME ARG1 ARG2 ARG3                                      " << endl;
	fout << " " << endl;
	fout << "-" << endl;
	fout << "CONFIG_FILE: The rml configuration file. It should contain a TRestManager section.   " << endl;
	fout << "This argument MUST be provided. The others can be also specified in the rml file.    " << endl;
	fout << "-" << endl;
	fout << "INPUT_FILE : Input file name. If not given it will be acquired from the rml file.    " << endl;
	fout << "If you want to use multiple input file, you can either specify the string of matching" << endl;
	fout << "pattern with quotation marks surrounding it, or put the file names in a .list file   " << endl;
	fout << "-" << endl;
	fout << "OUTPUT_FILE: Output file name. It can be given as a name string (abc.root), or as an " << endl;
	fout << "expression for the program to replace parameters in it.                              " << endl;
	fout << "-" << endl;
	fout << "THREADS    : Request specific number of threads to run the jobs. In most time 3~6    " << endl;
	fout << "threads can squeeze out the full potential of the computer. More may be negative     " << endl;
	fout << " " << endl;
	fout << "=" << endl;
}



int main( int argc, char *argv[] )
{

    vector <TString> list = TRestTools::GetListOfRESTLibraries( );
    for( unsigned int n = 0; n < list.size(); n++ )
    {
        cout << "Loading library : " << list[n] << endl;
        gSystem->Load( list[n] );
    }

	//char command[] = "find $REST_PATH/macros |grep .hh | grep -v \"swo\" | grep -v \"swp\"  | grep -v \"svn\"> /tmp/macros.list";

	//system(command);

	//FILE *f = fopen("/tmp/macros.list", "r");

	//char str[256];
	//char cmd[256];
	//while (fscanf(f, "%s\n", str) != EOF)
	//{
	//	//printf("Loading macro : %s\n", str);
	//	sprintf(cmd, ".L %s", str);
	//	gROOT->ProcessLine(cmd);
	//}

	//fclose(f);

	//system("rm /tmp/macros.list");


    if( argc <= 1 ) { PrintHelp(); exit(1); }

	if (argc >= 2)
	{
		if (*argv[1] == '-') {//usage1
			for (int i = 1; i < argc; i++)
			{
				if (*argv[i] == '-')
				{
					argv[i]++;
					if (*argv[i] == '-') argv[i]++;
					{
						switch (*argv[i])
						{
						case 'c': sprintf(cfgFileName, "%s", argv[i + 1]); break;
						case 'i': setenv("inputFile", argv[i + 1], 1); break;
						case 'o': setenv("outputFile", argv[i + 1], 1); break;
						case 'j': setenv("threadNumber", argv[i + 1], 1); break;
						case 'h': PrintHelp(); exit(1);
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
			fout << "Launching TRestManager..." << endl;
			fout << endl;
			TRestManager* a = new TRestManager();

			a->LoadConfigFromFile(cfgFileName);

			fout << "Done!" << endl;
			a->GetChar();

			delete a;

			return 0;


		}
		else//usage2
		{


			string type = (argv[1]);
			fout <<"Initializing "<< type << endl;
			TClass* c= TClass::GetClass(type.c_str());
			if (c == NULL) {
				c= TClass::GetClass(("REST_"+type).c_str());
				if (c == NULL) {
					fout.setcolor(COLOR_BOLDRED);
					fout << "ERROR: Task \"" << type << "\" is not defined !" << endl;
					fout << endl;
					PrintHelp();
					exit(0);
				}
			}
			if (!c->InheritsFrom("TRestTask")) {
				fout.setcolor(COLOR_BOLDRED);
				fout << "This class is not inherted from TRestTask!" << endl;
				fout << endl;
				PrintHelp();
				exit(0);
			}
			TRestTask* tsk = (TRestTask*)c->New();
			vector<string> argumentlist;
			for (int i = 2; i < argc; i++)
			{
				string a = argv[i];
				argumentlist.push_back(a);
			}
			tsk->InitTask(argumentlist);
			tsk->RunTask(NULL);

		}
	}


}

