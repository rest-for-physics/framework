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

#ifndef RestCore_TRestHitsAnalysisProcess
#define RestCore_TRestHitsAnalysisProcess

#include <TH1D.h>

#include <TCanvas.h>

#include <TRestGas.h>
#include <TRestHitsEvent.h>
#include <TRestReadout.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

//! An analysis REST process to extract valuable information from Hits type of data.
class TRestHitsAnalysisProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestHitsEvent* fInputHitsEvent;   //!
    TRestHitsEvent* fOutputHitsEvent;  //!

    Bool_t fCylinderFiducial;  //!
    Bool_t fPrismFiducial;     //!
#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process
    //
    TVector3 fFid_x0;
    TVector3 fFid_x1;
    Double_t fFid_R;
    Double_t fFid_sX;
    Double_t fFid_sY;
    Double_t fFid_theta;

   public:
    void InitProcess();
    void BeginOfEventProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndOfEventProcess();
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        std::cout << " Fiducial parameters" << std::endl;
        std::cout << " -------------------" << std::endl;
        std::cout << " x0 : (" << fFid_x0.X() << " , " << fFid_x0.Y() << " , " << fFid_x0.Z() << ")"
                  << std::endl;
        std::cout << " x1 : (" << fFid_x1.X() << " , " << fFid_x1.Y() << " , " << fFid_x1.Z() << ")"
                  << std::endl;
        std::cout << " R : " << fFid_R << std::endl;
        std::cout << " sX : " << fFid_sX << std::endl;
        std::cout << " sY : " << fFid_sY << std::endl;
        std::cout << " -------------------" << std::endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "hitsAnalysis"; }

    // Constructor
    TRestHitsAnalysisProcess();
    TRestHitsAnalysisProcess(char* cfgFileName);
    // Destructor
    ~TRestHitsAnalysisProcess();

    ClassDef(TRestHitsAnalysisProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
