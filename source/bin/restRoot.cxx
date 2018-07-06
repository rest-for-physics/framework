#include <TSystem.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TRint.h>

#include <TRestTools.h>
#include <TRestMetadata.h>

bool verbose = true;
int main(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++) {
		if (ToUpper((string)argv[i]) == "-L")
		{
			verbose = false;
			break;
		}
	}
	TRint theApp("App",&argc,argv);

	TRestTools::LoadRESTLibrary(verbose);

	auto a = ExecuteShellCommand("find $REST_PATH/macros |grep .hh | grep -v \"swo\" | grep -v \"swp\"  | grep -v \"svn\"");
	auto b = Spilt(a, "\n");
	for (auto c : b) {
		if (verbose)
			printf("Loading macro : %s\n", c.c_str());
		gROOT->ProcessLine((".L "+c).c_str());
	}

	//char command[]= "find $REST_PATH/macros |grep .hh | grep -v \"swo\" | grep -v \"swp\"  | grep -v \"svn\"> /tmp/macros.list";

	//system(command);

	//FILE *f = fopen("/tmp/macros.list", "r");

	//char str[256];
	//char cmd[256];
	//while (fscanf(f, "%s\n", str) != EOF)
	//{
 //       printf("Loading macro : %s\n", str );
	//	sprintf(cmd, ".L %s", str);
	//	gROOT->ProcessLine(cmd);
	//}

	//fclose(f);

	//system("rm /tmp/macros.list");

	theApp.Run();
}
