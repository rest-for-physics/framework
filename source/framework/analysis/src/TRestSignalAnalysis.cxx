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

#include <TF1.h>
#include <TFitResult.h>
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
/// \brief Return derivative of a vector of data
/// points
///
template <typename T>
std::vector<Float_t> TRestSignalAnalysis::GetDerivative(const std::vector<T>& signal) {
    std::vector<Float_t> derivative(0, signal.size() - 1);

    for (size_t i = 0; i < signal.size() - 1; i++) {
        derivative[i] = signal[i + 1] - signal[i];
    }

    return derivative;
}

template std::vector<Float_t> TRestSignalAnalysis::GetDerivative(const std::vector<Float_t>& signal);
template std::vector<Float_t> TRestSignalAnalysis::GetDerivative(const std::vector<Short_t>& signal);

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

///////////////////////////////////////////////
/// \brief It returns the integral of the signal in the
/// range passed as argument
///
template <typename T>
Double_t TRestSignalAnalysis::GetIntegral(const std::vector<T>& signal, Int_t startBin, Int_t endBin) {
    Double_t sum = 0;
    for (int i = startBin; i < endBin; i++)
        if (i > 0 && i < (int)signal.size()) sum += signal[i];

    return sum;
}
template Double_t TRestSignalAnalysis::GetIntegral(const std::vector<Short_t>& signal, Int_t startBin,
                                                   Int_t endBin);
template Double_t TRestSignalAnalysis::GetIntegral(const std::vector<Float_t>& signal, Int_t startBin,
                                                   Int_t endBin);

///////////////////////////////////////////////
/// \brief It returns the width of the pulses
/// as the number of bins between the half of the
/// maximum
///
template <typename T>
Double_t TRestSignalAnalysis::GetMaxPeakWidth(const std::vector<T>& signal) {
    Int_t maxIndex = GetMaxBin(signal);
    Double_t maxValue = signal[maxIndex];

    const int signalSize = signal.size();

    Double_t value = maxValue;
    Int_t rightIndex = maxIndex;
    while (value > maxValue / 2. && rightIndex < signalSize) {
        value = signal[rightIndex];
        rightIndex++;
    }
    Int_t leftIndex = maxIndex;
    value = maxValue;
    while (value > maxValue / 2. && leftIndex > 0) {
        value = signal[rightIndex];
        leftIndex--;
    }

    return rightIndex - leftIndex;
}
template Double_t TRestSignalAnalysis::GetMaxPeakWidth(const std::vector<Short_t>& signal);
template Double_t TRestSignalAnalysis::GetMaxPeakWidth(const std::vector<Float_t>& signal);

///////////////////////////////////////////////
/// \brief It performs a gaussian fit to the signal
/// which is stored inside a TGraph. It returns
/// a TVector 2 with the maximum and the mean of the
/// gaussian fit
///
TVector2 TRestSignalAnalysis::GetMaxGauss(TGraph* signal) {
    Int_t maxBin = TMath::LocMax(signal->GetN(), signal->GetY());
    Double_t maxTime = signal->GetPointX(maxBin);
    Double_t gaussMax = -1, gaussMean = -1;
    Double_t lowerLimit = maxTime - maxTime * 0.1;  // us
    Double_t upperLimit = maxTime + maxTime * 0.2;  // us

    TF1* gaus = new TF1("gaus", "gaus", lowerLimit, upperLimit);
    TFitResultPtr fitResult = signal->Fit(gaus, "QNRS");

    if (fitResult->IsValid()) {
        gaussMax = gaus->GetParameter(0);
        gaussMean = gaus->GetParameter(1);
    } else {
        // the fit failed, return -1 to indicate failure
        std::cout << std::endl
                  << "WARNING: bad fit with maximum at time = " << maxTime
                  << "Failed fit parameters = " << gaus->GetParameter(0) << " || " << gaus->GetParameter(1)
                  << " || " << gaus->GetParameter(2) << "\n"
                  << "Assigned fit parameters : energy = " << gaussMax << ", time = " << gaussMean
                  << std::endl;
        /*
        TCanvas* c2 = new TCanvas("c2", "Signal fit", 200, 10, 1280, 720);
        signal->Draw();
        c2->Update();
        getchar();
        delete c2;
        */
    }

    delete gaus;

    return TVector2(gaussMean, gaussMax);
}

