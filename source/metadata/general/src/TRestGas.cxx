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
///  <section TRestGas name="Xenon 10-10E3V/cm" title="Xenon">
///     <parameter name="pressure" value="1" />
///     <parameter name="temperature" value="293.15" />
///     <parameter name="maxElectronEnergy" value="400" />
///     <parameter name="W_value" value="21.9" />
///     <parameter name="nCollisions" value="10" />
///     <eField Emin="10" Emax="1000." nodes="20" />
///     <gasComponent name="xe" fraction="1" />
///  </section>
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
/// In order to avoid this calculation each time a new instance of TRestGas is created, a
/// Magboltz gas file, containning the summary of the gas properties, will be generated and 
/// automatically re-used by TRestGas later on.
/// 
/// These files will be searched by TRestGas at TRestMetadata::GetGasDataPath that can be
/// defined inside our main RML file through the parameter *gasDataPath* at the
/// *globals* section (see TRestMetadata description for additional details).
///
/// \code
///
/// <section globals>
///
///     <parameter name="gasDataPath" value="userPathToGasFiles" />
///
///      ...
///
/// </section>
///
/// \endcode
///
/// If no gas data path is specified the following route $REST_PATH/inputData/gases will
/// be used as default. Therefore, if we want to produce additional gas files we must 
/// assure that we have write access rights at that location.
///
/// Few pre-generated gas files, with extension .gas, can be found inside the REST repository
/// at the directory $REST_PATH/inputData/gases/. The TRestGas sections that were used to
/// generate those gas files can be found at $REST_PATH/inputData/definitions/gases.rml.
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
/// be for example *Xenon 10-10E3V/cm*, found at *gases.rml*.
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

ClassImp(TRestGas)

/////////////////////////////////////////////
/// \brief TRestGas default constructor
///
TRestGas::TRestGas() : TRestMetadata( )
{
	
    Initialize( );

}

/////////////////////////////////////////////
/// \brief TRestGas constructor loading data from a config file.
///
/// This constructor will load the gas with properties defined inside the correspoding
/// TRestGas section in an RML file. A pre-generated gas file will be loaded if found 
/// in TRestMetadata::gasDataPath which can be defined as an input parameter in the 
/// globals metadata section.
/// 
/// \param cfgFileName It defines the path to an RML file containning a TRestGas section.
/// \param name The name of the TRestGas section to be read.
/// \param gasGeneration Parameter allowing to activate the gas generation.
///
TRestGas::TRestGas( const char *cfgFileName, string name, bool gasGeneration) : TRestMetadata (cfgFileName)
{
    Initialize( );

    LoadConfigFromFile( fConfigFileName, name );

    fGasGeneration = gasGeneration;

    if ( fStatus == RESTGAS_CFG_LOADED ) LoadGasFile( );
}

/////////////////////////////////////////////
/// \brief TRestGas default destructor
///
TRestGas::~TRestGas()
{
    delete fGasMedium;
}

/////////////////////////////////////////////
/// \brief Defines the metadata section name and initalizes the TRestGas members.
///
void TRestGas::Initialize()
{
    SetSectionName( this->ClassName() );

	fPressureInAtm = 1;
	fTemperatureInK = 300;

	fNofGases = 0;

    fGasComponentName.clear();
    fGasComponentFraction.clear();

    fStatus = RESTGAS_INTITIALIZED;

    fGasFilename = "";

    fGasMedium = new Garfield::MediumMagboltz();

    fGasGeneration = false;
}

/////////////////////////////////////////////
/// \brief It loads a pre-generated gas file corresponding to the gas defined using the RML TRestGas section.
///
/// If a pre-generated gas file is not found this method will launch the gas generation in case 
/// TRestGas::fGasGeneration is true. This must be activated using EnableGasGeneration() or at the class
/// construction time.
///
void TRestGas::LoadGasFile( )
{
    Double_t pressure = fPressureInAtm;
    fPressureInAtm = 1;
    ConstructFilename( );

    if( !fileExists( (string) (GetGasDataPath() + fGasFilename) ) ) 
    {
        cout << __PRETTY_FUNCTION__ << endl;
        cout << "The gas file does not exist." << endl; 
        if( GasFileGenerationEnabled() ) 
        {
            cout << "Launching gas file generation process." << endl;
            GenerateGasFile( );
        }
        else
        {
            cout << "Please, use EnableGasGeneration() if you want to produce a gas file." << endl;
            cout << "This probably an error. Press a key to continue..." << endl;
            getchar();
            fStatus = RESTGAS_ERROR; return; 
        }
    }

    // We might print this in debug mode
    //cout << "Loading gas file : " << fGasFilename << endl;

    fGasMedium->LoadGasFile( (string) (GetGasDataPath() + fGasFilename) ); 

    fEFields.clear(); fBFields.clear(); fAngles.clear();
    fGasMedium->GetFieldGrid( fEFields, fBFields, fAngles);

    /*
    // We might print this in debug mode
    for( unsigned int i = 0; i < fEFields.size(); i++ )
        cout << "node " << i << " Field : " << fEFields[i] << " V/cm" << endl;
        */

    this->SetPressure( pressure );

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
void TRestGas::AddGasComponent( string gasName, Double_t fraction )
{
    fGasComponentName.push_back( gasName );
    fGasComponentFraction.push_back( fraction );
    fNofGases++;
}

/////////////////////////////////////////////
/// \brief Loads the gas parameters that define the gas calculation 
/// and properties.
///
/// These parameters will be used to define the pre-generated gas filename.
/// The method TRestGas::ConstructFilename is used to define the filename 
/// format.
///
void TRestGas::InitFromConfigFile( )
{
    fPressureInAtm = StringToDouble ( GetParameter( "pressure" ) );
    fTemperatureInK = StringToDouble ( GetParameter( "temperature" ) );
    fNCollisions = StringToInteger( GetParameter( "nCollisions" ) );
    fMaxElectronEnergy = StringToDouble( GetParameter( "maxElectronEnergy" ) );
    fW = StringToDouble( GetParameter( "W_value" ) );

    if( fMaxElectronEnergy == -1 ) { fMaxElectronEnergy = 40; cout << "Setting default maxElectronEnergy to : " << fMaxElectronEnergy << endl; }
    if( fW == -1 ) { fW = 21.9; cout << "Setting default W-value : " << fW << endl; }

    string gasComponentString;
    size_t position = 0;
    while( ( gasComponentString = GetKEYDefinition( "gasComponent", position ) ) != "" )
    {
        string gasName = GetFieldValue( "name", gasComponentString );
        Double_t gasFraction = StringToDouble ( GetFieldValue( "fraction", gasComponentString ) );

        AddGasComponent( gasName, gasFraction );
    }
    
    string eFieldString = GetKEYDefinition( "eField" );

    fEmax = StringToDouble( GetFieldValue( "Emax", eFieldString ) );
    fEmin = StringToDouble( GetFieldValue( "Emin", eFieldString ) );
    fEnodes = StringToInteger( GetFieldValue( "nodes", eFieldString ) );


    double sum = 0;
    for( int i = 0; i < fNofGases; i++ )
        sum += GetGasComponentFraction( i );

    if( sum == 1 ) fStatus = RESTGAS_CFG_LOADED;
    else { cout << "REST WARNING : The total gas fractions is NOT 1." << endl;  fStatus = RESTGAS_ERROR; return; }

    ConstructFilename( );

    PrintGasInfo();

}

/////////////////////////////////////////////
/// \brief Returns a string definning the gas components and fractions.
/// 
TString TRestGas::GetGasMixture() 
{
    TString gasMixture;
    char tmpStr[64];
    for( int n = 0; n < fNofGases; n++ )
    {
        if( n > 0 ) gasMixture += "-";
        gasMixture += GetGasComponentName( n ) + "_";
        sprintf( tmpStr, "%03.1lf", GetGasComponentFraction( n ) * 100. );
        gasMixture += (TString) tmpStr;
    }
    return gasMixture;
}

/////////////////////////////////////////////
/// \brief Constructs the filename of the pre-generated gas file using the members defined in the RML file.
/// 
void TRestGas::ConstructFilename( )
{
    fGasFilename = "";
    char tmpStr[256];
    for( int n = 0; n < fNofGases; n++ )
    {
        if( n > 0 ) fGasFilename += "-";
        fGasFilename += GetGasComponentName( n ) + "_";
        sprintf( tmpStr, "%03.1lf", GetGasComponentFraction( n ) * 100. );
        fGasFilename += (TString) tmpStr;
    }

    fGasFilename += "-E_vs_P_";
    sprintf( tmpStr, "%03.1lf", fEmin/fPressureInAtm );
    fGasFilename += (TString) tmpStr;

    fGasFilename += "_";
    sprintf( tmpStr, "%03.1lf", fEmax/fPressureInAtm );
    fGasFilename += (TString) tmpStr;

    fGasFilename += "_nodes_";
    sprintf( tmpStr, "%02d", fEnodes );
    fGasFilename += (TString) tmpStr;

    fGasFilename += "-nCol_";
    sprintf( tmpStr, "%02d", fNCollisions );
    fGasFilename += (TString) tmpStr;

    fGasFilename += "-maxElectronEnergy_";
    sprintf( tmpStr, "%04lf", fMaxElectronEnergy );
    fGasFilename += (TString) tmpStr;

    fGasFilename += ".gas";
}

/////////////////////////////////////////////
/// \brief Launches the gas file generation.
///
/// Gas parameters for generation should have been defined: temperature, pressure, gas composition,
/// electric field range, etc. The gas file will be written to TRestMetadata::GetGasDataPath() that
/// can be defined in the globals section (see details in TRestMetadata).
/// 
void TRestGas::GenerateGasFile( )
{
    if ( !isPathWritable( (string) GetGasDataPath() ) )
    {
        cout << endl;
        cout << "REST ERROR. TRestGas. GasDataPath is not writtable." << endl;
        cout << "Path : " << GetGasDataPath() << endl;
        cout << "Make sure the final data path is writtable before proceed to gas generation." << endl;
        cout << "or change the gas data path ... " << endl;
        cout << endl;
        GetChar();
        return;
    }

    if( fEnodes <= 0 ) { cout << "REST ERROR : The number of nodes is not a positive number!!. Gas file generation cancelled." << endl; fStatus = RESTGAS_ERROR; return; }
    if( fEmin >= fEmax ) { cout << "REST ERROR : The Electric field grid boundaries are not properly defined." << endl; fStatus = RESTGAS_ERROR; return; }

    string gasStr[3];
    for( int i = 0; i < fNofGases; i++ )
    {
        gasStr[i] = (string) fGasComponentName[i];
        if( i == 2 ) break;
    }

    if( fNofGases == 1 )
        fGasMedium->SetComposition( gasStr[0], fGasComponentFraction[0]*100. );

    if( fNofGases == 2 )
        fGasMedium->SetComposition( gasStr[0], fGasComponentFraction[0]*100., gasStr[1], fGasComponentFraction[1]*100.  );

    if( fNofGases == 3 )
        fGasMedium->SetComposition( gasStr[0], fGasComponentFraction[0]*100., gasStr[1], fGasComponentFraction[1]*100., gasStr[2], fGasComponentFraction[2]*100.  );

    if( fNofGases > 3 ) { cout << "REST ERROR : Number of gas components higher than 3 not allowed" << endl; fStatus = RESTGAS_ERROR; return; }

    fGasMedium->SetTemperature( fTemperatureInK );
    fGasMedium->SetPressure( fPressureInAtm * 760. );

    fGasMedium->SetFieldGrid( fEmin, fEmax, fEnodes, true );

    fGasMedium->SetMaxElectronEnergy( fMaxElectronEnergy );

    fGasMedium->EnableDebugging();
    fGasMedium->Initialise();
    fGasMedium->DisableDebugging();

    fGasMedium->GenerateGasTable( fNCollisions, true );

    cout << "Writting gas file" << endl;
    cout << "-----------------" << endl;
    cout << "Path : " << GetGasDataPath() << endl;
    cout << "Filename : " << fGasFilename << endl;
    fGasMedium->WriteGasFile ( (string) (GetGasDataPath() + fGasFilename) );
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
void TRestGas::SetPressure( Double_t pressure )
{
    fPressureInAtm = pressure;
    fGasMedium->SetPressure( fPressureInAtm * 760. );
}

/////////////////////////////////////////////
/// \brief It creates a TCanvas where it plots the drift velocity as a function of the electric field.
///
/// \param eMin Minimum value of the electric field to be plotted in V/cm.
/// \param eMax Maximum value of the electric field to be plotted in V/cm.
/// \param steps Number of points to be given to be drawn
///
void TRestGas::PlotDriftVelocity( Double_t eMin, Double_t eMax, Int_t nSteps )
{
    Double_t eField[nSteps], driftVel[nSteps];

    for( int i = 0; i < nSteps; i++ )
    {	
        eField[i] = ( eMin + (double) i * (eMax-eMin)/nSteps );

        driftVel[i] = GetDriftVelocity( eField[i] );
    }


    TCanvas* c = new TCanvas("Drift velocity", "  ");
    TGraph*		fDriftVel = new TGraph( nSteps, eField, driftVel );
	TString str;
    str.Form("Drift Velocity for %s",GetName());
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
void TRestGas::PlotLongitudinalDiffusion( Double_t eMin, Double_t eMax, Int_t nSteps )
{
    Double_t eField[nSteps], longDiff[nSteps];

    for( int i = 0; i < nSteps; i++ )
    {	
        eField[i] = eMin + (double) i * (eMax-eMin)/nSteps;

        longDiff[i] = GetLongitudinalDiffusion( eField[i] );
    }


    TCanvas* c = new TCanvas("Longitudinal diffusion", "  ");
    TGraph*		fLongDiff = new TGraph( nSteps, eField, longDiff );
    TString str;
    str.Form("Longitudinal diffusion for %s",GetName());
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
void TRestGas::PlotTransversalDiffusion( Double_t eMin, Double_t eMax, Int_t nSteps )
{
    Double_t eField[nSteps], transDiff[nSteps];

    for( int i = 0; i < nSteps; i++ )
    {	
        eField[i] = eMin + (double) i * (eMax-eMin)/nSteps;

        transDiff[i] = GetTransversalDiffusion( eField[i] );
    }


    TCanvas* c = new TCanvas("Transitudinal diffusion", "  ");
    TGraph*		fTransDiff = new TGraph( nSteps, eField, transDiff );
    TString str;
    str.Form("Transversal diffusion for %s",GetName());
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
void TRestGas::PlotTownsendCoefficient( Double_t eMin, Double_t eMax, Int_t nSteps )
{
    Double_t eField[nSteps], townsendCoeff[nSteps];

    for( int i = 0; i < nSteps; i++ )
    {	
        eField[i] = eMin + (double) i * (eMax-eMin)/nSteps;

        townsendCoeff[i] = GetTownsendCoefficient( eField[i] );
    }


    TCanvas* c = new TCanvas("Townsend coefficient", "  ");
    TGraph*		fTownsend = new TGraph( nSteps, eField, townsendCoeff );
    TString str;
    str.Form("Townsend coefficient for %s",GetName());
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
Double_t TRestGas::GetDriftVelocity( Double_t E )
{
    Double_t vx, vy, vz;
	fGasMedium->ElectronVelocity( 0., 0, -E, 0, 0, 0, vx, vy, vz);
    return vz * 1000.;
}

/////////////////////////////////////////////
/// \brief It returns the longitudinal diffusion in (cm)^1/2 for a given electric field in V/cm.
///
Double_t TRestGas::GetLongitudinalDiffusion( Double_t E )
{
    Double_t dl, dt;
	fGasMedium->ElectronDiffusion( 0., 0, -E, 0, 0, 0, dl, dt);
    return dl;
}

/////////////////////////////////////////////
/// \brief It returns the transversal diffusion in (cm)^1/2 for a given electric field in V/cm.
///
Double_t TRestGas::GetTransversalDiffusion( Double_t E )
{
    Double_t dl, dt;
	fGasMedium->ElectronDiffusion( 0., 0, -E, 0, 0, 0, dl, dt);
    return dt;
}

/////////////////////////////////////////////
/// \brief It returns the townsend coefficient for a given electric field in V/cm.
///
Double_t TRestGas::GetTownsendCoefficient( Double_t E )
{
    Double_t alpha;
	fGasMedium->ElectronTownsend( 0., 0, -E, 0, 0, 0, alpha );
    return alpha;
}

/////////////////////////////////////////////
/// \brief It returns the attachment coefficient for a given electric field in V/cm.
///
Double_t TRestGas::GetAttachmentCoefficient( Double_t E )
{
    Double_t eta;
	fGasMedium->ElectronTownsend( 0., 0, -E, 0, 0, 0, eta );
    return eta;
}

/////////////////////////////////////////////
/// \brief Prints the metadata information from the gas
///
void TRestGas::PrintGasInfo()
{
    cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "TRestGas content" << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "Config file : " << fConfigFileName << endl;
    cout << "Section name : " << fSectionName << endl;        // section name given in the constructor of TRestSpecificMetadata
    cout << "---------------------------------------" << endl;
    cout << "Name : " << GetName() << endl;
    cout << "Title : " << GetTitle() << endl;
    cout << "Status : " << fStatus << endl;
    cout << "---------------------------------------" << endl;
    cout << "Gas filename : " << fGasFilename << endl;
    cout << "Pressure : " << fPressureInAtm << " atm" << endl;
    cout << "Temperature : " << fTemperatureInK << " K" << endl;
    cout << "W-value : " << fW<< " eV" << endl;
    cout << "Max. Electron energy : " << fMaxElectronEnergy << " eV" << endl;
    cout << "Field grid nodes : " << fEnodes << endl;
    cout << "Efield range : ( " << fEmin << " , " << fEmax << " ) V/cm " << endl;
    cout << "Number of Gases : " << fNofGases << endl;
    for ( int i = 0; i < fNofGases; i++) 
        cout << "Gas id : " << i << " Name : " << fGasComponentName[i] << " Fraction : " << fGasComponentFraction[i] << endl;
    cout << "******************************************" << endl;
}

