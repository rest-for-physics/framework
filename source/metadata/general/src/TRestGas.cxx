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


 //////////////////////////////////////////////////////////////////////////
 ///
 /// This metadata class allows to define an arbitrary gas mixture. TRestGas provides
 /// access to different gas properties, as drift velocity, diffusion, townsend
 /// coefficients, etc. TRestGas is a re-implementation of Garfield::MediumMagboltz
 /// allowing to generate or read gas Magboltz files, from which the gas properties
 /// are extracted.
 ///
 /// ### Gas definition
 /// 
 /// This class defines metadata members that can be used to define a gas mixture 
 /// of any number of components, the gas conditions, as pressure or temperature,
 /// and few other parameters required by Garfield to perform the gas properties
 ///  calculation.
 ///
 /// The following code shows an example of the implementation of the TRestGas section
 /// definition in an RML file.
 ///
 /// \code
 ///
 ///  <TRestGas name="Xenon 10-10E3Vcm" title="Xenon">
 ///     <parameter name="pressure" value="1" />
 ///     <parameter name="temperature" value="293.15" />
 ///     <parameter name="maxElectronEnergy" value="400" />
 ///     <parameter name="W_value" value="21.9" />
 ///     <parameter name="nCollisions" value="10" />
 ///     <eField Emin="10" Emax="1000." nodes="20" />
 ///     <gasComponent name="xe" fraction="1" />
 ///  </TRestGas>
 ///
 /// \endcode
 ///
 /// All those parameters are required for the Magboltz calculation. Any number of 
 /// gas components can be defined by adding new *gasComponent* lines to the 
 /// TRestGas section, assuring first that the addition of the gas fractions equals 1.
 /// Any gas component defined at the gases table in the  
 /// <a href="https://garfieldpp.web.cern.ch/garfieldpp/documentation/"> Garfield++ 
 /// user guide </a> can be used in REST. We use the same gas component name convention.
 /// 
 /// ### Pre-generated gas files
 ///
 /// The calculation of the gas properties is computationally expensive and time consuming.
 /// In order to avoid this calculation, we keep some gas files containning the summary of
 /// the gas properties inside the directory inputdata/gasFiles. These files cover different 
 /// conditions of gas and can be used if a new TRestGas object meets the condition among 
 /// one of them. The TRestGas sections that were used to
 /// generate those gas files can be found at $REST_PATH/inputData/definitions/gases.rml.
 ///
 /// Gas files will be searched by TRestGas in the paths that can be
 /// defined inside our main RML file through the section "searchPath" at the
 /// *globals* section (see TRestMetadata description for additional details).
 ///
 /// \code
 ///
 /// <globals>
 ///
 ///     <searchPath value="${REST_INPUTDATA}/definitions/"/>
 ///
 ///      ...
 ///
 /// </globals>
 ///
 /// \endcode
 ///
 /// If no gas files are found meets the current gas conditon, TRestGas will perform a single 
 /// E calculation in the next Get() method, including GetDriftVelocity()/GetLongitudinalDiffusion(), etc.
 /// This usually takes several minutes. After the calculation, Get() methods can quickly 
 /// return the result if input drift field doesn't change.
 ///
 /// ### Better saving in root file
 ///
 /// In the new verison, we save TRestGas together with the gas file(as a TString). So when retrieved 
 /// from a root file, TRestGas is immediately ready after calling the method InitFromRootFile(). This
 /// makes "importMetadata" much easier in TRestRun. So we recommend to migrate gasFiles to root files. 
 /// 
 /// \code
 ///
 /// <TRestRun>
 ///
 ///  <TRestGas name="Xenon 10-10E3Vcm" title="Xenon">
 ///     <parameter name="pressure" value="1" />
 ///     <parameter name="temperature" value="293.15" />
 ///     <parameter name="maxElectronEnergy" value="400" />
 ///     <parameter name="W_value" value="21.9" />
 ///     <parameter name="nCollisions" value="10" />
 ///     <eField Emin="10" Emax="1000." nodes="20" />
 ///     <gasComponent name="xe" fraction="1" />
 ///  </TRestGas> //may be slow if gas file does not exist
 ///
 ///  <addMetadata name="Xenon 10-10E3Vcm" file="gases.root"/> //better to use in future
 ///
 ///      ...
 ///
 /// </TRestRun>
 ///
 /// \endcode
 ///
 /// ### Instantiating TRestGas
 ///
 /// The most common use of TRestGas starts by creating an instance of TRestGas giving as
 /// argument the RML configuration file and the name of the TRestGas section describing the 
 /// gas we are willing to use in our event processing.
 ///
 /// \code
 /// 
 /// TRestGas *gas = new TRestGas( "gases.rml", "GasName" );
 ///
 /// \endcode
 ///
 /// where *GasName* must be an existing TRestGas section with that name, *GasName* could 
 /// be for example *Xenon 10-10E3Vcm*, found at *gases.rml*.
 ///
 /// By default, the gas generation is disabled. So that we are warned by default of the 
 /// inexistence of a pre-generated gas file. To force the generation of a gas file (requiring 
 /// few hours of computation time), we can specify it at construction time as follows.
 ///
 /// \code
 /// 
 /// TRestGas *gas = new TRestGas( "config.rml", "GasName", true );
 ///
 /// \endcode
 /// 
 /// ### Using TRestGas to obtain gas properties
 ///
 /// Once the gas file has been generated we can use it directly calling to the same RML gas 
 /// section used to generate it.
 ///
 /// \code
 /// 
 /// TRestGas *gas = new TRestGas( "config.rml", "GasName" );
 ///
 /// \endcode
 /// 
 /// we can then modify some gas conditions as, i.e. the pressure,
 ///
 /// \code
 /// 
 /// gas->SetPressure( 5. ); // To set the gas pressure to 5 bar
 ///
 /// \endcode
 ///
 /// then we could get the gas mixture properties, as drift velocity,
 /// or diffusion coefficients, at 5 bar by using
 ///
 /// \code
 /// 
 /// gas->GetDriftVelocity( 100. ); // To get the electron drift velocity at 100V/cm
 /// gas->GetLongitudinalDiffusion( 100. ); // To get the longitudinal diffusion coefficient at 100V/cm
 ///
 /// \endcode
 /// 
 /// 
 ///
 ///--------------------------------------------------------------------------
 ///
 /// RESTsoft - Software for Rare Event Searches with TPCs
 ///
 /// History of developments:
 ///
 ///	jan 2006:	first concept (as MagboltzGas)
 ///				Igor G. Irastorza
 ///
 ///	jul 2006:	minor improvements for inclusion in RESTSoft
 ///				(renamed as TMagboltzGas)
 ///				Igor G. Irastorza
 ///
 ///	jul 2007:   added a 3-compounds gas mixture constructor
 ///				A. Tomas	
 ///
 ///	apr 2012:   minimal modifications for inclusion in new RESTSoft scheme
 ///		        A. Tomas
 ///
 ///	jun 2014:   change name to TRestGas and minimal modifications 
 ///              for inclusion in new RESTSoft scheme   
 ///		        Igor G. Irastorza
 ///
 /// aug 2015:   Major revision to introduce into REST v2.0. MediumMagboltz from Garfield++ is used now
 ///
 /// \class TRestGas
 /// \author Igor G. Irastorza
 /// \author Alfredo Tomas
 /// \author Javier Galan
 ///
 /// <hr>

