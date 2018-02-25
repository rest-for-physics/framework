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
///
///	       
///_______________________________________________________________________________


#include "TRest2DHitsEvent.h"
#include "TRestTools.h"

using namespace std;

ClassImp(TRest2DHitsEvent)
//______________________________________________________________________________
TRest2DHitsEvent::TRest2DHitsEvent()
{
	// TRest2DHitsEvent default constructor
	TRestEvent::Initialize();
	fReadout = NULL;
	fNSignalx = 0; fNSignaly = 0;
	fNz = 512; fNx = 192; fNy = 192;
	fXZHits = vector<map<double, double>>(fNz);
	fYZHits = vector<map<double, double>>(fNz);
	fXZIdPos.clear();
	fYZIdPos.clear();
	fPad = NULL;
}

//______________________________________________________________________________
TRest2DHitsEvent::~TRest2DHitsEvent() {}

void TRest2DHitsEvent::Initialize()
{
	TRestEvent::Initialize();
	fXZHits = vector<map<double, double>>(fNz);
	fYZHits = vector<map<double, double>>(fNz);
	fXZIdPos.clear();
	fYZIdPos.clear();
	fNSignalx = 0;
	fNSignaly = 0;
}

void TRest2DHitsEvent::AddSignal(TRestRawSignal *s)
{
	if (fReadout != NULL) {
		double x = fReadout->GetX(s->GetID());
		double y = fReadout->GetY(s->GetID());
		if (TMath::IsNaN(x) || TMath::IsNaN(y)) {
			if (!TMath::IsNaN(x)) {
				for (int i = 0; i < s->GetNumberOfPoints(); i++) {
					fXZHits[i][x] = s->GetData(i);
				}
				fXZIdPos[fNSignalx] = x;
				fNSignalx++;
			}
			else if (!TMath::IsNaN(y)) {
				for (int i = 0; i < s->GetNumberOfPoints(); i++) {
					fYZHits[i][y] = s->GetData(i);
				}
				fXZIdPos[fNSignaly] = y;
				fNSignaly++;
			}
		}
		else
		{
			cout << "This cannot happen!" << endl;
		}
	}

}

void TRest2DHitsEvent::AddSignal(TRestSignal *s)
{
	if (fReadout != NULL) {
		double x = fReadout->GetX(s->GetID());
		double y = fReadout->GetY(s->GetID());
		if (TMath::IsNaN(x) || TMath::IsNaN(y)) {
			if (!TMath::IsNaN(x)) {
				for (int i = 0; i < s->GetNumberOfPoints(); i++) {
					fXZHits[s->GetTime(i)][x] = s->GetData(i);
				}
				fXZIdPos[fNSignalx] = x;
				fNSignalx++;
			}
			else if (!TMath::IsNaN(y)) {
				for (int i = 0; i < s->GetNumberOfPoints(); i++) {
					fYZHits[s->GetTime(i)][y] = s->GetData(i);
				}
				fXZIdPos[fNSignaly] = y;
				fNSignaly++;
			}
		}
		else
		{
			cout << "This cannot happen!" << endl;
		}
	}
}

void TRest2DHitsEvent::SetXZSignal(int zIndex, double xPosition, double energy)
{
	double sum = 0;
	for (int i = 0; i < fNz; i++) {
		sum += fXZHits[i][xPosition];
	}
	if (sum == 0 && energy > 0) {
		fXZHits[zIndex][xPosition] = energy;
		fXZIdPos[fNSignalx] = xPosition;
		fNSignalx++;
	}
	else
	{
		fXZHits[zIndex][xPosition] = energy;
	}
}


void TRest2DHitsEvent::SetYZSignal(int zIndex, double yPosition, double energy)
{
	double sum = 0;
	for (int i = 0; i < fNz; i++) {
		sum += fYZHits[i][yPosition];
	}
	if (sum == 0 && energy > 0) {
		fYZHits[zIndex][yPosition] = energy;
		fXZIdPos[fNSignaly] = yPosition;
		fNSignaly++;
	}
	else
	{
		fYZHits[zIndex][yPosition] = energy;
	}
}

void TRest2DHitsEvent::ResetHits()
{
	Initialize();
}


map<double, double> TRest2DHitsEvent::GetXZHitsWithZ(int z) {
	return fXZHits[z];
}


map<double, double> TRest2DHitsEvent::GetYZHitsWithZ(int z) {
	return fYZHits[z];
}


map<int, double> TRest2DHitsEvent::GetXZHitsWithX(double x) {
	map<double, double>::iterator iter;
	map<int, double> result;

	for (int z = 0; z < fNz; z++) {
		result[z] = fXZHits[z][x];
	}

	return result;
}


