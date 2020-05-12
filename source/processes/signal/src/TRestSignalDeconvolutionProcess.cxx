///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalDeconvolutionProcess.cxx
///
///             oct 2015:  Javier Galan
///_______________________________________________________________________________

#include "TRestSignalDeconvolutionProcess.h"
using namespace std;

#include <TRestFFT.h>

#include <TFile.h>

#include <TCanvas.h>

TRestFFT* responseFFT;
// TRestFFT *cleanFFT;

ClassImp(TRestSignalDeconvolutionProcess)
    //______________________________________________________________________________
    TRestSignalDeconvolutionProcess::TRestSignalDeconvolutionProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestSignalDeconvolutionProcess::TRestSignalDeconvolutionProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();

    PrintMetadata();
    // TRestSignalDeconvolutionProcess default constructor
}

//______________________________________________________________________________
TRestSignalDeconvolutionProcess::~TRestSignalDeconvolutionProcess() {
    delete fOutputSignalEvent;
    // TRestSignalDeconvolutionProcess destructor

    //   delete cleanFFT;
    delete responseFFT;
}

void TRestSignalDeconvolutionProcess::LoadConfig(string cfgFilename) {
    if (LoadConfigFromFile(cfgFilename)) LoadDefaultConfig();

    PrintMetadata();
}

void TRestSignalDeconvolutionProcess::LoadDefaultConfig() {
    SetName("signalDeconvolutionProcess-Default");
    SetTitle("Default config");

    fFreq1 = 11.1063e-2 / M_PI / 1.5;
    fFreq2 = 14.8001e-2 / M_PI / 1.5;

    fSmoothingPoints = 3;
    fSmearingPoints = 5;

    fBaseLineStart = 32;
    fBaseLineEnd = 64;

    fFFTStart = 32;
    fFFTEnd = 32;

    fCutFrequency = 12;

    fResponseFilename = "AGET_Response_100MHz_Gain0x2_Shaping0x7.root";
}

//______________________________________________________________________________
void TRestSignalDeconvolutionProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputSignalEvent = NULL;
    fOutputSignalEvent = new TRestSignalEvent();

    responseFFT = new TRestFFT();
    // cleanFFT = new TRestFFT();
}

//______________________________________________________________________________
void TRestSignalDeconvolutionProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();

    TRestSignal* responseSignal = new TRestSignal();

    TString fullPathName = REST_PATH + "/data/signal/" + fResponseFilename;

    TFile* f = new TFile(fullPathName);

    responseSignal = (TRestSignal*)f->Get("signal Response");

    f->Close();

    TRestSignal smoothSignal, difSignal, dif2Signal;

    // Smoothing response signal
    responseSignal->GetSignalSmoothed(&smoothSignal, 3);
    if (GetVerboseLevel() >= REST_Debug) smoothSignal.WriteSignalToTextFile("/tmp/smooth.txt");

    // Obtainning dV/dt
    // smoothSignal.GetDifferentialSignal( &difSignal, fSmearingPoints );
    // difSignal.WriteSignalToTextFile ( "diff.txt" );
    /*
    // Obtainning d2V/dt2
    difSignal.GetDifferentialSignal( &dif2Signal, fSmearingPoints );
    dif2Signal.WriteSignalToTextFile ( "diff2.txt" );


    // Applying transform -> difSignal
    difSignal.MultiplySignalBy ( 1/fFreq1 );
    dif2Signal.MultiplySignalBy ( 1/fFreq2/fFreq1 );
    difSignal.SignalAddition ( &smoothSignal );
    difSignal.SignalAddition ( &dif2Signal );

    difSignal.WriteSignalToTextFile( "diff3.txt");
    exit(1);
    */
    //   cout << "oFFT : " << responseFFT << endl;
    //  responseFFT->ForwardSignalFFT( &smoothSignal, fFFTStart, fFFTEnd );
    if (GetVerboseLevel() >= REST_Debug) responseFFT->WriteFrequencyToTextFile("/tmp/responseFFT.txt");

    // We overwrite the response signal and replace by the analytical version
    responseFFT->SetNfft(512);
    responseFFT->GaussianSecondOrderResponse(27.1385, 186.388, 21355.9, 2.18342e-3);

    responseFFT->WriteFrequencyToTextFile("ANALYTICAL.txt");

    // Exponential convolution for signal tail-cleaning
    /*
    Double_t baseline = difSignal.GetAverage( fBaseLineStart, fBaseLineEnd );
    Double_t maxIndex = difSignal.GetMaxIndex();
    Double_t width = difSignal.GetMaxPeakWidth();
    difSignal.ExponentialConvolution( maxIndex+width/2., width, baseline );

    difSignal.WriteSignalToTextFile( "diffProc.txt" );

    cleanFFT->ForwardSignalFFT( &difSignal, fFFTStart, fFFTEnd );
    cleanFFT->WriteFrequencyToTextFile( "cleanFFT.txt" );
    cleanFFT->DivideBy( originalFFT );
    cleanFFT->WriteFrequencyToTextFile( "responseFFT.txt" );
  */

    delete responseSignal;
}

