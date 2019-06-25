///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackAnalysisProcess.cxx
///
///
///             First implementation of Geant4 analysis process into REST_v2
///             Date : mar/2016
///             Date : may/2018 Added twist parameters
///             Author : J. Galan
///
///_______________________________________________________________________________

#include "TRestTrackAnalysisProcess.h"
using namespace std;

ClassImp(TRestTrackAnalysisProcess)
    //______________________________________________________________________________
    TRestTrackAnalysisProcess::TRestTrackAnalysisProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestTrackAnalysisProcess::TRestTrackAnalysisProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestTrackAnalysisProcess::~TRestTrackAnalysisProcess() {
    delete fInputTrackEvent;
    delete fOutputTrackEvent;
}

void TRestTrackAnalysisProcess::LoadDefaultConfig() { SetTitle("Default config"); }

//______________________________________________________________________________
void TRestTrackAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputTrackEvent = new TRestTrackEvent();
    fOutputTrackEvent = new TRestTrackEvent();

    fInputEvent = fInputTrackEvent;
    fOutputEvent = fOutputTrackEvent;

    fCutsEnabled = false;

    fEnableTwistParameters = false;
}

void TRestTrackAnalysisProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::InitProcess() {
    std::vector<string> fObservables;
    fObservables = TRestEventProcess::ReadObservables();

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("nTracks_LE_") != string::npos) {
            Double_t energy = StringToDouble(fObservables[i].substr(11, fObservables[i].length()).c_str());

            fTrack_LE_EnergyObservables.push_back(fObservables[i]);
            fTrack_LE_Threshold.push_back(energy);
            nTracks_LE.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("nTracks_HE_") != string::npos) {
            Double_t energy = StringToDouble(fObservables[i].substr(11, fObservables[i].length()).c_str());

            fTrack_HE_EnergyObservables.push_back(fObservables[i]);
            fTrack_HE_Threshold.push_back(energy);
            nTracks_HE.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("nTracks_En_") != string::npos) {
            Double_t energy = StringToDouble(fObservables[i].substr(11, fObservables[i].length()).c_str());

            fTrack_En_EnergyObservables.push_back(fObservables[i]);
            fTrack_En_Threshold.push_back(energy);
            nTracks_En.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistLow_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(9, fObservables[i].length()).c_str());

            fTwistLowObservables.push_back(fObservables[i]);
            fTwistLowTailPercentage.push_back(tailPercentage);
            fTwistLowValue.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistHigh_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(10, fObservables[i].length()).c_str());

            fTwistHighObservables.push_back(fObservables[i]);
            fTwistHighTailPercentage.push_back(tailPercentage);
            fTwistHighValue.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistBalance_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(13, fObservables[i].length()).c_str());

            fTwistBalanceObservables.push_back(fObservables[i]);
            fTwistBalanceTailPercentage.push_back(tailPercentage);
            fTwistBalanceValue.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistRatio_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(11, fObservables[i].length()).c_str());

            fTwistRatioObservables.push_back(fObservables[i]);
            fTwistRatioTailPercentage.push_back(tailPercentage);
            fTwistRatioValue.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistWeightedLow_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(17, fObservables[i].length()).c_str());

            fTwistWeightedLowObservables.push_back(fObservables[i]);
            fTwistWeightedLowTailPercentage.push_back(tailPercentage);
            fTwistWeightedLowValue.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistWeightedHigh_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(18, fObservables[i].length()).c_str());

            fTwistWeightedHighObservables.push_back(fObservables[i]);
            fTwistWeightedHighTailPercentage.push_back(tailPercentage);
            fTwistWeightedHighValue.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistLow_X_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(11, fObservables[i].length()).c_str());

            fTwistLowObservables_X.push_back(fObservables[i]);
            fTwistLowTailPercentage_X.push_back(tailPercentage);
            fTwistLowValue_X.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistHigh_X_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(12, fObservables[i].length()).c_str());

            fTwistHighObservables_X.push_back(fObservables[i]);
            fTwistHighTailPercentage_X.push_back(tailPercentage);
            fTwistHighValue_X.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistBalance_X_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(15, fObservables[i].length()).c_str());

            fTwistBalanceObservables_X.push_back(fObservables[i]);
            fTwistBalanceTailPercentage_X.push_back(tailPercentage);
            fTwistBalanceValue_X.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistRatio_X_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(13, fObservables[i].length()).c_str());

            fTwistRatioObservables_X.push_back(fObservables[i]);
            fTwistRatioTailPercentage_X.push_back(tailPercentage);
            fTwistRatioValue_X.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistWeightedLow_X_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(19, fObservables[i].length()).c_str());

            fTwistWeightedLowObservables_X.push_back(fObservables[i]);
            fTwistWeightedLowTailPercentage_X.push_back(tailPercentage);
            fTwistWeightedLowValue_X.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistWeightedHigh_X_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(20, fObservables[i].length()).c_str());

            fTwistWeightedHighObservables_X.push_back(fObservables[i]);
            fTwistWeightedHighTailPercentage_X.push_back(tailPercentage);
            fTwistWeightedHighValue_X.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistLow_Y_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(11, fObservables[i].length()).c_str());

            fTwistLowObservables_Y.push_back(fObservables[i]);
            fTwistLowTailPercentage_Y.push_back(tailPercentage);
            fTwistLowValue_Y.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistHigh_Y_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(12, fObservables[i].length()).c_str());

            fTwistHighObservables_Y.push_back(fObservables[i]);
            fTwistHighTailPercentage_Y.push_back(tailPercentage);
            fTwistHighValue_Y.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistBalance_Y_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(15, fObservables[i].length()).c_str());

            fTwistBalanceObservables_Y.push_back(fObservables[i]);
            fTwistBalanceTailPercentage_Y.push_back(tailPercentage);
            fTwistBalanceValue_Y.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistRatio_Y_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(13, fObservables[i].length()).c_str());

            fTwistRatioObservables_Y.push_back(fObservables[i]);
            fTwistRatioTailPercentage_Y.push_back(tailPercentage);
            fTwistRatioValue_Y.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistWeightedLow_Y_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(19, fObservables[i].length()).c_str());

            fTwistWeightedLowObservables_Y.push_back(fObservables[i]);
            fTwistWeightedLowTailPercentage_Y.push_back(tailPercentage);
            fTwistWeightedLowValue_Y.push_back(0);
        }

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("twistWeightedHigh_Y_") != string::npos) {
            Double_t tailPercentage =
                StringToDouble(fObservables[i].substr(20, fObservables[i].length()).c_str());

            fTwistWeightedHighObservables_Y.push_back(fObservables[i]);
            fTwistWeightedHighTailPercentage_Y.push_back(tailPercentage);
            fTwistWeightedHighValue_Y.push_back(0);
        }
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::BeginOfEventProcess() { fOutputTrackEvent->Initialize(); }

