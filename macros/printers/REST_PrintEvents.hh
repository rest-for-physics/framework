#include "TRestTask.h"
#include "TRestG4Metadata.h"
#include "TRestG4Event.h"
#include "TRestHitsEvent.h"
#include "TRestLinearTrackEvent.h"
#include "TRestSignalEvent.h"
#include "TRestRawSignalEvent.h"
#include "TRestTrackEvent.h"

#ifndef RESTTask_PrintEvents
#define RESTTask_PrintEvents

Int_t REST_Printer(TString fName, TString EventType="", Int_t firstEvent = 0)
{
	TRestRun *run = new TRestRun();
	TClass*cl = TClass::GetClass(EventType);
	if (cl == NULL || EventType == "") {
		run->OpenInputFile(fName);
		TRestEvent*eve = run->GetInputEvent();
		if (eve == NULL) {
			cout << "Failed to launch the printer!" << endl;
			return 0;
		}
	}
	else
	{
		TRestEvent*eve = (TRestEvent*)cl->New();
		run->SetInputEvent(eve);
		run->OpenInputFile(fName);

	}

	TRestEvent*evt = run->GetInputEvent();

	run->PrintInfo();

	run->GetEntry(firstEvent);

	evt->PrintEvent();

	delete run;

	return 0;
}


Int_t REST_Printer_G4Event(TString fName, Int_t firstEvent = 0)
{
	TRestRun *run = new TRestRun();

	run->OpenInputFile(fName);

	run->PrintInfo();

	TRestG4Event *evt = new TRestG4Event();

	run->SetInputEvent(evt);

	run->GetEntry(firstEvent);

	evt->PrintEvent();

	delete run;

	return 0;
}

Int_t REST_Printer_HitsEvent(TString fName, Int_t firstEvent = 0)
{
	TRestRun *run = new TRestRun();

	run->OpenInputFile(fName);

	run->PrintInfo();

	TRestHitsEvent *evt = new TRestHitsEvent();

	run->SetInputEvent(evt);

	run->GetEntry(firstEvent);

	evt->PrintEvent();

	delete run;

	return 0;
}

Int_t REST_Printer_LinearTrackEvent(TString fName, Int_t firstEvent = 0)
{
	TRestRun *run = new TRestRun();

	run->OpenInputFile(fName);

	run->PrintInfo();

	TRestLinearTrackEvent *evt = new TRestLinearTrackEvent();

	run->SetInputEvent(evt);

	run->GetEntry(firstEvent);

	evt->PrintEvent();

	delete run;

	return 0;
}

Int_t REST_Printer_RawSignalEvent(TString fName, Int_t firstEvent = 0)
{
	TRestRun *run = new TRestRun();

	run->OpenInputFile(fName);

	run->PrintInfo();

	TRestRawSignalEvent *evt = new TRestRawSignalEvent();

	run->SetInputEvent(evt);

	run->GetEntry(firstEvent);

	evt->PrintEvent();

	delete run;

	return 0;
}

Int_t REST_Printer_SignalEvent(TString fName, Int_t firstEvent = 0)
{
	TRestRun *run = new TRestRun();

	run->OpenInputFile(fName);

	run->PrintInfo();

	TRestSignalEvent *evt = new TRestSignalEvent();

	run->SetInputEvent(evt);

	run->GetEntry(firstEvent);

	evt->PrintEvent();

	delete run;

	return 0;
}

Int_t REST_Printer_TrackEvent(TString fName, Int_t firstEvent = 0)
{
	TRestRun *run = new TRestRun();

	run->OpenInputFile(fName);

	run->PrintInfo();

	TRestTrackEvent *evt = new TRestTrackEvent();

	run->SetInputEvent(evt);

	run->GetEntry(firstEvent);

	evt->PrintEvent();

	delete run;

	return 0;
}


class REST_PrintEvents :public TRestTask {
public:
	ClassDef(REST_PrintEvents, 1);

	REST_PrintEvents() { fNRequiredArgument = 2; }
	~REST_PrintEvents() {}

	TString filename = " ";
	TString eventype = "";
	int firsteve = 0;

	void RunTask(TRestManager*mgr)
	{
		REST_Printer(filename, eventype, firsteve);
	}

};









#endif