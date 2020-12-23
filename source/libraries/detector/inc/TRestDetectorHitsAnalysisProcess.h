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

#ifndef RestCore_TRestDetectorHitsAnalysisProcess
#define RestCore_TRestDetectorHitsAnalysisProcess

#include <TH1D.h>

#include <TCanvas.h>

#include <TRestDetectorReadout.h>
#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

//! An analysis REST process to extract valuable information from Hits type of data.
class TRestDetectorHitsAnalysisProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestDetectorHitsEvent* fInputHitsEvent;   //!
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

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
    any GetInputEvent() { return fInputHitsEvent; }
    any GetOutputEvent() { return fOutputHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        if (fCylinderFiducial) metadata << "Cylinder fiducial active" << endl;
        if (fPrismFiducial) metadata << "Prism fiducial active" << endl;

        metadata << " -------------------" << endl;
        metadata << " Fiducial parameters" << endl;
        metadata << " -------------------" << endl;
        metadata << " x0 : (" << fFid_x0.X() << " , " << fFid_x0.Y() << " , " << fFid_x0.Z() << ")" << endl;
        metadata << " x1 : (" << fFid_x1.X() << " , " << fFid_x1.Y() << " , " << fFid_x1.Z() << ")" << endl;
        metadata << " R : " << fFid_R << endl;
        metadata << " sX : " << fFid_sX << endl;
        metadata << " sY : " << fFid_sY << endl;
        metadata << " -------------------" << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "hitsAnalysis"; }

    // Constructor
    TRestDetectorHitsAnalysisProcess();
    TRestDetectorHitsAnalysisProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorHitsAnalysisProcess();

    ClassDef(TRestDetectorHitsAnalysisProcess, 1);  // Template for a REST "event process" class inherited from
                                            // TRestEventProcess
};
#endif
