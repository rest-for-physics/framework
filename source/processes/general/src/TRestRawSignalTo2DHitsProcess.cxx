///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalTo2DHitsProcess.cxx
///
///_______________________________________________________________________________


#include "TRestRawSignalTo2DHitsProcess.h"
#include "TF2.h"
using namespace std;

ClassImp(TRestRawSignalTo2DHitsProcess)
//______________________________________________________________________________
TRestRawSignalTo2DHitsProcess::TRestRawSignalTo2DHitsProcess()
{
	Initialize();
}

//______________________________________________________________________________
TRestRawSignalTo2DHitsProcess::~TRestRawSignalTo2DHitsProcess()
{
	delete fOutput2DHitsEvent;
	delete fInputSignalEvent;
}

//______________________________________________________________________________
void TRestRawSignalTo2DHitsProcess::Initialize()
{
	// We define the section name (by default we use the name of the class)
	SetSectionName(this->ClassName());

	// We create the input/output specific event data
	fInputSignalEvent = new TRestRawSignalEvent();
	fOutput2DHitsEvent = new TRest2DHitsEvent();

	// We connect the TRestEventProcess input/output event pointers
	fInputEvent = fInputSignalEvent;
	fOutputEvent = fOutput2DHitsEvent;
}


void TRestRawSignalTo2DHitsProcess::InitProcess()
{
	fReadout = (TRestReadout*)GetReadoutMetadata();
	if (fReadout != NULL)
		fOutput2DHitsEvent->SetReadout(fReadout);
}

