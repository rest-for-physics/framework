///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestMuonAnalysisProcess.cxx
///
///             This process adds several observables and histograms to output
///             root file, if input event has tag "muon"
///_______________________________________________________________________________

#include "TRestMuonAnalysisProcess.h"
#include <numeric>
#include "TRandom.h"
using namespace std;

ClassImp(TRestMuonAnalysisProcess)
    //______________________________________________________________________________
    TRestMuonAnalysisProcess::TRestMuonAnalysisProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestMuonAnalysisProcess::~TRestMuonAnalysisProcess() {}

//______________________________________________________________________________
void TRestMuonAnalysisProcess::Initialize() {
    // We define the section name (by default we use the name of the class)
    SetSectionName(this->ClassName());

    // We create the input/output specific event data
    fAnaEvent = NULL;
}

void TRestMuonAnalysisProcess::InitProcess() {
    nummudeposxz = 0;
    nummudeposyz = 0;
    // numsmearxy = 0;
    // numsmearz = 0;
    hxzt = new TH1D((TString) "hxzt" + ToString(this), "hh", 200, 0, 3.14);
    hxzr = new TH1D((TString) "hxzr" + ToString(this), "hh", 200, -(X2 - X1), (X2 - X1));
    fxz = new TF1((TString) "fxz" + ToString(this), "gaus");
    hyzt = new TH1D((TString) "hyzt" + ToString(this), "hh", 200, 0, 3.14);
    hyzr = new TH1D((TString) "hyzr" + ToString(this), "hh", 200, -(Y2 - Y1), (Y2 - Y1));
    fyz = new TF1((TString) "fyz" + ToString(this), "gaus");

    mudeposxz = new TH1D("mudeposxz", "muonXZenergydepos", 512, 0., 512.);
    mudeposyz = new TH1D("mudeposyz", "muonYZenergydepos", 512, 0., 512.);

    hdiffz = new TH1D((TString) "hdiffz" + ToString(this), "hzxdiffz", 20, 0, 20);
    fdiffz = new TF1((TString) "fdiffz" + ToString(this), "gaus");

    mudepos = new TH1D("mudepos", "muonTotalenergydepos", 512, 0., 512.);
    mutanthe = new TH1D("mutanthe", "muAngularDistribution tanTheta", 50, 0, 2.5);

    muhitmap =
        new TH2D("muhitmap", "muon hitmap on micromegas", (X2 - X1) / 3, X1, X2, (Y2 - Y1) / 3, Y1, Y2);
    muhitdir = new TH2D("muhitdir", "muon hit direction towards center", (X2 - X1) / 3, X1, X2, (Y2 - Y1) / 3,
                        Y1, Y2);
}

