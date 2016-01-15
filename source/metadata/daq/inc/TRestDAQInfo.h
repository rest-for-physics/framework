///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///            TRestDaqInfo inherited from TObject
///
///            Container class for the DAQ info.
///
///             Jan 2016:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia
///_______________________________________________________________________________


#ifndef RestCore_TRestDAQInfo
#define RestCore_TRestDAQInfo

#include <iostream>
using namespace std;

#include "TObject.h"

classTRestDaqInfo:public TObject{
    
    protected:
	
	TString fBinFileName;
	vector <string> fPedBuffer;//Pedestal script
	vector <string> fRunBuffer;//Run script
	Short_t fGain;
	Short_t fShappingTime;
	
	//Add more if needed
	
    public:

        void PrintMetadata( );
							
        //Construtor
       TRestDaqInfo();
       TRestDaqInfo( char *cfgFileName);
        //Destructor
        virtual ~TRestDaqInfo();
	
	void SetPedStringBuffer(TString fName);
	void SetRunStringBuffer(TString fName);
	
	void GetGain(){return fGain;}
	void GetShappingTime{return fShappingTime;}
	
	void SetParFromBuffer(string buffer);//Set gain and shapping time from a given buffer

        ClassDef(TRestDaqInfo, 1);     // REST run class
};
#endif
