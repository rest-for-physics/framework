///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSignalViewerProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestDetectorSignalViewerProcess
///             Date : feb/2017
///             Author : J. Galan
///
///_______________________________________________________________________________

#include <TLegend.h>
#include <TPaveText.h>

#include "TRestDetectorSignalViewerProcess.h"
using namespace std;

int rawCounter3 = 0;

ClassImp(TRestDetectorSignalViewerProcess)
    //______________________________________________________________________________
    TRestDetectorSignalViewerProcess::TRestDetectorSignalViewerProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestDetectorSignalViewerProcess::TRestDetectorSignalViewerProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestDetectorSignalViewerProcess::~TRestDetectorSignalViewerProcess() {}

void TRestDetectorSignalViewerProcess::LoadDefaultConfig() { SetTitle("Default config"); }

//______________________________________________________________________________
void TRestDetectorSignalViewerProcess::Initialize() {
    SetSectionName(this->ClassName());

    fSignalEvent = NULL;

    fDrawRefresh = 0;

    fSingleThreadOnly = true;
}

void TRestDetectorSignalViewerProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestDetectorSignalViewerProcess::InitProcess() { this->CreateCanvas(); }

//______________________________________________________________________________
TRestEvent* TRestDetectorSignalViewerProcess::ProcessEvent(TRestEvent* evInput) {
    TString obsName;

    TRestDetectorSignalEvent* fInputSignalEvent = (TRestDetectorSignalEvent*)evInput;

    /// Copying the signal event to the output event
    fSignalEvent = fInputSignalEvent;
    //   fSignalEvent->SetID( fInputSignalEvent->GetID() );
    //   fSignalEvent->SetSubID( fInputSignalEvent->GetSubID() );
    //   fSignalEvent->SetTimeStamp( fInputSignalEvent->GetTimeStamp() );
    //   fSignalEvent->SetSubEventTag( fInputSignalEvent->GetSubEventTag() );

    //   //for( int sgnl = 0; sgnl < fInputSignalEvent->GetNumberOfSignals();
    //   sgnl++ ) Int_t N = fInputSignalEvent->GetNumberOfSignals();
    //  // if( GetVerboseLevel() >= REST_Debug ) N = 1;
    //   for( int sgnl = 0; sgnl < N; sgnl++ )
    // fSignalEvent->AddSignal( *fInputSignalEvent->GetSignal( sgnl ) );
    /////////////////////////////////////////////////

    GetCanvas()->cd();
    GetCanvas()->SetGrid();
    fCanvas->cd();
    eveCounter++;
    if (eveCounter >= fDrawRefresh) {
        eveCounter = 0;
        sgnCounter = 0;
        if (GetVerboseLevel() >= REST_Debug) {
            GetAnalysisTree()->PrintObservables();
        }
        for (unsigned int i = 0; i < fDrawingObjects.size(); i++) delete fDrawingObjects[i];
        fDrawingObjects.clear();

        TPad* pad2 = DrawSignal(sgnCounter);

        fCanvas->cd();
        pad2->Draw();
        fCanvas->Update();

        fout.setborder("");
        fout.setorientation(1);
        fout << "Press Enter to continue\nPress Esc to stop viewing\nPress n/p to "
                "switch signals"
             << endl;

        while (1) {
            int a = GetChar("");
            if (a == 10)  // enter
            {
                break;
            } else if (a == 27)  // esc
            {
                fDrawRefresh = 1e99;
                while (getchar() != '\n')
                    ;
                break;
            } else if (a == 110 || a == 78)  // n
            {
                sgnCounter++;
                if (sgnCounter >= 0 && sgnCounter < fInputSignalEvent->GetNumberOfSignals()) {
                    TPad* pad2 = DrawSignal(sgnCounter);
                    fCanvas->cd();
                    pad2->Draw();
                    fCanvas->Update();
                } else {
                    warning << "cannot plot signal with id " << sgnCounter << endl;
                }
            } else if (a == 112 || a == 80)  // p
            {
                sgnCounter--;
                if (sgnCounter >= 0 && sgnCounter < fInputSignalEvent->GetNumberOfSignals()) {
                    TPad* pad2 = DrawSignal(sgnCounter);
                    fCanvas->cd();
                    pad2->Draw();
                    fCanvas->Update();
                } else {
                    warning << "cannot plot signal with id " << sgnCounter << endl;
                }
            }
            while (getchar() != '\n')
                ;
        }
    }

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestDetectorSignalViewerProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestDetectorSignalViewerProcess::InitFromConfigFile() {
    fDrawRefresh = StringToDouble(GetParameter("refreshEvery", "0"));

    fBaseLineRange = StringTo2DVector(GetParameter("baseLineRange", "(5,55)"));

    fCanvasSize = StringTo2DVector(GetParameter("canvasSize", "(800,600)"));
}

TPad* TRestDetectorSignalViewerProcess::DrawSignal(Int_t signal) {
    TPad* pad = new TPad(this->GetName(), this->GetTitle(), 0, 0, 1, 1);
    pad->cd();
    pad->DrawFrame(fSignalEvent->GetMinTime() - 1, -10, fSignalEvent->GetMaxTime() + 1,
                   fSignalEvent->GetMaxValue() + 10, this->GetTitle());

    TMultiGraph* mg = new TMultiGraph();
    mg->SetTitle(this->GetTitle());
    mg->GetXaxis()->SetTitle("time bins");
    mg->GetYaxis()->SetTitleOffset(1.4);
    mg->GetYaxis()->SetTitle("Energy");

    for (int n = 0; n < fSignalEvent->GetNumberOfSignals(); n++) {
        TGraph* gr = fSignalEvent->GetSignal(n)->GetGraph((n + 1) % 6);
        mg->Add(gr);
    }

    mg->Draw("");

    return pad;
}
