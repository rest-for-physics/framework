#include "TRestTask.h"
#include "TRestRawSignalEvent.h"
#include "TRestSignalEvent.h"
Int_t minTime = 156;
Int_t maxTime = 159;

Int_t REST_Tools_ProduceResponseSignal( TString inputFileName, TString outputFileName, Int_t nPoints = 512, Double_t threshold = 1 )
{
    TRestRun *run = new TRestRun();
    run->OpenInputFile( inputFileName );

    TRestRawSignalEvent *evt = new TRestRawSignalEvent();
    run->PrintAllMetadata();
    run->SetInputEvent( evt );

    Double_t dataArray[512];
    for( int n = 0; n < 512; n++ )
		dataArray[n] = 0;

    Int_t N = 0;
    for( int i = 0; i < run->GetEntries(); i++ )
    {
        run->GetEntry(i);
        if( evt->GetNumberOfSignals() == 1 )
        {
            TRestRawSignal *s = evt->GetSignal(0);
            Int_t maxPeakBin = s->GetMaxPeakBin();
	    if( maxPeakBin > minTime && maxPeakBin < maxTime )
            {
		if( s->GetNumberOfPoints() != 512 )
		{
			cout << "Warning. Number of points is not 512" << endl;
			continue;
		}

		for( int n = 0; n < s->GetNumberOfPoints(); n++ )
		{
			dataArray[n] += (Double_t) s->GetData(n);
		}
                N++;
            }
        }
    }

    for( int n = 0; n < 512; n++ )
	cout << "n : " << n << " Data : " << dataArray[n]/N << endl;

    TRestSignal *responseSignal = new TRestSignal();
    responseSignal->Initialize();

    dataArray[0] = dataArray[1];

    for( int i = 0; i < 512; i++ )
        responseSignal->NewPoint( (Float_t) i, (Float_t) (dataArray[i]/N) );

    Double_t integral = responseSignal->GetIntegral();

    responseSignal->MultiplySignalBy( 1./integral );

    cout << "Normalized integral : " << responseSignal->GetIntegral() << endl;

    TFile *f = new TFile( outputFileName, "RECREATE" );
    f->cd();
    responseSignal->Write("signal Response");
 //   responseSignal->GetGraph()->Draw();

    f->Close();

    cout << "Number of entries : " << run->GetEntries() << endl;
    cout << "Number of good signals : " << N << endl;

    return 0;
}
