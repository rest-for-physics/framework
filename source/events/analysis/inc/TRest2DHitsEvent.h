///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRest2DHitsEvent.h
///
///             Event class to store DAQ events either from simulation and acquisition 
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Gracia
///_______________________________________________________________________________


#ifndef RestDAQ_TRest2DHitsEvent
#define RestDAQ_TRest2DHitsEvent

#include <iostream>

#include <TObject.h>
#include <TVirtualPad.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TMultiGraph.h>
#include <TAxis.h>

#include <TRestEvent.h>
#include <TRestRawSignal.h>
#include <TRestSignal.h>
#include "TRestReadout.h"

class TRest2DHitsEvent : public TRestEvent {

protected:

	vector<map<double, double>> fXZHits;  // XZsignal[z_index][xposition,energy]
	vector<map<double, double>> fYZHits;  // YZsignal[z_index][yposition,energy]
	map<int, double> fXZIdPos;  //signaId ,xPosition
	map<int, double> fYZIdPos;  //signaId ,yPosition

	int fNz;
	int fNx;
	int fNy;
	int fNSignalx;
	int fNSignaly;

	TGraph2D* gxz=NULL;//!
	TGraph2D* gyz=NULL;//!
	TGraph* exz = NULL;//!
	TGraph* eyz = NULL;//!

	TRestReadout* fReadout;//!

	TPad *fPad;//!

public:

	TPad *DrawEvent(TString option = "");

	//Setters
	void AddSignal(TRestRawSignal *s);

	void AddSignal(TRestSignal *s);

	void SetXZSignal(int zIndex, double xPosition, double energy);

	void SetYZSignal(int zIndex, double yPosition, double energy);

	void SetSignal(int zIndex, int signalID, double energy);
	
	void ResetHits();

	void SetReadout(TRestReadout*r) { if (r != NULL)fReadout = r; }

	void SetZLength(int l) { fNz = l; }

	//Getters
	Int_t GetNumberOfSignals() { return GetNumberOfXZSignals()+GetNumberOfYZSignals(); }
	Int_t GetNumberOfXZSignals() { return fNSignalx; }
	Int_t GetNumberOfYZSignals() { return fNSignaly; }

	//2D hits are with three values
	map<double, double> GetXZHitsWithZ(int z);//x,e
	map<double, double> GetYZHitsWithZ(int z);//x,e
	map<int, double> GetXZHitsWithX(double x);//z,e
	map<int, double> GetYZHitsWithY(double y);//z,e
	//signals are one dementional vector
	vector<double> GetXZSignal(int n);
	vector<double> GetYZSignal(int n);

	double GetSumEnergy(map<double, double> hits);

	double GetSumEnergy(map<int, double> hits);

	TVector2 GetZRange();
	TVector2 GetXRange();
	TVector2 GetYRange();

	double GetX(int n) { return n < fNSignalx ? fXZIdPos[n] : numeric_limits<Double_t>::quiet_NaN(); }
	double GetY(int n) { return n < fNSignaly ? fYZIdPos[n] : numeric_limits<Double_t>::quiet_NaN(); }


	void Initialize();

	void PrintEvent(Bool_t fullInfo = false);

	//Construtor
	TRest2DHitsEvent();
	//Destructor
	~TRest2DHitsEvent();

	ClassDef(TRest2DHitsEvent, 1);     // REST event superclass
};
#endif
