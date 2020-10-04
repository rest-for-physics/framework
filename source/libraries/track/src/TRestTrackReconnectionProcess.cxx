//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackReconnectionProcess.cxx
///
///             Jan 2016:   First concept (Javier Galan)
///
//////////////////////////////////////////////////////////////////////////

#include "TRestTrackReconnectionProcess.h"
using namespace std;

ClassImp(TRestTrackReconnectionProcess)
    //______________________________________________________________________________
    TRestTrackReconnectionProcess::TRestTrackReconnectionProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestTrackReconnectionProcess::TRestTrackReconnectionProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();
    PrintMetadata();
}

//______________________________________________________________________________
TRestTrackReconnectionProcess::~TRestTrackReconnectionProcess() { delete fOutputTrackEvent; }

void TRestTrackReconnectionProcess::LoadDefaultConfig() {
    SetName("trackReconnectionProcess");
    SetTitle("Default config");
}

//______________________________________________________________________________
void TRestTrackReconnectionProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputTrackEvent = NULL;
    fOutputTrackEvent = new TRestTrackEvent();

    fSplitTrack = false;
}

void TRestTrackReconnectionProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();

    PrintMetadata();
}

//______________________________________________________________________________
void TRestTrackReconnectionProcess::InitProcess() { TRestEventProcess::ReadObservables(); }

//______________________________________________________________________________
TRestEvent* TRestTrackReconnectionProcess::ProcessEvent(TRestEvent* evInput) {
    Int_t trackBranches = 0;

    fInputTrackEvent = (TRestTrackEvent*)evInput;

    // Copying the input tracks to the output track
    for (int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++)
        fOutputTrackEvent->AddTrack(fInputTrackEvent->GetTrack(tck));

    for (int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++) {
        if (!fInputTrackEvent->isTopLevel(tck)) continue;
        Int_t tckId = fInputTrackEvent->GetTrack(tck)->GetTrackID();

        TRestVolumeHits* hits = fInputTrackEvent->GetTrack(tck)->GetVolumeHits();

        /* {{{ Debug output */
        if (this->GetVerboseLevel() >= REST_Debug) {
            cout << "TRestTrackReconnectionProcess. Input hits" << endl;
            Int_t pId = fInputTrackEvent->GetTrack(tck)->GetParentID();
            cout << "Track : " << tck << " TrackID : " << tckId << " ParentID : " << pId << endl;
            cout << "-----------------" << endl;
        }
        /* }}} */

        SetDistanceMeanAndSigma((TRestHits*)hits);

        if (fMeanDistance == 0) continue;  // We have just 1-hit

        TRestVolumeHits initialHits = *hits;
        vector<TRestVolumeHits> subHitSets;
        Int_t tBranches;

        // We do 3 times the break and re-connect process
        // Although another option would be to do it until we observe no change
        // Most of the times even 1-round is more than enough.
        for (int n = 0; n < 1; n++) {
            // The required distance between hits to break a track is increased in
            // each iteration
            BreakTracks(&initialHits, subHitSets, 1.5 * (n + 1));
            ReconnectTracks(subHitSets);

            TRestVolumeHits resultHits = subHitSets[0];
            SetDistanceMeanAndSigma(&resultHits);
            tBranches = GetTrackBranches(resultHits, fNSigmas);

            if (GetVerboseLevel() >= REST_Debug) {
                cout << "Break and reconnect finished" << endl;
                cout << "Branches : " << tBranches << endl;
                if (GetVerboseLevel() >= REST_Extreme) GetChar();
            }

            initialHits = resultHits;
        }

        // For the observable We just take the value for the track with more number
        // of branches
        if (tBranches > trackBranches) trackBranches = tBranches;

        // A fine tunning applied to consecutive hits
        for (int n = 0; n < subHitSets[0].GetNumberOfHits(); n++) {
            if (n > 0 && n < subHitSets[0].GetNumberOfHits() - 1) {
                Double_t distance = subHitSets[0].GetHitsPathLength(n - 2, n + 2);

                subHitSets[0].SwapHits(n - 1, n);

                Double_t distanceAfter = subHitSets[0].GetHitsPathLength(n - 2, n + 2);

                if (distanceAfter < distance) continue;

                subHitSets[0].SwapHits(n - 1, n);
            }
        }

        if (fSplitTrack) {
            BreakTracks(&initialHits, subHitSets, fNSigmas);

            if (GetVerboseLevel() >= REST_Debug) {
                cout << " **** Splitting track : " << endl;
                cout << "Number of subHitSets : " << subHitSets.size() << endl;
                if (GetVerboseLevel() >= REST_Extreme) GetChar();
            }
        }

        for (unsigned int n = 0; n < subHitSets.size(); n++) {
            TRestTrack aTrack;
            // We create the new track and add it giving its parent ID
            aTrack.SetTrackID(fOutputTrackEvent->GetNumberOfTracks() + 1);

            aTrack.SetParentID(tckId);

            aTrack.SetVolumeHits(subHitSets[n]);

            fOutputTrackEvent->AddTrack(&aTrack);
        }
    }

    SetObservableValue("branches", trackBranches);
    // cout << "Track branches : " << trackBranches << endl;

    if (GetVerboseLevel() >= REST_Debug) {
        cout << "++++++++++++++++++++++++++++++++" << endl;
        fOutputTrackEvent->PrintEvent();
        cout << "++++++++++++++++++++++++++++++++" << endl;
        if (GetVerboseLevel() >= REST_Extreme) GetChar();
    }

    return fOutputTrackEvent;
}

