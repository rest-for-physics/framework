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

#ifndef REST_TRestSensitivity
#define REST_TRestSensitivity

#include "TRestExperiment.h"

/// It combines a number of experimental conditions allowing to calculate a combined experimental sensitivity
class TRestSensitivity : public TRestMetadata {
   private:
    /// A list of experimental conditions included to get a final sensitivity plot
    std::vector<TRestExperiment*> fExperiment;  //<

   public:
    void Initialize() override;

    void PrintMetadata() override;

    TRestSensitivity();
    ~TRestSensitivity();

    ClassDefOverride(TRestSensitivity, 1);
};
#endif