#include "TRestGas.h"
using namespace std;

const char* ONLINE_GASFILE_PREFIX = "https://github.com/nkx111/gasFiles/raw/master/";

ClassImp(TRestGas)

/////////////////////////////////////////////
/// \brief TRestGas default constructor
///
TRestGas::TRestGas() : TRestMetadata()
{

	Initialize();

}

/////////////////////////////////////////////
/// \brief TRestGas constructor loading data from a config file.
///
/// This constructor will load the gas with properties defined inside the correspoding
/// TRestGas section in an RML file. A pre-generated gas file will be loaded if found 
/// in TRestMetadata::GetSearchPath() which can be defined as an input parameter in the 
/// globals metadata section.
/// 
/// \param cfgFileName It defines the path to an RML file containning a TRestGas section.
/// \param name The name of the TRestGas section to be read.
/// \param gasGeneration Parameter allowing to activate the gas generation.
///
TRestGas::TRestGas(const char *cfgFileName, string name, bool gasGeneration) : TRestMetadata(cfgFileName)
{
	Initialize();

	fGasGeneration = gasGeneration;

	LoadConfigFromFile(fConfigFileName, name);

	//if ( fStatus == RESTGAS_CFG_LOADED ) LoadGasFile( );
}

/////////////////////////////////////////////
/// \brief TRestGas default destructor
///
TRestGas::~TRestGas()
{
#if defined USE_Garfield
	delete fGasMedium;
#endif
}

