///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGenericEventViewer.cxx
///
///             nov 2015:   First concept
///                 Viewer class for a TRestSignalEvent
///                 JuanAn Garcia
///_______________________________________________________________________________

#include "TRestGenericEventViewer.h"
#include "TGButton.h"
#include "TRestBrowser.h"
using namespace std;

ClassImp(TRestGenericEventViewer)
    //______________________________________________________________________________
    TRestGenericEventViewer::TRestGenericEventViewer() {}

//______________________________________________________________________________
TRestGenericEventViewer::~TRestGenericEventViewer() {}

//______________________________________________________________________________
void TRestGenericEventViewer::Initialize() {
  fPad = NULL;

  fCanvas = new TCanvas("Event Viewer", "Event Viewer");

  fCanvas->SetWindowPosition(350, 10);

  if (fController == NULL) {
    return;
  }

  auto frame = fController->generateNewFrame();

  fLabel = new TGLabel(frame, "Plot Options:");
  frame->AddFrame(fLabel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

  fOptwindow = new TGTextEntry(frame, "");
  fOptwindow->SetText("");
  frame->AddFrame(fOptwindow,
                  new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

  auto frame1 = new TGHorizontalFrame(frame);
  {
    fButPrev = new TGTextButton(frame1, "<<Previous");  ///< Load Event button
    fButPrev->Connect("Clicked()", "TRestGenericEventViewer", this,
                      "PreviousOption()");
    frame1->AddFrame(fButPrev,
                     new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fButOpt = new TGTextButton(frame1, "Plot");  ///< Load Event button
    fButOpt->Connect("Clicked()", "TRestGenericEventViewer", this,
                     "OptionPlot()");
    frame1->AddFrame(fButOpt,
                     new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fButNext = new TGTextButton(frame1, "Next>>");  ///< Load Event button
    fButNext->Connect("Clicked()", "TRestGenericEventViewer", this,
                      "NextOption()");
    frame1->AddFrame(fButNext,
                     new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  }
  frame->AddFrame(frame1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

  fController->addFrame(frame);
  // fEvent = new TRestSignalEvent();
}

void TRestGenericEventViewer::AddEvent(TRestEvent* ev) {
  // fEvent=(TRestSignalEvent *)ev;
  fEvent = ev;

  fOptwindow->SetText("");

  OptionPlot();
}

void TRestGenericEventViewer::NextOption() {
  string text = fOptwindow->GetText();
  if (text == "") {
    text = "0";
  } else if (isANumber(text)) {
    text = ToString(StringToInteger(text) + 1);
  }

  fOptwindow->SetText(text.c_str());
  OptionPlot();
}

void TRestGenericEventViewer::PreviousOption() {
  string text = fOptwindow->GetText();
  if (text == "") {
    text = "0";
  } else if (isANumber(text)) {
    text = ToString(StringToInteger(text) - 1);
  }

  fOptwindow->SetText(text.c_str());
  OptionPlot();
}

void TRestGenericEventViewer::OptionPlot() {
  auto pad = fEvent->DrawEvent(fOptwindow->GetText());
  if (pad == NULL) pad = new TPad();
  fCanvas->cd();
  pad->Draw();
  pad->Update();
  fCanvas->Update();
}
