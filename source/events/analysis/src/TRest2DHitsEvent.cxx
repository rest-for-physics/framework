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
#include "TRandom.h"
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
				fYZIdPos[fNSignaly] = y;
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
				fYZIdPos[fNSignaly] = y;
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
		fYZIdPos[fNSignaly] = yPosition;
		fNSignaly++;
	}
	else
	{
		fYZHits[zIndex][yPosition] = energy;
	}
}

void TRest2DHitsEvent::SetSignal(int zIndex, int signalID, double energy)
{
	if (fReadout != NULL) {
		double x = fReadout->GetX(signalID);
		double y = fReadout->GetY(signalID);
		if (TMath::IsNaN(x) || TMath::IsNaN(y)) {
			if (!TMath::IsNaN(x)) {
				SetXZSignal(zIndex, x, energy);
			}
			else if (!TMath::IsNaN(y)) {
				SetYZSignal(zIndex, y, energy);
			}
		}
		else
		{
			cout << "This cannot happen!" << endl;
		}
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

double TRest2DHitsEvent::GetSumEnergy(map<double, double> hits)
{
	map<double, double>::iterator iter;
	double result = 0;
	for (int i = 0; i < hits.size(); i++) {
		iter = hits.begin();
		int sum = 0;
		while (iter != hits.end()) {
			result += iter->second;
			iter++;
		}
	}
	return result;
}

double TRest2DHitsEvent::GetSumEnergy(map<int, double> hits)
{
	map<int, double>::iterator iter;
	double result = 0;
	for (int i = 0; i < hits.size(); i++) {
		iter = hits.begin();
		int sum = 0;
		while (iter != hits.end()) {
			result += iter->second;
			iter++;
		}
	}
	return result;
}

TVector2 TRest2DHitsEvent::GetZRange() {
	int low1 = 0, low2 = 0, up1 = fNz - 1, up2 = fNz - 1;
	map<double, double>::iterator iter;
	for (int i = 0; i < fXZHits.size(); i++) {
		if (GetSumEnergy(fXZHits[i]) > 0) {
			low1 = i;
			break;
		}
	}
	for (int i = 0; i < fYZHits.size(); i++) {
		if (GetSumEnergy(fYZHits[i]) > 0) {
			low2 = i;
			break;
		}
	}
	for (int i = fXZHits.size() - 1; i > -1; i--) {
		if (GetSumEnergy(fXZHits[i]) > 0) {
			up1 = i;
			break;
		}
	}
	for (int i = fYZHits.size() - 1; i > -1; i--) {
		if (GetSumEnergy(fYZHits[i]) > 0) {
			up2 = i;
			break;
		}
	}
	return TVector2(low1 < low2 ? low1 : low2, up1 < up2 ? up2 : up1);
}

TVector2 TRest2DHitsEvent::GetXRange() {
	double lower = 9999, upper = -9999;
	for (int i = 0; i < GetNumberOfXZSignals(); i++) {
		if (GetX(i) < lower)
			lower = GetX(i);
		if (GetX(i) > upper)
			upper = GetX(i);
	}
	return TVector2(lower, upper);
}

TVector2 TRest2DHitsEvent::GetYRange() {
	double lower = 9999, upper = -9999;
	for (int i = 0; i < GetNumberOfXZSignals(); i++) {
		if (GetY(i) < lower)
			lower = GetY(i);
		if (GetY(i) > upper)
			upper = GetY(i);
	}
	return TVector2(lower, upper);
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



	vector<double> xzx;
	vector<double> xzz;
	vector<double> xze;
	vector<double> yzy;
	vector<double> yzz;
	vector<double> yze;

	for (int i = 0; i < GetNumberOfXZSignals(); i++) {
		auto signal = GetXZSignal(i);
		auto x = GetX(i);
		for (int j = 0;j<fNz; j++) {
			xzx.push_back(x);
			xze.push_back(signal[j]);
			xzz.push_back(j);
			//if (signal[j] != 0) {
			//	txz->SetBinContent(txz->FindBin(j, x), signal[j]);
			//}
		}
	}
	for (int i = 0; i < GetNumberOfYZSignals(); i++) {
		auto signal = GetYZSignal(i);
		auto y = GetY(i);
		for (int j = 0; j<fNz; j++) {
			yzy.push_back(y);
			yze.push_back(signal[j]);
			yzz.push_back(j);
			//if (signal[j] != 0) {
			//	tyz->SetBinContent(tyz->FindBin(j, y), signal[j]);
			//}
		}
	}

	fPad = new TPad(this->GetName(), " ", 0, 0, 1, 1);
	fPad->SetTitle((TString)"Event ID " + ToString(this->GetID()));
	cout << xzz.size() << " " << yzz.size() << endl;
	if ((GetZRange().Y() - GetZRange().X()) > 0) {
		if (gxz != NULL)
		{
			delete gxz;
		}
		gxz = new TGraph2D(xzz.size(), &xzz[0], &xzx[0], &xze[0]);
		gxz->SetTitle((TString)"XZ plot, " + ToString(GetNumberOfXZSignals()) + " Signals");
		gxz->GetXaxis()->SetTitle("Z");
		gxz->GetYaxis()->SetTitle("X");
		gxz->SetNpx(fNz);
		gxz->SetNpy(100);
	}
	if ((GetZRange().Y() - GetZRange().X()) > 0) {
		if (gyz != NULL) {
			delete gyz;
		}
		gyz = new TGraph2D(yzz.size(), &yzz[0], &yzy[0], &yze[0]);
		gyz->SetTitle((TString)"YZ plot, " + ToString(GetNumberOfYZSignals()) + " Signals");
		gyz->GetXaxis()->SetTitle("Z");
		gyz->GetYaxis()->SetTitle("Y");
		gyz->SetNpx(fNz);
		gyz->SetNpy(100);
	}


	if (option == "") {
		fPad->Divide(1, 2);

		fPad->cd(1);
		gxz->Draw("colz");

		fPad->cd(2);
		gyz->Draw("colz");
	}
	if (ToUpper(option) == "ENERGYZ") 
	{
		fPad->Divide(2, 2);

		fPad->cd(1);
		gxz->Draw("colz");

		fPad->cd(2);
		gyz->Draw("colz");

		fPad->cd(3);
		gxz->Project()->Draw("colz");

		fPad->cd(4);
		gyz->Project()->Draw("colz");
	}

	return fPad;
}

