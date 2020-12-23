//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHits3DReconstructionProcess.cxx
///
///             Jan 2016:   First concept (Javier Galan)
///
//////////////////////////////////////////////////////////////////////////

#include "TRestDetectorHits3DReconstructionProcess.h"

#include "TFunction.h"
#include "TLine.h"
#include "TMarker.h"

using namespace std;
#define XWidth 96.7
#define YWidth 96.3
#define PITCH 6.2
#define PlaneMaxX 800
#define PlaneMaxY 800
struct regionhit {
    double x = 0;
    double y = 0;
    double e = 0;
    bool iszombie() const { return e == 0; }
    void add(const regionhit& h) {
        x = (x * e + h.x * h.e) / (e + h.e);
        y = (y * e + h.y * h.e) / (e + h.e);
        e += h.e;
    }
    static double dist(const regionhit& h1, const regionhit& h2) {
        return sqrt((h1.x - h2.x) * (h1.x - h2.x) + (h1.y - h2.y) * (h1.y - h2.y));
    }
};

struct stripehit {
    double x1 = 0;
    double x2 = 0;
    double y1 = 0;
    double y2 = 0;
    double e = 0;
    REST_HitType type;

    void Print() { cout << "x1: " << x1 << ", x2: " << x2 << ", y1: " << y1 << ", y2: " << y2 << endl; }

    bool iszombie() const { return e == 0; }

    static bool IsRectCross(const stripehit& s1, const stripehit& s2) {
        bool ret = min(s1.x1, s1.x2) <= max(s2.x1, s2.x2) && min(s2.x1, s2.x2) <= max(s1.x1, s1.x2) &&
                   min(s1.y1, s1.y2) <= max(s2.y1, s2.y2) && min(s2.y1, s2.y2) <= max(s1.y1, s1.y2);
        return ret;
    }

    static bool IsLineSegmentCross(const stripehit& s1, const stripehit& s2) {
        if (((s2.x1 - s1.x1) * (s2.y1 - s2.y2) - (s2.y1 - s1.y1) * (s2.x1 - s2.x2)) *
                    ((s2.x1 - s1.x2) * (s2.y1 - s2.y2) - (s2.y1 - s1.y2) * (s2.x1 - s2.x2)) <
                0 ||
            ((s1.x1 - s2.x1) * (s1.y1 - s1.y2) - (s1.y1 - s2.y1) * (s1.x1 - s1.x2)) *
                    ((s1.x1 - s2.x2) * (s1.y1 - s1.y2) - (s1.y1 - s2.y2) * (s1.x1 - s1.x2)) <
                0)
            return true;
        else
            return false;
    }

    static bool GetCrossPoint(const stripehit& s1, const stripehit& s2, double& x, double& y) {
        if (IsRectCross(s1, s2)) {
            if (IsLineSegmentCross(s1, s2)) {
                long tmpLeft, tmpRight;
                tmpLeft = (s2.x2 - s2.x1) * (s1.y1 - s1.y2) - (s1.x2 - s1.x1) * (s2.y1 - s2.y2);
                tmpRight = (s1.y1 - s2.y1) * (s1.x2 - s1.x1) * (s2.x2 - s2.x1) +
                           s2.x1 * (s2.y2 - s2.y1) * (s1.x2 - s1.x1) -
                           s1.x1 * (s1.y2 - s1.y1) * (s2.x2 - s2.x1);

                x = (int)((double)tmpRight / (double)tmpLeft);

                tmpLeft = (s1.x1 - s1.x2) * (s2.y2 - s2.y1) - (s1.y2 - s1.y1) * (s2.x1 - s2.x2);
                tmpRight = s1.y2 * (s1.x1 - s1.x2) * (s2.y2 - s2.y1) +
                           (s2.x2 - s1.x2) * (s2.y2 - s2.y1) * (s1.y1 - s1.y2) -
                           s2.y2 * (s2.x1 - s2.x2) * (s1.y2 - s1.y1);
                y = (int)((double)tmpRight / (double)tmpLeft);
                return true;
            }
        }
        return false;
    }