map<int, double> TRest2DHitsEvent::GetYZHitsWithY(double y) {
	map<int, double> result;
	for (int z = 0; z < fNz; z++) {
		result[z] = fYZHits[z][y];
	}

	return result;
}


vector<double> TRest2DHitsEvent::GetXZSignal(int n) {
	vector<double> result;
	map<int, double>::iterator iter;
	auto hits = GetXZHitsWithX(GetX(n));
	iter = hits.begin();
	while (iter != hits.end()) {
		result.push_back(iter->second);
		iter++;
	}
	return result;
}


vector<double> TRest2DHitsEvent::GetYZSignal(int n) {
	vector<double> result;
	map<int, double>::iterator iter;
	auto hits = GetYZHitsWithY(GetY(n));
	iter = hits.begin();
	while (iter != hits.end()) {
		result.push_back(iter->second);
		iter++;
	}
	return result;
}


void TRest2DHitsEvent::PrintEvent(Bool_t fullInfo)
{
	TRestEvent::PrintEvent();

	cout << "Number of Signals : " << GetNumberOfSignals() << endl;
	cout << "Number of Signals XZ " << GetNumberOfXZSignals() << endl;
	cout << "Number of Signals YZ " << GetNumberOfYZSignals() << endl;

}

//Draw current event in a Tpad
TPad *TRest2DHitsEvent::DrawEvent(TString option)
{
	if (fPad != NULL) { delete fPad; fPad = NULL; }

	if (GetNumberOfSignals() == 0) {
		cout << "empty event!" << endl;
		return NULL;
	}

	fPad = new TPad(this->GetName(), " ", 0, 0, 1, 1);

	fPad->Divide(1, 2);

	fPad->SetTitle((TString)"Event ID " + ToString(this->GetID()));

	vector<double> xzx;
	vector<double> xzz;
	vector<double> xze;
	vector<double> yzy;
	vector<double> yzz;
	vector<double> yze;
	for (int i = 0; i < fNz; i++)
	{
		map<double, double>::iterator iter;

		iter = fXZHits[i].begin();
		while (iter != fXZHits[i].end()) {
			if (iter->second > 0) {
				xzx.push_back(iter->first);
				xze.push_back(iter->second);
				xzz.push_back(i);
			}
			//cout << iter->first << " : " << iter->second << endl;
			iter++;
		}

		iter = fYZHits[i].begin();
		while (iter != fYZHits[i].end()) {
			if (iter->second > 0) {
				yzy.push_back(iter->first);
				yze.push_back(iter->second);
				yzz.push_back(i);
			}
			//cout << iter->first << " : " << iter->second << endl;
			iter++;
		}
	}

	fPad->cd(1);
	if (xzz.size() > 0) {
		TGraph2D*gxz = new TGraph2D(xzz.size(), &xzz[0], &xzx[0], &xze[0]);
		gxz->SetTitle((TString)"XZ plot, " + ToString(xzz.size()) + " Points");
		gxz->GetXaxis()->SetTitle("Z");
		gxz->GetYaxis()->SetTitle("X");
		gxz->SetNpx(fNz);
		gxz->SetNpy(fNx);
		gxz->Draw("colz");
	}
	else
	{
		TGraph2D*gxz = new TGraph2D();
		gxz->SetTitle((TString)"XZ plot, " + ToString(xzz.size()) + " Points");
		gxz->GetXaxis()->SetTitle("Z");
		gxz->GetYaxis()->SetTitle("X");
		gxz->SetNpx(fNz);
		gxz->SetNpy(fNx);
		gxz->Draw("colz");
	}

	fPad->cd(2);
	if (yzz.size() > 0) {
		TGraph2D*gyz = new TGraph2D(yzz.size(), &yzz[0], &yzy[0], &yze[0]);
		gyz->SetTitle((TString)"YZ plot, " + ToString(yzz.size()) + " Points");
		gyz->GetXaxis()->SetTitle("Z");
		gyz->GetYaxis()->SetTitle("Y");
		gyz->SetNpx(fNz);
		gyz->SetNpy(fNy);
		gyz->Draw("colz");
	}
	else
	{
		TGraph2D*gyz = new TGraph2D();
		gyz->SetTitle((TString)"XZ plot, " + ToString(yzz.size()) + " Points");
		gyz->GetXaxis()->SetTitle("Z");
		gyz->GetYaxis()->SetTitle("Y");
		gyz->SetNpx(fNz);
		gyz->SetNpy(fNy);
		gyz->Draw("colz");
	}

	return fPad;
}