/////////////////////////////////////////////
/// \brief Defines the metadata section name and initalizes the TRestGas members.
///
void TRestGas::Initialize()
{
	SetSectionName(this->ClassName());

	fPressureInAtm = 1;
	fTemperatureInK = 300;

	fNofGases = 0;

	fGasComponentName.clear();
	fGasComponentFraction.clear();

	fStatus = RESTGAS_INTITIALIZED;

	fGasFilename = "";
	fGasFileContent = "";

#if defined USE_Garfield
	fGasMedium = new Garfield::MediumMagboltz();
#else
	fGasMedium = NULL;
#endif

	fGasGeneration = false;
	fGasFileLoaded = false;
	InitComplete = false;

	fEmin = 10;
	fEmax = 1000;
	fEnodes = 20;
	fLast_E = numeric_limits<double>::quiet_NaN();
}

/////////////////////////////////////////////
/// \brief It loads a pre-generated gas file corresponding to the gas defined using the RML TRestGas section.
///
/// If a pre-generated gas file is not found this method will launch the gas generation in case 
/// TRestGas::fGasGeneration is true. This must be activated using EnableGasGeneration() or at the class
/// construction time.
///
void TRestGas::LoadGasFile()
{
#if defined USE_Garfield
	//Double_t pressure = fPressureInAtm;
	//fPressureInAtm = 1;
	//ConstructFilename( );

	if (!fileExists((string)(fGasFilename)))
	{
		error << __PRETTY_FUNCTION__ << endl;
		error << "The gas file does not exist. (name:"<<fGasFilename<<")" << endl;
		fGasFileLoaded = false;
		fLast_E = numeric_limits<double>::quiet_NaN();
		return;
	}

	// We might print this in debug mode
	debug << "Loading gas file : " << fGasFilename << endl;

	fGasMedium->LoadGasFile((string)(fGasFilename));

	fEFields.clear(); fBFields.clear(); fAngles.clear();
	fGasMedium->GetFieldGrid(fEFields, fBFields, fAngles);

	/*
	// We might print this in debug mode
	for( unsigned int i = 0; i < fEFields.size(); i++ )
		cout << "node " << i << " Field : " << fEFields[i] << " V/cm" << endl;
		*/

	if (fGasMedium && fGasMedium->GetW() == 0.) {
		fGasMedium->SetW(GetWvalue());
	} // as it is probably not computed by Magboltz
	//else
	//{
	//	fW = fGasMedium->GetW();
	//}
	fGasFileLoaded = true;
	//in future we need to import fPressure, fTemperature from fGasMedium
	//this->SetPressure(pressure);
	info << "TRestGas : Gas file loaded!" << endl;
#else
	cout << "This REST is not complied with garfield, it cannot load any gas file!" << endl;
#endif
}


