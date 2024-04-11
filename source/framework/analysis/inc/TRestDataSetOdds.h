/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see https://gifna.unizar.es/trex                 *
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
 * If not, see https://www.gnu.org/licenses/.                            *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef REST_TRestDataSetOdds
#define REST_TRestDataSetOdds

#include "TH1F.h"
#include "TRestCut.h"
#include "TRestMetadata.h"

/// This class is meant to compute the log odds for different datasets
class TRestDataSetOdds : public TRestMetadata {
   private:
    /// Name of the output file
    std::string fOutputFileName = "";

    /// Name of the dataSet inside the config file
    std::string fDataSetName = "";

    /// Vector containing different obserbable names
    std::vector<std::string> fObsName;

    /// Vector containing different obserbable ranges
    std::vector<TVector2> fObsRange;

    /// Vector containing number of bins for the different observables
    std::vector<int> fObsNbins;

    /// Name of the odds file to be used to get the PDF
    std::string fOddsFile = "";

    /// Cuts over the dataset for PDF selection
    TRestCut* fCut = nullptr;

    /// Map containing the PDF of the different observables
    std::map<std::string, TH1F*> fHistos;  //!

    void Initialize() override;
    void InitFromConfigFile() override;

   public:
    void PrintMetadata() override;

    void ComputeLogOdds();

    std::vector<std::tuple<std::string, TVector2, int>> GetOddsObservables();
    std::string GetOddsFile() { return fOddsFile; }
    std::string GetDataSetName() { return fDataSetName; }
    std::string GetOutputFileName() { return fOutputFileName; }
    TRestCut* GetCut() { return fCut; }

    inline void SetDataSetName(const std::string& dSName) { fDataSetName = dSName; }
    inline void SetOutputFileName(const std::string& outName) { fOutputFileName = outName; }
    inline void SetOddsFile(const std::string& oddsFile) { fOddsFile = oddsFile; }
    inline void SetCut(TRestCut* cut) { fCut = cut; }
    void SetOddsObservables(const std::vector<std::tuple<std::string, TVector2, int>>& obs);
    void AddOddsObservable(const std::string& name, const TVector2& range, int nbins);

    TRestDataSetOdds();
    TRestDataSetOdds(const char* configFilename, std::string name = "");
    ~TRestDataSetOdds();

    ClassDefOverride(TRestDataSetOdds, 1);
};
#endif
