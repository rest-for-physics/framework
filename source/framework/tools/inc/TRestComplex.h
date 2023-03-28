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

#ifndef RestCore_TRestComplex
#define RestCore_TRestComplex

#include <string>

#include "Rtypes.h"
#include "TObject.h"
#include "mpreal.h"

/// A generic class to handle complex numbers with real precision
class TRestComplex {
   protected:
    /// The real part of the complex number using MPFR precision
    mpfr::mpreal fRe = 1;
    /// The imaginary part of the complex number using MPFR precision
    mpfr::mpreal fIm = 0;

   public:
    // ctors and dtors
    TRestComplex() {}
    TRestComplex(mpfr::mpreal re, mpfr::mpreal im = 0, Bool_t polar = kFALSE);
    virtual ~TRestComplex() {}

    static void SetPrecision(Int_t precision) {
        mpfr::mpreal::set_default_prec(mpfr::digits2bits(precision));
    }

    static int GetPrecision() { return mpfr::bits2digits(mpfr::mpreal::get_default_prec()); }

    // constants
    static TRestComplex I() { return TRestComplex(0, 1); }
    static TRestComplex One() { return TRestComplex(1, 0); }

    // getters and setters
    mpfr::mpreal Re() const { return fRe; }
    mpfr::mpreal Im() const { return fIm; }
    mpfr::mpreal Rho() const { return mpfr::sqrt(fRe * fRe + fIm * fIm); }
    mpfr::mpreal Rho2() const { return fRe * fRe + fIm * fIm; }
    mpfr::mpreal Theta() const { return (fIm || fRe) ? mpfr::atan2(fIm, fRe) : 0; }
    TRestComplex operator()(mpfr::mpreal x, mpfr::mpreal y, Bool_t polar = kFALSE) {
        if (polar) {
            fRe = x * mpfr::cos(y);
            fIm = x * mpfr::sin(y);
        } else {
            fRe = x;
            fIm = y;
        }
        return *this;
    }

    // Simple operators complex - complex
    TRestComplex operator*(const TRestComplex& c) const {
        return TRestComplex(fRe * c.fRe - fIm * c.fIm, fRe * c.fIm + fIm * c.fRe);
    }
    TRestComplex operator+(const TRestComplex& c) const { return TRestComplex(fRe + c.fRe, fIm + c.fIm); }
    TRestComplex operator/(const TRestComplex& c) const {
        return TRestComplex(fRe * c.fRe + fIm * c.fIm, -fRe * c.fIm + fIm * c.fRe) / c.Rho2();
    }
    TRestComplex operator-(const TRestComplex& c) const { return TRestComplex(fRe - c.fRe, fIm - c.fIm); }

    TRestComplex operator*=(const TRestComplex& c) { return ((*this) = (*this) * c); }
    TRestComplex operator+=(const TRestComplex& c) { return ((*this) = (*this) + c); }
    TRestComplex operator/=(const TRestComplex& c) { return ((*this) = (*this) / c); }
    TRestComplex operator-=(const TRestComplex& c) { return ((*this) = (*this) - c); }

    TRestComplex operator-() { return TRestComplex(-fRe, -fIm); }
    TRestComplex operator+() { return *this; }

    // Simple operators complex - double
    TRestComplex operator*(mpfr::mpreal c) const { return TRestComplex(fRe * c, fIm * c); }
    TRestComplex operator+(mpfr::mpreal c) const { return TRestComplex(fRe + c, fIm); }
    TRestComplex operator/(mpfr::mpreal c) const { return TRestComplex(fRe / c, fIm / c); }
    TRestComplex operator-(mpfr::mpreal c) const { return TRestComplex(fRe - c, fIm); }

    // Simple operators double - complex
    friend TRestComplex operator*(Double_t d, const TRestComplex& c) {
        return TRestComplex(d * c.fRe, d * c.fIm);
    }
    friend TRestComplex operator+(Double_t d, const TRestComplex& c) {
        return TRestComplex(d + c.fRe, c.fIm);
    }
    friend TRestComplex operator/(Double_t d, const TRestComplex& c) {
        return TRestComplex(d * c.fRe, -d * c.fIm) / c.Rho2();
    }
    friend TRestComplex operator-(Double_t d, const TRestComplex& c) {
        return TRestComplex(d - c.fRe, -c.fIm);
    }

    // Convertors
    operator Double_t() const { return static_cast<Double_t>(fRe); }
    operator Float_t() const { return static_cast<Float_t>(fRe); }
    operator Int_t() const { return static_cast<Int_t>(fRe); }

    // TMath:: extensions
    static TRestComplex Sqrt(const TRestComplex& c) {
        return TRestComplex(mpfr::sqrt(c.Rho()), 0.5 * c.Theta(), kTRUE);
    }

    static TRestComplex Exp(const TRestComplex& c) { return TRestComplex(mpfr::exp(c.fRe), c.fIm, kTRUE); }
    static TRestComplex Log(const TRestComplex& c) {
        return TRestComplex(0.5 * mpfr::log(c.Rho2()), c.Theta());
    }
    static TRestComplex Log2(const TRestComplex& c) { return Log(c) / mpfr::log(2); }
    static TRestComplex Log10(const TRestComplex& c) { return Log(c) / mpfr::log(10); }

