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

#ifndef RestCore_TRestRawSignalAnalysisProcess
#define RestCore_TRestRawSignalAnalysisProcess

#include <TH1D.h>
#include <TRestGas.h>
#include <TRestHitsEvent.h>
#include <TRestRawSignalEvent.h>
#include <TRestReadout.h>

#include "TRestEventProcess.h"

//! An analysis REST process to extract valuable information from RawSignal type of data.
class TRestRawSignalAnalysisProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    /// A pointer to the specific TRestRawSignalEvent input
    TRestRawSignalEvent* fSignalEvent;  //!
    
    TRestReadout* fReadout;  //!

    Double_t fFirstEventTime;             //!
    vector<Double_t> fPreviousEventTime;  //!

    vector<TObject*> fDrawingObjects;  //!
    Double_t fDrawRefresh;             //!

    time_t timeStored;  //!

    std::vector<std::string> fSignalAnalysisObservables;  //!

    // parameters
    TVector2 fBaseLineRange;
    TVector2 fIntegralRange;
    Double_t fPointThreshold;
    Double_t fSignalThreshold;
    Int_t fNPointsOverThreshold;
    // Bool_t fCutsEnabled;//!
    // TVector2 fMeanBaseLineCutRange;//!
    // TVector2 fMeanBaseLineSigmaCutRange;//!
    // TVector2 fMaxNumberOfSignalsCut;//!
    // TVector2 fMaxNumberOfGoodSignalsCut;//!
    // TVector2 fFullIntegralCut;//!
    // TVector2 fThresholdIntegralCut;//!
    // TVector2 fPeakTimeDelayCut;//!

    // analysis result(saved directly in root file)
    TH1D* fChannelsHisto;  //!
#endif

    TPad* DrawSignal(Int_t signal);
    TPad* DrawObservables();

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process
    
   public:
    void InitProcess();
    void BeginOfEventProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndOfEventProcess();
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Baseline range : ( " << fBaseLineRange.X() << " , " << fBaseLineRange.Y() << " ) "
                 << endl;
        metadata << "Integral range : ( " << fIntegralRange.X() << " , " << fIntegralRange.Y() << " ) "
                 << endl;
        metadata << "Point Threshold : " << fPointThreshold << " sigmas" << endl;
        metadata << "Signal threshold : " << fSignalThreshold << " sigmas" << endl;
        metadata << "Number of points over threshold : " << fNPointsOverThreshold << endl;
        metadata << " " << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "rawSignalAnalysis"; }

    TRestRawSignalAnalysisProcess(); // Constructor
    TRestRawSignalAnalysisProcess(char* cfgFileName);
    ~TRestRawSignalAnalysisProcess(); // Destructor

    ClassDef(TRestRawSignalAnalysisProcess, 1);  
    // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif
