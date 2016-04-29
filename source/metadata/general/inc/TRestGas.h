///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGas.h
///
///
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
///_______________________________________________________________________________


#ifndef RestCore_TRestGas
#define RestCore_TRestGas
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <TROOT.h>
#include "TSystem.h"
#include "TApplication.h"
#include "TNamed.h"
#include "TString.h"
#include "TGraph.h"
#include "TArrayI.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TVector3.h"

#include "TRestMetadata.h"

#ifndef __CINT__
#include "MediumMagboltz.hh"
#endif

const int RESTGAS_ERROR = -1;
const int RESTGAS_INTITIALIZED = 0;
const int RESTGAS_CFG_LOADED = 1;
const int RESTGAS_GASFILE_LOADED = 2;

class TRestGas : public TRestMetadata
{

private:


#ifndef __CINT__
    Garfield::MediumMagboltz *fGasMedium;
#endif


	///////////////////////////////// base data
    
    Int_t fStatus;
    TString fGasFilename;
    Int_t fNofGases;			// number of different species composing the gas mixture

    Int_t fNCollisions;
    Double_t fMaxElectronEnergy;
    Double_t fW;

    std::vector <TString> fGasComponentName;
    std::vector <Double_t> fGasComponentFraction;

    Double_t fPressureInAtm;		// pressure of the gas
    Double_t fTemperatureInK;	// temperature of the gas

    Int_t fEnodes;
    Double_t fEmax, fEmin; // In V/cm

    std::vector <Double_t> fEFields,  fBFields, fAngles;

    bool fGasGeneration;

	///////////////////////////////// Magboltz data
	// graphs to store the information on the gas (in function of the electric field)
    /*
	TGraph*		fDriftVel;			// drift velocity 
	TGraph*		fLonDiffusion;		// longitudinal diffusion
	TGraph*		fTransDiffusion;	// trnasversal diffusion
	TGraph*		fTownsend;			// townsend coefficient
	TGraph*		fAttachment;		// attachment coefficient
    */

	
	//Bool_t ReadFromFile(const char* filename);
	//Bool_t Construct(Int_t ngases, TString gas[], Int_t ratio[], Double_t pressure);
	//void BuildAxis();

    void InitFromConfigFile( );
    void ConstructFilename( );


    void AddGasComponent( std::string gasName, Double_t fraction );

    void GenerateGasFile( );

		
public:
	TRestGas();
    TRestGas( const char *cfgFileName, string name = "", bool gasGeneration = false);
	~TRestGas();

    void EnableGasGeneration ( ) { fGasGeneration = true; }
    bool GasFileGenerationEnabled() { return fGasGeneration; } 

    void Initialize();
	// setters
	void SetNofGases(Int_t ng) { fNofGases = ng; }

    void LoadGasFile();
	// getters
    /*
	TString GetGasName();

	TGraph* GetDriftVelGraph() { return fDriftVel; };
	TGraph* GetLonDiffusionGraph() { return fLonDiffusion; };
	TGraph* GetTransDiffusionGraph() { return fTransDiffusion; };
	TGraph* GetTownsendGraph() { return fTownsend; };
	TGraph* GetAttachmentGraph() { return fAttachment; };
    */

    Double_t GetMaxElectronEnergy() { fMaxElectronEnergy = fGasMedium->GetMaxElectronEnergy(); return fMaxElectronEnergy; }
	Int_t GetNofGases() { return fNofGases; }
    TString GetGasComponentName( Int_t n ) { return fGasComponentName[n]; }
    TString GetGasMixture() {
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

    Double_t GetGasComponentFraction( Int_t n ) { return fGasComponentFraction[n]; }
    Double_t GetPressure() { return fPressureInAtm; }; 
    Double_t GetTemperature() { return fTemperatureInK; }; 
    Double_t GetWvalue() { return fW; }

    TString GetGasDataPath() { return (TString ) getenv("REST_PATH") + "/inputData/gasFiles/"; }


    Double_t GetDriftVelocity( Double_t E );
    Double_t GetLongitudinalDiffusion( Double_t E );
    Double_t GetTransversalDiffusion( Double_t E );
    Double_t GetTownsendCoefficient( Double_t E );
    Double_t GetAttachmentCoefficient( Double_t E );

    void SetPressure( Double_t pressure );
    void SetMaxElectronEnergy( Double_t energy ) { fMaxElectronEnergy = energy; }
    void SetWvalue( Double_t iP ) { fW = iP; }


    void PlotDriftVelocity( Double_t eMin, Double_t eMax, Int_t nSteps );
    void PlotLongitudinalDiffusion( Double_t eMin, Double_t eMax, Int_t nSteps );
    void PlotTransversalDiffusion( Double_t eMin, Double_t eMax, Int_t nSteps );
    void PlotTownsendCoefficient( Double_t eMin, Double_t eMax, Int_t nSteps );
	void PrintGasInfo();

	void PrintMetadata() { PrintGasInfo(); }

	
	ClassDef(TRestGas,1);  // Gas Parameters
};

#endif
