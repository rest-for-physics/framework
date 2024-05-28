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
/// This class allows to ...
///
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2023-December: First implementation of TRestComponent
/// Javier Galan
///
/// \class TRestComponent
/// \author: Javier Galan (javier.galan.lacarra@cern.ch)
///
/// <hr>
///
#include "TRestComponent.h"

#include <TKey.h>
#include <TLatex.h>

#include <numeric>

ClassImp(TRestComponent);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestComponent::TRestComponent() {}

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
TRestComponent::TRestComponent(const char* cfgFileName, const std::string& name)
    : TRestMetadata(cfgFileName) {
    RESTDebug << "Entering TRestComponent constructor( cfgFileName, name )" << RESTendl;
    RESTDebug << "File: " << cfgFileName << " Name: " << name << RESTendl;
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestComponent::~TRestComponent() {}

///////////////////////////////////////////////
/// \brief It initializes the random number. We avoid to define the section name
/// here since we will never define a TRestComponent section in our RML file,
/// since this class is pure virtual. It will be the inherited class the
/// responsible to define the section name.
///
void TRestComponent::Initialize() {
    //   SetSectionName(this->ClassName());

	/// Avoiding double initialization
	if( !fNodeDensity.empty() && fRandom ) return;

    if (!fRandom) {
        delete fRandom;
        fRandom = nullptr;
    }

    fRandom = new TRandom3(fSeed);
    fSeed = fRandom->TRandom::GetSeed();

	if( fStepParameterValue > 0 )
	{
		RegenerateParametricNodes( fFirstParameterValue, fLastParameterValue, fStepParameterValue, fExponential );
	}
	else
	{
		if( !fParameterizationNodes.empty() ) FillHistograms();
	}
}

/////////////////////////////////////////////
/// \brief It will produce a histogram with the distribution defined using the
/// variables and the weights for each of the parameter nodes.
///
/// fPrecision is used to define the active node
///
void TRestComponent::RegenerateHistograms(UInt_t seed) {
    fNodeDensity.clear();

    fSeed = seed;
	FillHistograms();
}

/////////////////////////////////////////////
/// \brief It allows to produce a parameter nodes list providing the initial
/// value, the final value and the step. We might chose the step growing in
/// linear increase steps or exponential. Linear is the default value.
///
void TRestComponent::RegenerateParametricNodes(Double_t from, Double_t to, Double_t step, Bool_t expIncrease )
{
	fStepParameterValue = step;
	fFirstParameterValue = from;
	fLastParameterValue = to;
	fExponential = expIncrease;

	fParameterizationNodes.clear();

	if( expIncrease ) {
		for( double p = from; p < to; p *= step )
			fParameterizationNodes.push_back(p); 
	}
	else
	{
		for( double p = from; p < to; p += step )
			fParameterizationNodes.push_back(p);
	}

	if( fParameterizationNodes.empty() ) return;
	RegenerateHistograms( fSeed );
}

///////////////////////////////////////////
/// \brief It returns the position of the fVariable element for the variable
/// name given by argument.
///
Int_t TRestComponent::GetVariableIndex(std::string varName) {
    int n = 0;
    for (const auto& v : fVariables) {
        if (v == varName) return n;
        n++;
    }

    return -1;
}

///////////////////////////////////////////////
/// \brief It returns the intensity/rate (in seconds) corresponding to the
/// generated distribution or formula evaluated at the position of the parameter
/// space given by point.
///
/// The response matrix (if defined) will be used to convolute the expected rate.
/// The TRestResponse metadata class defines the variable where the response will
/// be applied.
///
Double_t TRestComponent::GetRate(std::vector<Double_t> point) {
    if (!fResponse) {
        return GetRawRate(point);
    }

    std::string responseVariable = fResponse->GetVariable();
    Int_t respVarIndex = GetVariableIndex(responseVariable);

    if (respVarIndex == -1) {
        RESTError << "The response variable `" << responseVariable << "`, defined inside TRestResponse,"
                  << RESTendl;
        RESTError << "could not be found inside the component." << RESTendl;
        RESTError << "Please, check the component variable names." << RESTendl;
        return 0;
    }

    std::vector<std::pair<Double_t, Double_t> > response = fResponse->GetResponse(point[respVarIndex]);

    Double_t rate = 0;
    for (const auto& resp : response) {
        std::vector<Double_t> newPoint = point;
        newPoint[respVarIndex] = resp.first;
        rate += resp.second * GetRawRate(newPoint);
    }

    return rate;
}

///////////////////////////////////////////////
/// \brief It returns the intensity/rate (in seconds) corresponding to the
/// generated distribution or formula evaluated at the position of the parameter
/// space given by point.
///
/// The rate returned by the TRestComponent::GetRawRate method will be normalized
/// to the corresponding parameter space. Thus, if the parameter consists of
/// 2-spatial dimensions and 1-energy dimension, the returned rate will be
/// expressed in standard REST units as, s-1 mm-2 keV-1.
///
/// The returned value may be recovered back with the desired units using
/// the REST_Units namespace.
///
/// \code
/// component->GetNormalizedRate( {0,0,0} ) * units("cm^-2*keV^-1")
/// \endcode
///
/// The response matrix (if defined) will be used to convolute the expected rate.
/// The TRestResponse metadata class defines the variable where the response will
/// be applied.
///
Double_t TRestComponent::GetNormalizedRate(std::vector<Double_t> point) {
    Double_t normFactor = 1;
    for (size_t n = 0; n < GetDimensions(); n++) normFactor *= fNbins[n] / (fRanges[n].Y() - fRanges[n].X());

    return normFactor * GetRate(point);
}

///////////////////////////////////////////////
/// \brief It returns the intensity/rate (in seconds) corresponding to the
/// generated distribution or formula evaluated at the position of the parameter
/// space given by point. The returned rate is integrated to the granularity
/// of the parameter space (cell size). To get a normalized rate use
/// TRestComponent::GetNormalizedRate.
///
/// The size of the point vector must have the same dimension as the dimensions
/// of the distribution.
///
/// If interpolation is enabled (which is disabled by default) the rate will be
/// evaluated using interpolation with neighbour histogram cells.
///
/// Interpolation technique extracted from:
/// https://math.stackexchange.com/questions/1342364/formula-for-n-dimensional-linear-interpolation
///
/// 𝑓(𝑥0,𝑥1,𝑥2)=𝐴000(1−𝑥0)(1−𝑥1)(1−𝑥2)+𝐴001𝑥0(1−𝑥1)(1−𝑥2)+𝐴010(1−𝑥0)𝑥1(1−𝑥2)⋯+𝐴111𝑥0𝑥1𝑥
///
Double_t TRestComponent::GetRawRate(std::vector<Double_t> point) {
    if (point.size() != GetDimensions()) {
        RESTError << "The size of the point given is : " << point.size() << RESTendl;
        RESTError << "The density distribution dimensions are : " << GetDimensions() << RESTendl;
        RESTError << "Point must have the same dimension as the distribution" << RESTendl;
        return 0;
    }

    if (!HasNodes() && !HasDensity()) {
        RESTError << "TRestComponent::GetRawRate. The component has no nodes!" << RESTendl;
        RESTError << "Try calling TRestComponent::Initialize" << RESTendl;

        RESTInfo << "Trying to initialize" << RESTendl;
        Initialize();
        if (HasNodes())
            RESTInfo << "Sucess!" << RESTendl;
        else
            return 0;
    }

    if (HasNodes() && fActiveNode == -1) {
        RESTError << "TRestComponent::GetRawRate. Active node has not been defined" << RESTendl;
        return 0;
    }

	for( size_t n = 0; n < point.size(); n++ )
	{
		// The point is outside boundaries
		if( point[n] < fRanges[n].X() || point[n] > fRanges[n].Y() )
			return 0;
	}

    Int_t centerBin[GetDimensions()];
    Double_t centralDensity = GetDensity()->GetBinContent(GetDensity()->GetBin(point.data()), centerBin);
    if (!Interpolation()) return centralDensity;

    std::vector<Int_t> direction;
    std::vector<Double_t> nDist;
    for (size_t dim = 0; dim < GetDimensions(); dim++) {
        Double_t x1 = GetBinCenter(dim, centerBin[dim] - 1);
        Double_t x2 = GetBinCenter(dim, centerBin[dim] + 1);

        if (centerBin[dim] == 1 || centerBin[dim] == fNbins[dim]) {
            direction.push_back(0);
            nDist.push_back(0);
        } else if (x2 - point[dim] > point[dim] - x1) {
            // we chose left bin (x1) since it is closer than right bin
            direction.push_back(-1);
            nDist.push_back(1 - 2 * (point[dim] - x1) / (x2 - x1));
        } else {
            direction.push_back(1);
            nDist.push_back(1 - 2 * (x2 - point[dim]) / (x2 - x1));
        }
    }

    // In 3-dimensions we got 8 points to interpolate
    // In 4-dimensions we would get 16 points to interpolate
    // ...
    Int_t nPoints = (Int_t)TMath::Power(2, (Int_t) GetDimensions());

    Double_t sum = 0;
    for (int n = 0; n < nPoints; n++) {
        std::vector<int> cell = REST_StringHelper::IntegerToBinary(n, GetDimensions());

        Double_t weightDistance = 1;
        int cont = 0;
        for (const auto& c : cell) {
            if (c == 0)
                weightDistance *= (1 - nDist[cont]);
            else
                weightDistance *= nDist[cont];
            cont++;
        }

        for (size_t k = 0; k < cell.size(); k++) cell[k] = cell[k] * direction[k] + centerBin[k];

        Double_t density = GetDensity()->GetBinContent(cell.data());
        sum += density * weightDistance;
    }

    return sum;
}

///////////////////////////////////////////////
/// \brief This method integrates the rate to all the parameter space defined in the density function.
/// The result will be returned in s-1.
///
Double_t TRestComponent::GetTotalRate() {
    THnD* dHist = GetDensityForActiveNode();
    if ( !dHist) return 0;

    Double_t integral = 0;
	for (Int_t n = 0; n < dHist->GetNbins(); ++n) {

		Int_t centerBin[GetDimensions()];
		std::vector <Double_t> point;

		dHist->GetBinContent(n, centerBin);
		for (size_t d = 0; d < GetDimensions(); ++d)
			point.push_back( GetBinCenter( d, centerBin[d] ) );

		Bool_t skip = false;
		for (size_t d = 0; d < GetDimensions(); ++d)
		{
			if( point[d] < fRanges[d].X() || point[d] > fRanges[d].Y() )
				skip = true;
		}
		if( !skip )
			integral += GetRate( point );
	}

    return integral;
}

///////////////////////////////////////////////
/// \brief It returns the bin center of the given component dimension.
///
/// It required implementation since I did not find a method inside THnD. Surprising.
///
Double_t TRestComponent::GetBinCenter(Int_t nDim, const Int_t bin) {
    return fRanges[nDim].X() + (fRanges[nDim].Y() - fRanges[nDim].X()) * ((double)bin - 0.5) / fNbins[nDim];
}

ROOT::RVecD TRestComponent::GetRandom() {
    Double_t* tuple = new Double_t[GetDimensions()];

    ROOT::RVecD result;
    if (!GetDensity()) {
        for (size_t n = 0; n < GetDimensions(); n++) result.push_back(0);
        RESTWarning << "TRestComponent::GetRandom. Component might not be initialized! Use "
                       "TRestComponent::Initialize()."
                    << RESTendl;
        return result;
    }

    GetDensity()->GetRandom(tuple);

    for (size_t n = 0; n < GetDimensions(); n++) result.push_back(tuple[n]);
    return result;
}

ROOT::RDF::RNode TRestComponent::GetMonteCarloDataFrame(Int_t N) {
    ROOT::RDF::RNode df = ROOT::RDataFrame(N);

    // Function to fill the RDataFrame using GetRandom method
    auto fillRndm = [&]() {
        ROOT::RVecD randomValues = GetRandom();
        return randomValues;
    };
    df = df.Define("Rndm", fillRndm);

    // Creating dedicated columns for each GetRandom component
    for (size_t i = 0; i < fVariables.size(); ++i) {
        auto varName = fVariables[i];
        auto FillRand = [i](const ROOT::RVecD& randomValues) { return randomValues[i]; };
        df = df.Define(varName, FillRand, {"Rndm"});
    }

    /* Excluding Rndm from df */
    std::vector<std::string> columns = df.GetColumnNames();
    std::vector<std::string> excludeColumns = {"Rndm"};
    columns.erase(std::remove_if(columns.begin(), columns.end(),
                                 [&excludeColumns](std::string elem) {
                                     return std::find(excludeColumns.begin(), excludeColumns.end(), elem) !=
                                            excludeColumns.end();
                                 }),
                  columns.end());

    std::string user = getenv("USER");
    std::string fOutName = "/tmp/rest_output_" + user + ".root";
    df.Snapshot("AnalysisTree", fOutName, columns);

    df = ROOT::RDataFrame("AnalysisTree", fOutName);

    return df;
}

///////////////////////////////////////////////
/// \brief A method allowing to draw a series of plots representing the density distributions.
///
/// The method will produce 1- or 2-dimensional histograms of the `drawVariables` given in the
/// argument. A third scan variable must be provided in order to show the distribution slices
/// along the scan variable.
///
/// The binScanSize argument can be used to define the binSize of the scanning variables.
///
TCanvas* TRestComponent::DrawComponent(std::vector<std::string> drawVariables,
                                       std::vector<std::string> scanVariables, Int_t binScanSize,
                                       TString drawOption) {
    if (drawVariables.size() > 2 || drawVariables.size() == 0) {
        RESTError << "TRestComponent::DrawComponent. The number of variables to be drawn must "
                     "be 1 or 2!"
                  << RESTendl;
        return fCanvas;
    }

    if (scanVariables.size() > 2 || scanVariables.size() == 0) {
        RESTError << "TRestComponent::DrawComponent. The number of variables to be scanned must "
                     "be 1 or 2!"
                  << RESTendl;
        return fCanvas;
    }

    //// Checking the number of plots to be generated
    std::vector<int> scanIndexes;
    for (const auto& x : scanVariables) scanIndexes.push_back(GetVariableIndex(x));

    Int_t nPlots = 1;
    size_t n = 0;
    for (const auto& x : scanIndexes) {
        if (fNbins[x] % binScanSize != 0) {
            RESTWarning << "The variable " << scanVariables[n] << " contains " << fNbins[x]
                        << " bins and it doesnt match with a bin size " << binScanSize << RESTendl;
            RESTWarning << "The bin size must be a multiple of the number of bins." << RESTendl;
            RESTWarning << "Redefining bin size to 1." << RESTendl;
            binScanSize = 1;
        }
        nPlots *= fNbins[x] / binScanSize;
        n++;
    }

    /// Finding canvas division scheme
    Int_t nPlotsX = 0;
    Int_t nPlotsY = 0;

    if (scanIndexes.size() == 2) {
        nPlotsX = fNbins[scanIndexes[0]] / binScanSize;
        nPlotsY = fNbins[scanIndexes[1]] / binScanSize;
    } else {
        nPlotsX = TRestTools::CanvasDivisions(nPlots)[1];
        nPlotsY = TRestTools::CanvasDivisions(nPlots)[0];
    }

    RESTInfo << "Number of plots to be generated: " << nPlots << RESTendl;
    RESTInfo << "Canvas size : " << nPlotsX << " x " << nPlotsY << RESTendl;

    //// Setting up the canvas with the appropriate number of divisions
    if (fCanvas != nullptr) {
        delete fCanvas;
        fCanvas = nullptr;
    }

    fCanvas = new TCanvas(this->GetName(), this->GetName(), 0, 0, nPlotsX * 640, nPlotsY * 480);
    fCanvas->Divide(nPlotsX, nPlotsY, 0.01, 0.01);

    std::vector<int> variableIndexes;
    for (const auto& x : drawVariables) variableIndexes.push_back(GetVariableIndex(x));

    for (int n = 0; n < nPlotsX; n++)
        for (int m = 0; m < nPlotsY; m++) {
            TPad* pad = (TPad*)fCanvas->cd(n * nPlotsY + m + 1);
            pad->SetFixedAspectRatio(true);

            THnD* hnd = GetDensity();

            int binXo = binScanSize * n + 1;
            int binXf = binScanSize * n + binScanSize;
            int binYo = binScanSize * m + 1;
            int binYf = binScanSize * m + binScanSize;

            if (scanVariables.size() == 2) {
                hnd->GetAxis(scanIndexes[0])->SetRange(binXo, binXf);
                hnd->GetAxis(scanIndexes[1])->SetRange(binYo, binYf);
            } else if (scanVariables.size() == 1) {
                binXo = binScanSize * nPlotsY * n + binScanSize * m + 1;
                binXf = binScanSize * nPlotsY * n + binScanSize * m + binScanSize;
                hnd->GetAxis(scanIndexes[0])->SetRange(binXo, binXf);
            }

            if (variableIndexes.size() == 1) {
                TH1D* h1 = hnd->Projection(variableIndexes[0]);
                std::string hName;

                if (scanIndexes.size() == 2)
                    hName = scanVariables[0] + "(" + IntegerToString(binXo) + ", " + IntegerToString(binXf) +
                            ") " + scanVariables[1] + "(" + IntegerToString(binYo) + ", " +
                            IntegerToString(binYf) + ") ";

                if (scanIndexes.size() == 1)
                    hName = scanVariables[0] + "(" + IntegerToString(binXo) + ", " + IntegerToString(binXf) +
                            ") ";

                TH1D* newh = (TH1D*)h1->Clone(hName.c_str());
                newh->SetTitle(hName.c_str());
                newh->SetStats(false);
                newh->GetXaxis()->SetTitle((TString)drawVariables[0]);
                newh->SetMarkerStyle(kFullCircle);
                newh->Draw("PLC PMC");

                TString entriesStr = "Entries: " + IntegerToString(newh->GetEntries());
                TLatex* textLatex = new TLatex(0.62, 0.825, entriesStr);
                textLatex->SetNDC();
                textLatex->SetTextColor(1);
                textLatex->SetTextSize(0.05);
                textLatex->Draw("same");
                delete h1;
            }

            if (variableIndexes.size() == 2) {
                TH2D* h2 = hnd->Projection(variableIndexes[0], variableIndexes[1]);

                std::string hName;
                if (scanIndexes.size() == 2)
                    hName = scanVariables[0] + "(" + IntegerToString(binXo) + ", " + IntegerToString(binXf) +
                            ") " + scanVariables[1] + "(" + IntegerToString(binYo) + ", " +
                            IntegerToString(binYf) + ") ";

                if (scanIndexes.size() == 1)
                    hName = scanVariables[0] + "(" + IntegerToString(binXo) + ", " + IntegerToString(binXf) +
                            ") ";

                TH2D* newh = (TH2D*)h2->Clone(hName.c_str());
                newh->SetStats(false);
                newh->GetXaxis()->SetTitle((TString)drawVariables[0]);
                newh->GetYaxis()->SetTitle((TString)drawVariables[1]);
                newh->SetTitle(hName.c_str());
                newh->Draw(drawOption);

                TString entriesStr = "Entries: " + IntegerToString(newh->GetEntries());
                TLatex* textLatex = new TLatex(0.62, 0.825, entriesStr);
                textLatex->SetNDC();
                textLatex->SetTextColor(1);
                textLatex->SetTextSize(0.05);
                textLatex->Draw("same");
                delete h2;
            }
        }

    return fCanvas;
}

///////////////////////////////////////////////
/// \brief
///
void TRestComponent::LoadResponse(const TRestResponse& resp) {
    if (fResponse) {
        delete fResponse;
        fResponse = nullptr;
    }

    fResponse = (TRestResponse*)resp.Clone("response");
    if (fResponse) fResponse->LoadResponse();

    fResponse->PrintMetadata();
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestComponent::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << "Component nature : " << fNature << RESTendl;
    RESTMetadata << " " << RESTendl;

    RESTMetadata << "Random seed : " << fSeed << RESTendl;
    if (fSamples) RESTMetadata << "Samples : " << fSamples << RESTendl;
    RESTMetadata << " " << RESTendl;

    if (fVariables.size() != fRanges.size())
        RESTWarning << "The number of variables does not match with the number of defined ranges!"
                    << RESTendl;

    else if (fVariables.size() != fNbins.size())
        RESTWarning << "The number of variables does not match with the number of defined bins!" << RESTendl;
    else {
        int n = 0;
        RESTMetadata << " === Variables === " << RESTendl;
        for (const auto& varName : fVariables) {
            RESTMetadata << " - Name: " << varName << " Range: (" << fRanges[n].X() << ", " << fRanges[n].Y()
                         << ") bins: " << fNbins[n] << RESTendl;
            n++;
        }
    }

    if (!fParameterizationNodes.empty()) {
        RESTMetadata << " " << RESTendl;
        RESTMetadata << " === Parameterization === " << RESTendl;
        RESTMetadata << "- Parameter : " << fParameter << RESTendl;

        RESTMetadata << " - Number of parametric nodes : " << fParameterizationNodes.size() << RESTendl;
        RESTMetadata << " " << RESTendl;
        RESTMetadata << " Use : PrintNodes() for additional info" << RESTendl;

		if( fStepParameterValue > 0 )
		{
			RESTMetadata << " " << RESTendl;
			RESTMetadata << " Nodes were automatically generated using these parameters" << RESTendl;
			RESTMetadata << " - First node : " << fFirstParameterValue << RESTendl;
			RESTMetadata << " - Upper limit node : " << fLastParameterValue << RESTendl;
			RESTMetadata << " - Increasing step : " << fStepParameterValue << RESTendl;
			if( fExponential ) 
				RESTMetadata << " - Increases exponentially" << RESTendl;
			else
				RESTMetadata << " - Increases linearly" << RESTendl;
		}
    }

    if (fResponse) {
        RESTMetadata << " " << RESTendl;
        RESTMetadata << "A response matrix was loaded inside the component" << RESTendl;
        RESTMetadata << "You may get more details using TRestComponent::GetResponse()->PrintMetadata()"
                     << RESTendl;
    }

    RESTMetadata << "----" << RESTendl;
}

/////////////////////////////////////////////
/// \brief It prints out on screen the values of the parametric node
///
void TRestComponent::PrintNodes() {
    std::cout << " - Number of nodes : " << fParameterizationNodes.size() << std::endl;
    for (const auto& x : fParameterizationNodes) std::cout << x << " ";
    std::cout << std::endl;
}

/////////////////////////////////////////////
/// \brief It customizes the retrieval of XML data values of this class
///
void TRestComponent::InitFromConfigFile() {
    TRestMetadata::InitFromConfigFile();

    auto ele = GetElement("cVariable");
    while (ele != nullptr) {
        std::string name = GetParameter("name", ele, "");
        TVector2 v = Get2DVectorParameterWithUnits("range", ele, TVector2(-1, -1));
        Int_t bins = StringToInteger(GetParameter("bins", ele, "0"));

        if (name.empty() || (v.X() == -1 && v.Y() == -1) || bins == 0) {
            RESTWarning << "TRestComponentFormula::fVariable. Problem with definition." << RESTendl;
            RESTWarning << "Name: " << name << " range: (" << v.X() << ", " << v.Y() << ") bins: " << bins
                        << RESTendl;
        } else {
            fVariables.push_back(name);
            fRanges.push_back(v);
            fNbins.push_back(bins);
        }

        ele = GetNextElement(ele);
    }

    if (fNbins.size() == 0)
        RESTError << "TRestComponent::InitFromConfigFile. No cVariables where found!" << RESTendl;

    if (fResponse) {
        delete fResponse;
        fResponse = nullptr;
    }

    fResponse = (TRestResponse*)this->InstantiateChildMetadata("Response");
    if (fResponse) fResponse->LoadResponse();
}

/////////////////////////////////////////////
/// \brief It returns the position of the fParameterizationNodes
/// element for the variable name given by argument.
///
Int_t TRestComponent::FindActiveNode(Double_t node) {
    int n = 0;
    for (const auto& v : fParameterizationNodes) {
        Double_t pUp = node * (1 + fPrecision / 2);
        Double_t pDown = node * (1 - fPrecision / 2);
        if (v > pDown && v < pUp) {
            fActiveNode = n;
            return fActiveNode;
        }
        n++;
    }

    return -1;
}

/////////////////////////////////////////////
/// \brief It returns the position of the fParameterizationNodes
/// element for the variable name given by argument.
///
Int_t TRestComponent::SetActiveNode(Double_t node) {
    int n = 0;
    for (const auto& v : fParameterizationNodes) {
        Double_t pUp = node * (1 + fPrecision / 2);
        Double_t pDown = node * (1 - fPrecision / 2);
        if (v > pDown && v < pUp) {
            fActiveNode = n;
            return fActiveNode;
        }
        n++;
    }

    RESTError << "Parametric node : " << node << " was not found in component" << RESTendl;
    RESTError << "Keeping previous node as active : " << fParameterizationNodes[fActiveNode] << RESTendl;
    PrintNodes();

    return fActiveNode;
}

/////////////////////////////////////////////
/// \brief
///
THnD* TRestComponent::GetDensityForNode(Double_t node) {
    int n = 0;
    for (const auto& x : fParameterizationNodes) {
        if (x == node) {
            return fNodeDensity[n];
        }
        n++;
    }

    RESTError << "Parametric node : " << node << " was not found in component" << RESTendl;
    PrintNodes();
    return nullptr;
}

/////////////////////////////////////////////
/// \brief
///
THnD* TRestComponent::GetDensityForActiveNode() {
    if (fActiveNode >= 0) return fNodeDensity[fActiveNode];

    RESTError << "The active node is invalid" << RESTendl;
    PrintNodes();
    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns a 1-dimensional projected histogram for the variable names
/// provided in the argument
///
TH1D* TRestComponent::GetHistogram(Double_t node, std::string varName) {
    SetActiveNode(node);
    return GetHistogram(varName);
}

/////////////////////////////////////////////
/// \brief It returns a 1-dimensional projected histogram for the variable names
/// provided in the argument. It will recover the histogram corresponding to
/// the active node.
///
TH1D* TRestComponent::GetHistogram(std::string varName) {
    if (fActiveNode < 0) return nullptr;

    Int_t v1 = GetVariableIndex(varName);

    if (v1 >= 0 && GetDensityForActiveNode()) return GetDensityForActiveNode()->Projection(v1);

    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns the 2-dimensional projected histogram for the variable names
/// provided in the argument
///
TH2D* TRestComponent::GetHistogram(Double_t node, std::string varName1, std::string varName2) {
    SetActiveNode(node);
    return GetHistogram(varName1, varName2);
}

/////////////////////////////////////////////
/// \brief It returns a 2-dimensional projected histogram for the variable names
/// provided in the argument. It will recover the histogram corresponding to
/// the active node.
///
TH2D* TRestComponent::GetHistogram(std::string varName1, std::string varName2) {
    if (fActiveNode < 0) return nullptr;

    Int_t v1 = GetVariableIndex(varName1);
    Int_t v2 = GetVariableIndex(varName2);

    if (v1 >= 0 && v2 >= 0)
        if (GetDensityForActiveNode()) return GetDensityForActiveNode()->Projection(v1, v2);

    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns the 3-dimensional projected histogram for the variable names
/// provided in the argument
///
TH3D* TRestComponent::GetHistogram(Double_t node, std::string varName1, std::string varName2,
                                   std::string varName3) {
    SetActiveNode(node);
    return GetHistogram(varName1, varName2, varName3);
}

/////////////////////////////////////////////
/// \brief It returns a 3-dimensional projected histogram for the variable names
/// provided in the argument. It will recover the histogram corresponding to
/// the active node.
///
TH3D* TRestComponent::GetHistogram(std::string varName1, std::string varName2, std::string varName3) {
    if (fActiveNode < 0) return nullptr;

    Int_t v1 = GetVariableIndex(varName1);
    Int_t v2 = GetVariableIndex(varName2);
    Int_t v3 = GetVariableIndex(varName3);

    if (v1 >= 0 && v2 >= 0 && v3 >= 0)
        if (GetDensityForActiveNode()) return GetDensityForActiveNode()->Projection(v1, v2, v3);

    return nullptr;
}
