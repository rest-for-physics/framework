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

#ifndef RestCore_TRestRealTimeDrawingProcess
#define RestCore_TRestRealTimeDrawingProcess

#include "TRestAnalysisPlot.h"
#include "TRestEventProcess.h"
#include "TVector2.h"

//! A process to draw analysis plots during processing, in case the process chain takes
//! long time while we want to see the result instantly.
class TRestRealTimeDrawingProcess : public TRestEventProcess {
   private:
    /// How many events passed when it starts next drawing
    Long64_t fDrawInterval;
    /// How many events passed when it starts next drawing
    Int_t fThreadWaitTimeoutMs;

    /// TRestAnalysisPlot object called for drawing
    static vector<TRestAnalysisPlot*> fPlots;  //!
    /// Last drawn entry of analysis tree
    static Long64_t fLastDrawnEntry;  //!
    /// Pause signal send for other threads when start drawing
    static bool fPauseInvoke;  //!
    /// Pause response flag from other threads when recieving pause signal
    static map<TRestRealTimeDrawingProcess*, bool> fPauseResponse;  //!
    /// The event pointer is not used in this process
    TRestEvent* fEvent = NULL;  //!

    void InitProcess();
    void EndProcess();

    void InitFromConfigFile();

    void Initialize();

   protected:
   public:
    any GetInputEvent() { return fEvent; }
    any GetOutputEvent() { return fEvent; }

    TRestEvent* ProcessEvent(TRestEvent* evInput);

    void PrintMetadata();

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestRealTimeDrawingProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "realtimedraw"; }

    TRestRealTimeDrawingProcess();

    ~TRestRealTimeDrawingProcess();

    // If new members are added, removed or modified in this class version number must be increased!
    ClassDef(TRestRealTimeDrawingProcess, 2);
};
#endif
