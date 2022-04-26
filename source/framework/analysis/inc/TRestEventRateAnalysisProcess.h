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

    void Initialize();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() { return fEvent; }
    any GetOutputEvent() { return fEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void PrintMetadata() {
        BeginPrintProcess();

        EndPrintProcess();
    }

    inline TString GetProcessName() const { return (TString) "eventRateAnalysis"; }

    TRestEventRateAnalysisProcess();   // Constructor
    ~TRestEventRateAnalysisProcess();  // Destructor

    ClassDef(TRestEventRateAnalysisProcess, 1);
};
#endif
