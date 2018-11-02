#include <TSystem.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TRint.h>

#include <TRestTools.h>
#include <TRestMetadata.h>

bool verbose = true;
int main(int argc, char *argv[])
{
	setenv("REST_VERSION", REST_RELEASE, 1);

	for (int i = 1; i < argc; i++) {
		if (ToUpper((string)argv[i]) == "-L")
		{
			verbose = false;
			break;
		}
	}
	TRint theApp("App",&argc,argv);

	TRestTools::LoadRESTLibrary(verbose);

	auto a = ExecuteShellCommand("find $REST_PATH/macros | grep REST_.*.C | grep -v \"swo\" | grep -v \"swp\"  | grep -v \"svn\"");
	auto b = Spilt(a, "\n");
	for (auto c : b) {
		if (verbose)
			printf("Loading macro : %s\n", c.c_str());
		gROOT->ProcessLine((".L "+c).c_str());
	}

	theApp.Run();
}
