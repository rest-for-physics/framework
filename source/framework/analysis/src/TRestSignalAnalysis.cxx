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

//////////////////////////////////////////////////////////////////////////
/// TRestSignalAnalysis defines several functions to calculate different
/// signal parameters.
///--------------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-December First implementation
/// JuanAn Garcia
///
/// \class TRestSignalAnalysis
/// \author: JuanAn Garcia   e-mail: juanangp@unizar.es
///
/// <hr>
///

#include <TRestSignalAnalysis.h>

///////////////////////////////////////////////
/// \brief This method is used to determine the value
/// of the baseline as average (arithmetic mean) of the
/// data in the range defined between startBin and endBin.
/// The baseline sigma is determined as the standard deviation
/// of the baseline in range provided.
template <typename T>
void TRestSignalAnalysis::CalculateBaselineAndSigmaSD(const std::vector<T>& signal, Int_t startBin,
                                                      Int_t endBin, Double_t& baseLine,
                                                      Double_t& baseLineSigma) {
    baseLine = 0;
    baseLineSigma = 0;

    int nPoints = 0;

    for (int i = startBin; i < endBin; i++) {
        if (i < 0 || i > (int)signal.size()) continue;
        baseLine += signal[i];
        baseLineSigma += signal[i] * signal[i];
        nPoints++;
    }

    if (nPoints > 0) {
        baseLine /= (double)nPoints;
        baseLineSigma = TMath::Sqrt(baseLineSigma / (double)nPoints - baseLine * baseLine);
    }
}
template void TRestSignalAnalysis::CalculateBaselineAndSigmaSD<Short_t>(const std::vector<Short_t>& signal,
                                                                        Int_t startBin, Int_t endBin,
                                                                        Double_t& baseLine,
                                                                        Double_t& baseLineSigma);
template void TRestSignalAnalysis::CalculateBaselineAndSigmaSD<Float_t>(const std::vector<Float_t>& signal,
                                                                        Int_t startBin, Int_t endBin,
                                                                        Double_t& baseLine,
                                                                        Double_t& baseLineSigma);
