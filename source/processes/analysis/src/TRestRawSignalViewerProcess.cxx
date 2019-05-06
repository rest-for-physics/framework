///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalViewerProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestSignalViewerProcess
///             Date : feb/2017
///             Author : J. Galan
///
///_______________________________________________________________________________

#include <TLegend.h>
#include <TPaveText.h>

#include "TRestRawSignalViewerProcess.h"
using namespace std;

ClassImp(TRestRawSignalViewerProcess)
    //______________________________________________________________________________
    TRestRawSignalViewerProcess::TRestRawSignalViewerProcess() {
  Initialize();
}

//______________________________________________________________________________
TRestRawSignalViewerProcess::TRestRawSignalViewerProcess(char* cfgFileName) {
  Initialize();

  if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestRawSignalViewerProcess::~TRestRawSignalViewerProcess() {
  delete fSignalEvent;
}

void TRestRawSignalViewerProcess::LoadDefaultConfig() {
  SetTitle("Default config");
}

//______________________________________________________________________________
void TRestRawSignalViewerProcess::Initialize() {
  SetSectionName(this->ClassName());

  fSignalEvent = new TRestRawSignalEvent();

  fOutputEvent = fSignalEvent;
  fInputEvent = fSignalEvent;

  fDrawRefresh = 0;

  fSingleThreadOnly = true;
}

void TRestRawSignalViewerProcess::LoadConfig(std::string cfgFilename,
                                             std::string name) {
  if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestRawSignalViewerProcess::InitProcess() { this->CreateCanvas(); }

//______________________________________________________________________________
void TRestRawSignalViewerProcess::BeginOfEventProcess() {
  // fSignalEvent->Initialize();
}

//______________________________________________________________________________
TRestEvent* TRestRawSignalViewerProcess::ProcessEvent(TRestEvent* evInput) {
  TString obsName;

  TRestRawSignalEvent* fInputSignalEvent = (TRestRawSignalEvent*)evInput;

  /// Copying the signal event to the output event
  fSignalEvent = fInputSignalEvent;
  // fSignalEvent->SetID(fInputSignalEvent->GetID());
  // fSignalEvent->SetSubID(fInputSignalEvent->GetSubID());
  // fSignalEvent->SetTimeStamp(fInputSignalEvent->GetTimeStamp());
  // fSignalEvent->SetSubEventTag(fInputSignalEvent->GetSubEventTag());

  ////for( int sgnl = 0; sgnl < fInputSignalEvent->GetNumberOfSignals(); sgnl++
  ///)
  // Int_t N = fInputSignalEvent->GetNumberOfSignals();
  // if (GetVerboseLevel() >= REST_Debug) N = 1;
  // for (int sgnl = 0; sgnl < N; sgnl++)
  //	fSignalEvent->AddSignal(*fInputSignalEvent->GetSignal(sgnl));
  /////////////////////////////////////////////////

  fCanvas->cd();
  eveCounter++;
  if (eveCounter >= fDrawRefresh) {
    eveCounter = 0;
    sgnCounter = 0;
    if (GetVerboseLevel() >= REST_Debug) {
      GetAnalysisTree()->PrintObservables();
    }
    for (unsigned int i = 0; i < fDrawingObjects.size(); i++)
      delete fDrawingObjects[i];
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
        if (sgnCounter >= 0 &&
            sgnCounter < fInputSignalEvent->GetNumberOfSignals()) {
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
        if (sgnCounter >= 0 &&
            sgnCounter < fInputSignalEvent->GetNumberOfSignals()) {
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
void TRestRawSignalViewerProcess::EndOfEventProcess() {}

//______________________________________________________________________________
void TRestRawSignalViewerProcess::EndProcess() {
  // Function to be executed once at the end of the process
  // (after all events have been processed)

  // Start by calling the EndProcess function of the abstract class.
  // Comment this if you don't want it.
  // TRestEventProcess::EndProcess();
}

TPad* TRestRawSignalViewerProcess::DrawSignal(Int_t signal) {
  TPad* pad = new TPad(this->GetName(), this->GetTitle(), 0, 0, 1, 1);

  pad->cd();

  fDrawingObjects.push_back((TObject*)pad);

  TGraph* gr = new TGraph();
  fDrawingObjects.push_back((TObject*)gr);

  TRestRawSignal* sgnl = fSignalEvent->GetSignal(signal);

  for (int n = 0; n < sgnl->GetNumberOfPoints(); n++)
    gr->SetPoint(n, n, sgnl->GetData(n));

  gr->Draw("AC*");

  TGraph* gr2 = new TGraph();
  fDrawingObjects.push_back((TObject*)gr2);

  gr2->SetLineWidth(2);
  gr2->SetLineColor(2);

  for (int n = fBaseLineRange.X(); n < fBaseLineRange.Y(); n++)
    gr2->SetPoint(n - fBaseLineRange.X(), n, sgnl->GetData(n));

  gr2->Draw("CP");

  vector<Int_t> pOver = sgnl->GetPointsOverThreshold();

  TGraph* gr3[5];
  Int_t nGraphs = 0;
  gr3[nGraphs] = new TGraph();
  fDrawingObjects.push_back((TObject*)gr3[nGraphs]);
  gr3[nGraphs]->SetLineWidth(2);
  gr3[nGraphs]->SetLineColor(3);
  Int_t point = 0;
  Int_t nPoints = pOver.size();
  for (int n = 0; n < nPoints; n++) {
    gr3[nGraphs]->SetPoint(point, pOver[n], sgnl->GetData(pOver[n]));
    point++;
    if (n + 1 < nPoints && pOver[n + 1] - pOver[n] > 1) {
      gr3[nGraphs]->Draw("CP");
      nGraphs++;
      if (nGraphs > 4) cout << "Ngraphs : " << nGraphs << endl;
      point = 0;
      gr3[nGraphs] = new TGraph();
      fDrawingObjects.push_back((TObject*)gr3[nGraphs]);
      gr3[nGraphs]->SetLineWidth(2);
      gr3[nGraphs]->SetLineColor(3);
    }
  }

  if (nPoints > 0) gr3[nGraphs]->Draw("CP");

  return pad;
}

//______________________________________________________________________________
void TRestRawSignalViewerProcess::InitFromConfigFile() {
  fDrawRefresh = StringToDouble(GetParameter("refreshEvery", "0"));

  fBaseLineRange = StringTo2DVector(GetParameter("baseLineRange", "(5,55)"));

  fCanvasSize = StringTo2DVector(GetParameter("canvasSize", "(800,600)"));
}
