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

//////////////////////////////////////////////////////////////////////////
/// The TRestExpressionEvaluationProcess ...
///
/// A small process to compute arbitrary boolean / float expressions at runtime
/// from a string defined in an RML file. These expressions are evaluated for
/// each event in a run and the result is stored in a user named observable.
///
/// Both floating point as well as boolean expressions are supported. For boolean
/// results the data is stored as integers for better compatibility with other
/// boolean variables used in REST.
///
/// The return type is determined automatically from the expression.
///
/// The expression strings are defined within a special `<expressionset>` tag
/// in the RML file under this process to allow for iteration over all child
/// tags.
/// Within that tag an arbitrary number of `<item>` tags can be added, which
/// must have a `name` and an `expr` field. The `name` field is the name of
/// the resulting observable. The `expr` field is similar to the syntax for
/// ROOT cut strings given to `TTree::draw` to perform filtering on a tree before
/// drawing it with a few small differences (see below).
///
/// A simple example:
///
/// ```xml
/// <addProcess type="TRestExpressionEvaluationProcess" name="my custom expression" value="ON" verboseLevel="silent">
///   <expressionset>
///     <!-- a simple, constant boolean expression -->
///     <item name="boolExpr" expr="5<10"/>
///     <!-- a simple, constant float expression -->
///     <item name="floatExpr" expr="5 * 10"/>
///     <!-- a float expression using an existing REST observable -->
///     <Item name="floatExprObs" expr="hitsAna_energy / 1000.0"/>
///     <!-- a bool expression using an existing REST observable -->
///     <item name="boolExprObs" expr="hitsAna_energy < 5000.0"/>
///     <!-- a bool expression of multiple statements combined by an `and` -->
///     <item name="boolAndExprMultiple" expr="hitsAna_energy < 5000.0 and tckAna_nTracks_X==1"/>
///     <!-- a bool expression of multiple statements combined by an `or` -->
///     <item name="boolOrExprMultiple" expr="hitsAna_energy < 5000.0 or tckAna_nTracks_X==1"/>
///     <!-- a complicated expression of nested parenthesis and multiple expressions -->
///     <item name="boolComplicated" expr="(hitsAna_energy / 1000.0) < 5.0 or (tckAna_nTracks_X==1 and tckAna_nTracks_Y==1)"/>
///   </expressionset>
/// </addProcess>
/// ```
///
/// The main difference to ROOT cut strings are the two facts that 1. the strings are not limited to
/// boolean expressions and 2. that the boolean operatiors `&&` and `||` are instead refered to
/// `and` and `or` respectively. The latter is due to the fact that `&` is an invalid character in
/// XML and thus parsing them is broken (or the user has to write `&amp;`, which is kind of unaccptable).
///
/// Each expression is stored as REST metadata in form of a lisp-like representation of
/// the input expression. For example the last `boolComplicated` example is represented as:
/// ```
/// (|| (< (/ hitsAna_energy 1000.0)) (&& (== tckAna_nTracks_X 1) (== tckAna_nTracks_Y 1)))
/// ```
///
/// *An importang note*: Due to the fact that this depends on *runtime evaluation* of a binary
/// tree, which stores the expression there is a performance penalty over native code!
/// This means expressions should be rewritten as a native REST process in case they are
/// generally useful and not specific.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2021-Mar: Base implementation based on `expressionEval` library
///             Sebastian Schmidt
///
/// \class      TRestExpressionEvaluationProcess
/// \author     Sebastian Schmidt (@Vindaar)
///
/// <hr>
///

#include "TRestExpressionEvaluationProcess.h"

ClassImp(TRestExpressionEvaluationProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestExpressionEvaluationProcess::TRestExpressionEvaluationProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
/// In this case we re-use the initialization of TRestRawToSignalProcess
/// interface class.
///
void TRestExpressionEvaluationProcess::Initialize() {
    SetSectionName(this->ClassName());

    fEvent = NULL;
}

///////////////////////////////////////////////
/// \brief Process initialization.
///
/// TODO It should fill the fEventIDs vector.
///
void TRestExpressionEvaluationProcess::InitProcess() {
    /// TOBE implemented
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestExpressionEvaluationProcess::ProcessEvent(TRestEvent* eventInput) {
    fEvent = eventInput;

    // first evaluate expressions that are used for their return values
    for(auto exprPair : fExprMap) {
        auto value = evaluate(exprPair.second);
	if(!value.isLeft()){
	    fAnalysisTree->SetObservableValue((string)exprPair.first, (double)value.getRight());
	}
	else{
	    fAnalysisTree->SetObservableValue((string)exprPair.first, value.getLeft());
	}
    }
    // now evaluate any filtering expressions for their side effect
    for(auto exprPair : fFilterMap) {
        auto value = evaluate(exprPair.second);
	if(value.isLeft()){
	    // if this evaluation yields a floating point variable, throw
	    throw runtime_error("The given filter expression " + astToStr(exprPair.second) + " returns a " +
				"floating point value instead of a boolean. Mark it as `expr` instead " +
				"to store the result of this expression.");
	}
	else if(!value.getRight()){
	    // if the boolean expression returned false, throw out this event
	    return NULL;
	}
	// else the filter is true, which means we keep the event
    }
    // fill the tree with the computation of the mapping expressions
    fAnalysisTree->Fill();
    return fEvent;
}

///////////////////////////////////////////////
/// \brief Initialization of the process from RML
///
void TRestExpressionEvaluationProcess::InitFromConfigFile() {
    // read the `expressionset` from the RML file and split it up by mapping and
    // filtering expressions.
    auto exprMap = GetExprStrings();
    for(auto namePair: exprMap) {
	// get the pair corresponding to this field (`expr` or `filter`)
	auto exprPair = namePair.second;
	// is filter?
	if(exprPair.first.size() > 0){
	    // is a filter expression
	    Expression expr = parseExpression(exprPair.first);
	    fFilterMap[namePair.first] = expr;
	}
	else{
	    assert(exprPair.second.size() > 0);
	    // is a mapping `expr` expression
	    Expression expr = parseExpression(exprPair.second);
	    fExprMap[namePair.first] = expr;
	}
    }
}

///////////////////////////////////////////////
/// \brief Prints on screen the process data members
///
void TRestExpressionEvaluationProcess::PrintMetadata() {
    BeginPrintProcess();

    for(auto exprPair: fExprMap){
	metadata << exprPair.first << " : " << astToStr(exprPair.second) << endl;
    }
    for(auto exprPair: fFilterMap){
	metadata << exprPair.first << " : " << astToStr(exprPair.second) << endl;
    }

    EndPrintProcess();
}

Either<double, bool> TRestExpressionEvaluationProcess::evaluate(Expression e){
    switch(e->kind){
	case nkBinary:
	    // recurse on both childern
	    switch(e->GetBinaryOp()){
		case boMul: return multiply(evaluate(e->GetLeft()), evaluate(e->GetRight()));
		case boDiv: return divide(evaluate(e->GetLeft()), evaluate(e->GetRight()));
		case boPlus: return plusCmp(evaluate(e->GetLeft()), evaluate(e->GetRight()));
		case boMinus: return minusCmp(evaluate(e->GetLeft()), evaluate(e->GetRight()));
		case boLess: return lessCmp(evaluate(e->GetLeft()), evaluate(e->GetRight()));
		case boGreater: return greaterCmp(evaluate(e->GetLeft()), evaluate(e->GetRight()));
		case boLessEq: return lessEq(evaluate(e->GetLeft()), evaluate(e->GetRight()));
		case boGreaterEq: return greaterEq(evaluate(e->GetLeft()), evaluate(e->GetRight()));
		case boEqual: return equal(evaluate(e->GetLeft()), evaluate(e->GetRight()));
		case boUnequal: return unequal(evaluate(e->GetLeft()), evaluate(e->GetRight()));
		case boAnd: return andCmp(evaluate(e->GetLeft()), evaluate(e->GetRight()));
		case boOr: return orCmp(evaluate(e->GetLeft()), evaluate(e->GetRight()));
		default:
		    throw runtime_error("Invalid binary op kind for token " + astToStr(e));
	    }
	case nkUnary:
	    // apply unary op
	    switch(e->GetUnaryOp()){
		case uoPlus: return evaluate(e->GetUnaryNode());
		case uoMinus: return negative(evaluate(e->GetUnaryNode()));
		case uoNot: return negateCmp(evaluate(e->GetUnaryNode()));
	    }
	case nkIdent: {
	    // return value stored for ident
	    // TODO: check if element exists in map (/in REST)
	    auto ident = e->GetIdent();
	    if(fAnalysisTree->GetObservableType(ident) == "int"){
		return Left<double, bool>((double)fAnalysisTree->GetObservableValue<int>(ident));
	    }
	    else{
		return Left<double, bool>(fAnalysisTree->GetObservableValue<double>(ident));
	    }
	    break;
	}
	case nkFloat:
	    return Left<double, bool>(e->GetVal());
	case nkExpression:
	    return evaluate(e->GetExprNode());
    }
    throw logic_error("Invalid code branch in `evaluate`. Should never end up here!");
}