/// BreakTracks and ReconnectTracks should be moved to libRestEvents in
/// events/tools at RESTv2.2 For the moment these methods might be replicated in
/// other TrackProcesses
void TRestTrackReconnectionProcess::BreakTracks(TRestVolumeHits* hits, vector<TRestVolumeHits>& hitSets,
                                                Double_t nSigma) {
    hitSets.clear();
    if (GetVerboseLevel() >= REST_Debug) {
        cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
        cout << "BreakTracks. Breaking tracks into hits subsets." << endl;
    }

    TRestVolumeHits subHits;
    for (int n = 0; n < hits->GetNumberOfHits(); n++) {
        Double_t x = hits->GetX(n);
        Double_t y = hits->GetY(n);
        Double_t z = hits->GetZ(n);
        Double_t sX = hits->GetSigmaX(n);
        Double_t sY = hits->GetSigmaY(n);
        Double_t sZ = hits->GetSigmaZ(n);
        Double_t energy = hits->GetEnergy(n);

        if (GetVerboseLevel() >= REST_Debug && n > 0) {
            cout << "Distance : " << hits->GetDistance(n - 1, n);
            if (hits->GetDistance(n - 1, n) > fMeanDistance + nSigma * fSigma) cout << " BREAKKKK";
            cout << endl;
        }

        if (n > 0 && hits->GetDistance(n - 1, n) > fMeanDistance + nSigma * fSigma) {
            hitSets.push_back(subHits);
            subHits.RemoveHits();
        }

        subHits.AddHit(x, y, z, energy, 0, hits->GetType(n), sX, sY, sZ);

        if (GetVerboseLevel() >= REST_Debug)
            cout << "H : " << n << " X : " << x << " Y : " << y << " Z : " << z << endl;
    }

    hitSets.push_back(subHits);

    if (GetVerboseLevel() >= REST_Debug) cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
}

