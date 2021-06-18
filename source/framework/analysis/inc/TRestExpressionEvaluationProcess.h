/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2021 GIFNA/TREX (University of Zaragoza)                *
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

#ifndef RestProcess_TRestExpressionEvaluationProcess
#define RestProcess_TRestExpressionEvaluationProcess

// C++ STL
#include <map>
//#include <variant>
#include <memory>
// ROOT
#include <TH1D.h>
// REST
#include "TRestEventProcess.h"
// private REST
#include "expression_eval.h"

//! An event to select events that should be accepted for further processing
class TRestExpressionEvaluationProcess : public TRestEventProcess {
   private:
    TRestEvent* fEvent;  //!
    // fExprMap stores expressions that are evaluated for their value and the resulting values
    // stored in the output file.
    std::map<string, Expression> fExprMap;
    // fFilterMap stores filtering expressions which are used for their side effects. If their
    // evaluated value is true we keep the event and else drop it.
    std::map<string, Expression> fFilterMap;

    /// A list with the event ids that have been selected.
    std::vector<Int_t> fEventIds;  //<

    void InitFromConfigFile();

    void Initialize();

   protected:
   public:
    any GetInputEvent() { return fEvent; }
    any GetOutputEvent() { return fEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void PrintMetadata();

    /// Returns the name of this process
    TString GetProcessName() { return (TString)"ExpressionEvaluationProcess"; }

    // evaluates an existing expression node
    Either<double, bool> evaluate(shared_ptr<Node> n);

    // Constructor
    TRestExpressionEvaluationProcess();
    // Destructor
    ~TRestExpressionEvaluationProcess() {}

    ClassDef(TRestExpressionEvaluationProcess, 1);
};
#endif
