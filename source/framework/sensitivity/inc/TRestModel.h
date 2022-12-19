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

#ifndef REST_TRestModel
#define REST_TRestModel

#include "TRestComponent.h"
#include "TRestMetadata.h"

/// A combination of signal and background components that build a complete signal and background model
class TRestModel : public TRestMetadata {
   private:
    // TODO At some point we may want to add here a coupling for each signal component

    /// A vector that includes the signal components in this model
    std::vector<TRestComponent*> fSignal;  //<

    /// A vector that includes the background components in this model
    std::vector<TRestComponent*> fBackground;

   public:
    void Initialize() override;

    Double_t GetSignal(std::vector<Double_t> point);

    Double_t GetBackground(std::vector<Double_t> point);

    void PrintMetadata() override;

    TRestModel();
    ~TRestModel();

    ClassDefOverride(TRestModel, 1);
};
#endif
