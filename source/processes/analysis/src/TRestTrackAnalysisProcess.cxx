/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

/////////////////////////////////////////////////////////////////////////////
///
/// ### Observables
///
/// Energy and number of tracks:
///
/// * **trackEnergy**: Energy of the track (fTrackEnergy)
/// * **nTracks_X**: Number of tracks in the event with hits' coordinates only in X and Z (not Y).
/// * **nTracks_Y**: Number of tracks in the event with hits' coordinates only in Y and Z (not X).
/// * **nTracks_XYZ**: Number of tracks in the event with hits' coordinates in X, Y and Z.
///
/// Max track energies and track energy ratio:
///
/// * **MaxTrackEnergy**: Energy of the most energetic track in the event with X, Y, Z coordinates.
/// * **MaxTrackEnergy_X**: Energy of the most energetic track in the event with X, Z coordinates.
/// * **MaxTrackEnergy_Y**: Energy of the most energetic track in the event with Y, Z coordinates.
/// * **MaxTrackEnergyRatio**: (totalEnergy - tckMaxEnergy) / totalEnergy
/// with tckMaxEnergy = tckMaxEnX + tckMaxEnY + tckMaxEnXYZ.
///
/// Maximum Second Track Energy observables:
///
/// * **SecondTrackMaxEnergy**: Energy of the second most energetic track in the event with X,Y,Z
/// coordinates.
/// * **SecondTrackMaxEnergy_X**: Energy of the second most energetic track in the event with X,Z
/// coordinates.
/// * **SecondTrackMaxEnergy_Y**: Energy of the second most energetic track in the event with Y,Z
/// coordinates.
///
/// Track Length observables:
///
/// * **MaxTrackLength_X**: Length of the most energetic track in the event with X,Z coordinates.
/// * **MaxTrackLength_Y**: Length of the most energetic track in the event with Y,Z coordinates.
/// * **MaxTrackLength_XYZ**: Length of the most energetic track in the event with X,Y,Z coordinates.
///
/// Track Volume observables:
///
/// * **MaxTrackVolume_X**: Squared distance of the most separated hits in the most energetic track
/// in the event with X, Z coordinates.
/// * **MaxTrackVolume_Y**: Squared distance of the most separated hits in the most energetic track
/// in the event with Y, Z coordinates.
/// * **MaxTrackVolume_XYZ**: Squared distance of the most separated hits in the most energetic track
/// in the event with X, Y, Z coordinates.
///
/// Mean position for max energy tracks:
///
/// * **MaxTrack_Xmean_XYZ**: X coordinate in the mean position of the most energetic track with X,Y,Z
/// in the event.
/// * **MaxTrack_Ymean_XYZ**: Y coordinate in the mean position of the most energetic track with X,Y,Z
/// in the event.
/// * **MaxTrack_Zmean_XYZ**: Z coordinate in the mean position of the most energetic track with X,Y,Z
/// in the event.
/// * **MaxTrack_Xmean_X**: X coordinate in the mean position of the most energetic track with only X
/// and Z coord in the event.
/// * **MaxTrack_Zmean_X**: Z coordinate in the mean position of the most energetic track with only X
/// and Z coord in the event.
/// * **MaxTrack_Ymean_Y**: Y coordinate in the mean position of the most energetic track with only Y
/// and Z coord in the event.
/// * **MaxTrack_Zmean_Y**: Z coordinate in the mean position of the most energetic track with only Y
/// and Z coord in the event.
///
/// Mean position for second max energy tracks:
///
/// * **SecondMaxTrack_Xmean_XYZ**: X coordinate in the mean position of the second most energetic track
/// with X,Y,Z in the event.
/// * **SecondMaxTrack_Ymean_XYZ**: Y coordinate in the mean position of the second most energetic track
/// with X,Y,Z in the event.
/// * **SecondMaxTrack_Zmean_XYZ**: Z coordinate in the mean position of the second most energetic track
/// with X,Y,Z in the event.
/// * **SecondMaxTrack_Xmean_X**: X coordinate in the mean position of the second most energetic track
/// with only X and Z coord in the event.
/// * **SecondMaxTrack_Zmean_X**: Z coordinate in the mean position of the second most energetic track
/// with only X and Z coord in the event.
/// * **SecondMaxTrack_Ymean_Y**: Y coordinate in the mean position of the second most energetic track
/// with only Y and Z coord in the event.
/// * **SecondMaxTrack_Zmean_Y**: Z coordinate in the mean position of the second most energetic track
/// with only Y and Z coord in the event.
///
/// Mean position: Very similar to the previous observables, main difference all tracks (XYZ, XZ, YZ)
/// together.
///
/// * **xMean**: X coordinate (if it has) in the mean position of the most energetic track in the event.
/// * **yMean**: Y coordinate (if it has) in the mean position of the most energetic track in the event.
/// * **zMean**: Z coordinate (if it has) in the mean position of the most energetic track in the event.
///
/// Main track distance measures the distance in X,Y,Z between the most energetic and second most
/// energetic tracks.
///
/// * **TracksDistance_Xmean_XYZ**: Distance between tracks in X-coordinate for XYZ tracks.
/// * **TracksDistance_Ymean_XYZ**: Distance between tracks in Y-coordinate for XYZ tracks.
/// * **TracksDistance_Zmean_XYZ**: Distance between tracks in Z-coordinate for XYZ tracks.
/// * **TracksDistance_Xmean_X**: Distance between tracks in X-coordinate for XZ tracks.
/// * **TracksDistance_Zmean_X**: Distance between tracks in X-coordinate for XZ tracks.
/// * **TracksDistance_Ymean_Y**: Distance between tracks in Y-coordinate for YZ tracks.
/// * **TracksDistance_Zmean_Y**: Distance between tracks in Y-coordinate for YZ tracks.
/// * **TracksDistance_XZ**: Distance between tracks in the XZ projection.
/// * **TracksDistance_YZ**: Distance between tracks in the YZ projection.
/// * **TracksDistance_XYZ**: Euclidean distance between the main XYZ-tracks.
/// * **TracksDistance_XZ_YZ**: Euclidean distance between main tracks. XZ+YZ projections added.
///
/// The sigma parameter measures the cluster size obtained from a given track hits weighted with each hit
/// energy.
///
/// * **MaxTrackSigmaX**: The cluster size in X of the main most energetic track.
/// * **MaxTrackSigmaY**: The cluster size in Y of the main most energetic track.
/// * **MaxTrack_XZ_SigmaX**: The cluster size in X of the main most energetic track in XZ projection.
/// * **MaxTrack_YZ_SigmaY**: The cluster size in Y of the main most energetic track in YZ projection.
/// * **SecondTrackMaxSigmaX**: The cluster size in X of the second most energetic track.
/// * **SecondTrackMaxSigmaY**: The cluster size in Y of the second most energetic track.
/// * **SecondTrackMax_XZ_SigmaX**: The cluster size in X of the second most energetic track in XZ projection.
/// * **SecondTrackMax_YZ_SigmaY**: The cluster size in Y of the second most energetic track in YZ projection.
///
/// Time observables:
///
/// * **EventTimeDelay**: Time between the event and the previous event.
/// * **MeanRate_InHz**: Rate of events in the last 100 events.
///
///______________________________________________________________________________
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-March:  First implementation of Geant4 analysis process into REST_v2.
///             Javier Galan
///
/// 2018-May: Added twist parameters.
///
/// \class      TRestTrackAnalysisProcess
/// \author     Javier Galan
///
///______________________________________________________________________________
///
/////////////////////////////////////////////////////////////////////////////