///////////////////////////////////////////////
/// \brief This method is used to determine the value
/// of the baseline as the median of the data in
/// the range defined between startBin and endBin.
/// The baseline sigma is determined as the interquartile
/// range (IQR) in the baseline range provided. The IQR
/// is more robust towards outliers than the standard deviation.
template <typename T>
void TRestSignalAnalysis::CalculateBaselineAndSigmaIQR(const std::vector<T>& signal, Int_t startBin,
                                                       Int_t endBin, Double_t& baseLine,
                                                       Double_t& baseLineSigma) {
    baseLine = 0;
    baseLineSigma = 0;

    if (startBin < 0) startBin = 0;
    if (endBin >= (int)signal.size()) endBin = signal.size() - 1;

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

template void TRestSignalAnalysis::CalculateBaselineAndSigmaIQR<Short_t>(const std::vector<Short_t>& signal,
                                                                         Int_t startBin, Int_t endBin,
                                                                         Double_t& baseLine,
                                                                         Double_t& baseLineSigma);
template void TRestSignalAnalysis::CalculateBaselineAndSigmaIQR<Float_t>(const std::vector<Float_t>& signal,
                                                                         Int_t startBin, Int_t endBin,
                                                                         Double_t& baseLine,
                                                                         Double_t& baseLineSigma);
///////////////////////////////////////////////
/// \brief This method performs the average of
/// the data points in a given range defined
/// between startBin and endBin
template <typename T>
Double_t TRestSignalAnalysis::GetAverage(const std::vector<T>& signal, Int_t startBin, Int_t endBin) {
    int nPoints = 0;
    Double_t avg = 0;

    for (int i = startBin; i < endBin; i++) {
        if (i < 0 || i > (int)signal.size()) continue;
        avg += signal[i];
        nPoints++;
    }

    if (nPoints > 0) avg /= (double)nPoints;

    return avg;
}

template Double_t TRestSignalAnalysis::GetAverage<Short_t>(const std::vector<Short_t>& signal, Int_t startBin,
                                                           Int_t endBin);
template Double_t TRestSignalAnalysis::GetAverage<Float_t>(const std::vector<Float_t>& signal, Int_t startBin,
                                                           Int_t endBin);

///////////////////////////////////////////////
/// \brief Return smoothing of signal as the
/// moving average of the data
///
/// \param averagingPoints defines the number of
/// neighbouring points used to average the signal
///
template <typename T>
std::vector<Float_t> TRestSignalAnalysis::GetSignalSmoothed(const std::vector<T>& signal,
                                                            int averagingPoints) {
    const int pulseDepth = signal.size();
    std::vector<Float_t> smoothed(pulseDepth, 0);

    averagingPoints = (averagingPoints / 2) * 2 + 1;  // make it odd >= averagingPoints

    Float_t sumAvg = TRestSignalAnalysis::GetAverage(signal, 0, averagingPoints);

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
template std::vector<Float_t> TRestSignalAnalysis::GetSignalSmoothed<Short_t>(
    const std::vector<Short_t>& signal, int averagingPoints);
template std::vector<Float_t> TRestSignalAnalysis::GetSignalSmoothed<Float_t>(
    const std::vector<Float_t>& signal, int averagingPoints);

///////////////////////////////////////////////
/// \brief Return smoothing of signal, the points
/// that are too far away from the median baseline
/// will be ignored to improve the smoothing result.
///
/// \param averagingPoints defines the number of
/// neighbouring points used to average the signal
///
template <typename T>
std::vector<Float_t> TRestSignalAnalysis::GetSignalSmoothed_ExcludeOutliers(const std::vector<T>& signal,
                                                                            int averagingPoints,
                                                                            Double_t& baseLine,
                                                                            Double_t& baseLineSigma) {
    const int pulseDepth = signal.size();
    std::vector<Float_t> smoothed(pulseDepth, 0);

    if (baseLine == 0) CalculateBaselineAndSigmaIQR(signal, 5, int(pulseDepth - 5), baseLine, baseLineSigma);

    averagingPoints = (averagingPoints / 2) * 2 + 1;  // make it odd >= averagingPoints

    Float_t sumAvg = TRestSignalAnalysis::GetAverage(signal, 0, averagingPoints);

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

template std::vector<Float_t> TRestSignalAnalysis::GetSignalSmoothed_ExcludeOutliers<Short_t>(
    const std::vector<Short_t>& signal, int averagingPoints, Double_t& baseLine, Double_t& baseLineSigma);
template std::vector<Float_t> TRestSignalAnalysis::GetSignalSmoothed_ExcludeOutliers<Float_t>(
    const std::vector<Float_t>& signal, int averagingPoints, Double_t& baseLine, Double_t& baseLineSigma);

///////////////////////////////////////////////
/// \brief It returns a vector of the data points
/// that are found over threshold.
/// The parameters provided to this method are
/// used to identify those points.
///
/// \param thrPar A TVector2 defining two parameters: *pointThreshold* and
/// *signalThreshold*. Both numbers
/// define the number of sigmas over the baseline fluctuation, stored in
/// baseLineSigma. The first parameter,
/// *pointThreshold*, serves to identify if a single point is over threshold by
/// satisfying the condition that
/// is above the baseline by the number of sigmas given in *pointThreshold*.
/// Once a certain number of
/// consecutive points have been identified, the parameter *signalThreshold*
/// will serve to reject the signals
/// (consecutive points over threshold) that their standard deviation is lower
/// that *signalThreshold* times
/// the baseline fluctuation.
///
/// \param nPointsOver Only data points with at least *nPointsOver* consecutive
/// points will be considered.
///
/// \param nPointsFlat It will serve to terminate the points over threshold
/// identification in signals where
/// we find an overshoot, being the baseline not returning to zero (or its
/// original value) at the signal tail.
///
/// \param baseLine value of the signal baseline calculated beforehand
///
/// \param baseLineSigma value of the baseline fluctuation calculated beforehand
///

template <typename T>
std::vector<std::pair<Float_t, Float_t> > TRestSignalAnalysis::GetPointsOverThreshold(
    const std::vector<T>& signal, TVector2& range, const TVector2& thrPar, Int_t nPointsOver,
    Int_t nPointsFlat, Double_t baseLine, Double_t baseLineSigma) {
    if (range.X() < 0) range.SetX(0);
    if (range.Y() <= 0) range.SetY(signal.size());

    std::vector<std::pair<Float_t, Float_t> > pointsOverThreshold;

    double pointTh = thrPar.X();
    double signalTh = thrPar.Y();

    double threshold = pointTh * baseLineSigma;

    for (int i = range.X(); i < range.Y(); i++) {
        // Filling a pulse with consecutive points that are over threshold
        if ((signal[i] - baseLine) > threshold) {
            int pos = i;
            std::vector<double> pulse;
            pulse.push_back(signal[i] - baseLine);
            i++;

            // If the pulse ends in a flat end above the threshold, the parameter
            // nPointsFlat will serve to artificially end the pulse.
            // If nPointsFlat is big enough, this parameter will not affect the
            // decision to cut this anomalous behaviour. And all points over threshold
            // will be added to the pulse vector.
            int flatN = 0;
            while (i < range.Y() && (signal[i] - baseLine) > threshold) {
                if (TMath::Abs(signal[i] - signal[i - 1]) > threshold) {
                    flatN = 0;
                } else {
                    flatN++;
                }

                if (flatN < nPointsFlat) {
                    pulse.push_back(signal[i] - baseLine);
                    i++;
                } else {
                    break;
                }
            }

            if (pulse.size() >= (unsigned int)nPointsOver) {
                double mean = std::accumulate(pulse.begin(), pulse.end(), 0.0) / pulse.size();
                double sq_sum = std::inner_product(pulse.begin(), pulse.end(), pulse.begin(), 0.0);
                double stdev = std::sqrt(sq_sum / pulse.size() - mean * mean);

                if (stdev > signalTh * baseLineSigma)
                    for (unsigned int j = 0; j < pulse.size(); j++)
                        pointsOverThreshold.push_back(std::make_pair(pos + j, pulse[j]));
            }
        }
    }

    return pointsOverThreshold;
}

template std::vector<std::pair<Float_t, Float_t> > TRestSignalAnalysis::GetPointsOverThreshold<Short_t>(
    const std::vector<Short_t>& signal, TVector2& range, const TVector2& thrPar, Int_t nPointsOver,
    Int_t nPointsFlat, Double_t baseLine, Double_t baseLineSigma);
template std::vector<std::pair<Float_t, Float_t> > TRestSignalAnalysis::GetPointsOverThreshold<Float_t>(
    const std::vector<Float_t>& signal, TVector2& range, const TVector2& thrPar, Int_t nPointsOver,
    Int_t nPointsFlat, Double_t baseLine, Double_t baseLineSigma);