///////////////////////////////////////////////
/// \brief It performs a landau fit to the signal
/// which is stored inside a TGraph. It returns
/// a TVector 2 with the maximum and the mean of the
/// landau fit
///
TVector2 TRestSignalAnalysis::GetMaxLandau(TGraph* signal) {
    Int_t maxBin = TMath::LocMax(signal->GetN(), signal->GetY());
    Double_t maxTime = signal->GetPointX(maxBin);
    Double_t landauMax = -1, landauMean = -1;
    Double_t lowerLimit = maxTime - maxTime * 0.1;  // us
    Double_t upperLimit = maxTime + maxTime * 0.2;  // us

    TF1* landau = new TF1("landau", "landau", lowerLimit, upperLimit);
    TFitResultPtr fitResult = signal->Fit(landau, "QNRS");

    if (fitResult->IsValid()) {
        landauMax = landau->GetParameter(0);
        landauMean = landau->GetParameter(1);
    } else {
        // the fit failed, return -1 to indicate failure
        std::cout << std::endl
                  << "WARNING: bad fit with maximum at time = " << maxTime
                  << "Failed fit parameters = " << landau->GetParameter(0) << " || "
                  << landau->GetParameter(1) << " || " << landau->GetParameter(2) << "\n"
                  << "Assigned fit parameters : energy = " << landauMax << ", time = " << landauMean
                  << std::endl;
        /*
        TCanvas* c2 = new TCanvas("c2", "Signal fit", 200, 10, 1280, 720);
        signal->Draw();
        c2->Update();
        getchar();
        delete c2;
        */
    }

    delete landau;

    return TVector2(landauMean, landauMax);
}

///////////////////////////////////////////////
/// \brief It performs a fit of the signal to a
/// the aget response function. It returns a
/// TVector 2 with the maximum and the mean of the
/// gaussian fit
///
TVector2 TRestSignalAnalysis::GetMaxAget(TGraph* signal) {
    Int_t maxBin = TMath::LocMax(signal->GetN(), signal->GetY());
    Double_t maxTime = signal->GetPointX(maxBin);
    Double_t agetMax = -1, agetMean = -1;
    Double_t lowerLimit = maxTime - maxTime * 0.25;  // us
    Double_t upperLimit = maxTime + maxTime * 0.35;  // us

    // 1.1664 is the x value where the maximum of the base function (i.e. without parameters)
    TF1* aget =
        new TF1("aget",
                "[&](double *x, double *p){ double arg = (x[0] - par[1] + 1.1664) / par[2]; return par[0] / "
                "0.0440895 * exp(-3 * (arg)) * (arg) * (arg) *               (arg)*sin(arg);}",
                lowerLimit, upperLimit, 3);
    TFitResultPtr fitResult = signal->Fit(aget, "QNRS");

    if (fitResult->IsValid()) {
        agetMax = aget->GetParameter(0);
        agetMean = aget->GetParameter(1);
    } else {
        // the fit failed, return -1 to indicate failure
        std::cout << std::endl
                  << "WARNING: bad fit with maximum at time = " << maxTime
                  << "Failed fit parameters = " << aget->GetParameter(0) << " || " << aget->GetParameter(1)
                  << " || " << aget->GetParameter(2) << "\n"
                  << "Assigned fit parameters : energy = " << agetMax << ", time = " << agetMean << std::endl;
        /*
        TCanvas* c2 = new TCanvas("c2", "Signal fit", 200, 10, 1280, 720);
        signal->Draw();
        c2->Update();
        getchar();
        delete c2;
        */
    }

    delete aget;

    return TVector2(agetMean, agetMax);
}

///////////////////////////////////////////////
/// \brief It averages the pulse inside the time window
/// passed as argument. It returns a vector of pairs
/// with the integrated window time and energy (charge)
///
std::vector<std::pair<double, double> > TRestSignalAnalysis::GetIntWindow(TGraph* signal, double intWindow) {
    const int nPoints = signal->GetN();

    std::map<int, std::pair<int, double> > windowMap;
    for (int j = 0; j < nPoints; j++) {
        int index = signal->GetPointX(j) / intWindow;
        auto it = windowMap.find(index);
        if (it != windowMap.end()) {
            it->second.first++;
            it->second.second += signal->GetPointY(j);
        } else {
            windowMap[index] = std::make_pair(1, signal->GetPointY(j));
        }
    }

    std::vector<std::pair<double, double> > result;

    for (const auto& [index, pair] : windowMap) {
        Double_t hitTime = index * intWindow + intWindow / 2.;
        Double_t energy = pair.second / pair.first;
        result.push_back(std::make_pair(hitTime, energy));
    }

    return result;
}