//______________________________________________________________________________
TRestEvent* TRestRawSignalTo2DHitsProcess::ProcessEvent(TRestEvent *evInput)
{
	fInputSignalEvent = (TRestRawSignalEvent *)evInput;

	if (fInputSignalEvent->GetNumberOfSignals() <= 0) return fOutputEvent;

	if (fReadout == NULL) {
		error << "Lack readout definition! Cannot process..." << endl;
		return NULL;
	}
	else
	{
		fOutput2DHitsEvent->Initialize();
		fOutput2DHitsEvent->SetEventInfo(fInputSignalEvent);
		fOutput2DHitsEvent->SetSubEventTag("general");

		for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++)
		{
			TRestRawSignal*s = fInputSignalEvent->GetSignal(n);
			if (s->GetMaxValue() >= 3700)fOutput2DHitsEvent->SetSubEventTag("firing");
			//s->GetIntegralWithThreshold((Int_t)fIntegralRange.X(), (Int_t)fIntegralRange.Y(), fBaseLineRange.X(), fBaseLineRange.Y(), fPointThreshold, fNPointsOverThreshold, fSignalThreshold);
			if (fNoiseReductionLevel == 0) {
				fOutput2DHitsEvent->AddSignal(s);
			}
			else if (fNoiseReductionLevel == 1)
			{
				s->SubstractBaseline(fBaseLineRange.X(), fBaseLineRange.Y());
				fOutput2DHitsEvent->AddSignal(s);
			}
			else if (fNoiseReductionLevel == 2)
			{
				TRestSignal sgn;
				sgn.SetID(s->GetID());
				double baseline = s->GetBaseLine(fBaseLineRange.X(), fBaseLineRange.Y());
				double baselinerms = s->GetBaseLineSigma(fBaseLineRange.X(), fBaseLineRange.Y());
				for (int i = fBaseLineRange.Y(); i < s->GetNumberOfPoints(); i++)
				{
					if (s->GetData(i) > baseline + fPointThreshold * baselinerms) {
						int pos = i;
						vector<double> pulse;
						pulse.push_back(s->GetData(i));
						i++;
						while (i < s->GetNumberOfPoints() && s->GetData(i) > baseline + fPointThreshold * baselinerms) {
							pulse.push_back(s->GetData(i));
							i++;
						}
						if (pulse.size() > fNPointsOverThreshold) {
							auto _e = max_element(begin(pulse), end(pulse));
							if (*_e > fSignalThreshold*baselinerms) {
								for (int j = pos; j < i; j++)
									sgn.NewPoint(j, s->GetData(j));
							}
						}
					}
				}
				//if (fOutput2DHitsEvent->GetID() == 92) {
				//	cout << s->GetID() << " " << sgn.GetNumberOfPoints() << endl;
				//}
				fOutput2DHitsEvent->AddSignal(&sgn);
			}
			else
			{
				TRestSignal sgn;
				sgn.SetID(s->GetID());
				double baseline = s->GetBaseLine(fBaseLineRange.X(), fBaseLineRange.Y());
				double baselinerms = s->GetBaseLineSigma(fBaseLineRange.X(), fBaseLineRange.Y());
				for (int i = fBaseLineRange.Y(); i < s->GetNumberOfPoints(); i++)
				{
					if (s->GetData(i) > baseline + fPointThreshold * baselinerms) {
						int pos = i;
						vector<double> pulse;
						pulse.push_back(s->GetData(i));
						i++;
						while (i < s->GetNumberOfPoints() && s->GetData(i) > baseline + fPointThreshold * baselinerms) {
							pulse.push_back(s->GetData(i));
							i++;
						}
						if (pulse.size() > fNPointsOverThreshold) {
							auto _e = max_element(begin(pulse), end(pulse));
							if (*_e > fSignalThreshold*baselinerms) {

								sgn.NewPoint(distance(std::begin(pulse), _e) + pos, *_e);
							}
						}
					}
				}
				fOutput2DHitsEvent->AddSignal(&sgn);
			}
		}
	}


	vector<double> xzz;
	vector<double> xzx;
	vector<double> xze;

	if (fOutput2DHitsEvent->GetNumberOfXZSignals() > 10) {
		double baselinemean = 0;
		double baselinermsmean = 0;
		// we first make cluster along z(same x)
		for (int i = 0; i < fOutput2DHitsEvent->GetNumberOfXZSignals(); i++)
		{
			auto s = fOutput2DHitsEvent->GetXZSignal(i);
			auto x = fOutput2DHitsEvent->GetX(i);
			double baseline = TMath::Mean(s.begin() + fBaseLineRange.X(), s.begin() + fBaseLineRange.Y());
			double baselinerms = TMath::StdDev(s.begin() + fBaseLineRange.X(), s.begin() + fBaseLineRange.Y());
			baselinemean += baseline / fOutput2DHitsEvent->GetNumberOfXZSignals();
			baselinermsmean += baselinerms / fOutput2DHitsEvent->GetNumberOfYZSignals();

			for (int j = fBaseLineRange.Y(); j < s.size(); j++)
			{
				if (s[j] > baseline + fSignalThreshold * baselinerms) {
					int pos = j;
					vector<double> pulse;
					pulse.push_back(s[j]);
					j++;
					while (j<s.size() && s[j] > baseline + fSignalThreshold * baselinerms) {
						pulse.push_back(s[j]);
						j++;
					}
					if (pulse.size() > fNPointsOverThreshold) {
						auto _e = max_element(begin(pulse), end(pulse));
						if (*_e > fSignalThreshold*baselinerms) {
							xzz.push_back((double)(distance(std::begin(pulse), _e) + pos));
							xzx.push_back(x);
							xze.push_back(*_e);
						}
					}
				}
			}
		}

		//then make cluster along x(same z)
		int a = fOutput2DHitsEvent->GetZRange().X();
		int b = fOutput2DHitsEvent->GetZRange().Y();
		for (int i = a; i <= b; i += 15) //we sample every 15 points
		{
			auto hits = fOutput2DHitsEvent->GetXZHitsWithZ(i);
			map<double, double>::iterator iter = hits.begin();
			double last_x = iter->first;
			while (iter != hits.end()) {

				if (iter->second > baselinemean + fSignalThreshold * baselinermsmean) {
					double pos = iter->first;
					double ene = iter->second;
					vector<double> pulse;
					pulse.push_back(iter->second);
					iter++;
					while (iter != hits.end() && iter->second > baselinemean + fSignalThreshold * baselinermsmean) {
						pulse.push_back(iter->second);
						if (iter->second > ene) {
							ene = iter->second;
							pos = iter->first;
						}
						iter++;
					}
					iter--;
					if (!TMath::IsNaN(pos)) {
						xzz.push_back((double)i);
						xzx.push_back(pos);
						xze.push_back(ene);
					}
				}
				iter++;
			}
		}

		for (int i = 0; i < xzz.size(); i++) {
			fOutput2DHitsEvent->AddXZCluster(xzx[i], xzz[i], xze[i]);
		}

		//tag firing event
		if (xzz.size() > 10 && (*max_element(begin(xzz), end(xzz)) - *min_element(begin(xzz), end(xzz))) / (*max_element(begin(xzx), end(xzx)) - *min_element(begin(xzx), end(xzx))) < 0.1) {
			fOutput2DHitsEvent->SetSubEventTag("firing");
		}
	}




	vector<double> yzz;
	vector<double> yzy;
	vector<double> yze;

	if (fOutput2DHitsEvent->GetNumberOfYZSignals() > 10) {
		double baselinemean = 0;
		double baselinermsmean = 0;
		// we first make cluster along z(same y)
		for (int i = 0; i < fOutput2DHitsEvent->GetNumberOfYZSignals(); i++)
		{
			auto s = fOutput2DHitsEvent->GetYZSignal(i);
			auto y = fOutput2DHitsEvent->GetY(i);
			double baseline = TMath::Mean(s.begin() + fBaseLineRange.X(), s.begin() + fBaseLineRange.Y());
			double baselinerms = TMath::StdDev(s.begin() + fBaseLineRange.X(), s.begin() + fBaseLineRange.Y());
			baselinemean += baseline / fOutput2DHitsEvent->GetNumberOfYZSignals();
			baselinermsmean += baselinerms / fOutput2DHitsEvent->GetNumberOfYZSignals();

			for (int j = fBaseLineRange.Y(); j < s.size(); j++)
			{
				if (s[j] > baseline + fSignalThreshold * baselinerms) {
					int pos = j;
					vector<double> pulse;
					pulse.push_back(s[j]);
					j++;
					while (j<s.size() && s[j] > baseline + fSignalThreshold * baselinerms) {
						pulse.push_back(s[j]);
						j++;
					}
					if (pulse.size() > fNPointsOverThreshold) {
						auto _e = max_element(begin(pulse), end(pulse));
						if (*_e > fSignalThreshold*baselinerms) {
							yzz.push_back((double)(distance(std::begin(pulse), _e) + pos));
							yzy.push_back(y);
							yze.push_back(*_e);
						}
					}
				}
			}
		}

		//then make cluster along x(same z)
		int a = fOutput2DHitsEvent->GetZRange().X();
		int b = fOutput2DHitsEvent->GetZRange().Y();
		for (int i = a; i <= b; i += 15) //we sample every 15 points
		{
			auto hits = fOutput2DHitsEvent->GetYZHitsWithZ(i);
			map<double, double>::iterator iter = hits.begin();
			double last_x = iter->first;
			while (iter != hits.end()) {

				if (iter->second > baselinemean + fSignalThreshold * baselinermsmean) {
					double pos = iter->first;
					double ene = iter->second;
					vector<double> pulse;
					pulse.push_back(iter->second);
					iter++;
					while (iter != hits.end() && iter->second > baselinemean + fSignalThreshold * baselinermsmean) {
						pulse.push_back(iter->second);
						if (iter->second > ene) {
							ene = iter->second;
							pos = iter->first;
						}
						iter++;
					}
					iter--;
					if (!TMath::IsNaN(pos)) {
						yzz.push_back((double)i);
						yzy.push_back(pos);
						yze.push_back(ene);
					}
				}
				iter++;
			}
		}

		for (int i = 0; i < yzz.size(); i++) {
			fOutput2DHitsEvent->AddYZCluster(yzy[i], yzz[i], yze[i]);
		}

		//tag firing event
		if (yzz.size() > 10 && (*max_element(begin(yzz), end(yzz)) - *min_element(begin(yzz), end(yzz))) / (*max_element(begin(yzy), end(yzy)) - *min_element(begin(yzy), end(yzy))) < 0.1) {
			fOutput2DHitsEvent->SetSubEventTag("firing");
		}

	}


	//cout << fOutput2DHitsEvent->GetID() << " " << xzz.size() <<" "<<yzz.size()<< endl;


	if (Count(fSelection, "0") == 0) {

		if (fOutput2DHitsEvent->GetSubEventTag() == "general") //1. firing should have been tagged
		{
			if (fOutput2DHitsEvent->GetNumberOfSignals() < 12) //2. tag low count event from source
			{
				fOutput2DHitsEvent->SetSubEventTag("weak");
			}
		}

		fHough_XZ.clear();
		fHough_YZ.clear();
		if (fOutput2DHitsEvent->GetSubEventTag() == "general")
		{
			fOutput2DHitsEvent->DoHough();
			fHough_XZ = fOutput2DHitsEvent->GetHoughXZ();
			fHough_YZ = fOutput2DHitsEvent->GetHoughYZ();
		}

		if (fOutput2DHitsEvent->GetSubEventTag() == "general"&&fOutput2DHitsEvent->GetNumberOfXZSignals() > 10 && fHough_XZ.size() > 10)//3. tag muon event
		{


			TH1D*xz = new TH1D((TString)"hh" + ToString(this), "hh", 120, 0, 3.14);
			TF1*f = new TF1("aa", "gaus");
			for (int i = 0; i < fHough_XZ.size(); i++) {
				xz->Fill(fHough_XZ[i].y(), fHough_XZ[i].z());
			}
			if (xz->GetMaximumBin() == 1) {
				fOutput2DHitsEvent->SetSubEventTag("firing");
			}
			else
			{
				f->SetParameter(1, xz->GetBinCenter(xz->GetMaximumBin()));

				if (fVerboseLevel >= REST_Debug) {
					debug << fOutput2DHitsEvent->GetID() << endl;
					xz->Fit(f);
				}
				else
				{
					xz->Fit(f, "Q");
				}

				if (f->GetParameter(2) < 0.1)
				{
					fOutput2DHitsEvent->SetSubEventTag("muon");
				}
			}


			delete xz;
			delete f;
		}
		if (fOutput2DHitsEvent->GetNumberOfYZSignals() > 10 && fHough_YZ.size() > 10 && fOutput2DHitsEvent->GetSubEventTag() == "general")//3. tag muon event
		{
			TH1D*yz = new TH1D((TString)"hh" + ToString(this), "hh", 120, 0, 3.14);
			TF1*f = new TF1("aa", "gaus");
			for (int i = 0; i < fHough_YZ.size(); i++) {
				yz->Fill(fHough_YZ[i].y(), fHough_YZ[i].z());
			}
			if (yz->GetMaximumBin() == 1) {
				fOutput2DHitsEvent->SetSubEventTag("firing");
			}
			else
			{
				f->SetParameter(1, yz->GetBinCenter(yz->GetMaximumBin()));
				if (fVerboseLevel >= REST_Debug) {
					debug << fOutput2DHitsEvent->GetID() << endl;
					yz->Fit(f);
				}
				else
				{
					yz->Fit(f, "Q");
				}

				if (f->GetParameter(2) < 0.1)
				{
					fOutput2DHitsEvent->SetSubEventTag("muon");
				}
			}
			delete yz;
			delete f;
		}

		if (fOutput2DHitsEvent->GetSubEventTag() == "general")//4. tag electron event
		{

		}


		if (Count(fSelection, "4") != 0)
		{
			if (fOutput2DHitsEvent->GetSubEventTag() == "firing") {
				info << "selecting firing event, id " << fOutput2DHitsEvent->GetID() << endl;
				return fOutput2DHitsEvent;
			}
		}
		if (Count(fSelection, "3") != 0)
		{
			if (fOutput2DHitsEvent->GetSubEventTag() == "weak") {
				info << "selecting event from calibration source, id " << fOutput2DHitsEvent->GetID() << endl;
				return fOutput2DHitsEvent;
			}
		}
		if (Count(fSelection, "1") != 0) {

			if (fOutput2DHitsEvent->GetSubEventTag() == "muon") {
				info << "selecting muon event, id " << fOutput2DHitsEvent->GetID() << endl;
				return fOutput2DHitsEvent;
			}
		}
		if (Count(fSelection, "2") != 0) {
			if (fOutput2DHitsEvent->GetSubEventTag() == "electron") {
				info << "selecting electron event, id " << fOutput2DHitsEvent->GetID() << endl;
				return fOutput2DHitsEvent;
			}
		}
		if (Count(fSelection, "5") != 0) {
			if (fOutput2DHitsEvent->GetSubEventTag() == "general") {
				info << "selecting unknown event, id " << fOutput2DHitsEvent->GetID() << endl;
				return fOutput2DHitsEvent;
			}
		}


		return NULL;
	}

	return fOutput2DHitsEvent;
}

void TRestRawSignalTo2DHitsProcess::InitFromConfigFile()
{
	fNoiseReductionLevel = StringToInteger(GetParameter("noiseReduction", "0"));
	fSelection = GetParameter("selection", "0");

	fBaseLineRange = StringTo2DVector(GetParameter("baseLineRange", "(5,55)"));
	fIntegralRange = StringTo2DVector(GetParameter("integralRange", "(10,500)"));
	fPointThreshold = StringToDouble(GetParameter("pointThreshold", "2"));
	fNPointsOverThreshold = StringToInteger(GetParameter("pointsOverThreshold", "5"));
	fSignalThreshold = StringToDouble(GetParameter("signalThreshold", "2.5"));
}

