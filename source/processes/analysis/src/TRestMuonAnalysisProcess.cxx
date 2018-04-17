///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestMuonAnalysisProcess.cxx
///
///             This process adds several histograms to output root file
///_______________________________________________________________________________


#include "TRestMuonAnalysisProcess.h"
using namespace std;

ClassImp(TRestMuonAnalysisProcess)
//______________________________________________________________________________
TRestMuonAnalysisProcess::TRestMuonAnalysisProcess()
{
	Initialize();
}

//______________________________________________________________________________
TRestMuonAnalysisProcess::~TRestMuonAnalysisProcess()
{
	delete fAnaEvent;
}

//______________________________________________________________________________
void TRestMuonAnalysisProcess::Initialize()
{
	// We define the section name (by default we use the name of the class)
	SetSectionName(this->ClassName());

	// We create the input/output specific event data
	fAnaEvent = new TRest2DHitsEvent;

	// We connect the TRestEventProcess input/output event pointers
	fInputEvent = fAnaEvent;
	fOutputEvent = fAnaEvent;
}

void TRestMuonAnalysisProcess::InitProcess()
{
	nummudeposxz = 0;
	nummudeposyz = 0;
	numsmear = 0;
	mudeposxz = new TH1D("mudeposxz", "muonXZenergydepos", 512, 0., 512.);
	mudeposyz = new TH1D("mudeposyz", "muonYZenergydepos", 512, 0., 512.);

	hxzt = new TH1D((TString)"hxzt" + ToString(this), "hh", 200, 0, 3.14);
	hxzr = new TH1D((TString)"hxzr" + ToString(this), "hh", 200, -(X2 - X1), (X2 - X1));
	hyzt = new TH1D((TString)"hyzt" + ToString(this), "hh", 200, 0, 3.14);
	hyzr = new TH1D((TString)"hyzr" + ToString(this), "hh", 200, -(Y2 - Y1), (Y2 - Y1));

	mudepos = new TH1D("mudepos", "muonTotalenergydepos", 512, 0., 512.);
	musmear = new TH1D("musme", "muonSmearingStat", 512, 0., 512.);
	mutanthe = new TH1D("mutanthe", "muAngularDistribution tanTheta", 50, 0, 2.5);

}

