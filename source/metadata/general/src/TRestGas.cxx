//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
//             
//
//             RESTSoft : Software for Rare Event Searches with TPCs
//
//             TRestGas.cxx
//
//		Class holding information of a given gas mixture regarding simulations 
//		of the response of a TPC, like townsend coefficients, diffusion and 
//		drift velocities. It interfaces with Magboltz output files (which are 
//		supposed to have been created with Magboltz previously) to read the 
//		values of all these parameters (based on old TMagboltzGas class)
//
//		history of corrections:
//
//		jan 2006:	first concept (as MagboltzGas)
//					Igor G. Irastorza
//		jul 2006:	minor improvements for inclusion in RESTSoft
//					(renamed as TMagboltzGas)
//					Igor G. Irastorza
//		jul 2007:   added a 3-compounds gas mixture constructor
//					A. Tomás	
//
//		apr 2012:   minimal modifications for inclusion in new RESTSoft scheme
//			        A. Tomas
//
//		jun 2014:   change name to TRestGas and minimal modifications 
//                  for inclusion in new RESTSoft scheme   
//			        Igor G. Irastorza
//
//      aug 2015:   Major revision to introduce into REST v2.0. MediumMagboltz from Garfield++ is used now
//
///_______________________________________________________________________________



#include "TRestGas.h"
using namespace std;

ClassImp(TRestGas)

//______________________________________________________________________________
TRestGas::TRestGas() : TRestMetadata( )
{
	
    Initialize( );

}

//______________________________________________________________________________
TRestGas::TRestGas( const char *cfgFileName, bool gasGeneration) : TRestMetadata (cfgFileName)
{
    Initialize( );

    LoadConfigFromFile( fConfigFileName );

    fGasGeneration = gasGeneration;

    if ( fStatus == RESTGAS_CFG_LOADED ) LoadGasFile( );

}

//______________________________________________________________________________
TRestGas::~TRestGas()
{

    delete fGasMedium;
}

void TRestGas::Initialize()
{
    SetName( "gas" );

	fPressureInAtm =1;
	fTemperatureInK = 300;

	fNofGases = 0;

    fGasComponentName.clear();
    fGasComponentFraction.clear();

    fStatus = RESTGAS_INTITIALIZED;

    fGasFilename = "";

    fGasMedium = new Garfield::MediumMagboltz();

    fGasGeneration = false;
}

void TRestGas::LoadGasFile( )
{

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

    cout << "Loading gas file : " << fGasFilename << endl;

    fGasMedium->LoadGasFile( (string) (GetGasDataPath() + fGasFilename) ); 

    fEFields.clear(); fBFields.clear(); fAngles.clear();
    fGasMedium->GetFieldGrid( fEFields, fBFields, fAngles);

    for( unsigned int i = 0; i < fEFields.size(); i++ )
        cout << "node " << i << " Field : " << fEFields[i] << " V/cm" << endl;

}

void TRestGas::AddGasComponent( string gasName, Double_t fraction )
{
    fGasComponentName.push_back( gasName );
    fGasComponentFraction.push_back( fraction );
    fNofGases++;
}