#include "TRestTrackAnalysisProcess.h"
using namespace std;

ClassImp(TRestTrackAnalysisProcess);
//______________________________________________________________________________
TRestTrackAnalysisProcess::TRestTrackAnalysisProcess() { Initialize(); }

//______________________________________________________________________________
TRestTrackAnalysisProcess::TRestTrackAnalysisProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestTrackAnalysisProcess::~TRestTrackAnalysisProcess() { delete fOutputTrackEvent; }

void TRestTrackAnalysisProcess::LoadDefaultConfig() { SetTitle("Default config"); }

//______________________________________________________________________________
void TRestTrackAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputTrackEvent = NULL;
    fOutputTrackEvent = new TRestTrackEvent();

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
TRestEvent* TRestTrackAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fInputTrackEvent = (TRestTrackEvent*)evInput;

    // Copying the input tracks to the output track
    for (int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++)
        fOutputTrackEvent->AddTrack(fInputTrackEvent->GetTrack(tck));

    if (this->GetVerboseLevel() >= REST_Debug) fInputTrackEvent->PrintOnlyTracks();

    /* {{{ Number of tracks observables */
    Int_t nTracksX = 0, nTracksY = 0, nTracksXYZ = 0;
    nTracksX = fInputTrackEvent->GetNumberOfTracks("X");
    nTracksY = fInputTrackEvent->GetNumberOfTracks("Y");
    nTracksXYZ = fInputTrackEvent->GetNumberOfTracks("XYZ");

    SetObservableValue((string) "trackEnergy", fInputTrackEvent->GetEnergy(""));

    SetObservableValue((string) "nTracks_X", nTracksX);
    SetObservableValue((string) "nTracks_Y", nTracksY);
    SetObservableValue((string) "nTracks_XYZ", nTracksXYZ);
    /* }}} */

    /* {{{ Producing nTracks above/below threshold ( nTracks_LE/HE_XXX ) */
    for (unsigned int n = 0; n < nTracks_HE.size(); n++) nTracks_HE[n] = 0;

    for (unsigned int n = 0; n < nTracks_LE.size(); n++) nTracks_LE[n] = 0;

    for (unsigned int n = 0; n < nTracks_En.size(); n++) nTracks_En[n] = 0;

    for (int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++) {
        if (!fInputTrackEvent->isTopLevel(tck)) continue;

        TRestTrack* t = fInputTrackEvent->GetTrack(tck);
        Double_t en = t->GetEnergy();

        if ((t->isXYZ()) || (t->isXZ()) || (t->isYZ())) {
            // cout<<"tracks "<<endl;
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

    TRestTrack* tXYZ = fInputTrackEvent->GetMaxEnergyTrack("XYZ");
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

        SetObservableValue("twist", twist);

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

        SetObservableValue((string) "twist_X", twist_X);

        SetObservableValue((string) "twistWeighted_X", twistWeighted_X);
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
    Double_t tckMaxXYZ_SigmaX = 0, tckMaxXYZ_SigmaY = 0;
    Double_t tckMaxXZ_SigmaX = 0;
    Double_t tckMaxYZ_SigmaY = 0;

    if (fInputTrackEvent->GetMaxEnergyTrack()) {
        tckMaxEnXYZ = fInputTrackEvent->GetMaxEnergyTrack()->GetEnergy();
        tckMaxXYZ_SigmaX = fInputTrackEvent->GetMaxEnergyTrack()->GetHits()->GetSigmaX();
        tckMaxXYZ_SigmaY = fInputTrackEvent->GetMaxEnergyTrack()->GetHits()->GetSigmaY();
        debug << "id: " << fInputTrackEvent->GetID() << " " << fInputTrackEvent->GetSubEventTag()
              << " tckMaxEnXYZ: " << tckMaxEnXYZ << endl;
    }

    SetObservableValue((string) "MaxTrackEnergy", tckMaxEnXYZ);
    SetObservableValue((string) "MaxTrackSigmaX", tckMaxXYZ_SigmaX);
    SetObservableValue((string) "MaxTrackSigmaY", tckMaxXYZ_SigmaY);

    if (fInputTrackEvent->GetMaxEnergyTrack("X")) {
        tckMaxEnX = fInputTrackEvent->GetMaxEnergyTrack("X")->GetEnergy();
        tckMaxXZ_SigmaX = fInputTrackEvent->GetMaxEnergyTrack("X")->GetHits()->GetSigmaX();
        debug << "id: " << fInputTrackEvent->GetID() << " " << fInputTrackEvent->GetSubEventTag()
              << " tckMaxEnX: " << tckMaxEnX << endl;
    }

    SetObservableValue((string) "MaxTrackEnergy_X", tckMaxEnX);
    SetObservableValue((string) "MaxTrack_XZ_SigmaX", tckMaxXZ_SigmaX);

    if (fInputTrackEvent->GetMaxEnergyTrack("Y")) {
        tckMaxEnY = fInputTrackEvent->GetMaxEnergyTrack("Y")->GetEnergy();
        tckMaxYZ_SigmaY = fInputTrackEvent->GetMaxEnergyTrack("Y")->GetHits()->GetSigmaY();
        debug << "id: " << fInputTrackEvent->GetID() << " " << fInputTrackEvent->GetSubEventTag()
              << " tckMaxEnY: " << tckMaxEnY << endl;
    }

    SetObservableValue((string) "MaxTrackEnergy_Y", tckMaxEnY);
    SetObservableValue((string) "MaxTrack_YZ_SigmaY", tckMaxYZ_SigmaY);

    Double_t tckMaxEnergy = tckMaxEnX + tckMaxEnY + tckMaxEnXYZ;

    Double_t totalEnergy = fInputTrackEvent->GetEnergy();

    Double_t trackEnergyRatio = (totalEnergy - tckMaxEnergy) / totalEnergy;

    SetObservableValue((string) "MaxTrackEnergyRatio", trackEnergyRatio);
    /* }}} */

    /* {{{ Maximum Second Track Energy observable */
    Double_t maxSecondTrackEnergy = 0;
    Double_t tckSecondMaxXYZ_SigmaX = 0, tckSecondMaxXYZ_SigmaY = 0;
    if (fInputTrackEvent->GetSecondMaxEnergyTrack() != NULL) {
        tckSecondMaxXYZ_SigmaX = fInputTrackEvent->GetSecondMaxEnergyTrack()->GetHits()->GetSigmaX();
        tckSecondMaxXYZ_SigmaY = fInputTrackEvent->GetSecondMaxEnergyTrack()->GetHits()->GetSigmaY();
        maxSecondTrackEnergy = fInputTrackEvent->GetSecondMaxEnergyTrack()->GetEnergy();
    }

    Double_t maxSecondTrackEnergy_X = 0;
    Double_t tckSecondMaxXZ_SigmaX = 0;
    if (fInputTrackEvent->GetSecondMaxEnergyTrack("X") != NULL) {
        tckSecondMaxXZ_SigmaX = fInputTrackEvent->GetSecondMaxEnergyTrack("X")->GetHits()->GetSigmaX();
        maxSecondTrackEnergy_X = fInputTrackEvent->GetSecondMaxEnergyTrack("X")->GetEnergy();
    }

    Double_t maxSecondTrackEnergy_Y = 0;
    Double_t tckSecondMaxYZ_SigmaY = 0;
    if (fInputTrackEvent->GetSecondMaxEnergyTrack("Y") != NULL) {
        tckSecondMaxYZ_SigmaY = fInputTrackEvent->GetSecondMaxEnergyTrack("Y")->GetHits()->GetSigmaY();
        maxSecondTrackEnergy_Y = fInputTrackEvent->GetSecondMaxEnergyTrack("Y")->GetEnergy();
    }

    SetObservableValue((string) "SecondTrackMaxEnergy", maxSecondTrackEnergy);
    SetObservableValue((string) "SecondTrackMaxSigmaX", tckSecondMaxXYZ_SigmaX);
    SetObservableValue((string) "SecondTrackMaxSigmaY", tckSecondMaxXYZ_SigmaY);

    SetObservableValue((string) "SecondTrackMaxEnergy_X", maxSecondTrackEnergy_X);
    SetObservableValue((string) "SecondTrackMax_XZ_SigmaX", tckSecondMaxXZ_SigmaX);

    SetObservableValue((string) "SecondTrackMaxEnergy_Y", maxSecondTrackEnergy_Y);
    SetObservableValue((string) "SecondTrackMax_YZ_SigmaY", tckSecondMaxYZ_SigmaY);
    /* }}} */

    /* {{{ Track Length observables (MaxTrackLength_XX) */
    Double_t tckLenX = fInputTrackEvent->GetMaxEnergyTrackLength("X");
    Double_t tckLenY = fInputTrackEvent->GetMaxEnergyTrackLength("Y");
    Double_t tckLenXYZ = fInputTrackEvent->GetMaxEnergyTrackLength();

    SetObservableValue((string) "MaxTrackLength_X", tckLenX);
    SetObservableValue((string) "MaxTrackLength_Y", tckLenY);
    SetObservableValue((string) "MaxTrackLength_XYZ", tckLenXYZ);
    /* }}} */

    /* {{{ Track Volume observables (MaxTrackVolume_XX) */
    Double_t tckVolX = fInputTrackEvent->GetMaxEnergyTrackVolume("X");
    Double_t tckVolY = fInputTrackEvent->GetMaxEnergyTrackVolume("Y");
    Double_t tckVolXYZ = fInputTrackEvent->GetMaxEnergyTrackVolume();

    SetObservableValue((string) "MaxTrackVolume_X", tckVolX);
    SetObservableValue((string) "MaxTrackVolume_Y", tckVolY);
    SetObservableValue((string) "MaxTrackVolume_XYZ", tckVolXYZ);
    /* }}} */

    /* {{{ Setting mean position for max energy tracks (MaxTrack_{x,y,z}Mean_XXX)
     */

    /////////////////// XYZ-track //////////////////////////
    Double_t maxX = 0, maxY = 0, maxZ = 0;
    Double_t sMaxX = 0, sMaxY = 0, sMaxZ = 0;
    Double_t dX = 0, dY = 0, dZ = 0;
    Double_t dXZ = 0, dYZ = 0, dXYZ = 0;

    // Main max track
    TRestTrack* tMax = fInputTrackEvent->GetMaxEnergyTrack();

    if (tMax != NULL) {
        maxX = tMax->GetMeanPosition().X();
        maxY = tMax->GetMeanPosition().Y();
        maxZ = tMax->GetMeanPosition().Z();
    }

    SetObservableValue((string) "MaxTrack_Xmean_XYZ", maxX);
    SetObservableValue((string) "MaxTrack_Ymean_XYZ", maxY);
    SetObservableValue((string) "MaxTrack_Zmean_XYZ", maxZ);

    // Second max track
    TRestTrack* tSecondMax = fInputTrackEvent->GetSecondMaxEnergyTrack();

    if (tSecondMax != NULL) {
        sMaxX = tSecondMax->GetMeanPosition().X();
        sMaxY = tSecondMax->GetMeanPosition().Y();
        sMaxZ = tSecondMax->GetMeanPosition().Z();
    }

    SetObservableValue((string) "SecondMaxTrack_Xmean_XYZ", sMaxX);
    SetObservableValue((string) "SecondMaxTrack_Ymean_XYZ", sMaxY);
    SetObservableValue((string) "SecondMaxTrack_Zmean_XYZ", sMaxZ);

    if (sMaxX != 0) dX = abs(maxX - sMaxX);
    if (sMaxY != 0) dY = abs(maxY - sMaxY);
    if (sMaxZ != 0) dZ = abs(maxZ - sMaxZ);

    if (dX != 0 || dY != 0 || dZ != 0) dXYZ = TMath::Sqrt(dX * dX + dY * dY + dZ * dZ);
    SetObservableValue((string) "TracksDistance_XYZ", dXYZ);

    /////////////////// XZ-track //////////////////////////

    maxX = 0, maxY = 0, maxZ = 0;
    dX = 0, dY = 0, dZ = 0;
    tMax = fInputTrackEvent->GetMaxEnergyTrack("X");
    if (tMax != NULL) {
        maxX = tMax->GetMeanPosition().X();
        maxZ = tMax->GetMeanPosition().Z();
    }

    SetObservableValue((string) "MaxTrack_Xmean_X", maxX);
    SetObservableValue((string) "MaxTrack_Zmean_X", maxZ);

    sMaxX = 0, sMaxY = 0, sMaxZ = 0;
    tSecondMax = fInputTrackEvent->GetSecondMaxEnergyTrack("X");
    if (tSecondMax != NULL) {
        sMaxX = tSecondMax->GetMeanPosition().X();
        sMaxZ = tSecondMax->GetMeanPosition().Z();
    }

    SetObservableValue((string) "SecondMaxTrack_Xmean_X", sMaxX);
    SetObservableValue((string) "SecondMaxTrack_Zmean_X", sMaxZ);

    if (sMaxX != 0) dX = abs(maxX - sMaxX);
    if (sMaxZ != 0) dZ = abs(maxZ - sMaxZ);

    if (dX != 0 || dY != 0 || dZ != 0) dXZ = TMath::Sqrt(dX * dX + dZ * dZ);
    SetObservableValue((string) "TracksDistance_XZ", dXZ);

    /////////////////// YZ-track //////////////////////////

    maxX = 0, maxY = 0, maxZ = 0;
    dX = 0, dY = 0, dZ = 0;

    tMax = fInputTrackEvent->GetMaxEnergyTrack("Y");
    if (tMax != NULL) {
        maxY = tMax->GetMeanPosition().Y();
        maxZ = tMax->GetMeanPosition().Z();
    }

    SetObservableValue((string) "MaxTrack_Ymean_Y", maxY);
    SetObservableValue((string) "MaxTrack_Zmean_Y", maxZ);

    sMaxX = 0, sMaxY = 0, sMaxZ = 0;
    tSecondMax = fInputTrackEvent->GetSecondMaxEnergyTrack("Y");
    if (tSecondMax != NULL) {
        sMaxY = tSecondMax->GetMeanPosition().Y();
        sMaxZ = tSecondMax->GetMeanPosition().Z();
    }

    SetObservableValue((string) "SecondMaxTrack_Ymean_Y", sMaxY);
    SetObservableValue((string) "SecondMaxTrack_Zmean_Y", sMaxZ);

    if (sMaxY != 0) dY = abs(maxY - sMaxY);
    if (sMaxZ != 0) dZ = abs(maxZ - sMaxZ);

    if (dX != 0 || dY != 0 || dZ != 0) dYZ = TMath::Sqrt(dY * dY + dZ * dZ);
    SetObservableValue((string) "TracksDistance_YZ", dYZ);

    Double_t dXZ_YZ = 0.5 * TMath::Sqrt(dYZ * dYZ + dXZ * dXZ);
    SetObservableValue((string) "TracksDistance_XZ_YZ", dXZ_YZ);

    /////////////////// xMean, yMean and zMean //////////////////////////
    Double_t x = 0, y = 0, z = 0;

    if (tXYZ != NULL) {
        x = tXYZ->GetMeanPosition().X();
        y = tXYZ->GetMeanPosition().Y();
        z = tXYZ->GetMeanPosition().Z();
    } else {
        double zxz = 0;
        int i = 0;
        if (tX != NULL) {
            x = tX->GetMeanPosition().X();
            zxz += tX->GetMeanPosition().Z();
            i++;
        }
        if (tY != NULL) {
            y = tY->GetMeanPosition().Y();
            zxz += tY->GetMeanPosition().Z();
            i++;
        }

        z = i == 0 ? i : zxz / i;
    }

    SetObservableValue((string) "xMean", x);

    SetObservableValue((string) "yMean", y);

    SetObservableValue((string) "zMean", z);
    /* }}} */

    /// This kind of observables would be better in a separate process that
    /// measures the trigger rate
    Double_t evTimeDelay = 0;
    if (fPreviousEventTime.size() > 0) evTimeDelay = fInputTrackEvent->GetTime() - fPreviousEventTime.back();
    SetObservableValue((string) "EventTimeDelay", evTimeDelay);

    Double_t meanRate = 0;
    if (fPreviousEventTime.size() == 100)
        meanRate = 100. / (fInputTrackEvent->GetTime() - fPreviousEventTime.front());
    SetObservableValue((string) "MeanRate_InHz", meanRate);

    if (fCutsEnabled) {
        if (nTracksX < fNTracksXCut.X() || nTracksX > fNTracksXCut.Y()) return NULL;
        if (nTracksY < fNTracksYCut.X() || nTracksY > fNTracksYCut.Y()) return NULL;
    }

    if (GetVerboseLevel() >= REST_Extreme) GetChar();

    return fOutputTrackEvent;
}

////______________________________________________________________________________
// void TRestTrackAnalysisProcess::EndOfEventProcess() {
//    fPreviousEventTime.push_back(fInputTrackEvent->GetTimeStamp());
//    if (fPreviousEventTime.size() > 100) fPreviousEventTime.erase(fPreviousEventTime.begin());
//}

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
