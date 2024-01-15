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

#include <TFormula.h>

#include "TRestComponent.h"
#include "TRestDataSet.h"

/// It defines an analytical component model distribution in a given parameter space (tipically x,y,en)
class TRestComponentFormula : public TRestComponent {
   private:
    /// A vector of formulas that will be added up to integrate a given rate
    std::vector<TFormula> fFormulas;

    /// The formulas should be expressed in the following units
    std::string fUnits = "cm^-2*keV^-1";  //<

   protected:
    void InitFromConfigFile() override;

    void FillHistograms(Double_t precision = 0.01) override;

   public:
    Double_t GetFormulaRate(std::vector<Double_t> point);

    void PrintMetadata() override;

    void Initialize() override;
    TRestComponentFormula(const char* cfgFileName, const std::string& name);
    TRestComponentFormula();
    ~TRestComponentFormula();

    ClassDefOverride(TRestComponentFormula, 1);
};
#endif