void TRestGas::InitFromConfigFile( )
{
    fPressureInAtm = StringToDouble ( GetParameter( "pressure" ) );
    fTemperatureInK = StringToDouble ( GetParameter( "temperature" ) );
    fNCollisions = StringToInteger( GetParameter( "nCollisions" ) );
    fMaxElectronEnergy = StringToDouble( GetParameter( "maxElectronEnergy" ) );
    fIonizationPotential = StringToDouble( GetParameter( "ionizationPotential" ) );

    if( fMaxElectronEnergy == -1 ) { fMaxElectronEnergy = 40; cout << "Setting default maxElectronEnergy to : " << fMaxElectronEnergy << endl; }
    if( fIonizationPotential == -1 ) { fIonizationPotential = 10; cout << "Setting default ionization potential : " << fIonizationPotential << endl; }

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

    /*
    fGasFilename += "-P_";
    sprintf( tmpStr, "%03.1lf", fPressureInAtm );
    fGasFilename += (TString) tmpStr + "atm";

    fGasFilename += "-T_";
    sprintf( tmpStr, "%03.1lf", fTemperatureInK );
    fGasFilename += (TString) tmpStr + "K";
    */

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

void TRestGas::GenerateGasFile( )
{
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

    fGasMedium->WriteGasFile ( (string) (GetGasDataPath() + fGasFilename) );
}

void TRestGas::SetGasPressure( Double_t pressure )
{
    fPressureInAtm = pressure;
    fGasMedium->SetPressure( fPressureInAtm * 760. );
}


void TRestGas::PlotDriftVelocity( Double_t eMin, Double_t eMax, Int_t nSteps )
{
    Double_t eField[nSteps], driftVel[nSteps];

    for( int i = 0; i < nSteps; i++ )
    {	
        eField[i] = eMin + (double) i * (eMax-eMin)/nSteps;

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

//______________________________________________________________________________
/*
void TRestGas::BuildAxis()
{
	//
	// to add the axis titles to the TGraph´s
	//
	if (fDriftVel) {
	}
	if (fLonDiffusion) {
		str.Form("Longitudinal diffusion for %s",GetName());
		fLonDiffusion->SetTitle(str);
		fLonDiffusion->GetXaxis()->SetTitle("E [V/cm]");
		fLonDiffusion->GetYaxis()->SetTitle("Longitudinal diffusion [#mum/#sqrt{cm}]");
	}
	if (fTransDiffusion) {
		str.Form("Transversal diffusion for %s",GetName());
		fTransDiffusion->SetTitle(str);
		fTransDiffusion->GetXaxis()->SetTitle("E [V/cm]");
		fTransDiffusion->GetYaxis()->SetTitle("Transversal diffusion [#mum/#sqrt{cm}]");
	}
	if (fTownsend) {
		str.Form("Townsend coefficient for %s",GetName());
		fTownsend->SetTitle(str);
		fTownsend->GetXaxis()->SetTitle("E [V/cm]");
		fTownsend->GetYaxis()->SetTitle("Townsend coefficient [1/cm]");
	}
	if (fAttachment) {
		str.Form("Attachment for %s",GetName());
		fAttachment->SetTitle(str);
		fAttachment->GetXaxis()->SetTitle("E [V/cm]");
		fAttachment->GetYaxis()->SetTitle("Attachment [1/cm]");
	}
}
*/


//______________________________________________________________________________
Double_t TRestGas::GetDriftVelocity( Double_t E ) // cm/us
{
    Double_t vx, vy, vz;
	fGasMedium->ElectronVelocity( 0., 0, -E, 0, 0, 0, vx, vy, vz);
    return vz * 1000.;
}

Double_t TRestGas::GetLongitudinalDiffusion( Double_t E )
{
    Double_t dl, dt;
	fGasMedium->ElectronDiffusion( 0., 0, -E, 0, 0, 0, dl, dt);
    return dl;
}

Double_t TRestGas::GetTransversalDiffusion( Double_t E )
{
    Double_t dl, dt;
	fGasMedium->ElectronDiffusion( 0., 0, -E, 0, 0, 0, dl, dt);
    return dt;
}

Double_t TRestGas::GetTownsendCoefficient( Double_t E )
{
    Double_t alpha;
	fGasMedium->ElectronTownsend( 0., 0, -E, 0, 0, 0, alpha );
    return alpha;
}

Double_t TRestGas::GetAttachmentCoefficient( Double_t E )
{
    Double_t eta;
	fGasMedium->ElectronTownsend( 0., 0, -E, 0, 0, 0, eta );
    return eta;
}


//______________________________________________________________________________
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
    cout << "Ionization potential : " << fIonizationPotential<< " eV" << endl;
    cout << "Max. Electron energy : " << fMaxElectronEnergy << " eV" << endl;
    cout << "Field grid nodes : " << fEnodes << endl;
    cout << "Efield range : ( " << fEmin << " , " << fEmax << " ) V/cm " << endl;
    cout << "Number of Gases : " << fNofGases << endl;
    for ( int i = 0; i < fNofGases; i++) 
        cout << "Gas id : " << i << " Name : " << fGasComponentName[i] << " Fraction : " << fGasComponentFraction[i] << endl;
    cout << "******************************************" << endl;
}