    static double dist(const stripehit& h1, const stripehit& h2) {
        if (IsRectCross(h1, h2) && IsLineSegmentCross(h1, h2)) {
            return 0;
        } else {
            double x = h1.x1;
            double y = h1.y1;
            double d1 = abs(((x - h2.x1) * (h2.y2 - h2.y1) - (h2.x2 - h2.x1) * (y - h2.y1)) /
                            sqrt((h2.x2 - h2.x1) * (h2.x2 - h2.x1) + (h2.y2 - h2.y1) * (h2.y2 - h2.y1)));

            x = h1.x2;
            y = h1.y2;
            double d2 = abs(((x - h2.x1) * (h2.y2 - h2.y1) - (h2.x2 - h2.x1) * (y - h2.y1)) /
                            sqrt((h2.x2 - h2.x1) * (h2.x2 - h2.x1) + (h2.y2 - h2.y1) * (h2.y2 - h2.y1)));

            x = h2.x1;
            y = h2.y1;
            double d3 = abs(((x - h1.x1) * (h1.y2 - h1.y1) - (h1.x2 - h1.x1) * (y - h1.y1)) /
                            sqrt((h1.x2 - h1.x1) * (h1.x2 - h1.x1) + (h1.y2 - h1.y1) * (h1.y2 - h1.y1)));

            x = h2.x2;
            y = h2.y2;
            double d4 = abs(((x - h1.x1) * (h1.y2 - h1.y1) - (h1.x2 - h1.x1) * (y - h1.y1)) /
                            sqrt((h1.x2 - h1.x1) * (h1.x2 - h1.x1) + (h1.y2 - h1.y1) * (h1.y2 - h1.y1)));

            return min({d1, d2, d3, d4});
        }
    }

    friend regionhit operator&(const stripehit& s1, const stripehit& s2) {
        if (s1.type * s2.type == XZ * YZ) {
            if (!s1.iszombie() && !s2.iszombie()) {
                double x;
                double y;
                if (stripehit::GetCrossPoint(s1, s2, x, y)) {
                    regionhit hit;
                    hit.x = x;
                    hit.y = y;
                    hit.e = s1.e + s2.e;
                    return hit;
                }
            }
        }
        return regionhit();
    }

    stripehit(TRestHits* hits, int i) {
        type = hits->GetType(i);
        if (type == XZ) {
            y1 = hits->GetY(i) - YWidth;
            y2 = hits->GetY(i) + YWidth;
            x1 = hits->GetX(i);
            x2 = hits->GetX(i);
        } else if (type == YZ) {
            y1 = hits->GetY(i);
            y2 = hits->GetY(i);
            x1 = hits->GetX(i) - XWidth;
            x2 = hits->GetX(i) + XWidth;
        } else {
            return;
        }
        e = hits->GetEnergy(i);
    }

    stripehit(double x1, double x2, double y1, double y2, double e = 0, REST_HitType type = unknown) {
        this->x1 = x1;
        this->x2 = x2;
        this->y1 = y1;
        this->y2 = y2;
        this->e = e;
        this->type = type;
    }
};

ClassImp(TRestDetectorHits3DReconstructionProcess)
    //______________________________________________________________________________
    TRestDetectorHits3DReconstructionProcess::TRestDetectorHits3DReconstructionProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestDetectorHits3DReconstructionProcess::~TRestDetectorHits3DReconstructionProcess() { delete fOutputHitsEvent; }

//______________________________________________________________________________
void TRestDetectorHits3DReconstructionProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputHitsEvent = NULL;
    fOutputHitsEvent = new TRestDetectorHitsEvent();
}

void TRestDetectorHits3DReconstructionProcess::PrintMetadata() {
    BeginPrintProcess();
    /////////////
    // write here the information to print

    ////////////
    EndPrintProcess();
}

