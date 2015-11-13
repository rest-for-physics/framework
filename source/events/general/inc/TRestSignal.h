///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignal.h
///
///             Event class to store signals fromm simulation and acquisition events 
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia/Javier Galan
///		nov 2015:
///		    Changed vectors fSignalTime and fSignalCharge from <Int_t> to <Float_t>
///	            JuanAn Garcia
///_______________________________________________________________________________


#ifndef RestCore_TRestSignal
#define RestCore_TRestSignal

#include <iostream>
using namespace std;

#include <TObject.h>
#include <TVector2.h>


class TRestSignal: public TObject {

    protected:
        
        Int_t fSignalID;
        
        vector <Float_t> fSignalTime;   //Vector with the time of the signal
        vector <Float_t> fSignalCharge; //Vector with the charge of the signal
        
        #ifndef __CINT__
	TVector2 vector2; 
	#endif		  
        
        void AddPoint(TVector2 p);
        
    public:
	
        //Getters
        TVector2 *GetPoint(Int_t n){
        vector2.Set(GetTime(n), GetData(n));
        
        return &vector2;
        }
        
        Int_t GetSignalID( ) { return fSignalID; }
        
        Int_t GetNumberOfPoints(){
       
        if(fSignalTime.size()!=fSignalCharge.size()){
        cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
        cout<<"WARNING, the two vector sizes did not match"<<endl;
        cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
        }
        return fSignalTime.size(); 
        }

        Double_t GetIntegral( );

        Double_t GetData( Int_t index ) { return (double)fSignalCharge[index]; }
        Double_t GetTime( Int_t index ) { return (double)fSignalTime[index]; }
        Int_t GetTimeIndex(Double_t t);
        
        //Setters
        void SetSignalID(Int_t sID) { fSignalID = sID; }

        void AddPoint( Double_t t, Double_t d );
        void AddCharge( Double_t t, Double_t d );
        void AddDeposit( Double_t t, Double_t d );
        
        Bool_t isSorted( );
        void Sort();


        void Reset() { fSignalTime.clear();fSignalCharge.clear();}

        void Print( );
                
        //Construtor
        TRestSignal();
        //Destructor
        ~TRestSignal();
        
        ClassDef(TRestSignal, 1);



};
#endif



