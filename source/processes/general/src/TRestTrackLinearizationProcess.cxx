//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackLinearizationProcess.cxx
///
///             Feb 2016:   First concept (Javier Galan)
///
//////////////////////////////////////////////////////////////////////////

#include "TRestTrackLinearizationProcess.h"
using namespace std;

vector<Double_t> distanceToNode;

ClassImp(TRestTrackLinearizationProcess);

TRestTrackLinearizationProcess::TRestTrackLinearizationProcess() { Initialize(); }

TRestTrackLinearizationProcess::TRestTrackLinearizationProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();
    PrintMetadata();
}

TRestTrackLinearizationProcess::~TRestTrackLinearizationProcess() {
    delete fInputTrackEvent;
    delete fOutputLinearTrackEvent;
}

void TRestTrackLinearizationProcess::LoadDefaultConfig() {
    SetName("trackLinearizationProcess");
    SetTitle("Default config");

    fTransversalResolution = .1;
    fLengthResolution = 1;
}

void TRestTrackLinearizationProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputTrackEvent = new TRestTrackEvent();
    fOutputLinearTrackEvent = new TRestLinearTrackEvent();

    fOutputEvent = fOutputLinearTrackEvent;
    fInputEvent = fInputTrackEvent;

    fTransversalResolution = .1;
    fLengthResolution = 1;

    fDivisions = 100;
}

void TRestTrackLinearizationProcess::LoadConfig(string cfgFilename) {
    if (LoadConfigFromFile(cfgFilename) == -1) LoadDefaultConfig();

    PrintMetadata();
}

void TRestTrackLinearizationProcess::InitProcess() {
    cout << __PRETTY_FUNCTION__ << endl;

    TRestEventProcess::ReadObservables();
}