void TRestGas::CalcGarField(double Emin, double Emax, int n) {
#if defined USE_Garfield
	if (fEnodes <= 0) { cout << "REST ERROR : The number of nodes is not a positive number!!. Gas file generation cancelled." << endl; fStatus = RESTGAS_ERROR; return; }
	if (fEmin >= fEmax) { cout << "REST ERROR : The Electric field grid boundaries are not properly defined." << endl; fStatus = RESTGAS_ERROR; return; }

	string gasStr[3];
	for (int i = 0; i < fNofGases; i++)
	{
		gasStr[i] = (string)fGasComponentName[i];
		if (i == 2) break;
	}

	if (fNofGases == 1)
		fGasMedium->SetComposition(gasStr[0], fGasComponentFraction[0] * 100.);

	if (fNofGases == 2)
		fGasMedium->SetComposition(gasStr[0], fGasComponentFraction[0] * 100., gasStr[1], fGasComponentFraction[1] * 100.);

	if (fNofGases == 3)
		fGasMedium->SetComposition(gasStr[0], fGasComponentFraction[0] * 100., gasStr[1], fGasComponentFraction[1] * 100., gasStr[2], fGasComponentFraction[2] * 100.);

	if (fNofGases > 3) { cout << "REST ERROR : Number of gas components higher than 3 not allowed" << endl; fStatus = RESTGAS_ERROR; return; }

	fGasMedium->SetTemperature(fTemperatureInK);
	fGasMedium->SetPressure(fPressureInAtm * 760.);

	fGasMedium->SetFieldGrid(Emin, Emax, n, n > 1);

	fGasMedium->SetMaxElectronEnergy(fMaxElectronEnergy);

	cout << "Garfield: calclulating..." << endl;

	if(fVerboseLevel>=REST_Info)
		fGasMedium->EnableDebugging();
	fGasMedium->Initialise();
	if (fVerboseLevel >= REST_Info)
		fGasMedium->DisableDebugging();

	fGasMedium->GenerateGasTable(fNCollisions, true);
#else
	cout << "This REST is not complied with garfield, it cannot calculate garfield!" << endl;
#endif
}
/////////////////////////////////////////////
/// \brief Adds a new element/compound to the gas.
///
/// This method is private to make gas intialization possible only through an RML 
/// file. This might change if necessary.
///
/// \param gasName A gas element/compound name valid in Garfield++.
/// \param fraction The element fraction in volume.
///
void TRestGas::AddGasComponent(string gasName, Double_t fraction)
{
	fGasComponentName.push_back(gasName);
	fGasComponentFraction.push_back(fraction);
	fNofGases++;
	ConditionChanged();
}

/////////////////////////////////////////////
/// \brief Loads the gas parameters that define the gas calculation 
/// and properties.
///
/// These parameters will be used to define the pre-generated gas filename.
/// The method TRestGas::ConstructFilename is used to define the filename 
/// format.
///
void TRestGas::InitFromConfigFile()
{

	fPressureInAtm = StringToDouble(GetParameter("pressure"));
	fTemperatureInK = StringToDouble(GetParameter("temperature"));
	fNCollisions = StringToInteger(GetParameter("nCollisions"));
	fMaxElectronEnergy = StringToDouble(GetParameter("maxElectronEnergy"));
	fW = StringToDouble(GetParameter("W_value","-1"));

	fGDMLMaterialRef = GetParameter("GDMLMaterialRef", "");

	if (fMaxElectronEnergy == -1) { fMaxElectronEnergy = 40; cout << "Setting default maxElectronEnergy to : " << fMaxElectronEnergy << endl; }
	if (fW == -1) { fW = 21.9; cout << "Setting default W-value : " << fW << endl; }

	string gasComponentString;
	size_t position = 0;
	while ((gasComponentString = GetKEYDefinition("gasComponent", position)) != "")
	{
		string gasName = GetFieldValue("name", gasComponentString);
		Double_t gasFraction = StringToDouble(GetFieldValue("fraction", gasComponentString));

		AddGasComponent(gasName, gasFraction);
	}

	string eFieldString = GetKEYDefinition("eField");

	fEmax = StringToDouble(GetFieldValue("Emax", eFieldString));
	fEmin = StringToDouble(GetFieldValue("Emin", eFieldString));
	fEnodes = StringToInteger(GetFieldValue("nodes", eFieldString));
	if (ToUpper(GetParameter("generate")) == "ON" || ToUpper(GetParameter("generate")) == "TRUE")
		fGasGeneration = true;

	double sum = 0;
	for (int i = 0; i < fNofGases; i++)
		sum += GetGasComponentFraction(i);

	if (sum == 1) fStatus = RESTGAS_CFG_LOADED;
	else { warning << "REST WARNING : TRestGas : The total gas fractions is NOT 1." << endl;  fStatus = RESTGAS_ERROR; return; }

	InitComplete = true;
	if (fGasGeneration)
	{
		CalcGarField(fEmax, fEmin, fEnodes);
		GenerateGasFile();
		fGasFileLoaded = true;
	}
	else if (GetParameter("gasFile") != PARAMETER_NOT_FOUND_STR)
	{
		SetGasFile(GetParameter("gasFile"));
		if (fGasFilename!="") {
			LoadGasFile();
		}
		else
		{
			warning << "TRestGas: gas file does not exist!" << endl;
			fGasFileLoaded = false;
		}

	}
	else
	{
		ConditionChanged();
	}

	PrintGasInfo();

#if defined USE_Garfield
	if (fGasMedium && fGasMedium->GetW() == 0.) fGasMedium->SetW(fW);  // as it is probably not computed by Magboltz
#endif

}

