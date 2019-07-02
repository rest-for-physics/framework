///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAnalysisHistogramViewerProcess.cxx
///
///
///             First implementation of Geant4 analysis process into REST_v2
///             Date : mar/2016
///             Date : may/2018 Added twist parameters
///             Author : J. Galan
///
///_______________________________________________________________________________

#include "TRestAnalysisHistogramViewerProcess.h"
using namespace std;

int rawCounter4 = 0;

ClassImp(TRestAnalysisHistogramViewerProcess)
    //______________________________________________________________________________
    TRestAnalysisHistogramViewerProcess::TRestAnalysisHistogramViewerProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestAnalysisHistogramViewerProcess::TRestAnalysisHistogramViewerProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestAnalysisHistogramViewerProcess::~TRestAnalysisHistogramViewerProcess() {
    delete fInputTrackEvent;
    delete fOutputTrackEvent;
}

void TRestAnalysisHistogramViewerProcess::LoadDefaultConfig() { SetTitle("Default config"); }

//______________________________________________________________________________
void TRestAnalysisHistogramViewerProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputTrackEvent = new TRestTrackEvent();
    fOutputTrackEvent = new TRestTrackEvent();

    fInputEvent = fInputTrackEvent;
    fOutputEvent = fOutputTrackEvent;
}

void TRestAnalysisHistogramViewerProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestAnalysisHistogramViewerProcess::InitProcess() {
    cout << "Canvas size X : " << fHistoCanvasSize.X() << endl;

    fAnaHistoCanvas =
        new TCanvas(this->GetName(), this->GetTitle(), fHistoCanvasSize.X(), fHistoCanvasSize.Y());

    fPlotString = "sAna.TripleMaxIntegral>>(1000,0,1000)";
}

//______________________________________________________________________________
void TRestAnalysisHistogramViewerProcess::BeginOfEventProcess() { fOutputTrackEvent->Initialize(); }

//______________________________________________________________________________
TRestEvent* TRestAnalysisHistogramViewerProcess::ProcessEvent(TRestEvent* evInput) {
    TRestTrackEvent* fInputTrackEvent = (TRestTrackEvent*)evInput;

    // Copying the input tracks to the output track
    for (int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++)
        fOutputTrackEvent->AddTrack(fInputTrackEvent->GetTrack(tck));

    if (this->GetVerboseLevel() >= REST_Debug) fInputTrackEvent->PrintOnlyTracks();

    if (GetVerboseLevel() >= REST_Info) {
        cout << "TRestAnalysisHistogramViewerProcess : " << GetName() << endl;
        cout << "----------------------------------------------" << endl;
        fAnalysisTree->PrintObservables();
        if (GetVerboseLevel() >= REST_Extreme) GetChar();
    }

    rawCounter4++;

    if (rawCounter4 % 10 == 0) {
        GetAnalysisTree()->Draw(fPlotString, "", "");
    }

    return fOutputTrackEvent;
}

//______________________________________________________________________________
void TRestAnalysisHistogramViewerProcess::EndOfEventProcess() {}

//______________________________________________________________________________
void TRestAnalysisHistogramViewerProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();

    delete fAnaHistoCanvas;
}

//______________________________________________________________________________
void TRestAnalysisHistogramViewerProcess::InitFromConfigFile() {
    fHistoCanvasSize = StringTo2DVector(GetParameter("canvasSize", "(800,600)"));

    fPlotString = GetParameter("plotString", "sAna.TripleMaxIntegral>>(1000,0,1000)");
}
