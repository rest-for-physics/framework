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

#include <Rtypes.h>
#include <TMath.h>

#include <iostream>
#include <vector>

#ifndef RestCore_TRestSignalAnalysis
#define RestCore_TRestSignalAnalysis

#include <TVector2.h>

#include <numeric>

/// This namespace define utilities (functions) to calculate different signal parameters
namespace TRestSignalAnalysis {

/// Generic functions for different calculations

template <typename T>
void CalculateBaselineAndSigmaSD(const std::vector<T>& signal, Int_t startBin, Int_t endBin,
                                 Double_t& baseLine, Double_t& baseLineSigma) {
    baseLine = 0;
    baseLineSigma = 0;

    int nPoints = 0;

    for (auto i = startBin; i < endBin; i++) {
        if (i < 0 || i > signal.size()) continue;
        baseLine += signal[i];
        baseLineSigma += signal[i] * signal[i];
        nPoints++;
    }

    if (nPoints > 0) {
        baseLine /= (double)nPoints;
        baseLineSigma = TMath::Sqrt(baseLineSigma / (double)nPoints - baseLine * baseLine);
    }
}
void CalculateBaselineAndSigmaSD(const std::vector<Short_t>& signal, Int_t startBin, Int_t endBin,
                                 Double_t& baseLine, Double_t& baseLineSigma);
void CalculateBaselineAndSigmaSD(const std::vector<Float_t>& signal, Int_t startBin, Int_t endBin,
                                 Double_t& baseLine, Double_t& baseLineSigma);

template <typename T>
void CalculateBaselineAndSigmaIQR(const std::vector<T>& signal, Int_t startBin, Int_t endBin,
                                  Double_t& baseLine, Double_t& baseLineSigma) {
    baseLine = 0;
    baseLineSigma = 0;

    if (startBin < 0) startBin = 0;
    if (endBin >= signal.size()) endBin = signal.size() - 1;

    if (endBin > startBin) return;

    auto first = signal.begin() + startBin;
    auto last = signal.begin() + endBin;
    std::vector<T> v(first, last);
    baseLine = TMath::Median(endBin - startBin, &v[0]);

    std::sort(v.begin(), v.end());
    baseLineSigma =
        v[(int)(endBin - startBin) * 0.75] -
        v[(int)(endBin - startBin) * 0.25] /
            1.349;  // IQR/1.349 equals the standard deviation in case of normally distributed data
}

void CalculateBaselineAndSigmaIQR(const std::vector<Short_t>& signal, Int_t startBin, Int_t endBin,
                                  Double_t& baseLine, Double_t& baseLineSigma);
void CalculateBaselineAndSigmaIQR(const std::vector<Float_t>& signal, Int_t startBin, Int_t endBin,
                                  Double_t& baseLine, Double_t& baseLineSigma);

template <typename T>
Double_t GetAverage(const std::vector<T>& signal, Int_t startBin, Int_t endBin) {
    int nPoints = 0;
    Double_t avg = 0;

    for (auto i = startBin; i < endBin; i++) {
        if (i < 0 || i > signal.size()) continue;
        avg += signal[i];
        nPoints++;
    }

    if (nPoints > 0) avg /= (double)nPoints;

    return avg;
}

Double_t GetAverage(const std::vector<Short_t>& signal, Int_t startBin, Int_t endBin);
Double_t GetAverage(const std::vector<Float_t>& signal, Int_t startBin, Int_t endBin);

///////////////////////////////////////////////
/// \brief Return smoothing of signal
///
///
/// \param neighbours It defines the number of neighbours
/// points used to average the signal
///
/// \param option If the option is set to "EXCLUDE OUTLIERS", points that are too far away from the median
/// baseline will be ignored to improve the smoothing result
///
template <typename T>
std::vector<Float_t> GetSignalSmoothed(const std::vector<T>& signal, int averagingPoints = 3) {
    const size_t pulseDepth = signal.size();
    std::vector<Float_t> smoothed(pulseDepth, 0);

    averagingPoints = (averagingPoints / 2) * 2 + 1;  // make it odd >= averagingPoints

    Float_t sumAvg = GetAverage(signal, 0, averagingPoints);

    // Points at the beginning, where we can calculate a moving average
    for (int i = 0; i <= averagingPoints / 2; i++) smoothed[i] = sumAvg;

    for (int i = averagingPoints / 2 + 1; i < pulseDepth - averagingPoints / 2; i++) {
        int index = i - (averagingPoints / 2 + 1);
        sumAvg -= signal[index] / averagingPoints;
        index = i + (averagingPoints / 2 + 1);
        sumAvg += signal[index] / averagingPoints;
        smoothed[i] = sumAvg;
    }

    // Points at the end, where we can calculate a moving average
    for (int i = pulseDepth - averagingPoints / 2; i < pulseDepth; i++) smoothed[i] = sumAvg;

    return smoothed;
}
std::vector<Float_t> GetSignalSmoothed(const std::vector<Short_t>& signal, int averagingPoints = 3);
std::vector<Float_t> GetSignalSmoothed(const std::vector<Float_t>& signal, int averagingPoints = 3);

template <typename T>
std::vector<Float_t> GetSignalSmoothed_ExcludeOutliers(const std::vector<T>& signal, int averagingPoints,
                                                       Double_t& baseLine, Double_t& baseLineSigma) {
    const size_t pulseDepth = signal.size();
    std::vector<Float_t> smoothed(pulseDepth, 0);

    if (baseLine == 0) CalculateBaselineAndSigmaIQR(signal, 5, int(pulseDepth - 5), baseLine, baseLineSigma);

    averagingPoints = (averagingPoints / 2) * 2 + 1;  // make it odd >= averagingPoints

    Float_t sumAvg = GetAverage(signal, 0, averagingPoints);

    // Points at the beginning, where we can calculate a moving average
    for (int i = 0; i <= averagingPoints / 2; i++) smoothed[i] = sumAvg;

    // Points in the middle
    float_t amplitude;
    for (int i = averagingPoints / 2 + 1; i < pulseDepth - averagingPoints / 2; i++) {
        int index = i - (averagingPoints / 2 + 1);
        amplitude = signal[index];
        sumAvg -= (std::abs(amplitude - baseLine) > 3 * baseLineSigma) ? baseLine / averagingPoints
                                                                       : amplitude / averagingPoints;
        index = i + (averagingPoints / 2 + 1);
        amplitude = signal[index];
        sumAvg += (std::abs(amplitude - baseLine) > 3 * baseLineSigma) ? baseLine / averagingPoints
                                                                       : amplitude / averagingPoints;
        smoothed[i] = sumAvg;
    }

    // Points at the end, where we can calculate a moving average
    for (int i = pulseDepth - averagingPoints / 2; i < pulseDepth; i++) smoothed[i] = sumAvg;

    return smoothed;
}

std::vector<Float_t> GetSignalSmoothed_ExcludeOutliers(const std::vector<Short_t>& signal,
                                                       int averagingPoints);
std::vector<Float_t> GetSignalSmoothed_ExcludeOutliers(const std::vector<Float_t>& signal,
                                                       int averagingPoints);

template <typename T>
std::vector<Int_t> GetPointsOverThreshold(const std::vector<T>& signal, TVector2& range,
                                          const TVector2& thrPar, Int_t nPointsOver, Int_t nPointsFlat,
                                          Double_t baseLine, Double_t baseLineSigma) {
    if (range.X() < 0) range.SetX(0);
    if (range.Y() <= 0) range.SetY(signal.size());

    std::vector<Int_t> pointsOverThreshold;

    double pointTh = thrPar.X();
    double signalTh = thrPar.Y();

    double threshold = pointTh * baseLineSigma;

    for (int i = range.X(); i < range.Y(); i++) {
        // Filling a pulse with consecutive points that are over threshold
        double data = signal[i] - baseLine;
        if (data > threshold) {
            int pos = i;
            std::vector<double> pulse;
            pulse.push_back(data);
            i++;

            // If the pulse ends in a flat end above the threshold, the parameter
            // nPointsFlat will serve to artificially end the pulse.
            // If nPointsFlat is big enough, this parameter will not affect the
            // decision to cut this anomalous behaviour. And all points over threshold
            // will be added to the pulse vector.
            int flatN = 0;
            while (i < range.Y() && data > threshold) {
                if (TMath::Abs(signal[i] - signal[i - 1]) > threshold) {
                    flatN = 0;
                } else {
                    flatN++;
                }

                if (flatN < nPointsFlat) {
                    pulse.push_back(data);
                    i++;
                } else {
                    break;
                }
                data = signal[i] - baseLine;
            }

            if (pulse.size() >= (unsigned int)nPointsOver) {
                double mean = std::accumulate(pulse.begin(), pulse.end(), 0.0) / pulse.size();
                double sq_sum = std::inner_product(pulse.begin(), pulse.end(), pulse.begin(), 0.0);
                double stdev = std::sqrt(sq_sum / pulse.size() - mean * mean);

                if (stdev > signalTh * baseLineSigma)
                    for (int j = pos; j < i; j++) pointsOverThreshold.push_back(j);
            }
        }
    }

    return pointsOverThreshold;
}

std::vector<Int_t> GetPointsOverThreshold(const std::vector<Short_t>& signal, TVector2& range,
                                          const TVector2& thrPar, Int_t nPointsOver, Int_t nPointsFlat,
                                          Double_t baseLine, Double_t baseLineSigma);
std::vector<Int_t> GetPointsOverThreshold(const std::vector<Float_t>& signal, TVector2& range,
                                          const TVector2& thrPar, Int_t nPointsOver, Int_t nPointsFlat,
                                          Double_t baseLine, Double_t baseLineSigma);

template <typename T>
Int_t GetMaxBin(const std::vector<T>& signal) {
    return std::distance(signal.begin(), std::max_element(signal.begin(), signal.end()));
}

template <typename T>
Int_t GetMinBin(const std::vector<T>& signal) {
    return std::distance(signal.begin(), std::min_element(signal.begin(), signal.end()));
}

}  // namespace TRestSignalAnalysis

#endif
