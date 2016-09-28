#include <TSystem.h>
#include <TRint.h>
#include <TApplication.h>
#include <TMath.h>
#include <TF1.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TRestRun.h>


char varName[256];
char iFile[256];

Double_t Qbb = 2457.83;
Double_t mean = Qbb;

std::vector <TString> inputFiles;

void PrintHelp( )
{
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << " This program will integrate the variable VAR_NAME, " << endl;
    cout << " which should be defined inside TRestAnalysisTree" << endl;
    cout << endl;
    cout << " If no MEAN_VALUE is specified the Qbb = 2457.84 is used." << endl;
    cout << endl;
    cout << " It will proide the integration in 3 different ranges: 0.5\%, 1\% and 3\% FWHM." << endl;
    cout << endl;
    cout << " Usage : ./restIntegrateSmearing --v VAR_NAME --m MEAN_VALUE --f INPUT_FILE" << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << endl;
    cout << " INPUT_FILE : Input file name. " << endl;
    cout << endl;
    cout << " You can also specify a file input range using the shell *,? characters as in ls." << endl;
    cout << " For example : \"Run_simulation_*.root\". " << endl;
    cout << endl;
    cout << " IMPORTANT : You should then write the filename range between quotes!! \"\"" << endl;
    cout << endl;
    cout << " ==================================================================================" << endl;
}

int main( int argc, char *argv[] )
{
    Double_t fwhm = 0.005;
    Double_t sigma_1 = mean * fwhm /  (2*TMath::Sqrt( 2 * TMath::Log(2.) )  );

    fwhm = 0.01;
    Double_t sigma_2 = mean * fwhm /  (2*TMath::Sqrt( 2 * TMath::Log(2.) )  );
    
    fwhm = 0.03;
    Double_t sigma_3 = mean * fwhm /  (2*TMath::Sqrt( 2 * TMath::Log(2.) )  );


	int argRint = 1;
	char *argVRint[3];

	char batch[64], quit[64], appName[64];
	sprintf ( appName, "restIntegrateSmearing" );
	sprintf( batch, "%s", "-b" );
	sprintf( quit, "%s", "-q" );

	argVRint[0] = appName;
	argVRint[1] = batch;
	argVRint[2] = quit;

	for( int i = 1; i < argc; i++ )
		if ( strstr( argv[i], "--batch") != NULL  )
			argRint = 3;

	TRint theApp("App", &argRint, argVRint );

	gSystem->Load("libRestCore.so");
	gSystem->Load("libRestMetadata.so");
	gSystem->Load("libRestEvents.so");
	gSystem->Load("libRestProcesses.so");

	if( argc <= 1 ) { PrintHelp(); exit(1); }

	if( argc >= 2 )
	{
		for(int i = 1; i < argc; i++)
			if( *argv[i] == '-')
			{
				argv[i]++;
				if( *argv[i] == '-')
				{
					argv[i]++;
					switch ( *argv[i] )
					{
						case 'v' : sprintf( varName, "%s", argv[i+1] ); break;
						case 'm' : mean = atof( argv[i+1] ); break;
						case 'f' : 
							   {
								   sprintf( iFile, "%s", argv[i+1] );
								   TString iFileStr = iFile;
								   inputFiles.push_back( iFileStr );
								   break;
							   }
						case 'h' : PrintHelp(); exit(1);
						default : ;
					}
				}
			}
	}

	std::vector <TString> inputFilesNew;
	for( unsigned int n = 0; n < inputFiles.size(); n++ )
	{
		if( inputFiles[n].First( "*" ) >= 0 || inputFiles[n].First( "?" ) >= 0  )
		{
			char command[256];
			sprintf( command, "find %s > /tmp/fileList.tmp", inputFiles[n].Data() );

			system( command );

			FILE *fin = fopen( "/tmp/fileList.tmp", "r" );
			char str[256];
			while ( fscanf ( fin, "%s\n", str ) != EOF )
			{
				TString newFile = str;
				inputFilesNew.push_back( newFile );
			}
			fclose( fin );

			system ( "rm /tmp/fileList.tmp" );
		}
		else
		{
			inputFilesNew.push_back( inputFiles[n] );
		}
	}

	for( unsigned int n = 0; n < inputFilesNew.size(); n++ )
	{
        TRestRun *run = new TRestRun();

        run->OpenInputFile( inputFilesNew[n] );

        run->SkipEventTree();
        run->PrintInfo();

        Int_t obsID = run->GetAnalysisTree( )->GetObservableID( varName );
        cout << "Entries : " << run->GetEntries() << endl;
        TH1D *h = new TH1D("histo", "histo", 180, Qbb-90, Qbb+90 );
        Int_t peak = 0;
        for( int i = 0; i < run->GetEntries( ); i++ )
        {
            run->GetEntry(i);
            Double_t en = run->GetAnalysisTree()->GetObservableValue( obsID );
            if( en > Qbb - 5 && en < Qbb + 5 )
                peak++;

            h->Fill(en);
        }

        h->Draw();

        TF1 *gausFunc = new TF1( "g", "[0]*exp(-0.5*((x-[1])/[2])**2)", 0, 10000 );

        Double_t contribution_1 = 0;
        Double_t contribution_2 = 0;
        Double_t contribution_3 = 0;
        for( int i = 1; i <= h->GetNbinsX(); i++ )
        {
            Double_t en = h->GetXaxis()->GetBinCenter(i);
            Double_t counts = h->GetBinContent(i);

            gausFunc->SetParameters( counts/TMath::Sqrt(2*TMath::Pi())/sigma_1, en, sigma_1 );
            contribution_1 += gausFunc->Integral( mean-2*sigma_1, mean+2*sigma_1 );

            gausFunc->SetParameters( counts/TMath::Sqrt(2*TMath::Pi())/sigma_2, en, sigma_2 );
            contribution_2 += gausFunc->Integral( mean-2*sigma_2, mean+2*sigma_2 );

            gausFunc->SetParameters( counts/TMath::Sqrt(2*TMath::Pi())/sigma_3, en, sigma_3 );
            contribution_3 += gausFunc->Integral( mean-2*sigma_3, mean+2*sigma_3 );

        }

        cout << "FWHM = 0.5% -> " << contribution_1 << endl;
        cout << "FWHM = 1.% -> " << contribution_2 << endl;
        cout << "FWHM = 3.% -> " << contribution_3 << endl;
        cout << "peak : " << peak << endl;

        delete run;
	}

	theApp.Run();

	return 0;
}

