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

#ifndef RestCore_TRestSignalZeroSuppresionProcess
#define RestCore_TRestSignalZeroSuppresionProcess

//#include <TRestGas.h>
//#include <TRestReadout.h>

#include <TRestRawSignalEvent.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

//! A process to identify signal and remove baseline noise from a TRestRawSignalEvent.
class TRestSignalZeroSuppresionProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestRawSignalEvent input
    TRestRawSignalEvent* fRawSignalEvent;  //!

    /// A pointer to the specific TRestSignalEvent output
    TRestSignalEvent* fSignalEvent;  //!

    /// The ADC range used for baseline offset definition
    TVector2 fBaseLineRange;

    /// The ADC range used for integral definition and signal identification
    TVector2 fIntegralRange;

    /// Number of sigmas over baseline fluctuation to accept a point is over threshold.
    Double_t fPointThreshold;

    /// A threshold parameter to accept or reject a pre-identified signal. See process description.
    Double_t fSignalThreshold;

    /// Number of consecutive points over threshold required to accept a signal.
    Int_t fNPointsOverThreshold;

    Int_t fNPointsFlatThreshold;

    /// A parameter to determine if baseline correction has been applied by a previous process
    bool fBaseLineCorrection;

    /// The ADC sampling used to transform ADC units to physical time in the output TRestSignalEvent. Given in
    /// us.
    Double_t fSampling;

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   public:
    any GetInputEvent() { return fRawSignalEvent; }
    any GetOutputEvent() { return fSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Base line range definition : ( " << fBaseLineRange.X() << " , " << fBaseLineRange.Y()
                 << " ) " << endl;
        metadata << "Integral range : ( " << fIntegralRange.X() << " , " << fIntegralRange.Y() << " ) "
                 << endl;
        metadata << "Point Threshold : " << fPointThreshold << " sigmas" << endl;
        metadata << "Signal threshold : " << fSignalThreshold << " sigmas" << endl;
        metadata << "Number of points over threshold : " << fNPointsOverThreshold << endl;
        metadata << "Sampling rate : " << 1. / fSampling << " MHz" << endl;
        metadata << "Max Number of points of flat signal tail : " << fNPointsFlatThreshold << endl;

        if (fBaseLineCorrection)
            metadata << "BaseLine correction is enabled for TRestRawSignalAnalysisProcess" << endl;

        EndPrintProcess();
    }

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestSignalZeroSuppresionProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "signalZeroSuppresion"; }

    TRestSignalZeroSuppresionProcess();
    TRestSignalZeroSuppresionProcess(char* cfgFileName);
    ~TRestSignalZeroSuppresionProcess();

    ClassDef(TRestSignalZeroSuppresionProcess, 2);
};
#endif
