/******************** REST disclaimer ***********************************
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

//////////////////////////////////////////////////////////////////////////
/// TRestComplex is a class that defines static methods that might be handy
/// when accessing files or system utilities, or other basic methods that
/// do not fit in a specialized class, and are generic enought to be
/// considered a REST tool.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-December: First concept.
///				   Javier Galan
///
/// \class      TRestComplex
/// \author     Javier Galan <javier.galan@unizar.es>
///
/// <hr>
///

#include "TRestComplex.h"
#include <iostream>
#include <sstream>

ClassImp(TRestComplex);

////////////////////////////////////////////////////////////////////////////////
/// Standard constructor

TRestComplex::TRestComplex(mpfr::mpreal re, mpfr::mpreal im, Bool_t polar) : fRe(re), fIm(im) {
    if (polar) {
        if (re < 0) {
            re = -re;
        }
        fRe = re * mpfr::cos(im);
        fIm = re * mpfr::sin(im);
    }
}

////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const TRestComplex& c) {
    out << "(" << c.fRe << "," << c.fIm << "i)";
    return out;
}

////////////////////////////////////////////////////////////////////////////////

std::istream& operator>>(std::istream& in, TRestComplex& c) {
    in >> c.fRe >> c.fIm;
    return in;
}

namespace cling {
std::string printValue(TRestComplex* c) {
    std::stringstream s;
    s << *c;
    return s.str();
}
}  // namespace cling
