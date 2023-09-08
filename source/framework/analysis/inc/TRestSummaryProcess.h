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

#ifndef RestCore_TRestSummaryProcess
#define RestCore_TRestSummaryProcess

#include "TRestEventProcess.h"
#include "TVector2.h"

//! A process to define basic data members to serve as a metadata summary in a data processing chain.
class TRestSummaryProcess : public TRestEventProcess {
   private:
    /// The mean rate during the run, calculated as the final number of entries in the run.
    Double_t fMeanRate;

    /// The mean rate sigma calculated as the square root of the corresponding mean rate counts
    Double_t fMeanRateSigma;

    /// It will register the average of an existing analyisTree observable.
    std::map<TString, Double_t> fAverage;

    /// If defined it will set the range where average is calculated
    std::map<TString, TVector2> fAverageRange;

    /// It will register the RMS of an existing analysisTree observable.
    std::map<TString, Double_t> fRMS;

    /// If defined it will set the range where RMS is calculated
    std::map<TString, TVector2> fRMSRange;

    /// It will register the maximum value of an existing analyisTree observable.
    std::map<TString, Double_t> fMaximum;

    /// If defined it will set the range where maximum is calculated
    std::map<TString, TVector2> fMaximumRange;

    /// It will register the minimum value of an existing analyisTree observable.
    std::map<TString, Double_t> fMinimum;

    /// If defined it will set the range where minimum is calculated
    std::map<TString, TVector2> fMinimumRange;

    /// The event pointer is not used in this process
    TRestEvent* fEvent = nullptr;  //!

    void InitProcess() override;
    void EndProcess() override;

    void InitFromConfigFile() override;

    void Initialize() override;

    void LoadDefaultConfig();

   protected:
   public:
    RESTValue GetInputEvent() const override { return fEvent; }
    RESTValue GetOutputEvent() const override { return fEvent; }

    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override;

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestSummaryProcess; }

    /// Returns the name of this process
    const char* GetProcessName() const override { return "summary"; }

    TRestSummaryProcess();
    TRestSummaryProcess(const char* configFilename);

    ~TRestSummaryProcess();

    // If new members are added, removed or modified in this class version number must be increased!
    ClassDefOverride(TRestSummaryProcess, 2);
};
#endif
