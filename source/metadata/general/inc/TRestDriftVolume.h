
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

#ifndef RestCore_TRestDriftVolume
#define RestCore_TRestDriftVolume
#include <stdlib.h>
#include <fstream>
#include <iostream>

#include <TROOT.h>
#include "TApplication.h"
#include "TArrayI.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TNamed.h"
#include "TString.h"
#include "TSystem.h"
#include "TVector3.h"

#include "TRestMetadata.h"

class TRestDriftVolume : public TRestMetadata {
   protected:
    string fMaterial;
    Double_t fW;

    Double_t fElectricField;
    Double_t fDriftVelocity;
    Double_t fElectronLifeTime;
    Double_t fLongitudinalDiffusion;
    Double_t fTransversalDiffusion;
    Double_t fTownsendCoefficient;
    Double_t fAttachmentCoefficient;

   public:
    TRestDriftVolume();
    TRestDriftVolume(const char* cfgFileName, string name = "");
    virtual void Initialize();
    virtual void InitFromConfigFile();

    virtual TString GetMaterial() { return fMaterial; }
    virtual Double_t GetW() { return fW; }
    virtual Double_t GetElectricField() { return fElectricField; }

    virtual Double_t GetDriftVelocity() { return fDriftVelocity; }
    virtual Double_t GetElectronLifeTime() { return fElectronLifeTime; }
    virtual Double_t GetLongitudinalDiffusion() { return fLongitudinalDiffusion; }
    virtual Double_t GetTransversalDiffusion() { return fTransversalDiffusion; }
    virtual Double_t GetTownsendCoefficient() { return fTownsendCoefficient; }
    virtual Double_t GetAttachmentCoefficient() { return fAttachmentCoefficient; }

    virtual void SetMaterial(TString value) { fMaterial = value; }
    virtual void SetW(double value) { fW = value; }
    virtual void SetElectricField(double value) { fElectricField = value; }

    virtual void SetDriftVelocity(double value) { fDriftVelocity = value; }
    virtual void SetElectronLifeTime(double value) { fElectronLifeTime = value; }
    virtual void SetLongitudinalDiffusion(double value) { fLongitudinalDiffusion = value; }
    virtual void SetTransversalDiffusion(double value) { fTransversalDiffusion = value; }
    virtual void SetTownsendCoefficient(double value) { fTownsendCoefficient = value; }
    virtual void SetAttachmentCoefficient(double value) { fAttachmentCoefficient = value; }

    virtual void UpdateCondition() {}

    void PrintMetadata();

    ClassDef(TRestDriftVolume, 0);  // Gas Parameters
};

#endif