TRestEvent* TRestTrackLinearizationProcess::ProcessEvent(TRestEvent* evInput) {
    fInputTrackEvent = (TRestTrackEvent*)evInput;

    /* {{{ Projection method */
    if (fMethod == "projection") {
        for (int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++) {
            if (!fInputTrackEvent->isTopLevel(tck)) continue;

            TRestLinearTrack linTrack;

            TRestTrack* topTrack = fInputTrackEvent->GetTrack(tck);

            Int_t origin = fInputTrackEvent->GetOriginTrackID(tck);

            TRestTrack* parentTrack = fInputTrackEvent->GetTrackById(origin);

            /* {{{ Debug output
               cout << "Top Track " << topTrack->GetTrackID() << endl;
               cout << "--------------------------------------" << endl;
               topTrack->PrintTrack();
               cout << "--------------------------------------" << endl;

               cout << "Parent Track " << parentTrack->GetTrackID() << endl;
               cout << "--------------------------------------" << endl;
               parentTrack->PrintTrack( false );
               cout << "--------------------------------------" << endl;
               getchar();

               }}} */

            TRestVolumeHits* nodeHits = topTrack->GetVolumeHits();
            TRestVolumeHits* originHits = parentTrack->GetVolumeHits();

            distanceToNode.clear();
            for (int h = 0; h < nodeHits->GetNumberOfHits(); h++)
                distanceToNode.push_back(nodeHits->GetDistanceToNode(h));

            /* {{{  Debug output
               for( int h = 0; h < nodeHits->GetNumberOfHits(); h++ )
               cout << "Distance : " <<  h << " = " << distanceToNode[h] << endl;
               getchar();
               }}} */

            for (int oHit = 0; oHit < originHits->GetNumberOfHits(); oHit++) {
                TVector3 hitPosition = originHits->GetPosition(oHit);

                TVector2 projection = FindProjection(hitPosition, nodeHits);

                /* {{{ Debug output */
                if (GetVerboseLevel() >= REST_Extreme) {
                    cout << "Hit position" << endl;
                    cout << "------------" << endl;
                    hitPosition.Print();
                    cout << "------------" << endl;
                    cout << "Projection" << endl;
                    cout << "------------" << endl;
                    projection.Print();
                    cout << "------------" << endl;
                    GetChar();
                }
                /* }}} */

                Double_t energy = originHits->GetEnergy(oHit);

                Double_t longitudinal = fLengthResolution * ((Int_t)(projection.X() / fLengthResolution));
                Double_t transversal =
                    fTransversalResolution * ((Int_t)(projection.Y() / fTransversalResolution));

                linTrack.AddChargeToLinearTrack(longitudinal, transversal, energy);
            }

            linTrack.SortTracks();
            linTrack.SetEnergy(parentTrack->GetEnergy());
            linTrack.SetMeanPosition(parentTrack->GetMeanPosition());

            fOutputLinearTrackEvent->AddLinearTrack(linTrack);
        }
    }
    /* }}} */

    /* {{{ R-ball method */
    if (fMethod == "R-ball") {
        vector<TRestTrack*> tracks;

        TRestTrack* tXYZ = fInputTrackEvent->GetMaxEnergyTrack();
        if (tXYZ) tracks.push_back(tXYZ);

        TRestTrack* tX = fInputTrackEvent->GetMaxEnergyTrack("X");
        if (tX) tracks.push_back(tX);

        TRestTrack* tY = fInputTrackEvent->GetMaxEnergyTrack("Y");
        if (tY) tracks.push_back(tY);

        Double_t chargeCoverage = 0, chargeCoverage_X = 0, chargeCoverage_Y = 0;

        Double_t dEdx_low = 0, dEdx_low_X = 0, dEdx_low_Y = 0;
        Double_t dEdx_high = 0, dEdx_high_X = 0, dEdx_high_Y = 0;
        Double_t dEdx_middle = 0, dEdx_middle_X = 0, dEdx_middle_Y = 0;

        Double_t dEdx_Max_low = 0, dEdx_Max_low_X = 0, dEdx_Max_low_Y = 0;
        Double_t dEdx_Max_high = 0, dEdx_Max_high_X = 0, dEdx_Max_high_Y = 0;
        Double_t dEdx_Max_middle = 0, dEdx_Max_middle_X = 0, dEdx_Max_middle_Y = 0;

        Double_t Q_low = 0, Q_low_X = 0, Q_low_Y = 0;
        Double_t Q_high = 0, Q_high_X = 0, Q_high_Y = 0;
        Double_t Q_middle = 0, Q_middle_X = 0, Q_middle_Y = 0;

        Double_t Q_Peak_low = 0, Q_Peak_low_X = 0, Q_Peak_low_Y = 0;
        Double_t Q_Peak_high = 0, Q_Peak_high_X = 0, Q_Peak_high_Y = 0;
        Double_t Q_Peak_middle = 0, Q_Peak_middle_X = 0, Q_Peak_middle_Y = 0;

        Double_t transDiffusionXYZ = 0;
        Double_t transDiffusionX = 0;
        Double_t transDiffusionY = 0;

        for (unsigned int t = 0; t < tracks.size(); t++) {
            TRestLinearTrack linTrack;

            TRestTrack* originTrack = fInputTrackEvent->GetOriginTrackById(tracks[t]->GetTrackID());

            TRestVolumeHits* nodeHits = tracks[t]->GetVolumeHits();
            TRestVolumeHits* originHits = originTrack->GetVolumeHits();

            vector<Int_t> hitIsOutside;
            for (int n = 0; n < originHits->GetNumberOfHits(); n++)
                hitIsOutside.push_back(originHits->GetEnergy(n));

            Double_t totalLength = nodeHits->GetTotalDistance();
            Double_t step = totalLength / fDivisions;
            for (int n = 0; n < fDivisions + 1; n++) {
                Double_t dist = n * step;

                Int_t refNode = 0;
                Double_t vectorDistance = GetReferenceNode(nodeHits, dist, refNode);

                if (n == fDivisions) refNode = nodeHits->GetNumberOfHits() - 2;

                if (GetVerboseLevel() >= REST_Debug) {
                    cout << "step : " << n << " reference node : " << refNode << endl;
                    cout << "Evaluating distance : " << dist << endl;
                    cout << "Vector distance : " << vectorDistance << endl;
                    cout << "Distance to reference node : " << nodeHits->GetHitsPathLength(0, refNode)
                         << endl;
                    cout << "------" << endl;
                }

                TVector3 x0 = nodeHits->GetPosition(refNode);
                TVector3 x1 = nodeHits->GetPosition(refNode + 1);

                TVector3 direction = (x1 - x0).Unit();
                TVector3 sphereCenter = x0 + vectorDistance * direction;

                Double_t energy = originHits->GetEnergyInSphere(sphereCenter, fRadius);

                if (GetVerboseLevel() >= REST_Debug) {
                    x0.Print();

                    sphereCenter.Print();
                    cout << "Energy inside radius : " << fRadius << " is " << energy << endl;
                }

                linTrack.AddChargeToLongitudinalTrack(100. * dist / totalLength, energy);

                TVector3 v0 = sphereCenter - step / 2. * direction;
                TVector3 v1 = sphereCenter + step / 2. * direction;

                if (GetVerboseLevel() >= REST_Debug)
                    cout << "Hit : " << n << " Hits inside cylinder : "
                         << originHits->GetNumberOfHitsInsideCylinder(v0, v1, fRadius) << endl;

                for (int l = 0; l < originHits->GetNumberOfHits(); l++) {
                    TVector3 v0 = sphereCenter - step / 2. * direction;
                    TVector3 v1 = sphereCenter + step / 2. * direction;

                    if (hitIsOutside[l] && originHits->isHitNInsideSphere(l, sphereCenter, fRadius))
                        hitIsOutside[l] = 0;

                    if (originHits->isHitNInsideCylinder(l, v0, v1, 1.5 * fRadius)) {
                        Double_t r =
                            originHits->GetTransversalProjection(v0, direction, originHits->GetPosition(l));

                        if (GetVerboseLevel() >= REST_Extreme) cout << "Trans proj : " << r << endl;

                        linTrack.AddChargeToTransversalTrack(((Int_t)(3. * r)) * 3., energy);
                    }
                }
            }

            linTrack.Normalize(((Double_t)fDivisions) / 100.);
            linTrack.SortTracks();
            linTrack.SetEnergy(originTrack->GetEnergy());
            linTrack.SetMeanPosition(originTrack->GetMeanPosition());

            fOutputLinearTrackEvent->AddLinearTrack(linTrack);

            /* {{{ Getting chargeCoverage observable */
            Double_t energyMissing = 0;
            for (unsigned int n = 0; n < hitIsOutside.size(); n++) energyMissing += hitIsOutside[n];

            Double_t missingEnergyRatio = energyMissing / originHits->GetEnergy();

            if (tracks[t]->isXYZ()) chargeCoverage = 1 - missingEnergyRatio;

            if (tracks[t]->isXZ()) chargeCoverage_X = 1 - missingEnergyRatio;

            if (tracks[t]->isYZ()) chargeCoverage_Y = 1 - missingEnergyRatio;
            /* }}} */

            //////////////////////////////////////////////////////////////////////////////
            // This SHOULD be moved later to an independent LinearTrackAnalysisProcess
            // //

            /* {{{ Longitudinal observables */
            TRestSignal* s = linTrack.GetLongitudinalTrack();

            /* {{{ Generating differential profile observables (dEdx) */
            TRestSignal* diffS = new TRestSignal();
            s->GetDifferentialSignal(diffS, 5);

            Double_t dEdx_Start = 0, dEdx_Max_Start = 0;
            for (int n = 0; n <= fDivisions / 3; n++) {
                if (TMath::Abs(diffS->GetData(n)) > dEdx_Max_Start)
                    dEdx_Max_Start = TMath::Abs(diffS->GetData(n));

                dEdx_Start += TMath::Abs(diffS->GetData(n));
            }

            Double_t dEdx_End = 0, dEdx_Max_End = 0;
            for (int n = 2 * fDivisions / 3; n < fDivisions; n++) {
                if (TMath::Abs(diffS->GetData(n)) > dEdx_Max_End)
                    dEdx_Max_End = TMath::Abs(diffS->GetData(n));

                dEdx_End += TMath::Abs(diffS->GetData(n));
            }

            Double_t dEdx_Between = 0, dEdx_Max_Between = 0;
            for (int n = fDivisions / 3; n < (2 * fDivisions) / 3; n++) {
                if (TMath::Abs(diffS->GetData(n)) > dEdx_Max_Between)
                    dEdx_Max_Between = TMath::Abs(diffS->GetData(n));

                dEdx_Between += TMath::Abs(diffS->GetData(n));
            }

            /* {{{ dEdx_ */
            if (tracks[t]->isXYZ()) {
                if (dEdx_Start > dEdx_End) {
                    dEdx_low = dEdx_End;
                    dEdx_high = dEdx_Start;
                } else {
                    dEdx_low = dEdx_Start;
                    dEdx_high = dEdx_End;
                }

                dEdx_middle = dEdx_Between;
            }

            if (tracks[t]->isXZ()) {
                if (dEdx_Start > dEdx_End) {
                    dEdx_low_X = dEdx_End;
                    dEdx_high_X = dEdx_Start;
                } else {
                    dEdx_low_X = dEdx_Start;
                    dEdx_high_X = dEdx_End;
                }

                dEdx_middle_X = dEdx_Between;
            }

            if (tracks[t]->isYZ()) {
                if (dEdx_Start > dEdx_End) {
                    dEdx_low_Y = dEdx_End;
                    dEdx_high_Y = dEdx_Start;
                } else {
                    dEdx_low_Y = dEdx_Start;
                    dEdx_high_Y = dEdx_End;
                }

                dEdx_middle_Y = dEdx_Between;
            }
            /* }}} */

            /* {{{ dEdx_Max_ */
            if (tracks[t]->isXYZ()) {
                if (dEdx_Max_Start > dEdx_Max_End) {
                    dEdx_Max_low = dEdx_Max_End;
                    dEdx_Max_high = dEdx_Max_Start;
                } else {
                    dEdx_Max_low = dEdx_Max_Start;
                    dEdx_Max_high = dEdx_Max_End;
                }

                dEdx_Max_middle = dEdx_Max_Between;
            }

            if (tracks[t]->isXZ()) {
                if (dEdx_Max_Start > dEdx_Max_End) {
                    dEdx_Max_low_X = dEdx_Max_End;
                    dEdx_Max_high_X = dEdx_Max_Start;
                } else {
                    dEdx_Max_low_X = dEdx_Max_Start;
                    dEdx_Max_high_X = dEdx_Max_End;
                }

                dEdx_Max_middle_X = dEdx_Max_Between;
            }

            if (tracks[t]->isYZ()) {
                if (dEdx_Max_Start > dEdx_Max_End) {
                    dEdx_Max_low_Y = dEdx_Max_End;
                    dEdx_Max_high_Y = dEdx_Max_Start;
                } else {
                    dEdx_Max_low_Y = dEdx_Max_Start;
                    dEdx_Max_high_Y = dEdx_Max_End;
                }

                dEdx_Max_middle_Y = dEdx_Max_Between;
            }
            /* }}} */

            delete diffS;
            /* }}} */

            /* {{{ Generating Q-blob observables */

            Double_t totalQ = s->GetIntegralWithTime(0, 100);
            Double_t Q_Left = s->GetIntegralWithTime(0, 25) / totalQ;
            Double_t Q_Between = s->GetIntegralWithTime(25, 75) / totalQ;
            Double_t Q_Right = s->GetIntegralWithTime(25, 75) / totalQ;

            Double_t Q_Peak_Left = s->GetMaxPeakWithTime(0, 25);
            Double_t Q_Peak_Between = s->GetMaxPeakWithTime(25, 75);
            Double_t Q_Peak_Right = s->GetMaxPeakWithTime(25, 75);

            if (GetVerboseLevel() >= REST_Debug) {
                cout << "Ql : " << Q_Left << endl;
                cout << "Qr : " << Q_Right << endl;
                cout << "Qm : " << Q_Between << endl;

                if (GetVerboseLevel() >= REST_Extreme) GetChar();
            }

            /* {{{ Q_ */
            if (tracks[t]->isXYZ()) {
                if (Q_Left > Q_Right) {
                    Q_low = Q_Right;
                    Q_high = Q_Left;
                } else {
                    Q_low = Q_Left;
                    Q_high = Q_Right;
                }

                Q_middle = Q_Between;
            }

            if (tracks[t]->isXZ()) {
                if (Q_Left > Q_Right) {
                    Q_low_X = Q_Right;
                    Q_high_X = Q_Left;
                } else {
                    Q_low_X = Q_Left;
                    Q_high_X = Q_Right;
                }

                Q_middle_X = Q_Between;
            }

            if (tracks[t]->isYZ()) {
                if (Q_Left > Q_Right) {
                    Q_low_Y = Q_Right;
                    Q_high_Y = Q_Left;
                } else {
                    Q_low_Y = Q_Left;
                    Q_high_Y = Q_Right;
                }

                Q_middle_Y = Q_Between;
            }
            /* }}} */

            /* {{{ Q_peak_ */
            if (tracks[t]->isXYZ()) {
                if (Q_Peak_Left > Q_Peak_Right) {
                    Q_Peak_low = Q_Peak_Right;
                    Q_Peak_high = Q_Peak_Left;
                } else {
                    Q_Peak_low = Q_Peak_Left;
                    Q_Peak_high = Q_Peak_Right;
                }

                Q_Peak_middle = Q_Peak_Between;
            }

            if (tracks[t]->isXZ()) {
                if (Q_Peak_Left > Q_Peak_Right) {
                    Q_Peak_low_X = Q_Peak_Right;
                    Q_Peak_high_X = Q_Peak_Left;
                } else {
                    Q_Peak_low_X = Q_Peak_Left;
                    Q_Peak_high_X = Q_Peak_Right;
                }

                Q_Peak_middle_X = Q_Peak_Between;
            }

            if (tracks[t]->isYZ()) {
                if (Q_Peak_Left > Q_Peak_Right) {
                    Q_Peak_low_Y = Q_Peak_Right;
                    Q_Peak_high_Y = Q_Peak_Left;
                } else {
                    Q_Peak_low_Y = Q_Peak_Left;
                    Q_Peak_high_Y = Q_Peak_Right;
                }

                Q_Peak_middle_Y = Q_Peak_Between;
            }
            /* }}} */
            /* }}} */

            /* }}} */

            /* {{{ Transversal diffusion observable */
            TRestSignal* transSignal = linTrack.GetTransversalTrack();

            Double_t transDiffusion = transSignal->GetAverage();

            if (tracks[t]->isXYZ()) transDiffusionXYZ = transDiffusion;

            if (tracks[t]->isXZ()) transDiffusionX = transDiffusion;

            if (tracks[t]->isYZ()) transDiffusionY = transDiffusion;
            /* }}} */

            //////////////////////////////////////////////////////////////////////////////
        }

        SetObservableValue((string) "coverage", chargeCoverage);
        SetObservableValue((string) "coverage_X", chargeCoverage_X);
        SetObservableValue((string) "coverage_Y", chargeCoverage_Y);

        SetObservableValue((string) "dEdx_low", dEdx_low);
        SetObservableValue((string) "dEdx_high", dEdx_high);
        SetObservableValue((string) "dEdx_middle", dEdx_middle);

        SetObservableValue((string) "dEdx_Max_low", dEdx_Max_low);
        SetObservableValue((string) "dEdx_Max_high", dEdx_Max_high);
        SetObservableValue((string) "dEdx_Max_middle", dEdx_Max_middle);

        SetObservableValue((string) "dEdx_low_X", dEdx_low_X);
        SetObservableValue((string) "dEdx_high_X", dEdx_high_X);
        SetObservableValue((string) "dEdx_middle_X", dEdx_middle_X);

        SetObservableValue((string) "dEdx_low_Y", dEdx_low_Y);
        SetObservableValue((string) "dEdx_high_Y", dEdx_high_Y);
        SetObservableValue((string) "dEdx_middle_Y", dEdx_middle_Y);

        SetObservableValue((string) "dEdx_Max_low_X", dEdx_Max_low_X);
        SetObservableValue((string) "dEdx_Max_high_X", dEdx_Max_high_X);
        SetObservableValue((string) "dEdx_Max_middle_X", dEdx_Max_middle_X);

        SetObservableValue((string) "dEdx_Max_low_Y", dEdx_Max_low_Y);
        SetObservableValue((string) "dEdx_Max_high_Y", dEdx_Max_high_Y);
        SetObservableValue((string) "dEdx_Max_middle_Y", dEdx_Max_middle_Y);

        SetObservableValue((string) "Q_low", Q_low);
        SetObservableValue((string) "Q_high", Q_high);
        SetObservableValue((string) "Q_middle", Q_middle);

        SetObservableValue((string) "Q_low_X", Q_low_X);
        SetObservableValue((string) "Q_high_X", Q_high_X);
        SetObservableValue((string) "Q_middle_X", Q_middle_X);

        SetObservableValue((string) "Q_low_Y", Q_low_Y);
        SetObservableValue((string) "Q_high_Y", Q_high_Y);
        SetObservableValue((string) "Q_middle_Y", Q_middle_Y);

        SetObservableValue((string) "Q_Peak_low", Q_Peak_low);
        SetObservableValue((string) "Q_Peak_high", Q_Peak_high);
        SetObservableValue((string) "Q_Peak_middle", Q_Peak_middle);

        SetObservableValue((string) "Q_Peak_low_X", Q_Peak_low_X);
        SetObservableValue((string) "Q_Peak_high_X", Q_Peak_high_X);
        SetObservableValue((string) "Q_Peak_middle_X", Q_Peak_middle_X);

        SetObservableValue((string) "Q_Peak_low_Y", Q_Peak_low_Y);
        SetObservableValue((string) "Q_Peak_high_Y", Q_Peak_high_Y);
        SetObservableValue((string) "Q_Peak_middle_Y", Q_Peak_middle_Y);

        SetObservableValue((string) "transDiff", transDiffusionXYZ);
        SetObservableValue((string) "transDiff_X", transDiffusionX);
        SetObservableValue((string) "transDiff_Y", transDiffusionY);
    }
    /* }}} */

    return fOutputLinearTrackEvent;
}

