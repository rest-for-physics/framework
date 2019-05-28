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

#ifndef TRestSoft_TRestDummyEvent
#define TRestSoft_TRestDummyEvent

#include <iostream>

#include "TMath.h"
#include "TObject.h"
#include "TVector3.h"

#include "TRestEvent.h"

/// A dummy event data class to serve as example on the implementation of new event data types
class TRestDummyEvent : public TRestEvent {
   private:
    TVector3 fDummyVector;      //-> A vector defining a property of the dummy event
    Double_t fDummyDouble = 0;  //-> A dummy double value

   protected:
   public:
    TVector3* GetDummyVector() { return &fDummyVector; }  // A dummy vector

    Double_t GetDummyVectorX() { return fDummyVector.X(); }  // returns X coordinate
    Double_t GetDummyVectorY() { return fDummyVector.Y(); }  // returns X coordinate
    Double_t GetDummyVectorZ() { return fDummyVector.Z(); }  // returns X coordinate

    Double_t GetDummyValue() { return fDummyDouble; }  // returns value in given units

    void SetDummyVector(TVector3 v) { fDummyVector = v; }  // Sets the value of dummy vector
    void SetDummyVector(Double_t x, Double_t y, Double_t z) { SetDummyVector(TVector3(x, y, z)); }

    void SetDummyValue(Double_t val) { fDummyDouble = val; }

    virtual void Initialize();

    virtual void PrintEvent();

    TPad* DrawEvent(TString option = "");

    // Construtor
    TRestDummyEvent();
    // Destructor
    ~TRestDummyEvent();

    // Increase the value of ClassDef if members in this class are added,removed or modified.
    ClassDef(TRestDummyEvent, 1);
};
#endif
