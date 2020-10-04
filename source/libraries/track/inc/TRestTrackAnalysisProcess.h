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

#ifndef RestCore_TRestTrackAnalysisProcess
#define RestCore_TRestTrackAnalysisProcess

#include <TRestTrackEvent.h>

#include "TRestEventProcess.h"

//! An analysis REST process to extract valuable information from Track type of data.
class TRestTrackAnalysisProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestTrackEvent* fInputTrackEvent;   //!
    TRestTrackEvent* fOutputTrackEvent;  //!

    vector<Double_t> fPreviousEventTime;  //!

    Bool_t fCutsEnabled;  //!

    std::vector<std::string> fTrack_LE_EnergyObservables;  //!
    std::vector<Double_t> fTrack_LE_Threshold;             //!
    std::vector<Int_t> nTracks_LE;                         //!

    std::vector<std::string> fTrack_HE_EnergyObservables;  //!
    std::vector<Double_t> fTrack_HE_Threshold;             //!
    std::vector<Int_t> nTracks_HE;                         //!

    std::vector<std::string> fTrack_En_EnergyObservables;  //!
    std::vector<Double_t> fTrack_En_Threshold;             //!
    std::vector<Int_t> nTracks_En;                         //!

    Bool_t fEnableTwistParameters;  //!

    std::vector<TString> fTwistLowObservables;      //!
    std::vector<Double_t> fTwistLowTailPercentage;  //!
    std::vector<Double_t> fTwistLowValue;           //!

    std::vector<TString> fTwistHighObservables;      //!
    std::vector<Double_t> fTwistHighTailPercentage;  //!
    std::vector<Double_t> fTwistHighValue;           //!

    std::vector<TString> fTwistBalanceObservables;      //!
    std::vector<Double_t> fTwistBalanceTailPercentage;  //!
    std::vector<Double_t> fTwistBalanceValue;           //!

    std::vector<TString> fTwistRatioObservables;      //!
    std::vector<Double_t> fTwistRatioTailPercentage;  //!
    std::vector<Double_t> fTwistRatioValue;           //!

    std::vector<TString> fTwistWeightedLowObservables;      //!
    std::vector<Double_t> fTwistWeightedLowTailPercentage;  //!
    std::vector<Double_t> fTwistWeightedLowValue;           //!

    std::vector<TString> fTwistWeightedHighObservables;      //!
    std::vector<Double_t> fTwistWeightedHighTailPercentage;  //!
    std::vector<Double_t> fTwistWeightedHighValue;           //!

    std::vector<TString> fTwistLowObservables_X;      //!
    std::vector<Double_t> fTwistLowTailPercentage_X;  //!
    std::vector<Double_t> fTwistLowValue_X;           //!

    std::vector<TString> fTwistHighObservables_X;      //!
    std::vector<Double_t> fTwistHighTailPercentage_X;  //!
    std::vector<Double_t> fTwistHighValue_X;           //!

    std::vector<TString> fTwistBalanceObservables_X;      //!
    std::vector<Double_t> fTwistBalanceTailPercentage_X;  //!
    std::vector<Double_t> fTwistBalanceValue_X;           //!

    std::vector<TString> fTwistRatioObservables_X;      //!
    std::vector<Double_t> fTwistRatioTailPercentage_X;  //!
    std::vector<Double_t> fTwistRatioValue_X;           //!

    std::vector<TString> fTwistWeightedLowObservables_X;      //!
    std::vector<Double_t> fTwistWeightedLowTailPercentage_X;  //!
    std::vector<Double_t> fTwistWeightedLowValue_X;           //!

    std::vector<TString> fTwistWeightedHighObservables_X;      //!
    std::vector<Double_t> fTwistWeightedHighTailPercentage_X;  //!
    std::vector<Double_t> fTwistWeightedHighValue_X;           //!

    std::vector<TString> fTwistLowObservables_Y;      //!
    std::vector<Double_t> fTwistLowTailPercentage_Y;  //!
    std::vector<Double_t> fTwistLowValue_Y;           //!

    std::vector<TString> fTwistHighObservables_Y;      //!
    std::vector<Double_t> fTwistHighTailPercentage_Y;  //!
    std::vector<Double_t> fTwistHighValue_Y;           //!

    std::vector<TString> fTwistBalanceObservables_Y;      //!
    std::vector<Double_t> fTwistBalanceTailPercentage_Y;  //!
    std::vector<Double_t> fTwistBalanceValue_Y;           //!

    std::vector<TString> fTwistRatioObservables_Y;      //!
    std::vector<Double_t> fTwistRatioTailPercentage_Y;  //!
    std::vector<Double_t> fTwistRatioValue_Y;           //!

    std::vector<TString> fTwistWeightedLowObservables_Y;      //!
    std::vector<Double_t> fTwistWeightedLowTailPercentage_Y;  //!
    std::vector<Double_t> fTwistWeightedLowValue_Y;           //!

    std::vector<TString> fTwistWeightedHighObservables_Y;      //!
    std::vector<Double_t> fTwistWeightedHighTailPercentage_Y;  //!
    std::vector<Double_t> fTwistWeightedHighValue_Y;           //!
#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

    TVector2 fNTracksXCut;
    TVector2 fNTracksYCut;
    Double_t fDeltaEnergy;

   public:
    any GetInputEvent() { return fInputTrackEvent; }
    any GetOutputEvent() { return fOutputTrackEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        if (fCutsEnabled) {
            cout << "Number of tracks in X cut : ( " << fNTracksXCut.X() << " , " << fNTracksXCut.Y() << " ) "
                 << endl;
            cout << "Number of tracks in Y cut : ( " << fNTracksYCut.X() << " , " << fNTracksYCut.Y() << " ) "
                 << endl;
        } else {
            cout << endl;
            cout << "No cuts have been enabled" << endl;
        }

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "trackAnalysis"; }

    // Constructor
    TRestTrackAnalysisProcess();
    TRestTrackAnalysisProcess(char* cfgFileName);
    // Destructor
    ~TRestTrackAnalysisProcess();

    ClassDef(TRestTrackAnalysisProcess, 1);  // Template for a REST "event process" class inherited from
                                             // TRestEventProcess
};
#endif