//______________________________________________________________________________
TRestEvent* TRestSignalDeconvolutionProcess::ProcessEvent(TRestEvent* evInput) {
    fInputSignalEvent = (TRestSignalEvent*)evInput;

    if (fInputSignalEvent->GetNumberOfSignals() <= 0) return NULL;

    for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++) {
        TRestSignal smoothSignal, difSignal, dif2Signal;

        fInputSignalEvent->SubstractBaselines(40, 100);
        // Smoothing input signal
        fInputSignalEvent->GetSignal(n)->GetSignalSmoothed(&smoothSignal, fSmoothingPoints);
        difSignal.SetSignalID(fInputSignalEvent->GetSignal(n)->GetSignalID());

        /* **** This could be implemented in another process */
        // Obtainning dV(t)/dt
        // smoothSignal.GetDifferentialSignal( &difSignal, fSmearingPoints );
        // Obtainning d2V(t)/dt2
        // difSignal.GetDifferentialSignal( &dif2Signal, fSmearingPoints );

        // Applying second order deconvolution -> difSignal
        // difSignal.MultiplySignalBy ( 1/fFreq1 );
        // dif2Signal.MultiplySignalBy ( 1/fFreq2/fFreq1 );
        // difSignal.SignalAddition ( &smoothSignal );
        // difSignal.SignalAddition ( &dif2Signal );
        /* ************************************************ */

        // Applying response
        TRestFFT* mySignalFFT = new TRestFFT();
        mySignalFFT->ForwardSignalFFT(&smoothSignal, fFFTStart, fFFTEnd);
        if (GetVerboseLevel() >= REST_Debug) smoothSignal.WriteSignalToTextFile("/tmp/smoothSignal.txt");

        if (GetVerboseLevel() >= REST_Debug) mySignalFFT->WriteFrequencyToTextFile("/tmp/smoothFFT.txt");

        // responseFFT->DivideBy( responseFFT, 500 );

        // mySignalFFT->KillFrequencies( 60, 260 );
        // mySignalFFT->ButterWorthFilter( 80, 1 );//, Double_t amp, Double_t decay
        // )

        if (GetVerboseLevel() >= REST_Debug) mySignalFFT->WriteFrequencyToTextFile("/tmp/filteredFFT.txt");

        if (GetVerboseLevel() >= REST_Debug) {
            mySignalFFT->BackwardFFT();
            mySignalFFT->GetSignal(&difSignal);
            difSignal.WriteSignalToTextFile("/tmp/filteredSignal.txt");
        }

        // mySignalFFT->DivideBy( responseFFT, 0, 20 );
        mySignalFFT->ApplyResponse(responseFFT, fCutFrequency);
        // mySignalFFT->ButterWorthFilter( 43, 4 );//, Double_t amp, Double_t decay
        // ) mySignalFFT->KillFrequencies( 63 );//, Double_t amp, Double_t decay )

        if (GetVerboseLevel() >= REST_Debug)
            mySignalFFT->WriteFrequencyToTextFile("/tmp/unconvolutedFFT.txt");

        // Removing baseline
        mySignalFFT->RemoveBaseline();

        // Recovering time signal
        mySignalFFT->BackwardFFT();
        mySignalFFT->GetSignal(&difSignal);

        if (GetVerboseLevel() >= REST_Debug) difSignal.WriteSignalToTextFile("/tmp/resultingSignal.txt");

        if (GetVerboseLevel() >= REST_Debug) GetChar();

        fOutputSignalEvent->AddSignal(difSignal);

        delete mySignalFFT;
    }

    return fOutputSignalEvent;
}

//______________________________________________________________________________
void TRestSignalDeconvolutionProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestSignalDeconvolutionProcess::InitFromConfigFile() {
    fFreq1 = StringToDouble(GetParameter("frequency1"));
    fFreq2 = StringToDouble(GetParameter("frequency2"));

    fSmoothingPoints = StringToInteger(GetParameter("smoothingPoints"));
    fSmearingPoints = StringToInteger(GetParameter("smearingPoints"));

    fBaseLineStart = StringToInteger(GetParameter("baselineStart"));
    fBaseLineEnd = StringToInteger(GetParameter("baselineEnd"));

    fFFTStart = StringToInteger(GetParameter("nFFTStart"));
    fFFTEnd = StringToInteger(GetParameter("nFFTEnd"));

    fCutFrequency = StringToInteger(GetParameter("cutFrequency"));

    fResponseFilename = GetParameter("responseFile");

    PrintMetadata();
}
