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

/////////////////////////////////////////////////////////////////////////
/// Documentation TOBE written
///
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2023-December: First implementation of TRestResponse
/// Javier Galan
///
/// \class TRestResponse
/// \author: Javier Galan (javier.galan.lacarra@cern.ch)
///
/// <hr>
///
#include "TRestResponse.h"

ClassImp(TRestResponse);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestResponse::TRestResponse() { Initialize(); }

/////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// corresponding TRestAxionMagneticField section inside the RML.
///
TRestResponse::TRestResponse(const char* cfgFileName, const std::string& name) : TRestMetadata(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestResponse::~TRestResponse() {}

///////////////////////////////////////////////
/// \brief It will initialize the data frame with the filelist and column names
/// (or observables) that have been defined by the user.
///
void TRestResponse::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief It loads into the fResponseMatrix data member the response from a file.
///
/// For the moment only binary data files with format .N???f have been implemented.
///
/// The response file should be arranged in a way that the more internal `std::vector`
/// (row) inside the `std::vector <std::vector>` table corresponds to a specific
/// transformed energy (output). For example, if we have the incident particle energy
/// (input) and the expected detected energy response (output) for that energy, then
/// each row in the matrix corresponds to a detected energy and each element of that
/// row defines the fraction of incident energies that contributed to that detected
/// energy.
///
/// Thats why we may need to transpose the matrix, so that when we can extract a row
/// (detected energy) from the matrix directly, such as fMatrix[n], and we just get
/// the vector that should convolute with the Signal(Ei) that is a vector of signals
/// as a function of incident energy. The resulting scalar product will give the
/// expected signal at the detection energy.
///
void TRestResponse::LoadResponse(Bool_t transpose) {
    if (fFilename == "") {
        RESTError << "TRestResponse::LoadResponse. The response filename was not defined" << RESTendl;
        return;
    }

    std::string fullFilename = SearchFile(fFilename);
    if (fullFilename.empty()) {
        RESTError << "TRestResponse::LoadResponse. The response filename was not found!" << RESTendl;
        RESTError << "Filename : " << fFilename << RESTendl;
        RESTError << "You may want to define <seachPath inside <globals> definition" << RESTendl;
        return;
    }

    std::string extension = TRestTools::GetFileNameExtension(fFilename);
    if (!extension.empty() && extension[0] == 'N' && extension.back() == 'f') {
        TRestTools::ReadBinaryTable(fullFilename, fResponseMatrix);

        fTransposed = false;
        if (transpose) {
            fTransposed = transpose;
            TRestTools::TransposeTable(fResponseMatrix);
        }

        return;
    }

    RESTError << "Extension format - " << extension << " - not recognized!" << RESTendl;
}

/////////////////////////////////////////////
/// \brief This method will return a vector of std::pair, each pair will contain the
/// output/frequency/energy value for the corresponding response.
///
/// The output value will be mapped following the binning and the origin given on the
/// metadata members.
///
std::vector<std::pair<Double_t, Double_t>> TRestResponse::GetResponse(Double_t input) {
    std::vector<std::pair<Double_t, Double_t>> response;

    if (fResponseMatrix.empty()) {
        RESTError << "TRestResponse::GetResponse. Response matrix has not been loaded yet!" << RESTendl;
        return response;
    }

    if (input < GetInputRange().X() || input > GetInputRange().Y()) {
        RESTError << "TRestResponse::GetResponse. The input value " << input << " is outside range!"
                  << RESTendl;
        return response;
    }

    Int_t binLeft = (Int_t)((input - fBinSize / 2. - fOrigin.X()) / fBinSize);
    Int_t binRight = binLeft + 1;

    Double_t distLeft = (input - fBinSize / 2. + fOrigin.X()) - binLeft * fBinSize;

    if (input <= GetInputRange().X() + fBinSize / 2. || input >= GetInputRange().Y() - fBinSize / 2.)
        binRight = binLeft;

    /*
    std::cout << "Top : " << GetInputRange().Y() - fBinSize/2. << std::endl;
    std::cout << "binLeft : " << binLeft << std::endl;
    std::cout << "binRight : " << binRight << std::endl;
    std::cout << "dLeft : " << distLeft << std::endl;
    std::cout << "dLeft/fBinSize : " << distLeft/fBinSize << std::endl;
    std::cout << "1 - distLeft/fBinSize : " << 1 - distLeft/fBinSize << std::endl;
    */

    for (std::size_t n = 0; n < fResponseMatrix[binLeft].size(); n++) {
        Double_t output = fOrigin.Y() + ((double)n + 0.5) * fBinSize;
        Double_t value = fResponseMatrix[binLeft][n] * (1 - distLeft / fBinSize) +
                         fResponseMatrix[binRight][n] * distLeft / fBinSize;

        std::pair<Double_t, Double_t> outp{output, value};

        response.push_back(outp);

        /*
        std::cout << "n: " << n << " output : " << output << std::endl;
        std::cout << "response: " << response << std::endl;
        */
    }

    return response;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestResponse::PrintResponseMatrix(Int_t fromRow = 0, Int_t toRow = 0) {
    TRestTools::PrintTable(fResponseMatrix, fromRow, toRow);
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestResponse::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << "Response file : " << fFilename << RESTendl;
    RESTMetadata << "Variable : " << fVariable << RESTendl;
    RESTMetadata << "Bin size : " << fBinSize << RESTendl;
    RESTMetadata << " " << RESTendl;

    if (!fResponseMatrix.empty()) {
        RESTMetadata << "Response matrix has been loaded" << RESTendl;
        RESTMetadata << " - Number of columns: " << fResponseMatrix[0].size() << RESTendl;
        RESTMetadata << " - Number of rows : " << fResponseMatrix.size() << RESTendl;
        RESTMetadata << " - Input range : " << GetInputRange().X() << " - " << GetInputRange().Y()
                     << RESTendl;
        RESTMetadata << " - Output range : " << GetOutputRange().X() << " - " << GetOutputRange().Y()
                     << RESTendl;

        if (fTransposed) {
            RESTMetadata << " " << RESTendl;
            RESTMetadata << "Original matrix was transposed" << RESTendl;
        }
    } else {
        RESTMetadata << "Response matrix has NOT been loaded" << RESTendl;
        RESTMetadata << "Try calling TRestResponse::LoadResponse()" << RESTendl;
    }
    RESTMetadata << "----" << RESTendl;
}