void TRestGas::InitFromRootFile() {

	if(fGasFileContent!="")//use gas file content by default
	{
		fGasFilename = "/tmp/restGasFile.gas";
		ofstream outf;
		outf.open(fGasFilename,ios::ate);
		outf << fGasFileContent << endl;
		outf.close();
		LoadGasFile();
		system("rm " + fGasFilename);
	}
	else
	{
		SetGasFile((string)fGasFilename);
		if (fGasFilename != "") {
			LoadGasFile();
		}
	}
}

void TRestGas::SetGasFile(string name) {
	string absolutename = SearchFile(name);
	if (absolutename == "")//we try to download the gas file from git hub
	{
		warning << "TRestGas : gas file not found locally, trying to download with wget..." << endl;
		string _name = Replace(name, "(", "\\(", 0);
		_name = Replace(_name, ")", "\\)", 0);
		string cmd = "wget " + (string)ONLINE_GASFILE_PREFIX + _name + " -O /tmp/restGasDownload.gas -q";
		int a = system(cmd.c_str());
		debug << a << endl;
		if (a == 0)
		{
			warning << "download successful!" << endl;
			fGasFilename = "/tmp/restGasDownload.gas";
		}
		else {
			warning << "download failed!" << endl;
			warning << "FileName: " << name << endl;
			fGasFilename = "";
		}
	}
	else
	{
		fGasFilename = absolutename;
	}
}

void TRestGas::ConditionChanged() {
	if (InitComplete) {
		string name = ConstructFilename();
		SetGasFile(name);
		if (fGasFilename != "")
		{
			LoadGasFile();
		}
		else
		{
			warning << "TRestGas : gas file not found for the specified gas mixture!" << endl;
			warning << "REST will perform single-E calculation in the next Get()" << endl;
			warning << "To generate a gas file, turn on the parameter \"generate\"" << endl;
			fGasFileLoaded = false;
			fLast_E = numeric_limits<double>::quiet_NaN();
		}
	}
}

/////////////////////////////////////////////
/// \brief Returns a string definning the gas components and fractions.
/// 
TString TRestGas::GetGasMixture()
{
	TString gasMixture;
	char tmpStr[64];
	for (int n = 0; n < fNofGases; n++)
	{
		if (n > 0) gasMixture += "-";
		gasMixture += GetGasComponentName(n) + "_";
		sprintf(tmpStr, "%03.1lf", GetGasComponentFraction(n) * 100.);
		gasMixture += (TString)tmpStr;
	}
	return gasMixture;
}

/////////////////////////////////////////////
/// \brief Constructs the filename of the pre-generated gas file using the members defined in the RML file.
/// 
/// The filename is a full name, containing path and name. The path is by default current directory.
string TRestGas::ConstructFilename()
{
	string name = "";
	char tmpStr[256];
	for (int n = 0; n < fNofGases; n++)
	{
		if (n > 0) name += "-";
		name += GetGasComponentName(n) + "_";
		sprintf(tmpStr, "%03.1lf", GetGasComponentFraction(n) * 100.);
		name += (TString)tmpStr;
	}

	name += "-E_vs_P_";
	sprintf(tmpStr, "%03.1lf", fEmin / fPressureInAtm);
	name += (TString)tmpStr;

	name += "_";
	sprintf(tmpStr, "%03.1lf", fEmax / fPressureInAtm);
	name += (TString)tmpStr;

	name += "_nodes_";
	sprintf(tmpStr, "%02d", fEnodes);
	name += (TString)tmpStr;

	name += "-nCol_";
	sprintf(tmpStr, "%02d", fNCollisions);
	name += (TString)tmpStr;

	name += "-maxElectronEnergy_";
	sprintf(tmpStr, "%04lf", fMaxElectronEnergy);
	name += (TString)tmpStr;

	name += ".gas";

	return name;

}

