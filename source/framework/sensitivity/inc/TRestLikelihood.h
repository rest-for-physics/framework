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

#ifndef REST_TRestLikelihood
#define REST_TRestLikelihood

#include "TRestMetadata.h"

/// UPDATE Write here a brief description. Just one line!
class TRestLikelihood : public TRestMetadata {
   private:
    /// It defines the parameter space for background and signal (e.g: x,y,energy)
    std::vector<std::string> fObservables;  //<

    TRestAnalysisTree* fSignalTree = nullptr;  //!

    TRestAnalysisTree* fBackgroundTree = nullptr;  //!

    void Initialize() override;

   protected:
    /// TO BE DOCUMENTED
    virtual Double_t GetSignal() = 0;

    /// TO BE DOCUMENTED
    virtual Double_t GetBackground() = 0;

   public:
    /// UPDATE Documentation of dummy getter
    Double_t GetDummy() { return fDummy; }

    /// UPDATE Documentation of dummy getter
    Double_t GetDummyVar() { return fDummy; }

    /// UPDATE Documentation of dummy setter
    void SetDummy(const Double_t& dummy) { fDummy = dummy; }

    /// UPDATE Documentation of dummy setter
    void SetDummyVar(const Double_t& dummy) { fDummyVar = dummy; }

   public:
    void PrintMetadata() override;

    TRestLikelihood();
    TRestLikelihood(const char* configFilename, std::string name = "");
    ~TRestLikelihood();

    // REMOVE COMMENT. ROOT class definition helper. Increase the number in it every time
    // you add/rename/remove the metadata members
    ClassDefOverride(TRestLikelihood, 1);
};
#endif
