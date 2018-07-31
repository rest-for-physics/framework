#include <TSystem.h>
#include <TRint.h>
#include <TApplication.h>
#include <TMath.h>
#include <TRestTools.h>

#include <TRestRun.h>


char varName[256];
char iFile[256];
Double_t start = 0;
Double_t endVal = 0;

std::vector <TString> inputFiles;

void PrintHelp( )
{
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << "This program prints on screen the ROOT obejects inside INPUT_FILE" << endl;
    cout << endl;
    cout << "Usage : ./restPrintFileContents INPUT_FILE" << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << endl;
}

int main( int argc, char *argv[] )
{
	int argRint = 1;
	char *argVRint[3];

	char batch[64], quit[64], appName[64];
	sprintf ( appName, "restPrintFileContents" );
	sprintf( batch, "%s", "-b" );
	sprintf( quit, "%s", "-q" );

	argVRint[0] = appName;
	argVRint[1] = batch;
	argVRint[2] = quit;

    argRint = 3;

	TRint theApp("App", &argRint, argVRint );

    gSystem->Load("libGdml.so");
    gSystem->Load("libRestCore.so");
    gSystem->Load("libRestEvents.so");
    gSystem->Load("libRestMetadata.so");
    // Loading REST or Rest Libraries found in LD_LIBRARY_PATH
    vector <TString> list = TRestTools::GetListOfRESTLibraries( );
    for( unsigned int n = 0; n < list.size(); n++ )
    {
        cout << "Loading library : " << list[n] << endl;
        gSystem->Load( list[n] );
    }

	if( argc <= 1 || argc > 2 ) 
    {
        cout << "REST Error : Wrong number of arguments given" << endl;
        PrintHelp(); 
        exit(1); 
    }

    sprintf( iFile, "%s", argv[1] );

    TFile * fInputFile = new TFile( iFile );

    if( fInputFile == NULL ) { cout << "I could not open file : " << iFile << endl; return 0; }

    TIter nextkey(fInputFile->GetListOfKeys());
    TKey *key;
    Int_t c = 0;
    cout << "==========================================" << endl;
    while ( (key = (TKey*)nextkey() ) ) {
        if( c > 0 ) cout << " ------------------------------------- " << endl;

        cout << "ClassName : " << key->GetClassName() << endl;
        cout << "Name : " << key->GetName() << endl;
        cout << "Title : " << key->GetTitle() << endl;
    }
    cout << "==========================================" << endl;

	return 0;
}