/////////////////////////////////////////////
/// \brief Save a gas file with a structured file name
void TRestGas::GenerateGasFile()
{
#if defined USE_Garfield

	ConstructFilename();

	string path = SeparatePathAndName((string)fGasFilename).first;
	string name = SeparatePathAndName((string)fGasFilename).second;

	if (!isPathWritable(path))
	{
		cout << endl;
		cout << "REST ERROR. TRestGas. Path is not writtable." << endl;
		cout << "Path : " << path << endl;
		cout << "Make sure the final data path is writtable before proceed to gas generation." << endl;
		cout << "or change the gas data path ... " << endl;
		cout << endl;
		GetChar();
		return;
	}

	cout << "Writting gas file" << endl;
	cout << "-----------------" << endl;
	cout << "Path : " << path << endl;
	cout << "Filename : " << name << endl;
	fGasMedium->WriteGasFile((string)fGasFilename);
#else
	cout << "This REST is not complied with garfield, it cannot save any gas file!" << endl;
#endif
}

/////////////////////////////////////////////
/// \brief Defines the pressure of the gas.
///
/// The gas pressure is used during gas file generation to define the E over P range.
/// Once the gas file has been loaded, the pressure might be changed at any time, the
/// gas properties requested will be valid for the given pressure.
/// 
/// \param pressure The new pressure of the gas in atm.
/// 
void TRestGas::SetPressure(Double_t pressure)
{
	fPressureInAtm = pressure;
#if defined USE_Garfield
	fGasMedium->SetPressure(fPressureInAtm * 760.);
	ConditionChanged();
#endif
}

/////////////////////////////////////////////
/// \brief It creates a TCanvas where it plots the drift velocity as a function of the electric field.
///
/// \param eMin Minimum value of the electric field to be plotted in V/cm.
/// \param eMax Maximum value of the electric field to be plotted in V/cm.
/// \param steps Number of points to be given to be drawn
///
void TRestGas::PlotDriftVelocity(Double_t eMin, Double_t eMax, Int_t nSteps)
{
	vector<Double_t> eField(nSteps), driftVel(nSteps);

	for (int i = 0; i < nSteps; i++)
	{
		eField[i] = (eMin + (double)i * (eMax - eMin) / nSteps);

		driftVel[i] = GetDriftVelocity(eField[i]);
	}


	TCanvas* c = new TCanvas("Drift velocity", "  ");
	TGraph*		fDriftVel = new TGraph(nSteps, &eField[0], &driftVel[0]);
	TString str;
	str.Form("Drift Velocity for %s", GetName());
	fDriftVel->SetTitle(str);
	fDriftVel->GetXaxis()->SetTitle("E [V/cm]");
	fDriftVel->GetYaxis()->SetTitle("Drift velocity [cm/#mus]");
	fDriftVel->GetYaxis()->SetTitleOffset(2);
	fDriftVel->Draw("");
	c->Update();
}

/////////////////////////////////////////////
/// \brief It creates a TCanvas where it plots the longitudinal diffusion as a function of the electric field.
///
/// \param eMin Minimum value of the electric field to be plotted in V/cm.
/// \param eMax Maximum value of the electric field to be plotted in V/cm.
/// \param steps Number of points to be given to be drawn
///
void TRestGas::PlotLongitudinalDiffusion(Double_t eMin, Double_t eMax, Int_t nSteps)
{
	vector<Double_t> eField(nSteps), longDiff(nSteps);

	for (int i = 0; i < nSteps; i++)
	{
		eField[i] = eMin + (double)i * (eMax - eMin) / nSteps;

		longDiff[i] = GetLongitudinalDiffusion(eField[i]);
	}


	TCanvas* c = new TCanvas("Longitudinal diffusion", "  ");
	TGraph*		fLongDiff = new TGraph(nSteps, &eField[0], &longDiff[0]);
	TString str;
	str.Form("Longitudinal diffusion for %s", GetName());
	fLongDiff->SetTitle(str);
	fLongDiff->GetXaxis()->SetTitle("E [V/cm]");
	fLongDiff->GetYaxis()->SetTitle("Longitudinal diffusion [#mum/#sqrt{cm}]");
	fLongDiff->GetYaxis()->SetTitleOffset(2);
	fLongDiff->Draw("");
	c->Update();
}

