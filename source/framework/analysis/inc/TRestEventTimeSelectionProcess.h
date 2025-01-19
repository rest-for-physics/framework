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

#ifndef RestProcess_TRestEventTimeSelectionProcess
#define RestProcess_TRestEventTimeSelectionProcess

#include <TRestEventProcess.h>

#include <iostream>

class TRestEventTimeSelectionProcess : public TRestEventProcess {
   private:
    TRestEvent* fEvent;  //!
    std::string fFileWithTimes;
    Bool_t fIsActiveTime;
    Char_t fDelimiter;
    Long_t fTimeOffsetInSeconds;
    Long_t fTimeStartMarginInSeconds;
    Long_t fTimeEndMarginInSeconds;
    std::vector<std::pair<std::string, std::string>> fStartEndTimes;

    /// Information about the events processed

    Int_t fNEventsRejected;
    Int_t fNEventsSelected;
    Double_t fTotalTimeInSeconds;

    void Initialize() override;

   protected:
   public:
    RESTValue GetInputEvent() const override { return fEvent; }
    RESTValue GetOutputEvent() const override { return fEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void PrintMetadata() override;

    // Constructor
    TRestEventTimeSelectionProcess();
    // Destructor
    ~TRestEventTimeSelectionProcess() {}

    const char* GetProcessName() const override { return "EventTimeSelectionProcess"; }

    std::string GetFileWithTimes() const { return fFileWithTimes; }

    Bool_t GetIsActiveTime() const { return fIsActiveTime; }
    Char_t GetDelimiter() const { return fDelimiter; }

    std::vector<std::pair<std::string, std::string>> GetStartEndTimes() const { return fStartEndTimes; }
    std::string GetTimeStampCut(std::string timeStampObsName = "timeStamp", Bool_t useOffset = true,
                                Bool_t useMargins = true, Int_t nTimes = -1);
    Int_t GetNEventsRejected() const { return fNEventsRejected; }
    Int_t GetNEventsSelected() const { return fNEventsSelected; }
    Double_t GetTotalTimeInSeconds() const { return fTotalTimeInSeconds; }
    Long_t GetTimeOffsetInSeconds() const { return fTimeOffsetInSeconds; }
    Long_t GetTimeStartMarginInSeconds() const { return fTimeStartMarginInSeconds; }
    Long_t GetTimeEndMarginInSeconds() const { return fTimeEndMarginInSeconds; }

    Double_t CalculateTotalTimeInSeconds();

    void SetFileWithTimes(const std::string& fileWithTimes) { fFileWithTimes = fileWithTimes; }
    void SetIsActiveTime(Bool_t isActiveTime) { fIsActiveTime = isActiveTime; }
    void SetDelimiter(Char_t delimiter) { fDelimiter = delimiter; }
    void SetStartEndTimes(const std::vector<std::pair<std::string, std::string>>& startEndTimes) {
        fStartEndTimes = startEndTimes;
    }
    void SetTimeOffsetInSeconds(Long_t timeOffsetInSeconds) { fTimeOffsetInSeconds = timeOffsetInSeconds; }
    void SetTimeStartMarginInSeconds(Long_t timeStartMarginInSeconds) {
        fTimeStartMarginInSeconds = timeStartMarginInSeconds;
    }
    void SetTimeEndMarginInSeconds(Long_t timeEndMarginInSeconds) {
        fTimeEndMarginInSeconds = timeEndMarginInSeconds;
    }

    ClassDefOverride(TRestEventTimeSelectionProcess, 1);
};
#endif