//______________________________________________________________________________
void TRestDetectorHits3DReconstructionProcess::InitProcess() {
    if (fDraw) {
        htemp = new TH2D("hhits", "hhits", 100, -PlaneMaxX, PlaneMaxX, 100, -PlaneMaxY, PlaneMaxY);
        CreateCanvas();
    }

    fCompareProc = GetFriendLive("TRestGeant4ToHitsProcess");
}

//______________________________________________________________________________
TRestEvent* TRestDetectorHits3DReconstructionProcess::ProcessEvent(TRestEvent* evInput) {
    fInputHitsEvent = (TRestDetectorHitsEvent*)evInput;

    // sort with z from small to large
    fInputHitsEvent->Sort();

    double lastz = 0;
    double totalambiguity = 0;
    double Nlayers = 0;
    for (int n = 0; n < fInputHitsEvent->GetNumberOfHits(); n++) {
        double z = fInputHitsEvent->GetZ(n);
        if (z == lastz) continue;

        lastz = z;

        // find the hits near this z slice
        map<int, double> ids;  //[id, weight]
        ids[n] = 1;
        for (int i = n - 1; i >= 0; i--) {
            if (abs(fInputHitsEvent->GetZ(i) - z) < fZRange * 2) {
                ids[i] = TMath::Gaus(abs(fInputHitsEvent->GetZ(i) - z), 0, fZRange);
            } else {
                break;
            }
        }
        for (int i = n + 1; i < fInputHitsEvent->GetNumberOfHits(); i++) {
            if (abs(fInputHitsEvent->GetZ(i) - z) < fZRange * 2) {
                ids[i] = TMath::Gaus(abs(fInputHitsEvent->GetZ(i) - z), 0, fZRange);
            } else {
                break;
            }
        }

        // extract stripe hits
        vector<stripehit> stripehits;
        for (auto i : ids) {
            stripehit h = stripehit(fInputHitsEvent->GetHits(), i.first);
            h.e *= i.second;
            if (!h.iszombie()) {
                if (stripehits.size() > 0) {
                    for (int k = 0; k < stripehits.size(); k++) {
                        stripehit& _h = stripehits[k];
                        if (_h.x1 == h.x1 && _h.x2 == h.x2 && _h.y1 == h.y1 && _h.y2 == h.y2) {
                            _h.e += h.e;
                            break;
                        } else if (k == stripehits.size() - 1) {
                            stripehits.push_back(h);
                        }
                    }
                } else {
                    stripehits.push_back(h);
                }
            }
        }

        if (stripehits.size() > 50) {
            warning << "(TRestDetectorHits3DReconstructionProcess) event id: " << fInputHitsEvent->GetID()
                    << ", z: " << z << ", bad frame, too much hits! (" << stripehits.size() << ")" << endl;
            continue;
        }

        if (fVerboseLevel >= REST_Extreme) {
            cout << "stripehits: ";
            for (auto h : stripehits) {
                cout << h.x1 << "," << h.y1 << "," << h.x2 << "," << h.y2 << "," << h.e << " ";
            }
            cout << endl;
        }

        // find all the crossing points of stripe hits
        vector<regionhit> regionhits;
        double layerambiguity = 0;
        set<int> ambcalculatedstripes;
        for (int i = 0; i < stripehits.size(); i++) {
            vector<int> crossedids;
            double crossedsumenergy = 0;
            for (int j = 0; j < stripehits.size(); j++) {
                if (i == j) continue;
                if (stripehit::IsRectCross(stripehits[i], stripehits[j])) {
                    crossedids.push_back(j);
                    crossedsumenergy += stripehits[j].e;
                }
            }

            for (int id : crossedids) {
                regionhit rh = stripehits[i] & stripehits[id];
                rh.e = stripehits[i].e * stripehits[id].e / crossedsumenergy;

                if (regionhits.size() > 0) {
                    for (int k = 0; k < regionhits.size(); k++) {
                        regionhit& _rh = regionhits[k];
                        if (_rh.x == rh.x && _rh.y == rh.y) {
                            _rh.e += rh.e;
                            break;
                        } else if (k == regionhits.size() - 1) {
                            regionhits.push_back(rh);
                        }
                    }
                } else {
                    regionhits.push_back(rh);
                }
            }

            // calculate ambiguouity
            if (ambcalculatedstripes.count(i) == 0 && crossedids.size() > 0) {
                for (int id : crossedids) {
                    ambcalculatedstripes.insert(id);
                }

                auto line = stripehits[i];
                auto newline = stripehits[crossedids[0]];
                vector<int> crossedids_newline;
                for (int j = 0; j < stripehits.size(); j++) {
                    if (j == crossedids[0]) continue;
                    if (stripehit::IsRectCross(stripehits[j], newline)) {
                        crossedids_newline.push_back(j);
                    }
                }
                for (int id : crossedids_newline) {
                    ambcalculatedstripes.insert(id);
                }

                vector<pair<double, double>> V_PosE;
                vector<pair<double, double>> H_PosE;

                if (line.x1 == line.x2) {
                    // cout << "adding H from crossed lines of first crossed line(" << crossedids[0]
                    //     << ") of line: " << i << endl;
                    for (int id : crossedids_newline) {
                        H_PosE.push_back({stripehits[id].x1, stripehits[id].e});
                    }
                } else if (line.y1 == line.y2) {
                    // cout << "adding V from crossed lines of first crossed line(" << crossedids[0]
                    //     << ") of line: " << i << endl;
                    for (int id : crossedids_newline) {
                        V_PosE.push_back({stripehits[id].y1, stripehits[id].e});
                    }
                }

                if (newline.x1 == newline.x2) {
                    // cout << "adding H from crossed lines of line: " << i << endl;
                    for (int id : crossedids) {
                        H_PosE.push_back({stripehits[id].x1, stripehits[id].e});
                    }
                } else if (newline.y1 == newline.y2) {
                    // cout << "adding V from crossed lines of line: " << i << endl;
                    for (int id : crossedids) {
                        V_PosE.push_back({stripehits[id].y1, stripehits[id].e});
                    }
                }

                // sort with position form small to large
                auto comp = [](const pair<double, double>& p1, const pair<double, double>& p2) -> bool {
                    return p1.first < p2.first;
                };
                sort(V_PosE.begin(), V_PosE.end(), comp);
                sort(H_PosE.begin(), H_PosE.end(), comp);

                for (int j = 1; j < V_PosE.size(); j++) {
                    if (V_PosE[j].first - V_PosE[j - 1].first > PITCH) {
                        V_PosE.insert(V_PosE.begin() + j, {(V_PosE[j].first + V_PosE[j - 1].first) / 2, 0});
                        j++;
                    }
                }
                V_PosE.insert(V_PosE.begin(), {(*V_PosE.begin()).first - 1, 0});
                V_PosE.insert(V_PosE.end(), {(*(V_PosE.end() - 1)).first + 1, 0});

                for (int j = 1; j < H_PosE.size(); j++) {
                    if (H_PosE[j].first - H_PosE[j - 1].first > PITCH) {
                        H_PosE.insert(H_PosE.begin() + j, {(H_PosE[j].first + H_PosE[j - 1].first) / 2, 0});
                        j++;
                    }
                }
                H_PosE.insert(H_PosE.begin(), {(*H_PosE.begin()).first - 1, 0});
                H_PosE.insert(H_PosE.end(), {(*(H_PosE.end() - 1)).first + 1, 0});

                if (fVerboseLevel >= REST_Extreme) {
                    cout << "V: ";
                    for (auto pose : V_PosE) {
                        cout << pose.first << "," << pose.second << " ";
                    }
                    cout << endl;

                    cout << "H: ";
                    for (auto pose : H_PosE) {
                        cout << pose.first << "," << pose.second << " ";
                    }
                    cout << endl;
                }

                int Vsegs = 0;
                int Hsegs = 0;

                for (int j = 1; j < V_PosE.size() - 1; j++) {
                    if (V_PosE[j].second > V_PosE[j - 1].second && V_PosE[j].second > V_PosE[j + 1].second) {
                        Vsegs++;
                    }
                }

                for (int j = 1; j < H_PosE.size() - 1; j++) {
                    if (H_PosE[j].second > H_PosE[j - 1].second && H_PosE[j].second > H_PosE[j + 1].second) {
                        Hsegs++;
                    }
                }

                if (fVerboseLevel >= REST_Debug) {
                    cout << i << " z: " << z << " " << Vsegs << " " << Hsegs << " -> "
                         << LogAmbiguity(Vsegs, Hsegs) << endl;
                    cout << endl;
                }

                if (Vsegs == 0) Vsegs = 1;
                if (Hsegs == 0) Hsegs = 1;

                layerambiguity += LogAmbiguity(Vsegs, Hsegs);
            }
        }
        // cout << "z: " << z << ", amb: " << layerambiguity << endl;
        totalambiguity += layerambiguity;

        // merge the cossing points
        vector<regionhit> regionhits_merged;
        set<int> mergeflag_all;
        while (mergeflag_all.size() < regionhits.size()) {
            // find the initial point(with max energy)
            double max = 0;
            int maxposition;
            for (int i = 0; i < regionhits.size(); i++) {
                if (mergeflag_all.count(i) == 0) {
                    if (regionhits[i].e > max) {
                        max = regionhits[i].e;
                        maxposition = i;
                    }
                }
            }
            set<int> mergeflag;
            mergeflag.insert(maxposition);

            // loop over regionhits to find any hits that is **connected** to the initial point
            for (auto _i = mergeflag.begin(); _i != mergeflag.end();) {
                int i = *_i;

                // cout << i << ": ";
                // for (auto __i : mergeflag) cout << __i << " ";
                // cout << endl;

                bool inserted = false;
                for (int j = 0; j < regionhits.size(); j++) {
                    if (regionhit::dist(regionhits[i], regionhits[j]) < PITCH &&
                        regionhits[i].e > regionhits[j].e && mergeflag_all.count(j) == 0) {
                        auto insertresult = mergeflag.insert(j);
                        inserted += insertresult.second;
                    }
                }
                // if inserted, we go back to the begin incase it is inserted to the former
                if (inserted)
                    _i = mergeflag.begin();
                else
                    _i++;
            }

            // cout << z << " " << regionhits.size() << " " << mergeflag.size() << endl;
            // cout << endl;

            // the collection of all the connected hits are merged
            regionhit h;
            for (auto i : mergeflag) {
                mergeflag_all.insert(i);
                h.add(regionhits[i]);
            }
            regionhits_merged.push_back(h);
        }
        if (regionhits_merged.size() > 0) Nlayers++;

        if (fDraw && (layerambiguity > fDrawThres)) {
            if (regionhits.size() == 0) continue;
            double maxenergyofregionhits =
                (*min_element(regionhits.begin(), regionhits.end(), [](const regionhit& h1,
                                                                       const regionhit& h2) -> bool {
                    return h1.e > h2.e;
                })).e;

            fCanvas->Clear();
            htemp->SetStats(false);
            htemp->SetTitle(Form("event id %i, z = %.1f", fInputHitsEvent->GetID(), z));
            htemp->Draw();
            vector<TLine*> lines;
            vector<TMarker*> markers;
            for (auto h : stripehits) {
                TLine* line = new TLine(h.x1, h.y1, h.x2, h.y2);
                line->Draw();
                lines.push_back(line);
            }
            for (auto rh : regionhits) {
                TMarker* marker = new TMarker(rh.x, rh.y, kFullDotLarge);
                marker->SetMarkerColor(kGreen);
                marker->SetMarkerSize(rh.e / maxenergyofregionhits);
                marker->Draw();
                markers.push_back(marker);
            }
            for (auto rhm : regionhits_merged) {
                TMarker* marker = new TMarker(rhm.x, rhm.y, kFullDotLarge);
                marker->SetMarkerColor(kRed);
                marker->Draw();
                markers.push_back(marker);
            }

            fCanvas->Update();
            GetChar();
            for (auto l : lines) {
                delete l;
            }
            for (auto m : markers) {
                delete m;
            }
        }

        // for (auto rh : regionhits) {
        for (auto rh : regionhits_merged) {
            fOutputHitsEvent->AddHit(rh.x, rh.y, z, rh.e, 0, XYZ);
        }
    }

    // scale the total energy
    if (fDoEnergyScaling) {
        double e1 = fInputHitsEvent->GetEnergy();
        double e2 = fOutputHitsEvent->GetEnergy();
        for (auto h : *fOutputHitsEvent->GetHits()) {
            h.e() = h.e() / e2 * e1;
        }
    }

    SetObservableValue("MeanAmbiguity", totalambiguity / Nlayers);
    SetObservableValue("DiffRecon", numeric_limits<double>::quiet_NaN());
    if (fCompareProc != NULL && fOutputHitsEvent->GetNumberOfHits() > 0 &&
        (fObservablesDefined.count("DiffRecon") != 0 || fDynamicObs)) {
        TRestDetectorHitsEvent* reference = (TRestDetectorHitsEvent*)fCompareProc->GetOutputEvent();
        auto hits1 = *fOutputHitsEvent->GetHits();
        auto hits2 = *reference->GetHits();

        double maxz1 = (*min_element(hits1.begin(), hits1.end(), [](const TRestHits::iterator& h1,
                                                                    const TRestHits::iterator& h2) -> bool {
                           return h1.z() > h2.z();
                       })).z();
        double maxz2 = (*min_element(hits2.begin(), hits2.end(), [](const TRestHits::iterator& h1,
                                                                    const TRestHits::iterator& h2) -> bool {
                           return h1.z() > h2.z();
                       })).z();
        double dz = maxz2 - maxz1;

        double mindistmean = 1e9;
        for (double i = -fZRange * 2; i <= fZRange * 2; i += fZRange / 2) {
            double distsum2 = 0;
            for (auto hit : hits1) {
                auto pos = TVector3(hit.x(), hit.y(), hit.z() + dz);
                int n = hits2.GetClosestHit(pos);
                auto dist2 = (pos - hits2.GetPosition(n)).Mag2();
                distsum2 += dist2;
            }
            double distmean = sqrt(distsum2) / fOutputHitsEvent->GetNumberOfHits();
            if (distmean < mindistmean) {
                mindistmean = distmean;
            }
        }
        SetObservableValue("DiffRecon", mindistmean);
    }

    return fOutputHitsEvent;
}

