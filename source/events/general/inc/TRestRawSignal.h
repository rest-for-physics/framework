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

#ifndef RestCore_TRestRawSignal
#define RestCore_TRestRawSignal

#include <iostream>

#include <TGraph.h>
#include <TObject.h>
#include <TString.h>
#include <TVector2.h>

/// This class defines a Short_t array with a physical parameter that evolves in time using a fixed time bin.
class TRestRawSignal : public TObject {
   private:
    void CalculateThresholdIntegral();

    void CalculateBaseLineSigma(Int_t startBin, Int_t endBin);

   protected:
    /// An integer value used to attribute a unique identification number to the signal.
    Int_t fSignalID;

    /// Vector with the data of the signal
    std::vector<Short_t> fSignalData;

   public:
    /// A TGraph pointer used to store the TRestRawSignal drawing
    TGraph* fGraph;  //!

    /// A vector containing the index of points that are identified over threshold.
    std::vector<Int_t> fPointsOverThreshold;  //!

    /// It stores the integral value obtained from the points identified over threshold.
    Double_t fThresholdIntegral = -1;  //!

    /// It defines the number of points to inclulde before point over threshold definition. NOT implemented.
    Int_t fHeadPoints;  //!

    /// It defines the number of points to include after point over threshold definition. NOT implemented.
    Int_t fTailPoints;  //!

    /// This baseline value will be substracted from GetData for any raw signal observable calculation.
    Double_t fBaseLine = 0;  //!

    /// The baseline fluctuation calculated as the standard deviation of the baseline.
    Double_t fBaseLineSigma = 0;  //!

    /// Any signal calculation will be restricted to the following range definition.
    TVector2 fRange = TVector2(0, 0);  //!

    /// Returns the value of signal ID
    Int_t GetSignalID() { return fSignalID; }

    /// Returns the value of signal ID
    Int_t GetID() { return fSignalID; }

    /// Returns the actual number of points, or size of the signal
    Int_t GetNumberOfPoints() { return fSignalData.size(); }

    /// Returns a vector containing the indexes of data points over threshold
    std::vector<Int_t> GetPointsOverThreshold() { return fPointsOverThreshold; }

    /// Returns the maximum value found in the data points. It includes baseline correction
    Double_t GetMaxValue() { return GetMaxPeakValue(); }

    /// Returns the lowest value found in the data points. It includes baseline correction
    Double_t GetMinValue() { return GetMinPeakValue(); }

    /// Returns the number of head points used on points over threshold definition
    Int_t GetHeadPoints() { return fHeadPoints; }

    /// Returns the number of tail points used on points over threshold definition
    Int_t GetTailPoints() { return fTailPoints; }

    /// Returns the value of baseline that it is initialized after calling CalculateBaseLine.
    Double_t GetBaseLine() { return fBaseLine; }

    /// Returns the value of baseline that it is initialized after calling CalculateBaseLineSigma.
    Double_t GetBaseLineSigma() { return fBaseLineSigma; }

    /// Returns the range defined by user
    TVector2 GetRange() { return fRange; }

    Double_t GetData(Int_t n);

    Short_t operator[](Int_t n);

    /// It sets the id number of the signal
    void SetSignalID(Int_t sID) { fSignalID = sID; }

    /// It sets the id number of the signal
    void SetID(Int_t sID) { fSignalID = sID; }

    /// It sets the number of head points
    void SetHeadPoints(Int_t p) { fHeadPoints = p; }

    /// It sets the number of tail points
    void SetTailPoints(Int_t p) { fTailPoints = p; }

    /// It sets/constrains the range for any calculation.
    void SetRange(TVector2 rng) { fRange = rng; }

    /// It sets/constrains the range for any calculation.
    void SetRange(Int_t from, Int_t to) { fRange = TVector2(from, to); }

    void Reset();

    void Initialize();

    void AddPoint(Short_t d);

    void AddCharge(Short_t d);

    void AddDeposit(Short_t d);

    void IncreaseBinBy(Int_t bin, Double_t data);

    void InitializePointsOverThreshold(TVector2 thrPar, Int_t nPointsOver, Int_t nPointsFlat = 512);

    Double_t GetIntegral();

    Double_t GetIntegralInRange(Int_t startBin, Int_t endBin);

    Double_t GetThresholdIntegral();

    Double_t GetTripleMaxIntegral();

    Double_t GetSlopeIntegral();

    Double_t GetRiseSlope();

    Int_t GetRiseTime();

    Double_t GetAverageInRange(Int_t startBin, Int_t endBin);

    Int_t GetMaxPeakWidth();

    Double_t GetMaxPeakValue();

    Int_t GetMaxPeakBin();

    Double_t GetMinPeakValue();

    Int_t GetMinPeakBin();

    void GetDifferentialSignal(TRestRawSignal* diffSgnl, Int_t smearPoints);

    void GetSignalSmoothed(TRestRawSignal* smthSignal, Int_t averagingPoints);

    void GetWhiteNoiseSignal(TRestRawSignal* noiseSgnl, Double_t noiseLevel = 1.);

    void CalculateBaseLine(Int_t startBin, Int_t endBin);

    Double_t GetStandardDeviation(Int_t startBin, Int_t endBin);

    void AddOffset(Short_t offset);

    void SignalAddition(TRestRawSignal* inSgnl);

    void Scale(Double_t value);

    void WriteSignalToTextFile(TString filename);

    void Print();

    TGraph* GetGraph(Int_t color = 1);

    TRestRawSignal();
    TRestRawSignal(Int_t nBins);
    ~TRestRawSignal();

    ClassDef(TRestRawSignal, 1);
};
#endif
