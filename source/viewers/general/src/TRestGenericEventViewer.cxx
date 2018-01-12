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
#include "TRestBrowser.h"
#include "TGButton.h"
using namespace std;

ClassImp(TRestGenericEventViewer)
//______________________________________________________________________________
TRestGenericEventViewer::TRestGenericEventViewer()
{

}


//______________________________________________________________________________
TRestGenericEventViewer::~TRestGenericEventViewer()
{

}

//______________________________________________________________________________
void TRestGenericEventViewer::Initialize()
{

	fPad = NULL;

	fCanvas = new TCanvas("Event Viewer", "Event Viewer");

	if (fController == NULL) {
		return;
	}





	auto frame = fController->generateNewFrame();

	auto frame1 = new TGVerticalFrame(frame);
	{
		fLabel = new TGLabel(frame1, "Plot Options:");
		frame1->AddFrame(fLabel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

		fOptwindow = new TGTextEntry(frame1, "");
		fOptwindow->SetText("");
		frame1->AddFrame(fOptwindow, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

		fButOpt = new  TGTextButton(frame1, "Plot");///< Load Event button
		fButOpt->Connect("Clicked()", "TRestGenericEventViewer", this, "OptionPlot()");
		frame1->AddFrame(fButOpt, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
	}
	frame->AddFrame(frame1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));


	fButPrev = new  TGTextButton(frame, "<<Previous");///< Load Event button
	fButPrev->Connect("Clicked()", "TRestGenericEventViewer", this, "PreviousOption()");
	frame->AddFrame(fButPrev, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

	fButNext = new  TGTextButton(frame, "Next>>");///< Load Event button
	fButNext->Connect("Clicked()", "TRestGenericEventViewer", this, "NextOption()");
	frame->AddFrame(fButNext, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));


	fController->addFrame(frame);
	//fEvent = new TRestSignalEvent();

}

void TRestGenericEventViewer::AddEvent(TRestEvent *ev) {


	//fEvent=(TRestSignalEvent *)ev;
	fEvent = ev;

	fOptwindow->SetText("");

	OptionPlot();


}

void TRestGenericEventViewer::NextOption()
{
	string text = fOptwindow->GetText();
	if (text == "") {
		text = "0";
	}
	else if (isANumber(text))
	{
		text = ToString(StringToInteger(text) + 1);
	}

	fOptwindow->SetText(text.c_str());
	OptionPlot();
}

void TRestGenericEventViewer::PreviousOption()
{
	string text = fOptwindow->GetText();
	if (text == "") {
		text = "0";
	}
	else if (isANumber(text))
	{
		text = ToString(StringToInteger(text) - 1);
	}

	fOptwindow->SetText(text.c_str());
	OptionPlot();
}

void TRestGenericEventViewer::OptionPlot()
{

	fPad = fEvent->DrawEvent(fOptwindow->GetText());
	if (fPad == NULL)fPad = new TPad();

	fCanvas->cd();
	fPad->Draw();
	fPad->Update();
	fCanvas->Update();
}



