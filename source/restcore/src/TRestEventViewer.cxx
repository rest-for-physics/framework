///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEventViewer.cxx
///
///             A geometry class to store detector geometry
///
///             jul 2015:   First concept
///                 J. Galan
///		nov 2015: Generic class for event visualization
///		    JuanAn Garcia
///
///_______________________________________________________________________________

#include "TRestEventViewer.h"
#include "TRestBrowser.h"

ClassImp(TRestEventViewer)
    //______________________________________________________________________________
    TRestEventViewer::TRestEventViewer() {
    // TRestEventViewer default constructor
    Initialize();
}

//______________________________________________________________________________
TRestEventViewer::~TRestEventViewer() {
    // TRestEventViewer destructor
    // DeleteCurrentEvent(  );
}

void TRestEventViewer::Initialize() {
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
    frame->AddFrame(fOptwindow, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    auto frame1 = new TGHorizontalFrame(frame);
    {
        fButPrev = new TGTextButton(frame1, "<<Previous");  ///< Load Event button
        fButPrev->Connect("Clicked()", "TRestEventViewer", this, "PreviousOption()");
        frame1->AddFrame(fButPrev, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        fButOpt = new TGTextButton(frame1, "Plot");  ///< Load Event button
        fButOpt->Connect("Clicked()", "TRestEventViewer", this, "OptionPlot()");
        frame1->AddFrame(fButOpt, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        fButNext = new TGTextButton(frame1, "Next>>");  ///< Load Event button
        fButNext->Connect("Clicked()", "TRestEventViewer", this, "NextOption()");
        frame1->AddFrame(fButNext, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    }
    frame->AddFrame(frame1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fController->addFrame(frame);
    // fEvent = new TRestSignalEvent();
}

void TRestEventViewer::AddEvent(TRestEvent* ev) {
    // fEvent=(TRestSignalEvent *)ev;
    fEvent = ev;

    // fOptwindow->SetText("");

    OptionPlot();
}

void TRestEventViewer::NextOption() {
    string text = fOptwindow->GetText();
    if (text == "") {
        text = "0";
    } else if (isANumber(text)) {
        text = ToString(StringToInteger(text) + 1);
    }

    fOptwindow->SetText(text.c_str());
    OptionPlot();
}

void TRestEventViewer::PreviousOption() {
    string text = fOptwindow->GetText();
    if (text == "") {
        text = "0";
    } else if (isANumber(text)) {
        text = ToString(StringToInteger(text) - 1);
    }

    fOptwindow->SetText(text.c_str());
    OptionPlot();
}

void TRestEventViewer::OptionPlot() {
    fPad = fEvent->DrawEvent(fOptwindow->GetText());
    if (fPad == NULL) fPad = new TPad();
    fCanvas->cd();
    fPad->Draw();
    fPad->Update();
    fCanvas->Update();
}

void TRestEventViewer::DeleteCurrentEvent() { delete fEvent; }
