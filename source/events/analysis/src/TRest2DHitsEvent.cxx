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
#include "TStyle.h"
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
	xzz.clear(); xzx.clear(); xze.clear();
	yzz.clear(); yzy.clear(); yze.clear();
	fHough_XZ.clear(); fHough_YZ.clear();
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


void TRest2DHitsEvent::DoHough() 
{
	if (GetNumberOfXZSignals() > 10) {
		for (int i = 0; i < xzz.size(); i++) {
			for (int j = i + 1; j < xzz.size(); j++) {
				double x1 = xzz[i]; double x2 = xzz[j];
				double y1 = xzx[i]; double y2 = xzx[j];
				double weight = log10(xze[i] + xze[j]);

				if (y1 == y2 && TMath::Abs(x1 - x2) < 40)continue;
				if (x1 == x2)continue;

				double a = (y2 - y1) / (x2 - x1);
				double b = (y1*x2 - y2 * x1) / (x2 - x1);

				double r = TMath::Abs(-b / sqrt(a*a + 1));
				double t = -TMath::ACos(a / sqrt(a*a + 1)) + TMath::Pi();

				fHough_XZ.push_back(TVector3(r, t, weight));
			}
		}
	}

	if (GetNumberOfYZSignals() > 10) {
		for (int i = 0; i < yzz.size(); i++) {
			for (int j = i + 1; j < yzz.size(); j++) {
				double x1 = yzz[i]; double x2 = yzz[j];
				double y1 = yzy[i]; double y2 = yzy[j];
				double weight = log10(yze[i] + yze[j]);

				if (y1 == y2 && TMath::Abs(x1 - x2) < 40)continue;
				if (x1 == x2)continue;

				double a = (y2 - y1) / (x2 - x1);
				double b = (y1*x2 - y2 * x1) / (x2 - x1);

				double r = TMath::Abs(-b / sqrt(a*a + 1));
				double t = -TMath::ACos(a / sqrt(a*a + 1)) + TMath::Pi();

				fHough_YZ.push_back(TVector3(r, t, weight));
			}
		}
	}
}