int TRestDetectorHits3DReconstructionProcess::Ambiguity(const int& n, const int& m) {
    if (n <= 0 || m <= 0) return 0;
    if (n == 1 || m == 1) return 1;
    if (n > 5 && m > 5) return 1e9;
    if (n > 8 || m > 8) return 1e9;

    int N = min({n, m});
    int M = max({n, m});

    int ambiguity = pow(N, M);
    for (int i = N - 1; i > 0; i--) {
        // value of nC(n-i)
        int combinations = Factorial(N) / Factorial(N - i) / Factorial(i);
        ambiguity -= Ambiguity(i, M) * combinations;
    }
    return ambiguity;
}

int TRestDetectorHits3DReconstructionProcess::Factorial(const int& n) {
    int result = n;
    int N = n;
    while (N > 1) result *= (--N);
    return result;
}

//______________________________________________________________________________
void TRestDetectorHits3DReconstructionProcess::EndProcess() {
    if (fDraw) {
        delete htemp;
        delete fCanvas;
    }
}

//______________________________________________________________________________
void TRestDetectorHits3DReconstructionProcess::InitFromConfigFile() {
    fZRange = StringToDouble(GetParameter("zRange", "5"));
    fDraw = StringToBool(GetParameter("draw", "false"));
    if (fDraw) fSingleThreadOnly = true;
    fDrawThres = StringToDouble(GetParameter("drawThreshold", "3"));
    fDoEnergyScaling = StringToBool(GetParameter("scaleE", "true"));

    fCanvasSize = StringTo2DVector(GetParameter("canvasSize", "(800,600)"));
}
