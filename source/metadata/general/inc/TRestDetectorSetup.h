///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSetup.h
///
///             Metadata class to be used to store basic detector setup info
///             inherited from TRestMetadata
///
///             jun 2016:   First concept. Javier Galan
//  
///_______________________________________________________________________________


#ifndef RestCore_TRestDetectorSetup
#define RestCore_TRestDetectorSetup

#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>

#include <TRestMetadata.h>

class TRestDetectorSetup:public TRestMetadata {
    private:
        void Initialize();

        void InitFromConfigFile();

	Int_t fRunNumber;
	Int_t fSubRunNumber;
	TString fRunTag;

	Double_t fMeshVoltage;
	Double_t fDriftField;
	Double_t fDetectorPressure;

	TString fElectronicsGain;
	TString fShapingTime;
	TString fSamplingTime;
	

    public:

	Int_t GetRunNumber() { return fRunNumber; }

	void InitFromFileName( TString fName );

        void PrintMetadata( );

        //Constructors
        TRestDetectorSetup();
        TRestDetectorSetup( char *cfgFileName, std::string name = "");
        //Destructor
        ~TRestDetectorSetup();


        ClassDef(TRestDetectorSetup, 1); 
};
#endif