//______________________________________________________________________________
TRestEvent* TRestTrackAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    TRestTrackEvent* fInputTrackEvent = (TRestTrackEvent*)evInput;

    // Copying the input tracks to the output track
    for (int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++)
        fOutputTrackEvent->AddTrack(fInputTrackEvent->GetTrack(tck));

    if (this->GetVerboseLevel() >= REST_Debug) fInputTrackEvent->PrintOnlyTracks();

    TString obsName;

    /* {{{ Number of tracks observables */
    Int_t nTracksX = 0, nTracksY = 0, nTracksXYZ = 0;
    nTracksX = fInputTrackEvent->GetNumberOfTracks("X");
    nTracksY = fInputTrackEvent->GetNumberOfTracks("Y");
    nTracksXYZ = fInputTrackEvent->GetNumberOfTracks("XYZ");

    //obsName = this->GetName() + (TString) ".nTracks_X";
    SetObservableValue("nTracks_X", nTracksX);

    //obsName = this->GetName() + (TString) ".nTracks_Y";
    SetObservableValue("nTracks_Y", nTracksY);

    //obsName = this->GetName() + (TString) ".nTracks_XYZ";
    SetObservableValue("nTracks_XYZ", nTracksXYZ);
    /* }}} */

    if (fCutsEnabled) {
        if (nTracksX < fNTracksXCut.X() || nTracksX > fNTracksXCut.Y()) return NULL;
        if (nTracksY < fNTracksYCut.X() || nTracksY > fNTracksYCut.Y()) return NULL;
    }

    /* {{{ Producing nTracks above/below threshold ( nTracls_LE/HE_XXX ) */
    for (unsigned int n = 0; n < nTracks_HE.size(); n++) nTracks_HE[n] = 0;

    for (unsigned int n = 0; n < nTracks_LE.size(); n++) nTracks_LE[n] = 0;

    for (unsigned int n = 0; n < nTracks_En.size(); n++) nTracks_En[n] = 0;

    for (int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++) {
        if (!fInputTrackEvent->isTopLevel(tck)) continue;

        TRestTrack* t = fInputTrackEvent->GetTrack(tck);
        Double_t en = t->GetEnergy();

        if ((t->isXYZ()) || (t->isXZ()) || (t->isYZ())) {
            for (unsigned int n = 0; n < fTrack_HE_EnergyObservables.size(); n++)
                if (en > fTrack_HE_Threshold[n]) nTracks_HE[n]++;

            for (unsigned int n = 0; n < fTrack_LE_EnergyObservables.size(); n++)
                if (en < fTrack_LE_Threshold[n]) nTracks_LE[n]++;

            for (unsigned int n = 0; n < fTrack_En_EnergyObservables.size(); n++)
                if (en > fTrack_En_Threshold[n] - fDeltaEnergy && en < fTrack_En_Threshold[n] + fDeltaEnergy)
                    nTracks_En[n]++;
        }
    }

    for (unsigned int n = 0; n < fTrack_LE_EnergyObservables.size(); n++) {
        SetObservableValue(fTrack_LE_EnergyObservables[n], nTracks_LE[n]);
    }

    for (unsigned int n = 0; n < fTrack_HE_EnergyObservables.size(); n++) {
        SetObservableValue(fTrack_HE_EnergyObservables[n], nTracks_HE[n]);
    }

    for (unsigned int n = 0; n < fTrack_En_EnergyObservables.size(); n++) {
        SetObservableValue(fTrack_En_EnergyObservables[n], nTracks_En[n]);
    }
    /* }}} */

    TRestTrack* tXYZ = fInputTrackEvent->GetMaxEnergyTrack();
    TRestTrack* tX = fInputTrackEvent->GetMaxEnergyTrack("X");
    TRestTrack* tY = fInputTrackEvent->GetMaxEnergyTrack("Y");

    if (fEnableTwistParameters) {
        /* {{{ Adding twist observables from XYZ track */

        Double_t twist = -1, twistWeighted = -1;

        for (unsigned int n = 0; n < fTwistWeightedHighValue.size(); n++) fTwistWeightedHighValue[n] = -1;

        for (unsigned int n = 0; n < fTwistWeightedLowValue.size(); n++) fTwistWeightedLowValue[n] = -1;

        for (unsigned int n = 0; n < fTwistLowValue.size(); n++) fTwistLowValue[n] = -1;

        for (unsigned int n = 0; n < fTwistHighValue.size(); n++) fTwistHighValue[n] = -1;

        for (unsigned int n = 0; n < fTwistBalanceValue.size(); n++) fTwistBalanceValue[n] = -1;

        for (unsigned int n = 0; n < fTwistRatioValue.size(); n++) fTwistRatioValue[n] = -1;

        if (tXYZ) {
            TRestVolumeHits* hits = tXYZ->GetVolumeHits();
            Int_t Nhits = hits->GetNumberOfHits();

            twist = hits->GetHitsTwist(0, 0);
            twistWeighted = hits->GetHitsTwistWeighted(0, 0);

            for (unsigned int n = 0; n < fTwistLowObservables.size(); n++) {
                Int_t Nend = fTwistLowTailPercentage[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwist(0, Nend);
                Double_t twistEnd = hits->GetHitsTwist(Nhits - Nend, Nhits);

                if (twistStart < twistEnd)
                    fTwistLowValue[n] = twistStart;
                else
                    fTwistLowValue[n] = twistEnd;
            }

            for (unsigned int n = 0; n < fTwistHighObservables.size(); n++) {
                Int_t Nend = fTwistHighTailPercentage[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwist(0, Nend);
                Double_t twistEnd = hits->GetHitsTwist(Nhits - Nend, Nhits);

                if (twistStart > twistEnd)
                    fTwistHighValue[n] = twistStart;
                else
                    fTwistHighValue[n] = twistEnd;
            }

            for (unsigned int n = 0; n < fTwistBalanceObservables.size(); n++) {
                Int_t Nend = fTwistBalanceTailPercentage[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwist(0, Nend);
                Double_t twistEnd = hits->GetHitsTwist(Nhits - Nend, Nhits);

                if (twistStart < twistEnd)
                    fTwistBalanceValue[n] = (twistEnd - twistStart) / (twistEnd + twistStart);
                else
                    fTwistBalanceValue[n] = (twistStart - twistEnd) / (twistEnd + twistStart);
            }

            for (unsigned int n = 0; n < fTwistRatioObservables.size(); n++) {
                Int_t Nend = fTwistRatioTailPercentage[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwist(0, Nend);
                Double_t twistEnd = hits->GetHitsTwist(Nhits - Nend, Nhits);

                if (twistStart > twistEnd) {
                    if (twistEnd <= 0) twistEnd = -1;
                    fTwistRatioValue[n] = twistStart / twistEnd;
                } else {
                    if (twistStart <= 0) twistStart = -1;
                    fTwistRatioValue[n] = twistEnd / twistStart;
                }
            }

            for (unsigned int n = 0; n < fTwistWeightedLowObservables.size(); n++) {
                Int_t Nend = fTwistWeightedLowTailPercentage[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwistWeighted(0, Nend);
                Double_t twistEnd = hits->GetHitsTwistWeighted(Nhits - Nend, Nhits);

                if (twistStart < twistEnd)
                    fTwistWeightedLowValue[n] = twistStart;
                else
                    fTwistWeightedLowValue[n] = twistEnd;
            }

            for (unsigned int n = 0; n < fTwistWeightedHighObservables.size(); n++) {
                Int_t Nend = fTwistWeightedHighTailPercentage[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwistWeighted(0, Nend);
                Double_t twistEnd = hits->GetHitsTwistWeighted(Nhits - Nend, Nhits);

                if (twistStart > twistEnd)
                    fTwistWeightedHighValue[n] = twistStart;
                else
                    fTwistWeightedHighValue[n] = twistEnd;
            }
        }

        for (unsigned int n = 0; n < fTwistLowObservables.size(); n++) {
            SetObservableValue((string)fTwistLowObservables[n], fTwistLowValue[n]);
        }

        for (unsigned int n = 0; n < fTwistHighObservables.size(); n++) {

            SetObservableValue((string)fTwistHighObservables[n], fTwistHighValue[n]);
        }

        for (unsigned int n = 0; n < fTwistBalanceObservables.size(); n++) {
            SetObservableValue((string)fTwistBalanceObservables[n], fTwistBalanceValue[n]);
        }

        for (unsigned int n = 0; n < fTwistRatioObservables.size(); n++) {
            SetObservableValue((string)fTwistRatioObservables[n], fTwistRatioValue[n]);
        }

        for (unsigned int n = 0; n < fTwistWeightedLowObservables.size(); n++) {
            SetObservableValue((string)fTwistWeightedLowObservables[n], fTwistWeightedLowValue[n]);
        }

        for (unsigned int n = 0; n < fTwistWeightedHighObservables.size(); n++) {
            SetObservableValue((string)fTwistWeightedHighObservables[n], fTwistWeightedHighValue[n]);
        }

        obsName = this->GetName() + (TString) ".twist";
        SetObservableValue("twist", twist);

        obsName = this->GetName() + (TString) ".twistWeighted";
        SetObservableValue("twistWeighted", twistWeighted);
        /* }}} */

        /* {{{ Adding twist observables from X track */
        Double_t twist_X = -1, twistWeighted_X = -1;

        for (unsigned int n = 0; n < fTwistWeightedHighValue_X.size(); n++) fTwistWeightedHighValue_X[n] = -1;

        for (unsigned int n = 0; n < fTwistWeightedLowValue_X.size(); n++) fTwistWeightedLowValue_X[n] = -1;

        for (unsigned int n = 0; n < fTwistLowValue_X.size(); n++) fTwistLowValue_X[n] = -1;

        for (unsigned int n = 0; n < fTwistHighValue_X.size(); n++) fTwistHighValue_X[n] = -1;

        for (unsigned int n = 0; n < fTwistBalanceValue_X.size(); n++) fTwistBalanceValue_X[n] = -1;

        for (unsigned int n = 0; n < fTwistRatioValue_X.size(); n++) fTwistRatioValue_X[n] = -1;

        if (tX) {
            TRestVolumeHits* hits = tX->GetVolumeHits();
            Int_t Nhits = hits->GetNumberOfHits();

            twist_X = hits->GetHitsTwist(0, 0);
            twistWeighted_X = hits->GetHitsTwistWeighted(0, 0);

            for (unsigned int n = 0; n < fTwistLowObservables_X.size(); n++) {
                Int_t Nend = fTwistLowTailPercentage_X[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwist(0, Nend);
                Double_t twistEnd = hits->GetHitsTwist(Nhits - Nend, Nhits);

                if (twistStart < twistEnd)
                    fTwistLowValue_X[n] = twistStart;
                else
                    fTwistLowValue_X[n] = twistEnd;
            }

            for (unsigned int n = 0; n < fTwistHighObservables_X.size(); n++) {
                Int_t Nend = fTwistHighTailPercentage_X[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwist(0, Nend);
                Double_t twistEnd = hits->GetHitsTwist(Nhits - Nend, Nhits);

                if (twistStart > twistEnd)
                    fTwistHighValue_X[n] = twistStart;
                else
                    fTwistHighValue_X[n] = twistEnd;
            }

            for (unsigned int n = 0; n < fTwistBalanceObservables_X.size(); n++) {
                Int_t Nend = fTwistBalanceTailPercentage_X[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwist(0, Nend);
                Double_t twistEnd = hits->GetHitsTwist(Nhits - Nend, Nhits);

                if (twistStart < twistEnd)
                    fTwistBalanceValue_X[n] = (twistEnd - twistStart) / (twistEnd + twistStart);
                else
                    fTwistBalanceValue_X[n] = (twistStart - twistEnd) / (twistEnd + twistStart);
            }

            for (unsigned int n = 0; n < fTwistRatioObservables_X.size(); n++) {
                Int_t Nend = fTwistRatioTailPercentage_X[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwist(0, Nend);
                Double_t twistEnd = hits->GetHitsTwist(Nhits - Nend, Nhits);

                if (twistStart > twistEnd) {
                    if (twistEnd <= 0) twistEnd = -1;
                    fTwistRatioValue[n] = twistStart / twistEnd;
                } else {
                    if (twistStart <= 0) twistStart = -1;
                    fTwistRatioValue[n] = twistEnd / twistStart;
                }
            }

            for (unsigned int n = 0; n < fTwistWeightedLowObservables_X.size(); n++) {
                Int_t Nend = fTwistWeightedLowTailPercentage_X[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwistWeighted(0, Nend);
                Double_t twistEnd = hits->GetHitsTwistWeighted(Nhits - Nend, Nhits);

                if (twistStart < twistEnd)
                    fTwistWeightedLowValue_X[n] = twistStart;
                else
                    fTwistWeightedLowValue_X[n] = twistEnd;
            }

            for (unsigned int n = 0; n < fTwistWeightedHighObservables_X.size(); n++) {
                Int_t Nend = fTwistWeightedHighTailPercentage_X[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwistWeighted(0, Nend);
                Double_t twistEnd = hits->GetHitsTwistWeighted(Nhits - Nend, Nhits);

                if (twistStart > twistEnd)
                    fTwistWeightedHighValue_X[n] = twistStart;
                else
                    fTwistWeightedHighValue_X[n] = twistEnd;
            }
        }

        for (unsigned int n = 0; n < fTwistLowObservables_X.size(); n++) {
            SetObservableValue((string)fTwistLowObservables_X[n], fTwistLowValue_X[n]);
        }

        for (unsigned int n = 0; n < fTwistHighObservables_X.size(); n++) {
            SetObservableValue((string)fTwistHighObservables_X[n], fTwistHighValue_X[n]);
        }

        for (unsigned int n = 0; n < fTwistBalanceObservables_X.size(); n++) {
            SetObservableValue((string)fTwistBalanceObservables_X[n], fTwistBalanceValue_X[n]);
        }

        for (unsigned int n = 0; n < fTwistRatioObservables_X.size(); n++) {
            SetObservableValue((string)fTwistRatioObservables_X[n], fTwistRatioValue_X[n]);
        }

        for (unsigned int n = 0; n < fTwistWeightedLowObservables_X.size(); n++) {
            SetObservableValue((string)fTwistWeightedLowObservables_X[n], fTwistWeightedLowValue_X[n]);
        }

        for (unsigned int n = 0; n < fTwistWeightedHighObservables_X.size(); n++) {
            SetObservableValue((string)fTwistWeightedHighObservables_X[n], fTwistWeightedHighValue_X[n]);
        }

        obsName = this->GetName() + (TString) ".twist_X";
        SetObservableValue("twist_X", twist_X);

        obsName = this->GetName() + (TString) ".twistWeighted_X";
        SetObservableValue("twistWeighted_X", twistWeighted_X);
        /* }}} */

        /* {{{ Adding twist observables from Y track */
        Double_t twist_Y = -1, twistWeighted_Y = -1;

        for (unsigned int n = 0; n < fTwistWeightedHighValue_Y.size(); n++) fTwistWeightedHighValue_Y[n] = -1;

        for (unsigned int n = 0; n < fTwistWeightedLowValue_Y.size(); n++) fTwistWeightedLowValue_Y[n] = -1;

        for (unsigned int n = 0; n < fTwistLowValue_Y.size(); n++) fTwistLowValue_Y[n] = -1;

        for (unsigned int n = 0; n < fTwistHighValue_Y.size(); n++) fTwistHighValue_Y[n] = -1;

        for (unsigned int n = 0; n < fTwistBalanceValue_Y.size(); n++) fTwistBalanceValue_Y[n] = -1;

        for (unsigned int n = 0; n < fTwistRatioValue_Y.size(); n++) fTwistRatioValue_Y[n] = -1;

        if (tY) {
            TRestVolumeHits* hits = tY->GetVolumeHits();
            Int_t Nhits = hits->GetNumberOfHits();

            twist_Y = hits->GetHitsTwist(0, 0);
            twistWeighted_Y = hits->GetHitsTwistWeighted(0, 0);

            for (unsigned int n = 0; n < fTwistLowObservables_Y.size(); n++) {
                Int_t Nend = fTwistLowTailPercentage_Y[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwist(0, Nend);
                Double_t twistEnd = hits->GetHitsTwist(Nhits - Nend, Nhits);

                if (twistStart < twistEnd)
                    fTwistLowValue_Y[n] = twistStart;
                else
                    fTwistLowValue_Y[n] = twistEnd;
            }

            for (unsigned int n = 0; n < fTwistHighObservables_Y.size(); n++) {
                Int_t Nend = fTwistHighTailPercentage_Y[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwist(0, Nend);
                Double_t twistEnd = hits->GetHitsTwist(Nhits - Nend, Nhits);

                if (twistStart > twistEnd)
                    fTwistHighValue_Y[n] = twistStart;
                else
                    fTwistHighValue_Y[n] = twistEnd;
            }

            for (unsigned int n = 0; n < fTwistBalanceObservables_Y.size(); n++) {
                Int_t Nend = fTwistBalanceTailPercentage_Y[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwist(0, Nend);
                Double_t twistEnd = hits->GetHitsTwist(Nhits - Nend, Nhits);

                if (twistStart < twistEnd)
                    fTwistBalanceValue_Y[n] = (twistEnd - twistStart) / (twistEnd + twistStart);
                else
                    fTwistBalanceValue_Y[n] = (twistStart - twistEnd) / (twistEnd + twistStart);
            }

            for (unsigned int n = 0; n < fTwistRatioObservables_Y.size(); n++) {
                Int_t Nend = fTwistRatioTailPercentage_Y[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwist(0, Nend);
                Double_t twistEnd = hits->GetHitsTwist(Nhits - Nend, Nhits);

                if (twistStart > twistEnd) {
                    if (twistEnd <= 0) twistEnd = -1;
                    fTwistRatioValue[n] = twistStart / twistEnd;
                } else {
                    if (twistStart <= 0) twistStart = -1;
                    fTwistRatioValue[n] = twistEnd / twistStart;
                }
            }

            for (unsigned int n = 0; n < fTwistWeightedLowObservables_Y.size(); n++) {
                Int_t Nend = fTwistWeightedLowTailPercentage_Y[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwistWeighted(0, Nend);
                Double_t twistEnd = hits->GetHitsTwistWeighted(Nhits - Nend, Nhits);

                if (twistStart < twistEnd)
                    fTwistWeightedLowValue_Y[n] = twistStart;
                else
                    fTwistWeightedLowValue_Y[n] = twistEnd;
            }

            for (unsigned int n = 0; n < fTwistWeightedHighObservables_Y.size(); n++) {
                Int_t Nend = fTwistWeightedHighTailPercentage_Y[n] * Nhits / 100.;
                Double_t twistStart = hits->GetHitsTwistWeighted(0, Nend);
                Double_t twistEnd = hits->GetHitsTwistWeighted(Nhits - Nend, Nhits);

                if (twistStart > twistEnd)
                    fTwistWeightedHighValue_Y[n] = twistStart;
                else
                    fTwistWeightedHighValue_Y[n] = twistEnd;
            }
        }

        for (unsigned int n = 0; n < fTwistLowObservables_Y.size(); n++) {
            SetObservableValue((string)fTwistLowObservables_Y[n], fTwistLowValue_Y[n]);
        }

        for (unsigned int n = 0; n < fTwistHighObservables_Y.size(); n++) {
            SetObservableValue((string)fTwistHighObservables_Y[n], fTwistHighValue_Y[n]);
        }

        for (unsigned int n = 0; n < fTwistBalanceObservables_Y.size(); n++) {
            SetObservableValue((string)fTwistBalanceObservables_Y[n], fTwistBalanceValue_Y[n]);
        }

        for (unsigned int n = 0; n < fTwistRatioObservables_Y.size(); n++) {
            SetObservableValue((string)fTwistRatioObservables_Y[n], fTwistRatioValue_Y[n]);
        }

        for (unsigned int n = 0; n < fTwistWeightedLowObservables_Y.size(); n++) {
            SetObservableValue((string)fTwistWeightedLowObservables_Y[n], fTwistWeightedLowValue_Y[n]);
        }

        for (unsigned int n = 0; n < fTwistWeightedHighObservables_Y.size(); n++) {
            SetObservableValue((string)fTwistWeightedHighObservables_Y[n], fTwistWeightedHighValue_Y[n]);
        }

        SetObservableValue("twist_Y", twist_Y);

        SetObservableValue("twistWeighted_Y", twistWeighted_Y);
        /* }}} */
    }

    /* {{{ Getting max track energies and track energy ratio */
    Double_t tckMaxEnXYZ = 0, tckMaxEnX = 0, tckMaxEnY = 0;

    if (fInputTrackEvent->GetMaxEnergyTrack())
        tckMaxEnXYZ = fInputTrackEvent->GetMaxEnergyTrack()->GetEnergy();

    SetObservableValue("maxTrackEnergy", tckMaxEnXYZ);

    if (fInputTrackEvent->GetMaxEnergyTrack("X"))
        tckMaxEnX = fInputTrackEvent->GetMaxEnergyTrack("X")->GetEnergy();

    SetObservableValue("maxTrack_X_Energy", tckMaxEnX);

    if (fInputTrackEvent->GetMaxEnergyTrack("Y"))
        tckMaxEnY = fInputTrackEvent->GetMaxEnergyTrack("Y")->GetEnergy();

    SetObservableValue("maxTrack_Y_Energy", tckMaxEnY);

    Double_t tckMaxEnergy = tckMaxEnX + tckMaxEnY + tckMaxEnXYZ;

    Double_t totalEnergy = fInputTrackEvent->GetEnergy();

    Double_t trackEnergyRatio = (totalEnergy - tckMaxEnergy) / totalEnergy;

    SetObservableValue("maxTrackEnergyRatio", trackEnergyRatio);
    /* }}} */

    /* {{{ Maximum Second Track Energy observable */
    Double_t maxSecondTrackEnergy = 0;
    if (fInputTrackEvent->GetSecondMaxEnergyTrack() != NULL)
        maxSecondTrackEnergy = fInputTrackEvent->GetSecondMaxEnergyTrack()->GetEnergy();

    Double_t maxSecondTrackEnergy_X = 0;
    if (fInputTrackEvent->GetSecondMaxEnergyTrack("X") != NULL)
        maxSecondTrackEnergy_X = fInputTrackEvent->GetSecondMaxEnergyTrack("X")->GetEnergy();

    Double_t maxSecondTrackEnergy_Y = 0;
    if (fInputTrackEvent->GetSecondMaxEnergyTrack("Y") != NULL)
        maxSecondTrackEnergy_Y = fInputTrackEvent->GetSecondMaxEnergyTrack("Y")->GetEnergy();

    SetObservableValue("secondTrackMaxEnergy", maxSecondTrackEnergy);

    SetObservableValue("secondTrackMaxEnergy_X", maxSecondTrackEnergy_X);

    SetObservableValue("secondTrackMaxEnergy_Y", maxSecondTrackEnergy_Y);
    /* }}} */

    /* {{{ Track Length observables (MaxTrackLength_XX) */
    Double_t tckLenX = fInputTrackEvent->GetMaxEnergyTrackLength("X");
    Double_t tckLenY = fInputTrackEvent->GetMaxEnergyTrackLength("Y");
    Double_t tckLenXYZ = fInputTrackEvent->GetMaxEnergyTrackLength();

    SetObservableValue("MaxTrackLength_X", tckLenX);

    SetObservableValue("MaxTrackLength_Y", tckLenY);

    SetObservableValue("MaxTrackLength_XYZ", tckLenXYZ);
    /* }}} */

    /* {{{ Track Volume observables (MaxTrackVolume_XX) */
    Double_t tckVolX = fInputTrackEvent->GetMaxEnergyTrackVolume("X");
    Double_t tckVolY = fInputTrackEvent->GetMaxEnergyTrackVolume("Y");
    Double_t tckVolXYZ = fInputTrackEvent->GetMaxEnergyTrackVolume();

    SetObservableValue("MaxTrackVolume_X", tckVolX);

    SetObservableValue("MaxTrackVolume_Y", tckVolY);

    SetObservableValue("MaxTrackVolume_XYZ", tckVolXYZ);
    /* }}} */

    /* {{{ Setting mean position for max energy tracks (MaxTrack_{x,y,z}Mean_XXX)
     */

    /////////////////// XYZ-track //////////////////////////
    Double_t maxX = 0, maxY = 0, maxZ = 0;
    ;

    TRestTrack* tMax = fInputTrackEvent->GetMaxEnergyTrack();

    if (tMax != NULL) {
        maxX = tMax->GetMeanPosition().X();
        maxY = tMax->GetMeanPosition().Y();
        maxZ = tMax->GetMeanPosition().Z();
    }

    SetObservableValue("MaxTrack_Xmean_XYZ", maxX);

    SetObservableValue("MaxTrack_Ymean_XYZ", maxY);

    SetObservableValue("MaxTrack_Zmean_XYZ", maxZ);

    /////////////////// XZ-track //////////////////////////

    maxX = 0, maxY = 0, maxZ = 0;
    tMax = fInputTrackEvent->GetMaxEnergyTrack("X");
    if (tMax != NULL) {
        maxX = tMax->GetMeanPosition().X();
        maxZ = tMax->GetMeanPosition().Z();
    }

    SetObservableValue("MaxTrack_Xmean_X", maxX);

    SetObservableValue("MaxTrack_Zmean_X", maxZ);

    /////////////////// YZ-track //////////////////////////

    maxX = 0, maxY = 0, maxZ = 0;
    tMax = fInputTrackEvent->GetMaxEnergyTrack("Y");
    if (tMax != NULL) {
        maxY = tMax->GetMeanPosition().Y();
        maxZ = tMax->GetMeanPosition().Z();
    }

    SetObservableValue("MaxTrack_Ymean_Y", maxY);

    SetObservableValue("MaxTrack_Zmean_Y", maxZ);

    /////////////////// xMean, yMean and zMean //////////////////////////
    Double_t x = 0, y = 0, z = 0;

    if (tXYZ != NULL) {
        x = tXYZ->GetMeanPosition().X();
        y = tXYZ->GetMeanPosition().Y();
        z = tXYZ->GetMeanPosition().Y();
    } else if (tX != NULL) {
        x = tX->GetMeanPosition().X();
        z = tX->GetMeanPosition().Y();
    } else if (tY != NULL) {
        y = tY->GetMeanPosition().Y();
        z = tY->GetMeanPosition().Y();
    }

    SetObservableValue("xMean", x);

    SetObservableValue("yMean", y);

    SetObservableValue("zMean", z);
    /* }}} */

    /// This kind of observables would be better in a separate process that
    /// measures the trigger rate
    Double_t evTimeDelay = 0;
    if (fPreviousEventTime.size() > 0) evTimeDelay = fInputTrackEvent->GetTime() - fPreviousEventTime.back();
    SetObservableValue("EventTimeDelay", evTimeDelay);

    Double_t meanRate = 0;
    if (fPreviousEventTime.size() == 100)
        meanRate = 100. / (fInputTrackEvent->GetTime() - fPreviousEventTime.front());
    SetObservableValue("MeanRate_InHz", meanRate);

    //   if( GetVerboseLevel() >= REST_Info )
    //   {
    //       cout << "TRestTrackAnalysisProcess : " << GetName() << endl;
    //       cout << "----------------------------------------------" << endl;
    //       fAnalysisTree->PrintObservables();
    // GetChar();
    if (GetVerboseLevel() >= REST_Extreme) GetChar();
    //}

    return fOutputTrackEvent;
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::EndOfEventProcess() {
    fPreviousEventTime.push_back(fInputTrackEvent->GetTimeStamp());
    if (fPreviousEventTime.size() > 100) fPreviousEventTime.erase(fPreviousEventTime.begin());
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::InitFromConfigFile() {
    fNTracksXCut = StringTo2DVector(GetParameter("nTracksXCut", "(1,10)"));
    fNTracksYCut = StringTo2DVector(GetParameter("nTracksYCut", "(1,10)"));
    fDeltaEnergy = GetDblParameterWithUnits("deltaEnergy", 1);

    if (GetParameter("cutsEnabled", "false") == "true") fCutsEnabled = true;

    if (GetParameter("enableTwistParameters", "false") == "true") fEnableTwistParameters = true;
}
