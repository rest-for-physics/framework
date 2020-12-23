
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

#ifndef RestCore_TRestCalibration
#define RestCore_TRestCalibration
#include <TROOT.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>

#include "TApplication.h"
#include "TArrayI.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH2.h"
#include "TH3.h"
#include "TNamed.h"
#include "TRestMetadata.h"
#include "TString.h"
#include "TSystem.h"
#include "TVector3.h"

class TRestDetectorReadoutModule;
class TRestDetectorReadout;

class TRestDetectorGainMap : public TRestMetadata {
   public:
    bool relative;
    map<int, double> fChannelGain;  // [channel id, gain]
    TH2F* f2DGainMapping = 0;       //->
    TH3F* f3DGainMapping = 0;       //->

    void InitFromConfigFile();

    void SaveToText(string filename) {}
    void ReadGainText(string filename) {}

    void DrawChannelGainMap(TRestDetectorReadoutModule* mod = 0);

    ClassDef(TRestDetectorGainMap, 1);  // Gas Parameters
};
#endif