///////////////////////////////////////////////
/// \brief It calculates the triple max over a TGraph
/// and returns an array of pairs with the time and
/// the energy of the maximum and the neighbouring
/// points(three points in total).
///
std::array<std::pair<Double_t, Double_t>, 3> TRestSignalAnalysis::GetTripleMax(TGraph* signal) {
    Int_t maxBin = TMath::LocMax(signal->GetN(), signal->GetY());

    std::array<std::pair<Double_t, Double_t>, 3> tripleMax;

    for (int i = 0; i < 3; i++) {
        int index = maxBin + i - 1;
        if (index > 0 && index < signal->GetN()) {
            tripleMax[i] = std::make_pair(signal->GetPointX(index), signal->GetPointY(index));
        } else {
            tripleMax[i] = std::make_pair(signal->GetPointX(maxBin), signal->GetPointY(maxBin));
        }
    }

    return tripleMax;
}

///////////////////////////////////////////////
/// \brief It calculates the triple max average over
/// a TGraph and returns a TVector2 with the average
/// time and energy (charge)
///
TVector2 TRestSignalAnalysis::GetTripleMaxAverage(TGraph* signal) {
    auto tripleMax = TRestSignalAnalysis::GetTripleMax(signal);
    double eAvg = 0;
    double hitTimeAvg = 0;
    for (const auto& [hitTime, energy] : tripleMax) {
        hitTimeAvg += hitTime * energy;
        eAvg += energy;
    }

    hitTimeAvg /= eAvg;
    eAvg /= 3.;

    return TVector2(hitTimeAvg, eAvg);
}

///////////////////////////////////////////////
/// \brief It calculates the triple max integral over
/// a TGraph and returns the addition of the maximum plus
/// the neigbouring bins.
///
Double_t TRestSignalAnalysis::GetTripleMaxIntegral(TGraph* signal) {
    auto tripleMax = TRestSignalAnalysis::GetTripleMax(signal);
    double totEnergy = 0;
    for (const auto& [hitTime, energy] : tripleMax) {
        totEnergy += energy;
    }

    return totEnergy;
}

///////////////////////////////////////////////
/// \brief It returns the integral of the first positive
/// rise (risetime) over a vector of pairs, that should
/// correspond to the points over threshold for a given signal.
///
Double_t TRestSignalAnalysis::GetSlopeIntegral(const std::vector<std::pair<Float_t, Float_t> >& signal) {
    Double_t sum = 0;
    /*Double_t pVal = 0;
    for (const auto& [index, val] : signal) {
        if (val - pVal < 0) break;
        sum += val;
        pVal = val;
    }*/
    auto max = std::max_element(std::begin(signal), std::end(signal),
                                [](const auto& p1, const auto& p2) { return p1.second < p2.second; });

    for (auto it = signal.begin(); it != max; ++it) sum += it->second;

    return sum;
}

///////////////////////////////////////////////
/// \brief It returns the slope of the first positive
/// rise (risetime) over a vector of pairs, that should
/// correspond to the points over threshold for a given signal.
///
Double_t TRestSignalAnalysis::GetRiseSlope(const std::vector<std::pair<Float_t, Float_t> >& signal) {
    if (signal.size() < 2) return 0;

    auto max = std::max_element(std::begin(signal), std::end(signal),
                                [](const auto& p1, const auto& p2) { return p1.second < p2.second; });

    auto maxBin = max->first;
    auto startBin = signal.front().first;
    Double_t hP = max->second;
    Double_t lP = signal.front().second;

    return (hP - lP) / (maxBin - startBin - 1);
}

///////////////////////////////////////////////
/// \brief It returns the time of the first positive
/// rise or risetime over a vector of pairs, that should
/// correspond to the points over threshold for a given signal.
///
Double_t TRestSignalAnalysis::GetRiseTime(const std::vector<std::pair<Float_t, Float_t> >& signal) {
    if (signal.size() < 2) {
        return 0;
    }

    auto max = std::max_element(std::begin(signal), std::end(signal),
                                [](const auto& p1, const auto& p2) { return p1.second < p2.second; });

    auto maxBin = max->first;
    auto startBin = signal.front().first;
    return maxBin - startBin;
}
