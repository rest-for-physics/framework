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

#ifndef RestProcess_TRestLikelihoodProcess
#define RestProcess_TRestLikelihoodProcess

#include <TH1D.h>

#include <iostream>
#include <vector>

#include "TRestEventProcess.h"

//! A template process to serve as an example to create new TRestRawSignalEventProcess
class TRestLikelihoodProcess : public TRestEventProcess {
   private:
    TRestEvent* fEvent;  //!

    std::string fFileWithPdfs = "";
    std::string fLikelihoodObservables = "";

    TFile* fFileHistos = 0;
    std::vector<std::string> fVectorObservables;

    void Initialize() override;

   protected:
   public:
    any GetInputEvent() const override { return fEvent; }
    any GetOutputEvent() const override { return fEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void LoadDefaultConfig();
    void PrintMetadata() override;

    // Constructor
    TRestLikelihoodProcess();
    TRestLikelihoodProcess(const char* configFilename);
    // Destructor
    ~TRestLikelihoodProcess() {}

    const char* GetProcessName() const override { return "LikelihoodProcess"; }

    ClassDefOverride(TRestLikelihoodProcess, 1);
};
#endif