//______________________________________________________________________________
TRestEvent* TRestMuonAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fAnaEvent = (TRest2DHitsEvent*)evInput;

    fAnaEvent->SetROIX(TVector2(X1, X2));
    fAnaEvent->SetROIY(TVector2(Y1, Y2));

    this->SetObservableValue("muphi", numeric_limits<double>::quiet_NaN());
    this->SetObservableValue("mutanthe", numeric_limits<double>::quiet_NaN());
    this->SetObservableValue("muproj", numeric_limits<double>::quiet_NaN());
    this->SetObservableValue("rawtantheXZ", numeric_limits<double>::quiet_NaN());
    this->SetObservableValue("rawtantheYZ", numeric_limits<double>::quiet_NaN());
    this->SetObservableValue("tantheXZ", numeric_limits<double>::quiet_NaN());
    this->SetObservableValue("tantheYZ", numeric_limits<double>::quiet_NaN());
    this->SetObservableValue("evefirstX", numeric_limits<double>::quiet_NaN());
    this->SetObservableValue("evefirstY", numeric_limits<double>::quiet_NaN());
    this->SetObservableValue("adjustedfirstX", numeric_limits<double>::quiet_NaN());
    this->SetObservableValue("adjustedfirstY", numeric_limits<double>::quiet_NaN());
    this->SetObservableValue("MMpenet", 0);
    this->SetObservableValue("longTrackX", 0);
    this->SetObservableValue("longTrackY", 0);

    MakeTag();

    this->SetObservableValue("tag", (string)fAnaEvent->GetSubEventTag());

    if (fAnaEvent->GetSubEventTag() == "muon") {
        if (fAnaEvent->GetHoughXZ().size() == 0 && fAnaEvent->GetHoughYZ().size() == 0) fAnaEvent->DoHough();
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

        TRest2DHitsEvent* eve = fAnaEvent;

        double a = eve->GetZRangeInXZ().X();
        double b = eve->GetZRangeInXZ().Y();
        double c = eve->GetZRangeInYZ().X();
        double d = eve->GetZRangeInYZ().Y();

        if ((a <= c && c <= b) || (c <= a && a <= d)) {
            double firstx, firsty, firstz;
            double thexz = 0, theyz = 0;
            if (fHough_XZ.size() > 20) {
                thexz = hxzt->GetBinCenter(hxzt->GetMaximumBin()) - 1.5708;
            }
            if (fHough_YZ.size() > 20) {
                theyz = hyzt->GetBinCenter(hyzt->GetMaximumBin()) - 1.5708;
            }

            // writing raw values(before adjustment)
            this->SetObservableValue("rawtantheXZ", tan(thexz));
            this->SetObservableValue("rawtantheYZ", tan(theyz));
            this->SetObservableValue("rawmutanthe", sqrt(tan(thexz) * tan(thexz) + tan(theyz) * tan(theyz)));
            double muphi = 0;
            if (tan(thexz) >= 0) {
                muphi = atan(tan(theyz) / tan(thexz));
            } else if (tan(theyz) > 0) {
                muphi = 3.1416 + atan(tan(theyz) / tan(thexz));
            } else {
                muphi = atan(tan(theyz) / tan(thexz)) - 3.1416;
            }
            this->SetObservableValue("rawmuphi", muphi);
            this->SetObservableValue("evefirstX", eve->GetFirstX());
            this->SetObservableValue("evefirstY", eve->GetFirstY());

            // adjusting thetas in the plot(when hough is not available)
            if (thexz == 0) {
                double xlen1 = eve->GetLastX() - eve->GetFirstX();
                // double xlen1 = eve->GetXRange().Y() - eve->GetXRange().X();
                if (xlen1 == 0) xlen1 = (rand() % 2 > 0) ? 1 : -1;
                double xlen2 = eve->GetNumberOfXZSignals() * stripWidth * (xlen1 > 0 ? 1 : -1);
                double zlen1 = eve->GetZRangeInXZ().Y() - eve->GetZRangeInXZ().X();
                double zlen2 = eve->GetZRange().Y() - eve->GetZRange().X();

                double xlen = abs(xlen1) > abs(xlen2) ? xlen1 : xlen2;
                double zlen = zlen1 > 80 ? zlen1 : zlen2;
                if (abs(xlen) > 3) {
                    double t = xlen / zlen;
                    thexz = atan(t);
                } else if (zlen < 80) {
                    return fAnaEvent;
                }
            }

            if (theyz == 0) {
                double ylen1 = eve->GetLastY() - eve->GetFirstY();
                // double ylen1 = eve->GetYRange().Y() - eve->GetYRange().X();
                if (ylen1 == 0) ylen1 = (rand() % 2 > 0) ? 1 : -1;
                double ylen2 = eve->GetNumberOfYZSignals() * stripWidth * (ylen1 > 0 ? 1 : -1);
                double zlen1 = eve->GetZRangeInYZ().Y() - eve->GetZRangeInYZ().X();
                double zlen2 = eve->GetZRange().Y() - eve->GetZRange().X();

                double ylen = abs(ylen1) > abs(ylen2) ? ylen1 : ylen2;
                double zlen = zlen1 > 80 ? zlen1 : zlen2;
                if (abs(ylen) > 3) {
                    double t = ylen / zlen;
                    theyz = atan(t);
                } else if (zlen < 80) {
                    return fAnaEvent;
                }
            }

            // adjusting firstx and firsty
            firstz = eve->GetZRange().X();
            firstx = eve->GetFirstX() - tan(thexz) * (eve->GetZRangeInXZ().X() - firstz);
            firsty = eve->GetFirstY() - tan(theyz) * (eve->GetZRangeInYZ().X() - firstz);

            // writing adjusted values
            this->SetObservableValue("tantheXZ", tan(thexz));
            this->SetObservableValue("tantheYZ", tan(theyz));
            this->SetObservableValue("adjustedfirstX", firstx);
            this->SetObservableValue("adjustedfirstY", firsty);
            this->SetObservableValue("mutanthe", sqrt(tan(thexz) * tan(thexz) + tan(theyz) * tan(theyz)));
            muphi = 0;
            if (tan(thexz) >= 0) {
                muphi = atan(tan(theyz) / tan(thexz));
            } else if (tan(theyz) > 0) {
                muphi = 3.1416 + atan(tan(theyz) / tan(thexz));
            } else {
                muphi = atan(tan(theyz) / tan(thexz)) - 3.1416;
            }
            this->SetObservableValue("muphi", muphi);
            muhitmap->Fill(firstx, firsty);

            // calculate projection
            int bin = muhitdir->FindBin(firstx, firsty);
            double p = ProjectionToCenter(firstx, firsty, thexz, theyz);
            muhitdir->SetBinContent(bin, muhitdir->GetBinContent(bin) + p);
            this->SetObservableValue("muproj", p);

            // the MM pentrating muon
            if (firstx > X1 + 30 && firstx < X2 - 30 && firsty > Y1 + 30 && firsty < Y2 - 30) {
                info << "MM penetration muon,";
                this->SetObservableValue("MMpenet", 1);
                // if (eve->GetNumberOfXZSignals()>7 && xlen<15)
                //	return;
                // if (eve->GetNumberOfYZSignals()>7 && ylen<15)
                //	return;
                // if (eve->GetZRangeInXZ().X() > 450 || eve->GetZRangeInYZ().X() > 450)
                //	return;
                double tan1 = tan(abs(thexz));
                double tan2 = tan(abs(theyz));
                mutanthe->Fill(sqrt(tan1 * tan1 + tan2 * tan2));
                if (GetVerboseLevel() >= REST_Debug) {
                    debug << eve->GetZRangeInXZ().X() << ", " << eve->GetZRangeInXZ().Y() << "   "
                          << eve->GetZRangeInYZ().X() << ", " << eve->GetZRangeInYZ().Y() << endl;
                    debug << eve->GetID() << " " << tan1 << " " << tan2 << endl;
                    // getchar();
                }
                // if((firstx < X1 + 30 || firstx > X2 - 30) && (firsty < Y1 + 30 ||
                // firsty > Y2 - 30)) 	mutanthe->Fill(sqrt(tan1*tan1 + tan2 *
                // tan2));//at
                // edge, fill again
            } else {
                info << "Side injection muon,";
            }

            // vector<int> xzwidths(512);
            // vector<int> yzwidths(512);
            // vector<int> xzFWHMs(512);
            // vector<int> yzFWHMs(512);

            vector<int> xzdepos(512);
            bool xzflag = false;
            vector<int> yzdepos(512);
            bool yzflag = false;
            // XZ depos
            if (eve->GetZRangeInXZ().Y() - eve->GetZRangeInXZ().X() > 345 && thexz != 0 &&
                tan(thexz) * 350 < (X2 - X1)) {
                info << " Long track XZ,";
                this->SetObservableValue("longTrackX", 1);
                xzflag = true;
                for (int i = 0; i < 512; i++) {
                    // cout << i << endl;
                    auto hitsz = eve->GetXZHitsWithZ(i);
                    double sumall = 0;
                    map<double, double>::iterator iter = hitsz.begin();
                    vector<vector<double>> energys;
                    vector<double> energy;
                    while (iter != hitsz.end()) {
                        sumall += iter->second;
                        if (iter->second > 0) {
                            energy.push_back(iter->second);
                        } else {
                            if (energy.size() > 0) {
                                energys.push_back(energy);
                                energy.clear();
                            }
                        }
                        iter++;
                        if (iter == hitsz.end()) {
                            energys.push_back(energy);
                            energy.clear();
                        }
                    }
                    double sum = 0;

                    double width = 0;
                    int n = 0;
                    int pos = 0;
                    for (int j = 0; j < energys.size(); j++) {
                        if (energys[j].size() > n)  // pulse with maximun signal number
                        {
                            pos = j;
                            n = energys[j].size();
                        }
                    }
                    if (energys.size() > 0) {
                        for (int j = 0; j < energys[pos].size(); j++) {
                            // cout << energys[pos][j] << " ";
                            sum += energys[pos][j];
                        }
                    }
                    xzdepos[i] = sum;
                }
            }

            // YZ depos
            if (eve->GetZRangeInYZ().Y() - eve->GetZRangeInYZ().X() > 345 && theyz != 0 &&
                tan(theyz) * 350 < (Y2 - Y1)) {
                info << " Long track YZ,";
                this->SetObservableValue("longTrackY", 1);
                yzflag = true;
                for (int i = 0; i < 512; i++) {
                    auto hitsz = eve->GetYZHitsWithZ(i);
                    double sumall = 0;
                    map<double, double>::iterator iter = hitsz.begin();
                    vector<vector<double>> energys;
                    vector<double> energy;
                    while (iter != hitsz.end()) {
                        sumall += iter->second;
                        if (iter->second > 0) {
                            energy.push_back(iter->second);
                        } else {
                            if (energy.size() > 0) {
                                energys.push_back(energy);
                                energy.clear();
                            }
                        }
                        iter++;
                        if (iter == hitsz.end()) {
                            energys.push_back(energy);
                            energy.clear();
                        }
                    }

                    double sum = 0;

                    double width = 0;
                    int n = 0;
                    int pos = 0;
                    for (int j = 0; j < energys.size(); j++) {
                        if (energys[j].size() > n)  // pulse with maximun signal number
                        {
                            pos = j;
                            n = energys[j].size();
                        }
                    }
                    if (energys.size() > 0) {
                        for (int j = 0; j < energys[pos].size(); j++) {
                            sum += energys[pos][j];
                        }
                    }
                    // if (n == 0 && i > 0) {
                    //	n = yzwidths[i - 1];
                    //}
                    // yzwidths[i] = n;
                    yzdepos[i] = sum;
                }
            }

            if (xzflag) {
                int shift = 150 - eve->GetZRangeInXZ().X();
                for (int i = shift; i < 512 + shift; i++) {
                    mudeposxz->SetBinContent(
                        mudeposxz->FindBin(i),
                        mudeposxz->GetBinContent(mudeposxz->FindBin(i)) + xzdepos[i - shift]);
                }
                nummudeposxz++;
            }
            if (yzflag) {
                int shift = 150 - eve->GetZRangeInYZ().X();
                for (int i = shift; i < 512 + shift; i++) {
                    mudeposyz->SetBinContent(
                        mudeposyz->FindBin(i),
                        mudeposyz->GetBinContent(mudeposyz->FindBin(i)) + yzdepos[i - shift]);
                }
                nummudeposyz++;
            }

            // diffusion
            musmearxy.clear();
            musmearz.clear();
            hdiffz->Reset();
            if (xzflag && firstx > X1 + 30 && firstx < X2 - 30 && firsty > Y1 + 30 && firsty < Y2 - 30) {
                map<int, pair<double, double>> musmearz0;
                double n = 0;
                double sum = 0;
                for (int i = 0; i < eve->GetNumberOfXZSignals(); i++) {
                    double x = eve->GetX(i);
                    auto sgn = eve->GetXZSignal(i);
                    double z = max_element(sgn.begin(), sgn.end()) - sgn.begin();
                    if (DistanceToTrack(x, z, firstx, 150, thexz) < 20 && z > 10 && z < 500) {
                        for (int j = 0; j < 10; j++) {
                            hdiffz->SetBinContent(10 - j, sgn[z - j]);
                            hdiffz->SetBinContent(10 + j, sgn[z + j]);
                        }
                        int fitstatus;
                        if (GetVerboseLevel() >= REST_Debug) {
                            hdiffz->SetTitle((TString) "Z index: " + ToString(z));
                            hdiffz->Draw();

                            fitstatus = hdiffz->Fit(fdiffz, "");
                            GetChar();
                        } else {
                            fitstatus = hdiffz->Fit(fdiffz, "QN");
                        }

                        if (fitstatus == 0) {
                            double sigma = fdiffz->GetParameter(2);
                            double chi2 = fdiffz->GetChisquare();
                            musmearz0[z].first = sigma;
                            musmearz0[z].second = chi2;
                            n++;
                            sum += sigma;
                        }

                        // if (tan(thexz) > 0.15)
                        musmearxy[z].push_back(sgn[z] / (xzdepos[z] + yzdepos[z]));
                    }
                }

                auto iter = musmearz0.begin();
                while (iter != musmearz0.end()) {
                    musmearz[iter->first].first = iter->second.first - sum / n;
                    musmearz[iter->first].second = iter->second.second;
                    iter++;
                }
            }

            if (yzflag && firstx > X1 + 30 && firstx < X2 - 30 && firsty > Y1 + 30 && firsty < Y2 - 30) {
                map<int, pair<double, double>> musmearz0;
                double n = 0;
                double sum = 0;
                for (int i = 0; i < eve->GetNumberOfYZSignals(); i++) {
                    double y = eve->GetY(i);
                    auto sgn = eve->GetYZSignal(i);
                    double z = max_element(sgn.begin(), sgn.end()) - sgn.begin();
                    if (DistanceToTrack(y, z, firsty, 150, theyz) < 20 && z > 10 && z < 500) {
                        for (int j = 0; j < 10; j++) {
                            hdiffz->SetBinContent(10 - j, sgn[z - j]);
                            hdiffz->SetBinContent(10 + j, sgn[z + j]);
                        }
                        int fitstatus = hdiffz->Fit(fdiffz, "QN");
                        if (fitstatus == 0) {
                            double sigma = fdiffz->GetParameter(2);
                            double chi2 = fdiffz->GetChisquare();
                            musmearz0[z].first = sigma;
                            musmearz0[z].second = chi2;
                            n++;
                            sum += sigma;
                        }

                        // if (tan(theyz) > 0.15)
                        musmearxy[z].push_back(sgn[z] / (xzdepos[z] + yzdepos[z]));
                    }
                }

                auto iter = musmearz0.begin();
                while (iter != musmearz0.end()) {
                    musmearz[iter->first].first = iter->second.first - sum / n;
                    musmearz[iter->first].second = iter->second.second;
                    iter++;
                }
            }
            info << " id : " << fAnaEvent->GetID();
            info << endl;
        }
    }

    return fAnaEvent;
}