//______________________________________________________________________________
TRestEvent* TRestMuonAnalysisProcess::ProcessEvent(TRestEvent *evInput)
{
	fAnaEvent = (TRest2DHitsEvent*)evInput;
	fOutputEvent = evInput;


	fAnalysisTree->SetObservableValue(this, "mutanthe", numeric_limits<double>::quiet_NaN());
	fAnalysisTree->SetObservableValue(this, "adjustedfirstX", numeric_limits<double>::quiet_NaN());
	fAnalysisTree->SetObservableValue(this, "adjustedfirstY", numeric_limits<double>::quiet_NaN());
	if (fAnaEvent->GetSubEventTag() == "muon") {
		if(fAnaEvent->GetHoughXZ().size()==0 && fAnaEvent->GetHoughYZ().size()==0)
			fAnaEvent->DoHough();
		vector<TVector3> fHough_XZ = fAnaEvent->GetHoughXZ();
		vector<TVector3> fHough_YZ = fAnaEvent->GetHoughYZ();


		if (fHough_XZ.size() > 20) {
			hxzt->Reset();
			hxzr->Reset();
			for (int i = 0; i < fHough_XZ.size(); i++) {
				hxzt->Fill(fHough_XZ[i].y(), fHough_XZ[i].z());
				hxzr->Fill(fHough_XZ[i].x(), fHough_XZ[i].z());
			}
		}
		if (fHough_YZ.size() > 20) {
			hyzt->Reset();
			hyzr->Reset();
			for (int i = 0; i < fHough_YZ.size(); i++) {
				hyzt->Fill(fHough_YZ[i].y(), fHough_YZ[i].z());
				hyzr->Fill(fHough_YZ[i].x(), fHough_YZ[i].z());
			}
		}

		hxzt->SetBinContent(101, 0);
		hyzt->SetBinContent(101, 0);

		TRest2DHitsEvent*eve = fAnaEvent;


		double a = eve->GetZRangeInXZ().X();
		double b = eve->GetZRangeInXZ().Y();
		double c = eve->GetZRangeInYZ().X();
		double d = eve->GetZRangeInYZ().Y();

		if ((a<c&&c<b)||(c<a&&a<d)) {

			double firstx, firsty, firstz;
			double thexz = 0, theyz = 0;
			if (fHough_XZ.size() > 20) {
				thexz = hxzt->GetBinCenter(hxzt->GetMaximumBin()) - 1.5708;
			}
			if (fHough_YZ.size() > 20) {
				theyz = hyzt->GetBinCenter(hyzt->GetMaximumBin()) - 1.5708;
			}

			if (thexz == 0) {
				double xlen = eve->GetXRange().Y() - eve->GetXRange().X();
				if (xlen > 20)
				{
					double t = (xlen) / (eve->GetZRangeInXZ().Y() - eve->GetZRangeInXZ().X());
					thexz = atan(t);
				}
				else if (eve->GetZRangeInXZ().Y() - eve->GetZRangeInXZ().X() > 100) {
					return fOutputEvent;
				}
			}

			if (theyz == 0) {
				double xlen = eve->GetXRange().Y() - eve->GetXRange().X();
				if (xlen > 20)
				{
					double t = (xlen) / (eve->GetZRangeInYZ().Y() - eve->GetZRangeInYZ().X());
					theyz = atan(t);
				}
				else if (eve->GetZRangeInYZ().Y() - eve->GetZRangeInYZ().X() > 100) {
					return fOutputEvent;
				}
			}

			info << "Track feature: ";
			//cout << thexz << " " << theyz << endl;

			firstz = eve->GetZRange().X();
			firstx = eve->GetFirstX() - tan(thexz)*(eve->GetZRangeInXZ().X() - firstz);
			firsty = eve->GetFirstY() - tan(theyz)*(eve->GetZRangeInYZ().X() - firstz);

			fAnalysisTree->SetObservableValue(this, "adjustedfirstX", firstx);
			fAnalysisTree->SetObservableValue(this, "adjustedfirstY", firsty);
			//if (firstx > X1 + 30 && firstx < X2 - 30 && firsty > Y1 + 30 && firsty < Y2 - 30)
			//if (firstx > X1 && firstx < X2 && firsty > Y1 && firsty < Y2)
			if (1)
				//the MM pentrating muon
			{
				info << "MM pentrating muon,";

				//if (eve->GetNumberOfXZSignals()>7 && xlen<15)
				//	return;
				//if (eve->GetNumberOfYZSignals()>7 && ylen<15)
				//	return;
				//if (eve->GetZRangeInXZ().X() > 450 || eve->GetZRangeInYZ().X() > 450)
				//	return;
				double tan1 = tan(abs(thexz));
				double tan2 = tan(abs(theyz));
				mutanthe->Fill(sqrt(tan1*tan1 + tan2 * tan2));
				if (GetVerboseLevel() >= REST_Debug) {
					debug << eve->GetZRangeInXZ().X() << ", " << eve->GetZRangeInXZ().Y() << "   " << eve->GetZRangeInYZ().X() << ", " << eve->GetZRangeInYZ().Y() << endl;
					debug << eve->GetID() << " " << tan1 << " " << tan2 << endl;
					getchar();
				}
				//if((firstx < X1 + 30 || firstx > X2 - 30) && (firsty < Y1 + 30 || firsty > Y2 - 30))
				//	mutanthe->Fill(sqrt(tan1*tan1 + tan2 * tan2));//at edge, fill again
				fAnalysisTree->SetObservableValue(this, "mutanthe", sqrt(tan1*tan1 + tan2 * tan2));
			}
			else
			{
				info << "Side injection muon,";
			}

			vector<int> xzwidths(512);
			vector<int> yzwidths(512);
			//XZ depos
			if (eve->GetZRangeInXZ().Y() - eve->GetZRangeInXZ().X() > 345 && thexz != 0) {
				info << " Long track XZ,";
				int shift = 150 - eve->GetZRangeInXZ().X();
				for (int i = shift; i < 512 + shift; i++) {
					auto hitsz = eve->GetXZHitsWithZ(i - shift);
					map<double, double>::iterator iter = hitsz.begin();
					double sum = 0;
					int n = 0;
					while (iter != hitsz.end()) {
						if (iter->second>0)
							n++;
						sum += iter->second;
						iter++;
					}
					if (n == 0 && i - shift > 0)
						n = xzwidths[i - shift - 1];
					xzwidths[i-shift] = n;
					//cout << i << " " << sum << endl;
					//if (sum < 1e5)
					mudeposxz->SetBinContent(mudeposxz->FindBin(i), mudeposxz->GetBinContent(mudeposxz->FindBin(i)) + sum);
				}
				nummudeposxz++;
			}

			//YZ depos
			if (eve->GetZRangeInYZ().Y() - eve->GetZRangeInYZ().X() > 345 && theyz != 0) {
				info << " Long track YZ,";
				int shift = 150 - eve->GetZRangeInYZ().X();
				for (int i = shift; i < 512 + shift; i++) {
					auto hitsz = eve->GetYZHitsWithZ(i - shift);
					map<double, double>::iterator iter = hitsz.begin();
					double sum = 0;
					int n = 0;
					while (iter != hitsz.end()) {
						if(iter->second>0)
							n++;
						sum += iter->second;
						iter++;
					}
					if (n == 0 && i - shift > 0)
						n = yzwidths[i - shift - 1];
					yzwidths[i-shift] = n;
					//cout << i << " " << sum << endl;
					//if (sum < 1e5)
					mudeposyz->SetBinContent(mudeposyz->FindBin(i), mudeposyz->GetBinContent(mudeposyz->FindBin(i)) + sum);
				}
				nummudeposyz++;
			}


			//smearing
			if (eve->GetZRangeInYZ().Y() - eve->GetZRangeInYZ().X() > 345 && theyz != 0 
				&& eve->GetZRangeInXZ().Y() - eve->GetZRangeInXZ().X() > 345 && thexz != 0
				&& abs(eve->GetZRangeInXZ().X()- eve->GetZRangeInYZ().X())<10
				&& firstx > X1 + 30 && firstx < X2 - 30 && firsty > Y1 + 30 && firsty < Y2 - 30)
			{
				int shift = 150 - firstz;
				for (int i = shift; i < 512 + shift; i++) {
					int j = i - shift;
					if (j >= 0 && j < 512) {
						double xwidth = (double)xzwidths[j] * stripWidth*cos(thexz);
						double ywidth = (double)yzwidths[j] * stripWidth*cos(theyz);

						musmear->SetBinContent(musmear->FindBin(i), musmear->GetBinContent(musmear->FindBin(i)) + xwidth + ywidth);

					}
				}
				numsmear++;
			}







			info << endl;




		}
	}

	return fOutputEvent;
}


void TRestMuonAnalysisProcess::EndProcess() 
{
	mutanthe->Write();

	if (nummudeposxz > 0)
	{
		mudeposxz->Scale(1 / (double)nummudeposxz);
		mudeposxz->SetEntries(nummudeposxz);
	}
	if (nummudeposyz > 0)
	{
		mudeposyz->Scale(1 / (double)nummudeposyz);
		mudeposyz->SetEntries(nummudeposyz);
	}
	if (numsmear > 0) {
		musmear->Scale(1 / (double)numsmear);
		musmear->SetEntries(numsmear);
	}

	mudeposxz->Write();
	mudeposyz->Write();

	mudepos->Add(mudeposxz);
	mudepos->Add(mudeposyz);
	mudepos->Write();

	musmear->Write();
}

void TRestMuonAnalysisProcess::InitFromConfigFile()
{
	TVector2 XROI = StringTo2DVector(GetParameter("XROI", "(-100,100)"));
	TVector2 YROI = StringTo2DVector(GetParameter("YROI", "(-100,100)"));

	X1 = XROI.X(), X2 = XROI.Y(), Y1 = YROI.X(), Y2 = YROI.Y();
}