void TRestTrackReconnectionProcess::ReconnectTracks(vector<TRestVolumeHits>& hitSets) {
    if (GetVerboseLevel() >= REST_Debug) {
        cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
        cout << "ReconnectTracks. Connecting back sub tracks " << endl;
    }

    Int_t nSubTracks = hitSets.size();

    if (nSubTracks == 1) return;

    Double_t minDistance = 1.e10;

    Int_t tracks[2][2];

    Int_t nHits[nSubTracks];
    for (int i = 0; i < nSubTracks; i++) nHits[i] = hitSets[i].GetNumberOfHits();

    if (GetVerboseLevel() >= REST_Debug) {
        cout << "ORIGINAL" << endl;
        cout << "--------" << endl;
        for (int i = 0; i < nSubTracks; i++) {
            cout << "Subset : " << i << endl;
            cout << " Sub hits : " << nHits[i] << endl;

            hitSets[i].PrintHits();
        }
        cout << "--------" << endl;
    }

    /* {{{ Finds the closest sub-track extremes */
    for (int i = 0; i < nSubTracks; i++) {
        for (int j = 0; j < nSubTracks; j++) {
            if (i != j) {
                Int_t x1_0 = hitSets[i].GetX(0);
                Int_t x1_1 = hitSets[i].GetX(nHits[i] - 1);

                Int_t y1_0 = hitSets[i].GetY(0);
                Int_t y1_1 = hitSets[i].GetY(nHits[i] - 1);

                Int_t z1_0 = hitSets[i].GetZ(0);
                Int_t z1_1 = hitSets[i].GetZ(nHits[i] - 1);

                Int_t x2_0 = hitSets[j].GetX(0);
                Int_t x2_1 = hitSets[j].GetX(nHits[j] - 1);

                Int_t y2_0 = hitSets[j].GetY(0);
                Int_t y2_1 = hitSets[j].GetY(nHits[j] - 1);

                Int_t z2_0 = hitSets[j].GetZ(0);
                Int_t z2_1 = hitSets[j].GetZ(nHits[j] - 1);

                Double_t d;
                d = TMath::Sqrt((x1_0 - x2_0) * (x1_0 - x2_0) + (y1_0 - y2_0) * (y1_0 - y2_0) +
                                (z1_0 - z2_0) * (z1_0 - z2_0));

                if (d < minDistance) {
                    tracks[0][0] = i;
                    tracks[0][1] = 0;
                    tracks[1][0] = j;
                    tracks[1][1] = 0;
                    minDistance = d;
                }

                d = TMath::Sqrt((x1_0 - x2_1) * (x1_0 - x2_1) + (y1_0 - y2_1) * (y1_0 - y2_1) +
                                (z1_0 - z2_1) * (z1_0 - z2_1));

                if (d < minDistance) {
                    tracks[0][0] = i;
                    tracks[0][1] = 0;
                    tracks[1][0] = j;
                    tracks[1][1] = 1;
                    minDistance = d;
                }

                d = TMath::Sqrt((x1_1 - x2_0) * (x1_1 - x2_0) + (y1_1 - y2_0) * (y1_1 - y2_0) +
                                (z1_1 - z2_0) * (z1_1 - z2_0));

                if (d < minDistance) {
                    tracks[0][0] = i;
                    tracks[0][1] = 1;
                    tracks[1][0] = j;
                    tracks[1][1] = 0;
                    minDistance = d;
                }

                d = TMath::Sqrt((x1_1 - x2_1) * (x1_1 - x2_1) + (y1_1 - y2_1) * (y1_1 - y2_1) +
                                (z1_1 - z2_1) * (z1_1 - z2_1));

                if (d < minDistance) {
                    tracks[0][0] = i;
                    tracks[0][1] = 1;
                    tracks[1][0] = j;
                    tracks[1][1] = 1;
                    minDistance = d;
                }
            }
        }
    }
    /* }}} */

    /* {{{ Debug output
       cout << "Tracks" << endl;
       cout << tracks[0][0] << " ::: " << tracks[0][1] << endl;
       cout << tracks[1][0] << " ::: " << tracks[1][1] << endl;
       }}} */

    TRestVolumeHits newHits;
    newHits.RemoveHits();

    Int_t tck1 = tracks[0][0];
    Int_t tck2 = tracks[1][0];

    /* {{{ Rejoins the closest sub-track extremes into 1-single track */
    if (tracks[0][1] == 0 && tracks[1][1] == 0) {
        // We invert the first and add the second
        for (int n = nHits[tck1] - 1; n >= 0; n--) newHits.AddHit(hitSets[tck1], n);

        for (int n = 0; n < nHits[tck2]; n++) newHits.AddHit(hitSets[tck2], n);
    } else if (tracks[0][1] == 1 && tracks[1][1] == 0) {
        // We add the first and then the second

        for (int n = 0; n < nHits[tck1]; n++) newHits.AddHit(hitSets[tck1], n);

        for (int n = 0; n < nHits[tck2]; n++) newHits.AddHit(hitSets[tck2], n);
    } else if (tracks[0][1] == 0 && tracks[1][1] == 1) {
        // We add the second and then the first

        for (int n = 0; n < nHits[tck2]; n++) newHits.AddHit(hitSets[tck2], n);

        for (int n = 0; n < nHits[tck1]; n++) newHits.AddHit(hitSets[tck1], n);
    } else {
        // We add the first and invert the second

        for (int n = 0; n < nHits[tck1]; n++) newHits.AddHit(hitSets[tck1], n);

        for (int n = nHits[tck2] - 1; n >= 0; n--) newHits.AddHit(hitSets[tck2], n);
    }

    hitSets.erase(hitSets.begin() + tck2);
    hitSets.erase(hitSets.begin() + tck1);
    hitSets.push_back(newHits);
    /* }}} */

    nSubTracks = hitSets.size();

    if (GetVerboseLevel() >= REST_Debug) {
        cout << "New subtracks : " << nSubTracks << endl;

        cout << "AFTER REMOVAL" << endl;
        cout << "--------" << endl;
        for (int i = 0; i < nSubTracks; i++) {
            cout << "Subset : " << i << endl;
            cout << " Sub hits : " << nHits[i] << endl;

            hitSets[i].PrintHits();
        }
        cout << "--------" << endl;
        if (GetVerboseLevel() >= REST_Extreme) GetChar();
    }

    if (nSubTracks > 1) ReconnectTracks(hitSets);

    if (GetVerboseLevel() >= REST_Debug) cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
}

Int_t TRestTrackReconnectionProcess::GetTrackBranches(TRestHits& h, Double_t nSigma) {
    Int_t breaks = 0;
    Int_t nHits = h.GetNumberOfHits();

    for (int n = 1; n < nHits; n++)
        if (h.GetDistance(n - 1, n) > fMeanDistance + nSigma * fSigma) breaks++;
    return breaks;
}

//______________________________________________________________________________
void TRestTrackReconnectionProcess::EndProcess() {}

//______________________________________________________________________________
void TRestTrackReconnectionProcess::InitFromConfigFile() {
    if (GetParameter("splitTrack", "false") == "true")
        fSplitTrack = true;
    else
        fSplitTrack = false;

    fNSigmas = StringToDouble(GetParameter("nSigmas", "5"));
}

void TRestTrackReconnectionProcess::SetDistanceMeanAndSigma(TRestHits* h) {
    Int_t nHits = h->GetNumberOfHits();

    fMeanDistance = 0;
    for (int n = 1; n < nHits; n++) fMeanDistance += h->GetDistance(n - 1, n);
    fMeanDistance /= nHits;

    fSigma = TMath::Sqrt(fMeanDistance);

    if (GetVerboseLevel() >= REST_Debug) {
        cout << "-----> Node distance average ; " << fMeanDistance << endl;
        cout << "-----> Node distance sigma : " << fSigma << endl;
        cout << endl;
    }
}
