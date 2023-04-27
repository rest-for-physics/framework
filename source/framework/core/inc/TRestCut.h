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

#ifndef RestCore_TRestCut
#define RestCore_TRestCut

#include "TCut.h"
#include "TRestMetadata.h"
#include "TRestRun.h"

//! A class to help on cuts definitions. To be used with TRestAnalysisTree
class TRestCut : public TRestMetadata {
   private:
    /// Vector of TCuts
    std::vector<TCut> fCuts;

    /// Vector of cut strings e.g. when you use a complex cut
    std::vector<std::string> fCutStrings;

    /// Vector of parameter cuts, first item is parameter and second is the condition
    std::vector<std::pair<std::string, std::string> > fParamCut;

   protected:
    void Initialize() override;
    void InitFromConfigFile() override;

   public:
    void AddCut(TCut cut);
    TCut GetCut(std::string name);

    inline auto GetCutStrings() const { return fCutStrings; }
    inline auto GetParamCut() const { return fParamCut; }
    inline auto GetCuts() const { return fCuts; }

    TRestCut& operator=(TRestCut& cut);

    void PrintMetadata() override;

    Int_t Write(const char* name, Int_t option, Int_t bufsize) override;

    // Constructor
    TRestCut();
    // Destructor
    ~TRestCut() {}

    ClassDefOverride(TRestCut, 2);  // Template for a REST "event process" class inherited from
                                    // TRestEventProcess
};
#endif