TVector2 TRestTrackLinearizationProcess::FindProjection(TVector3 position, TRestHits* nodes) {
    Int_t closestNode = nodes->GetClosestHit(position);

    TVector2 projection1, projection2;

    if (closestNode > 0)
        projection1 = nodes->GetProjection(closestNode, closestNode - 1, position);
    else
        projection1 = TVector2(0, 0);

    if (closestNode < nodes->GetNumberOfHits() - 1)
        projection2 = nodes->GetProjection(closestNode + 1, closestNode, position);
    else
        projection2 = TVector2(0, 0);

    /* {{{ Debug output */
    if (GetVerboseLevel() >= REST_Extreme) {
        cout << "Closest node : " << closestNode << endl;

        cout << endl;
        cout << "projection 1 " << endl;
        cout << "-------------" << endl;
        cout << "long : " << projection1.X() << " // tr : " << projection1.Y() << endl;
        cout << "-------------" << endl;
        cout << "projection 2 " << endl;
        cout << "-------------" << endl;
        cout << "long : " << projection2.X() << " // tr : " << projection2.Y() << endl;
        cout << "-------------" << endl;
        GetChar();
    }
    /* }}} */

    if (projection2.Y() == 0) {
        projection1 += TVector2(distanceToNode[closestNode - 1], 0);
        return projection1;
    } else if (projection1.Y() == 0) {
        projection2 += TVector2(distanceToNode[closestNode], 0);
        return projection2;
    } else if (projection1.Y() <= projection2.Y()) {
        projection1 += TVector2(distanceToNode[closestNode - 1], 0);
        return projection1;
    } else {
        projection2 += TVector2(distanceToNode[closestNode], 0);
        return projection2;
    }
}

void TRestTrackLinearizationProcess::InitFromConfigFile() {
    fLengthResolution = GetDblParameterWithUnits("lengthResolution", 1);
    fTransversalResolution = GetDblParameterWithUnits("transversalResolution", 1);

    fMethod = GetParameter("method", "R-ball");
    fRadius = GetDblParameterWithUnits("radius", 5);
    fDivisions = StringToInteger(GetParameter("divisions", "1000"));
}

Double_t TRestTrackLinearizationProcess::GetReferenceNode(TRestHits* nHits, Double_t dist, Int_t& ref) {
    for (int m = ref; m < nHits->GetNumberOfHits(); m++) {
        Double_t d = nHits->GetHitsPathLength(0, m);

        if (d > dist) {
            ref = m - 1;
            return dist - nHits->GetHitsPathLength(0, m - 1);
        }
    }

    return dist - nHits->GetHitsPathLength(0, nHits->GetNumberOfHits() - 2);
}
