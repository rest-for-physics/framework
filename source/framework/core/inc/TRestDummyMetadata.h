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

#ifndef RestCore_TRestDummyMetadata
#define RestCore_TRestDummyMetadata

#include "TRestMetadata.h"

/// The most simple metadata class to be used as a template
class TRestDummyMetadata : public TRestMetadata {
   protected:
    /// Documentation of my dummy member
    Double_t fDummy = 0;
    /// Dummy string
    TString fDummyString = "";
    /// Dummy CONSTANT member
    // It is better to use 'const' class members whenever possible (they won't be modified).
    // These members do not need a Setter.
    const Int_t fDummyConstNumber;  // const members can be initialized here, but... (see constructor)

   public:
    void Initialize();

    // Implement only if necessary
    // void InitFromConfigFile();

    void PrintMetadata();

    // Constructor & Destructor
    TRestDummyMetadata(Int_t dummyConstNumber);
    ~TRestDummyMetadata();

    // Setters & Getters
    // Should use "inline" when defining functions in the header
    inline Double_t GetDummy() const { return fDummy; }
    // Here you don't pass as const reference since its a built-in type (Double_t = double)
    inline void SetDummy(Double_t dummy) { fDummy = dummy; }

    inline TString GetDummyString() const { return fDummyString; }
    // IMPORTANT! you should use const references whenever possible, such as in this case.
    // Avoid copying large objects!
    // All types, except built-in types (such as int, double...) should be passed via const reference
    inline void SetDummyString(const TString& dummyString) { fDummyString = dummyString; }

    inline Int_t GetDummyConstNumber() const { return fDummyConstNumber; }

    ClassDef(TRestDummyMetadata, 1);
};

#endif