TRest2DHitsEvent* TRestMuonAnalysisProcess::MakeTag() {
    double zlen = fAnaEvent->GetZRange().Y() - fAnaEvent->GetZRange().X();
    double xlen = fAnaEvent->GetXRange().Y() - fAnaEvent->GetXRange().X();
    double ylen = fAnaEvent->GetYRange().Y() - fAnaEvent->GetYRange().X();

    if (fAnaEvent->GetSubEventTag() == "general")  // if no tags
    {
        if (fAnaEvent->GetNumberOfSignals() < 6 && zlen < 50)
        // 1. tag low count event from source
        {
            fAnaEvent->SetSubEventTag("weak");
        }
        if (zlen < 50 && xlen + ylen < 30)
        // 1. tag low count event from source
        {
            fAnaEvent->SetSubEventTag("weak");
        }
        if (TMath::IsNaN(zlen) || fAnaEvent->GetZRange().X() > 200)
        // 2. tag abnormal event from source
        {
            fAnaEvent->SetSubEventTag("abnormal");
        }
    }

    fxz->SetParameter(1, -1);
    fyz->SetParameter(1, -1);
    fHough_XZ.clear();
    fHough_YZ.clear();
    if (fAnaEvent->GetSubEventTag() == "general") {
        // MakeCluster();
        fAnaEvent->DoHough();
        fHough_XZ = fAnaEvent->GetHoughXZ();
        fHough_YZ = fAnaEvent->GetHoughYZ();
    }
    // 3. fit hough and tag muon/electron event
    if (fAnaEvent->GetSubEventTag() == "general") {
        if (fHough_XZ.size() > 100) {
            hxzt->Reset();
            hxzr->Reset();
            for (int i = 0; i < fHough_XZ.size(); i++) {
                hxzt->Fill(fHough_XZ[i].y(), fHough_XZ[i].z());
                hxzr->Fill(fHough_XZ[i].x(), fHough_XZ[i].z());
            }

            if (hxzt->GetMaximumBin() == 1) {
                fAnaEvent->SetSubEventTag("sparking");
            }

            fxz->SetParameter(1, hxzt->GetBinCenter(hxzt->GetMaximumBin()));
            int fitStatus;
            if (fVerboseLevel >= REST_Debug) {
                debug << "ID: " << fAnaEvent->GetID() << ", hxzt points: " << fHough_XZ.size()
                      << ", center: " << hxzt->GetBinCenter(hxzt->GetMaximumBin()) << endl;
                fitStatus = hxzt->Fit(fxz, "N", "", hxzt->GetBinCenter(hxzt->GetMaximumBin()) - 0.2,
                                      hxzt->GetBinCenter(hxzt->GetMaximumBin()) + 0.2);
            } else {
                fitStatus = hxzt->Fit(fxz, "QN", "", hxzt->GetBinCenter(hxzt->GetMaximumBin()) - 0.2,
                                      hxzt->GetBinCenter(hxzt->GetMaximumBin()) + 0.2);
            }

            if (fitStatus == 0) {
                if (fxz->GetParameter(2) < fHoughSigmaLimit) {
                    double c = fxz->GetParameter(1);
                    int from = hxzt->FindBin(c - fxz->GetParameter(2) * 2);  // 2 sigma
                    int to = hxzt->FindBin(c + fxz->GetParameter(2));

                    int sum = hxzt->Integral(from, to);

                    if (sum > (double)hxzt->Integral() * fPeakPointRateLimit)
                        fAnaEvent->SetSubEventTag("muon");
                } else if (fxz->GetParameter(2) > 0.8 && zlen > 250) {
                    fAnaEvent->SetSubEventTag("electron");
                }
            }
        }
        if (fHough_YZ.size() > 100) {
            hyzt->Reset();
            hyzr->Reset();
            for (int i = 0; i < fHough_YZ.size(); i++) {
                hyzt->Fill(fHough_YZ[i].y(), fHough_YZ[i].z());
                hyzr->Fill(fHough_YZ[i].x(), fHough_YZ[i].z());
            }
            if (hyzt->GetMaximumBin() == 1) {
                fAnaEvent->SetSubEventTag("sparking");
            }

            fyz->SetParameter(1, hyzt->GetBinCenter(hyzt->GetMaximumBin()));
            int fitStatus;
            if (fVerboseLevel >= REST_Debug) {
                debug << "ID: " << fAnaEvent->GetID() << ", hyzt points: " << fHough_YZ.size()
                      << ", center: " << hyzt->GetBinCenter(hyzt->GetMaximumBin()) << endl;
                fitStatus = hyzt->Fit(fyz, "N", "", hyzt->GetBinCenter(hyzt->GetMaximumBin()) - 0.2,
                                      hyzt->GetBinCenter(hyzt->GetMaximumBin()) + 0.2);
            } else {
                fitStatus = hyzt->Fit(fyz, "QN", "", hyzt->GetBinCenter(hyzt->GetMaximumBin()) - 0.2,
                                      hyzt->GetBinCenter(hyzt->GetMaximumBin()) + 0.2);
            }

            if (fitStatus == 0) {
                if (fyz->GetParameter(2) < fHoughSigmaLimit) {
                    double c = fyz->GetParameter(1);
                    int from = hyzt->FindBin(c - fyz->GetParameter(2) * 2);  // 2 sigma
                    int to = hyzt->FindBin(c + fyz->GetParameter(2));

                    int sum = hyzt->Integral(from, to);

                    if (sum > (double)hyzt->Integral() * fPeakPointRateLimit) {
                        if (fAnaEvent->GetSubEventTag() == "electron")
                            fAnaEvent->SetSubEventTag("general");
                        else
                            fAnaEvent->SetSubEventTag("muon");
                    }

                } else if (fyz->GetParameter(2) > 0.8 && zlen > 250) {
                    if (fAnaEvent->GetSubEventTag() == "muon")
                        fAnaEvent->SetSubEventTag("general");
                    else
                        fAnaEvent->SetSubEventTag("electron");
                }
            }
        }
    }

    if (fAnaEvent->GetSubEventTag() == "general")  // 4. tag electron event
    {
    }

    //// 0: uses all, 1: muon, 2: strong electron, 3: weak electron, 4: sparking, 5:
    //// abnormal, 9: other
    // if (Count(fSelection, "0") == 0) {
    //    if (Count(fSelection, "4") != 0) {
    //        if (fAnaEvent->GetSubEventTag() == "sparking") {
    //            info << "selecting sparking event, id " << fAnaEvent->GetID() << endl;
    //            return fAnaEvent;
    //        }
    //    }
    //    if (Count(fSelection, "3") != 0) {
    //        if (fAnaEvent->GetSubEventTag() == "weak") {
    //            info << "selecting event from calibration source, id " << fAnaEvent->GetID() << endl;
    //            return fAnaEvent;
    //        }
    //    }
    //    if (Count(fSelection, "1") != 0) {
    //        if (fAnaEvent->GetSubEventTag() == "muon") {
    //            info << "selecting muon event, id " << fAnaEvent->GetID() << endl;
    //            return fAnaEvent;
    //        }
    //    }
    //    if (Count(fSelection, "2") != 0) {
    //        if (fAnaEvent->GetSubEventTag() == "electron") {
    //            info << "selecting electron event, id " << fAnaEvent->GetID() << endl;
    //            return fAnaEvent;
    //        }
    //    }
    //    if (Count(fSelection, "5") != 0) {
    //        if (fAnaEvent->GetSubEventTag() == "abnormal") {
    //            info << "selecting abnormal trigger, id " << fAnaEvent->GetID() << endl;
    //            return fAnaEvent;
    //        }
    //    }
    //    if (Count(fSelection, "6") != 0) {
    //        if (fAnaEvent->GetSubEventTag() == "pile up") {
    //            info << "selecting pile up event, id " << fAnaEvent->GetID() << endl;
    //            return fAnaEvent;
    //        }
    //    }
    //    if (Count(fSelection, "9") != 0) {
    //        if (fAnaEvent->GetSubEventTag() == "general") {
    //            info << "selecting unknown event, id " << fAnaEvent->GetID() << endl;
    //            return fAnaEvent;
    //        }
    //    }

    //    return NULL;
    //}
    return fAnaEvent;
}

