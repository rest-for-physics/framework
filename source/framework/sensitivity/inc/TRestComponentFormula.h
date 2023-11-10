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

#ifndef REST_TRestComponentFormula
#define REST_TRestComponentFormula

#include <THn.h>

#include "TRestComponent.h"
#include "TRestDataSet.h"

/// It defines a background/signal model distribution in a given parameter space (tipically x,y,en)
class TRestComponentFormula : public TRestComponent {
   private:
    /// The function used to initialize the distribution
    /// std::string fFunction = "";  //!
    ///
    /// The function used to initialize the distribution
    /// TFormula fFormula;  //!

    /// A pointer to the component distribution
    // THnD* fDistribution = nullptr;  //!

   protected:
    void InitFromConfigFile() override;

   public:
    Double_t GetRate(std::vector<Double_t> point) override;
    Double_t GetTotalRate() override;

    void PrintMetadata() override;

    void Initialize() override;
    TRestComponentFormula(const char* cfgFileName, const std::string& name);
    TRestComponentFormula();
    ~TRestComponentFormula();

    ClassDefOverride(TRestComponentFormula, 1);
};
#endif