    static TRestComplex Sin(const TRestComplex& c) {
        return TRestComplex(mpfr::sin(c.fRe) * mpfr::cosh(c.fIm), mpfr::cos(c.fRe) * mpfr::sinh(c.fIm));
    }
    static TRestComplex Cos(const TRestComplex& c) {
        return TRestComplex(mpfr::cos(c.fRe) * mpfr::cosh(c.fIm), -mpfr::sin(c.fRe) * mpfr::sinh(c.fIm));
    }
    static TRestComplex Tan(const TRestComplex& c) {
        TRestComplex cc = Cos(c);
        return Sin(c) * Conjugate(cc) / cc.Rho2();
    }

    inline int Sign(const mpfr::mpreal& re, const mpfr::mpreal& im) {
        /// MUST be implemented
        return 0;
    }

    static TRestComplex ASin(const TRestComplex& c) {
        /// MUST be implemented
        return I();
        // return -I() * Log(I() * c + TMath::Sign(1., c.Im()) * Sqrt(1. - c * c));
    }
    static TRestComplex ACos(const TRestComplex& c) {
        /// MUST be implemented
        return I();
        // return -I() * Log(c + TMath::Sign(1., c.Im()) * Sqrt(c * c - 1.));
    }
    static TRestComplex ATan(const TRestComplex& c) {
        return -0.5 * I() * Log((1. + I() * c) / (1. - I() * c));
    }

    static TRestComplex SinH(const TRestComplex& c) {
        return TRestComplex(mpfr::sinh(c.fRe) * mpfr::cos(c.fIm), mpfr::cosh(c.fRe) * mpfr::sin(c.fIm));
    }
    static TRestComplex CosH(const TRestComplex& c) {
        return TRestComplex(mpfr::cosh(c.fRe) * mpfr::cos(c.fIm), mpfr::sinh(c.fRe) * mpfr::sin(c.fIm));
    }
    static TRestComplex TanH(const TRestComplex& c) {
        TRestComplex cc = CosH(c);
        return SinH(c) * Conjugate(cc) / cc.Rho2();
    }

    static TRestComplex ASinH(const TRestComplex& c) {
        /// MUST be implemented
        return I();
        // return Log(c + TMath::Sign(1., c.Im()) * Sqrt(c * c + 1.));
    }
    static TRestComplex ACosH(const TRestComplex& c) {
        /// MUST be implemented
        return I();
        // return Log(c + TMath::Sign(1., c.Im()) * Sqrt(c * c - 1.));
    }
    static TRestComplex ATanH(const TRestComplex& c) { return 0.5 * Log((1. + c) / (1. - c)); }

    static mpfr::mpreal Abs(const TRestComplex& c) { return c.Rho(); }

    static TRestComplex Power(const TRestComplex& x, const TRestComplex& y) {
        mpfr::mpreal lrho = mpfr::log(x.Rho());
        mpfr::mpreal theta = x.Theta();
        return TRestComplex(mpfr::exp(lrho * y.Re() - theta * y.Im()), lrho * y.Im() + theta * y.Re(), kTRUE);
    }
    static TRestComplex Power(const TRestComplex& x, mpfr::mpreal y) {
        return TRestComplex(mpfr::pow(x.Rho(), y), x.Theta() * y, kTRUE);
    }
    static TRestComplex Power(mpfr::mpreal x, const TRestComplex& y) {
        mpfr::mpreal lrho = mpfr::log(mpfr::abs(x));
        mpfr::mpreal theta = (x > 0) ? 0 : mpfr::const_pi();
        return TRestComplex(mpfr::exp(lrho * y.Re() - theta * y.Im()), lrho * y.Im() + theta * y.Re(), kTRUE);
    }
    static TRestComplex Power(const TRestComplex& x, Int_t y) {
        return TRestComplex(mpfr::pow(x.Rho(), y), x.Theta() * y, kTRUE);
    }

    static Int_t IsNaN(const TRestComplex& c) { return mpfr::isnan(c.Re()) || mpfr::isnan(c.Im()); }

    static TRestComplex Min(const TRestComplex& a, const TRestComplex& b) {
        return a.Rho() <= b.Rho() ? a : b;
    }
    static TRestComplex Max(const TRestComplex& a, const TRestComplex& b) {
        return a.Rho() >= b.Rho() ? a : b;
    }
    static TRestComplex Normalize(const TRestComplex& c) { return TRestComplex(1., c.Theta(), kTRUE); }
    static TRestComplex Conjugate(const TRestComplex& c) { return TRestComplex(c.Re(), -c.Im()); }
    static TRestComplex Range(const TRestComplex& lb, const TRestComplex& ub, const TRestComplex& c) {
        return Max(lb, Min(c, ub));
    }

    // I/O
    friend std::ostream& operator<<(std::ostream& out, const TRestComplex& c);
    friend std::istream& operator>>(std::istream& in, TRestComplex& c);

    ClassDef(TRestComplex, 1)  // Complex Class
};

namespace cling {
std::string printValue(TRestComplex* c);
}

#endif