void TRestMuonAnalysisProcess::EndProcess() {
    mutanthe->Write();

    if (nummudeposxz > 0) {
        mudeposxz->Scale(1 / (double)nummudeposxz);
        mudeposxz->SetEntries(nummudeposxz);
    }
    if (nummudeposyz > 0) {
        mudeposyz->Scale(1 / (double)nummudeposyz);
        mudeposyz->SetEntries(nummudeposyz);
    }
    // if (numsmearxy > 0) {
    //	musmearxy->Scale(1 / (double)numsmearxy);
    //	musmearxy->SetEntries(numsmearxy);
    //}

    mudeposxz->Write();
    mudeposyz->Write();

    mudepos->Add(mudeposxz);
    mudepos->Add(mudeposyz);
    mudepos->Write();

    // musmearxy->Write();

    muhitmap->Write();
    muhitdir->Divide(muhitmap);
    muhitdir->Write();
}

double TRestMuonAnalysisProcess::ProjectionToCenter(double x, double y, double xzthe, double yzthe) {
    TVector2 angle = TVector2(tan(xzthe), tan(yzthe));
    TVector2 pos = TVector2((X1 + X2) / 2 - x, (Y1 + Y2) / 2 - y);

    return angle.Proj(pos) * pos / pos.Mod();
}

double TRestMuonAnalysisProcess::DistanceToTrack(double x, double z, double x0, double z0, double theta) {
    double px = (tan(theta) * (z - z0) + tan(theta) * tan(theta) * x + x0) / (tan(theta) * tan(theta) + 1);
    double pz = (tan(theta) * z0 + 1 / tan(theta) * z - x0 + x) / (1 / tan(theta) + tan(theta));

    return sqrt((px - x) * (px - x) + (pz - z) * (pz - z));
}

void TRestMuonAnalysisProcess::InitFromConfigFile() {
    fHoughSigmaLimit = StringToDouble(GetParameter("houghSigmaLimit", "0.05"));
    fPeakPointRateLimit = StringToDouble(GetParameter("peakPointRateLimit", "0.9"));

    TVector2 XROI = StringTo2DVector(GetParameter("XROI", "(-100,100)"));
    TVector2 YROI = StringTo2DVector(GetParameter("YROI", "(-100,100)"));

    X1 = XROI.X(), X2 = XROI.Y(), Y1 = YROI.X(), Y2 = YROI.Y();
}
