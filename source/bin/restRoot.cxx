#include <TSystem.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TRint.h>

#include <TRestTools.h>
#include <TRestMetadata.h>

int main(int argc, char *argv[])
{
	TRint theApp("App",&argc,argv);
	vector <TString> list = TRestTools::GetListOfRESTLibraries();
	for (unsigned int n = 0; n < list.size(); n++)
	{
		cout << "Loading library : " << list[n] << endl;
		gSystem->Load(list[n]);
	}



	char command[]= "find $REST_PATH/macros |grep .hh | grep -v \"swo\" | grep -v \"swp\"  | grep -v \"svn\"> /tmp/macros.list";

	system(command);

	FILE *f = fopen("/tmp/macros.list", "r");

	char str[256];
	char cmd[256];
	while (fscanf(f, "%s\n", str) != EOF)
	{
        printf("Loading macro : %s\n", str );
		sprintf(cmd, ".L %s", str);
		gROOT->ProcessLine(cmd);
	}

	fclose(f);

	system("rm /tmp/macros.list");

	theApp.Run();
}