/////////////////////////////////////////////
/// \brief It creates a TCanvas where it plots the transversal diffusion as a function of the electric field.
///
/// \param eMin Minimum value of the electric field to be plotted in V/cm.
/// \param eMax Maximum value of the electric field to be plotted in V/cm.
/// \param steps Number of points to be given to be drawn
///
void TRestGas::PlotTransversalDiffusion(Double_t eMin, Double_t eMax, Int_t nSteps)
{
	vector<Double_t> eField(nSteps), transDiff(nSteps);

	for (int i = 0; i < nSteps; i++)
	{
		eField[i] = eMin + (double)i * (eMax - eMin) / nSteps;

		transDiff[i] = GetTransversalDiffusion(eField[i]);
	}


	TCanvas* c = new TCanvas("Transitudinal diffusion", "  ");
	TGraph*		fTransDiff = new TGraph(nSteps, &eField[0], &transDiff[0]);
	TString str;
	str.Form("Transversal diffusion for %s", GetName());
	fTransDiff->SetTitle(str);
	fTransDiff->GetXaxis()->SetTitle("E [V/cm]");
	fTransDiff->GetYaxis()->SetTitle("Transversal diffusion [#mum/#sqrt{cm}]");
	fTransDiff->GetYaxis()->SetTitleOffset(2);
	fTransDiff->Draw("");
	c->Update();
}

/////////////////////////////////////////////
/// \brief It creates a TCanvas where it plots the townsend coefficient as a function of the electric field.
///
/// \param eMin Minimum value of the electric field to be plotted in V/cm.
/// \param eMax Maximum value of the electric field to be plotted in V/cm.
/// \param steps Number of points to be given to be drawn
///
void TRestGas::PlotTownsendCoefficient(Double_t eMin, Double_t eMax, Int_t nSteps)
{
	vector<Double_t> eField(nSteps), townsendCoeff(nSteps);

	for (int i = 0; i < nSteps; i++)
	{
		eField[i] = eMin + (double)i * (eMax - eMin) / nSteps;

		townsendCoeff[i] = GetTownsendCoefficient(eField[i]);
	}


	TCanvas* c = new TCanvas("Townsend coefficient", "  ");
	TGraph*		fTownsend = new TGraph(nSteps, &eField[0], &townsendCoeff[0]);
	TString str;
	str.Form("Townsend coefficient for %s", GetName());
	fTownsend->SetTitle(str);
	fTownsend->GetXaxis()->SetTitle("E [V/cm]");
	fTownsend->GetYaxis()->SetTitle("Townsend coefficient [1/cm]");
	fTownsend->GetYaxis()->SetTitleOffset(2);
	fTownsend->Draw("");
	c->Update();
}

/////////////////////////////////////////////
/// \brief It returns the drift velocity in cm/us for a given electric field in V/cm.
///
Double_t TRestGas::GetDriftVelocity(Double_t E)
{
#if defined USE_Garfield
	if (fGasFileLoaded == false && fLast_E != E)
	{
		CalcGarField(E, E, 1);
		fLast_E = E;
	}
	Double_t vx, vy, vz;
	fGasMedium->ElectronVelocity(0., 0, -E, 0, 0, 0, vx, vy, vz);
	return vz * 1000.;
#else
	cout << "This REST is not complied with garfield, Do not use Drift Velocity from TRestGas!" << endl;
	cout << "Please define the Drift Velocity in each process!" << endl;
	return 0.001;
#endif
}

/////////////////////////////////////////////
/// \brief It returns the longitudinal diffusion in (cm)^1/2 for a given electric field in V/cm.
///
Double_t TRestGas::GetLongitudinalDiffusion(Double_t E)
{
#if defined USE_Garfield
	if (fGasFileLoaded == false && fLast_E != E)
	{
		CalcGarField(E, E, 1);
		fLast_E = E;
	}
	Double_t dl, dt;
	fGasMedium->ElectronDiffusion(0., 0, -E, 0, 0, 0, dl, dt);
	return dl;
#else
	cout << "This REST is not complied with garfield, Do not use Longitudinal Diffusion from TRestGas!" << endl;
	cout << "Please define the Longitudinal Diffusion in each process!" << endl;
	return 0;
#endif
}

