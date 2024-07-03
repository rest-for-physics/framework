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

#ifndef REST_TRestResponse
#define REST_TRestResponse

#include "TRestMetadata.h"

/// A response matrix that might be applied to a given component inside a TRestComponent
class TRestResponse : public TRestMetadata {
   private:
    /// The filename used to import the response matrix
    std::string fFilename = "";

    /// It defines the variable name for which the response should be applied to
    std::string fVariable = "";

    /// First element of the response matrix (input/incident, output/detected)
    TVector2 fOrigin = TVector2(0, 0);

    /// The resolution of the response matrix (binning)
    Double_t fBinSize = 0.1;  //<

    /// The response matrix
    std::vector<std::vector<Float_t>> fResponseMatrix;  //<

    /// Determines if the response matrix has been transposed
    Bool_t fTransposed = false;  //<

    /// It allows to decide if the returned response should be interpolated (default:false)
    Bool_t fInterpolation = false;  //!

   public:
    void SetBinSize(Double_t bSize) { fBinSize = bSize; }
    void SetResponseFilename(std::string responseFile) { fFilename = responseFile; }
    void SetOrigin(const TVector2& v) { fOrigin = v; }
    void SetVariable(const std::string& var) { fVariable = var; }

    Bool_t ApplyInterpolation() { return fInterpolation; }
    void Interpolate(Bool_t interpolate = true) { fInterpolation = interpolate; }

    Double_t GetBinSize() const { return fBinSize; }
    std::string GetResponseFilename() const { return fFilename; }
    TVector2 GetOrigin() const { return fOrigin; }
    std::string GetVariable() const { return fVariable; }

    TVector2 GetInputRange() const {
		if( fResponseMatrix.empty() ) {
			RESTError << " TRestResponse::GetInputRange. Response matrix not loaded yet, try using LoadResponse" << RESTendl;
			return 0;
		}
        return TVector2(fOrigin.X(), fOrigin.X() + fResponseMatrix[0].size() * fBinSize);
    }

    TVector2 GetOutputRange() const {
		if( fResponseMatrix.empty() ) {
			RESTError << " TRestResponse::GetOutputRange. Response matrix not loaded yet, try using LoadResponse" << RESTendl;
			return 0;
		}
        return TVector2(fOrigin.Y(), fOrigin.Y() + fResponseMatrix.size() * fBinSize);
    }


    void Initialize() override;

    void LoadResponse(Bool_t transpose = true);

	Double_t GetOverallEfficiency(Double_t from, Double_t to);

    std::vector<std::pair<Double_t, Double_t>> GetResponse(Double_t input);

    void PrintResponseMatrix(Int_t fromRow, Int_t toRow);

    void PrintMetadata() override;

    std::vector<std::vector<Float_t>> GetMatrix() const { return fResponseMatrix; }

    TRestResponse(const char* cfgFileName, const std::string& name = "");
    TRestResponse();
    ~TRestResponse();

    ClassDefOverride(TRestResponse, 1);
};
#endif
