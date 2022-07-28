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

#ifndef RestCore_TRestEventRateAnalysisProcess
#define RestCore_TRestEventRateAnalysisProcess

#include "TRestEventProcess.h"

//! A pure analysis process used to calculate event rates and other time observables
class TRestEventRateAnalysisProcess : public TRestEventProcess {
   private:
    /// A pointer to the input event
    TRestEvent* fEvent;  //!

    /// It registers the timestamp from the first event to calculate time from start observables
    Double_t fFirstEventTime;  //!

    /// It keeps a historic list of timestamps of the previous N events. Now fixed to N=10 events.
    std::vector<Double_t> fPreviousEventTime;  //!
   
    /// It indicates whether to add rate observables which is correct only under single thread run.
    bool fRateAnalysis = false;//!
   
    void Initialize();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() const override { return fEvent; }
    any GetOutputEvent() const override { return fEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void PrintMetadata() override {
        BeginPrintProcess();

        if (fRateAnalysis) {
	          RESTMetadata << "Rate analysis is on under single thread mode" << RESTendl;
	      }
	      else {
		        RESTMetadata << "Rate analysis is off due to multi-thread" << RESTendl;
	      }
       
        EndPrintProcess();
    }

    const char* GetProcessName() const override { return "eventRateAnalysis"; }

    TRestEventRateAnalysisProcess();   // Constructor
    ~TRestEventRateAnalysisProcess();  // Destructor

    ClassDefOverride(TRestEventRateAnalysisProcess, 1);
};
#endif
