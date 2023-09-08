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
    int fDrawInterval;
    /// How many events passed when it starts next drawing
    Int_t fThreadWaitTimeoutMs;

    /// TRestAnalysisPlot object called for drawing
    static std::vector<std::string> fProcessesToDraw;  //!
    /// TRestAnalysisPlot object called for drawing
    static std::vector<TRestAnalysisPlot*> fPlots;  //!
    /// Last drawn entry of analysis tree
    static Long64_t fLastDrawnEntry;  //!
    /// Pause signal send for other threads when start drawing
    static bool fPauseInvoke;  //!
    /// Pause response flag from other threads when recieving pause signal
    static std::map<TRestRealTimeDrawingProcess*, bool> fPauseResponse;  //!
    /// The event pointer is not used in this process
    TRestEvent* fEvent = nullptr;  //!

    void InitProcess() override;
    void EndProcess() override;

    void Initialize() override;

   protected:
   public:
    RESTValue GetInputEvent() const override { return fEvent; }
    RESTValue GetOutputEvent() const override { return fEvent; }

    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void PrintMetadata() override;

    /// Returns a new instance of this class
    void DrawWithNotification();

    void DrawOnce();

    /// Returns the name of this process
    const char* GetProcessName() const override { return "realtimedraw"; }

    TRestRealTimeDrawingProcess();

    ~TRestRealTimeDrawingProcess();

    // If new members are added, removed or modified in this class version number must be increased!
    ClassDefOverride(TRestRealTimeDrawingProcess, 3);
};
#endif
