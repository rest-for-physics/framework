///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRest2DHitsEvent.h
///
///             Storing events which contains lists of XZ and YZ signal points, 
///             by default, Z=0~512, in daq unit, X/Y=-100~100, in unit mm.
///             Implemented hough transformation and 2D event drawing
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Ni Kaixiang
///
///	       
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

//#define X1 -100 
//#define X2 100
//#define Y1 100 
//#define Y2 300

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

	TGraph2D* gxz = NULL;//!
	TGraph2D* gyz = NULL;//!
	TH2D* pointxz = NULL;//!
	TH2D* pointyz = NULL;//!
	TH1D* houghxz = NULL;//!
	TH1D* houghyz = NULL;//!

	TRestReadout* fReadout;//!

	TPad *fPad;//!

	vector<double> xzz;
	vector<double> xzx;
	vector<double> xze;
	vector<double> yzz;
	vector<double> yzy;
	vector<double> yze;

	vector<TVector3> fHough_XZ; //!  y=ax+b, vertical line angle 牟, length 老, [id][老,牟,weight]
	vector<TVector3> fHough_YZ; //!  y=ax+b, vertical line angle 牟, length 老, [id][老,牟,weight]

	int X1;
	int X2;
	int Y1;
	int Y2;

public:

	TPad *DrawEvent(TString option = "");

	void DoHough();
	//Setters
	void AddSignal(TRestRawSignal *s);

	void AddSignal(TRestSignal *s);

	void SetXZSignal(int zIndex, double xPosition, double energy);

	void SetYZSignal(int zIndex, double yPosition, double energy);

	void SetSignal(int zIndex, int signalID, double energy);

	void SetROIX(TVector2 x) { X1 = x.X(); X2 = x.Y(); }

	void SetROIY(TVector2 y) { Y1 = y.X(); Y2 = y.Y(); }
	
	void ResetHits();

	void RemoveSeparateZ();

	void SetReadout(TRestReadout*r) { if (r != NULL)fReadout = r; }

	void SetZLength(int l) { fNz = l; }

	void AddXZCluster(double x, double z, double e) { xzx.push_back(x); xzz.push_back(z); xze.push_back(e); }

	void AddYZCluster(double y, double z, double e) { yzy.push_back(y); yzz.push_back(z); yze.push_back(e); }

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

	double GetFirstX();
	double GetFirstY();

	double GetLastX();
	double GetLastY();

	TVector2 GetZRange();
	TVector2 GetZRangeInXZ();
	TVector2 GetZRangeInYZ();
	TVector2 GetXRange();
	TVector2 GetYRange();

	double GetX(int n) { return n < fNSignalx ? fXZIdPos[n] : numeric_limits<Double_t>::quiet_NaN(); }
	double GetY(int n) { return n < fNSignaly ? fYZIdPos[n] : numeric_limits<Double_t>::quiet_NaN(); }

	vector<TVector3> GetHoughXZ() { return fHough_XZ; }
	vector<TVector3> GetHoughYZ() { return fHough_YZ; }

	void Initialize();

	void PrintEvent(Bool_t fullInfo = false);

	//Construtor
	TRest2DHitsEvent();
	//Destructor
	~TRest2DHitsEvent();

	ClassDef(TRest2DHitsEvent, 1);     // REST event superclass
};
#endif