//Draw current event in a Tpad
TPad *TRest2DHitsEvent::DrawEvent(TString option)
{
	if (fPad != NULL) { delete fPad; fPad = NULL; }

	if (GetNumberOfSignals() == 0) {
		cout << "empty event!" << endl;
		return NULL;
	}



	vector<double> _xzx;
	vector<double> _xzz;
	vector<double> _xze;
	vector<double> _yzy;
	vector<double> _yzz;
	vector<double> _yze;

	for (int i = 0; i < GetNumberOfXZSignals(); i++) {
		auto signal = GetXZSignal(i);
		auto x = GetX(i);
		for (int j = 0;j<fNz; j++) {
			_xzx.push_back(x);
			_xze.push_back(signal[j]);
			_xzz.push_back(j);
			//if (signal[j] != 0) {
			//	txz->SetBinContent(txz->FindBin(j, x), signal[j]);
			//}
		}
	}
	for (int i = 0; i < GetNumberOfYZSignals(); i++) {
		auto signal = GetYZSignal(i);
		auto y = GetY(i);
		for (int j = 0; j<fNz; j++) {
			_yzy.push_back(y);
			_yze.push_back(signal[j]);
			_yzz.push_back(j);
			//if (signal[j] != 0) {
			//	tyz->SetBinContent(tyz->FindBin(j, y), signal[j]);
			//}
		}
	}

	cout << _xzz.size() << " " << _yzz.size() << endl;

	double max = _xzx.size() > 0 ? *max_element(begin(_xzx), end(_xzx)) + 3 : 3;
	double min = _xzx.size() > 0 ? *min_element(begin(_xzx), end(_xzx)) - 3 : -3;
	for (int j = 0; j<fNz; j++) {

		_xzx.push_back(max);
		_xze.push_back(0);
		_xzz.push_back(j);
		_xzx.push_back(min);
		_xze.push_back(0);
		_xzz.push_back(j);
	}

	max = _yzy.size() > 0 ? *max_element(begin(_yzy), end(_yzy)) + 3 : 3;
	min = _yzy.size() > 0 ? *min_element(begin(_yzy), end(_yzy)) - 3 : -3;
	for (int j = 0; j<fNz; j++) {

		_yzy.push_back(max);
		_yze.push_back(0);
		_yzz.push_back(j);
		_yzy.push_back(min);
		_yze.push_back(0);
		_yzz.push_back(j);
	}

	fPad = new TPad(this->GetName(), " ", 0, 0, 1, 1);
	fPad->SetTitle((TString)"Event ID " + ToString(this->GetID()));
	if (gxz != NULL)
	{
		delete gxz;
		gxz = NULL;
	}
	if (gyz != NULL) {
		delete gyz;
		gyz = NULL;
	}
	if (pointxz != NULL) {
		delete pointxz;
		pointxz = NULL;
	}
	if (pointyz != NULL) {
		delete pointyz;
		pointyz = NULL;
	}



	if ((GetZRange().Y() - GetZRange().X()) > 0 ) {
		gxz = new TGraph2D(_xzz.size(), &_xzz[0], &_xzx[0], &_xze[0]);
		if (xzz.size() > 0) {
			pointxz = new TH2D("hxz", "hxz", 100, gxz->GetXmin(), gxz->GetXmax(), 100, gxz->GetYmin(), gxz->GetYmax());
			for (int i = 0; i < xzz.size(); i++) {
				pointxz->Fill(xzz[i], xzx[i]);
			}
			pointxz->SetLineColor(kRed);
			pointxz->SetFillColor(kRed);
			pointxz->SetMarkerColor(kRed);
		}
		gxz->SetTitle((TString)"XZ plot, " + ToString(GetNumberOfXZSignals()) + " Signals");
		gxz->GetXaxis()->SetTitle("Z");
		gxz->GetYaxis()->SetTitle("X");
		{
			gxz->SetPoint(_xzz.size(), 0, -100, 0);
			gxz->SetPoint(_xzz.size() + 1, 512, 100, 0);
		}
		gxz->SetNpx(500);
		gxz->SetNpy(100);
	}
	if ((GetZRange().Y() - GetZRange().X()) > 0 ) {
		gyz = new TGraph2D(_yzz.size(), &_yzz[0], &_yzy[0], &_yze[0]);
		if (yzz.size() > 0) {
			pointyz = new TH2D("hxz", "hxz", 100, gyz->GetXmin(), gyz->GetXmax(), 100, gyz->GetYmin(), gyz->GetYmax());
			for (int i = 0; i < yzz.size(); i++) {
				pointyz->Fill(yzz[i], yzy[i]);
				cout << yzz[i] << " " << yzy[i] << endl;
			}
			pointyz->SetLineColor(kRed);
			pointyz->SetFillColor(kRed);
			pointyz->SetMarkerColor(kRed);
		}
		gyz->SetTitle((TString)"YZ plot, " + ToString(GetNumberOfYZSignals()) + " Signals");
		gyz->GetXaxis()->SetTitle("Z");
		gyz->GetYaxis()->SetTitle("Y");
		{
			gyz->SetPoint(_yzz.size(), 0, 100, 0);
			gyz->SetPoint(_yzz.size() + 1, 512, 300, 0);
		}
		gyz->SetNpx(500);
		gyz->SetNpy(100);
	}


	if (option == "") {
		fPad->Divide(1, 2);

		fPad->cd(1);
		if (gxz != NULL)
			gxz->Draw("colz");

		if (pointxz != NULL)
			pointxz->Draw("boxsame");

		fPad->cd(2);
		if (gyz != NULL)
			gyz->Draw("colz");

		if (pointyz != NULL)
			pointyz->Draw("boxsame");
	}
	else if (ToUpper(option) == "ENERGYZ")
	{
		fPad->Divide(2, 2);

		fPad->cd(1);
		if (gxz != NULL)
			gxz->Draw("colz");

		if (pointxz != NULL)
			pointxz->Draw("boxsame");

		fPad->cd(2);
		if (gyz != NULL)
			gyz->Draw("colz");

		if (pointyz != NULL)
			pointyz->Draw("boxsame");

		fPad->cd(3);
		if (gxz != NULL)
			gxz->Project()->Draw("colz");

		fPad->cd(4);
		if (gyz != NULL)
			gyz->Project()->Draw("colz");
	}

	return fPad;
}