/////////////////////////////////////////////
/// \brief It returns the transversal diffusion in (cm)^1/2 for a given electric field in V/cm.
///
Double_t TRestGas::GetTransversalDiffusion(Double_t E)
{
#if defined USE_Garfield
	if (fGasFileLoaded == false && fLast_E != E)
	{
		CalcGarField(E, E, 1);
		fLast_E = E;
	}
	Double_t dl, dt;
	fGasMedium->ElectronDiffusion(0., 0, -E, 0, 0, 0, dl, dt);
	return dt;
#else
	cout << "This REST is not complied with garfield, Do not use Transversal Diffusion from TRestGas!" << endl;
	cout << "Please define the Transversal Diffusion in each process!" << endl;
	return 0;
#endif
}

/////////////////////////////////////////////
/// \brief It returns the townsend coefficient for a given electric field in V/cm.
///
Double_t TRestGas::GetTownsendCoefficient(Double_t E)
{
#if defined USE_Garfield
	if (fGasFileLoaded == false && fLast_E != E)
	{
		CalcGarField(E, E, 1);
		fLast_E = E;
	}
	Double_t alpha;
	fGasMedium->ElectronTownsend(0., 0, -E, 0, 0, 0, alpha);
	return alpha;
#else
	cout << "This REST is not complied with garfield, Do not use Townsend Coefficient from TRestGas!" << endl;
	cout << "Please define the Townsend Coefficient in each process!" << endl;
	return 0;
#endif
}

/////////////////////////////////////////////
/// \brief It returns the attachment coefficient for a given electric field in V/cm.
///
Double_t TRestGas::GetAttachmentCoefficient(Double_t E)
{
#if defined USE_Garfield
	if (fGasFileLoaded == false && fLast_E != E)
	{
		CalcGarField(E, E, 1);
		fLast_E = E;
	}
	Double_t eta;
	fGasMedium->ElectronAttachment(0., 0, -E, 0, 0, 0, eta);
	return eta;
#else
	cout << "This REST is not complied with garfield, Do not use Attachment Coefficient from TRestGas!" << endl;
	cout << "Please define the Attachment Coefficient in each process!" << endl;
	return 0;
#endif
}

/////////////////////////////////////////////
/// \brief Prints the metadata information from the gas
///
void TRestGas::PrintGasInfo()
{
	TRestMetadata::PrintMetadata();

	TRestStringOutput cout;
	cout.setborder("||");
	cout.setorientation(1);
	cout.setlength(100);
	cout << "Status : " << fStatus << endl;
	cout << "Gas filename : " << fGasFilename << endl;
	cout << "Pressure : " << fPressureInAtm << " atm" << endl;
	cout << "Temperature : " << fTemperatureInK << " K" << endl;
	cout << "W-value : " << fW << " eV" << endl;
	cout << "Max. Electron energy : " << fMaxElectronEnergy << " eV" << endl;
	cout << "Field grid nodes : " << fEnodes << endl;
	cout << "Efield range : ( " << fEmin << " , " << fEmax << " ) V/cm " << endl;
	cout << "Number of Gases : " << fNofGases << endl;
	for (int i = 0; i < fNofGases; i++)
		cout << "Gas id : " << i << ", Name : " << fGasComponentName[i] << ", Fraction : " << fGasComponentFraction[i] << endl;
	cout << "******************************************" << endl;
	cout << endl;
	cout << endl;
}

Int_t TRestGas::Write(const char *name, Int_t option, Int_t bufsize) {
	if (fGasFileContent == "" && fGasFileLoaded) {
		ifstream infile;
		infile.open(fGasFilename);
		if (!infile)
		{
			cout << "TRestGas: error reading gas file, gas file content won't be saved!" << endl;
		}
		else
		{
			string str;
			while (getline(infile, str)) {
				fGasFileContent += str + "\n";
			}
			//cout << fGasFileContent << endl;
		}
	}
	return TRestMetadata::Write();
}