///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignal.h
///
///             Event class to store signals form simulation and acquisition events 
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia
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
        
        vector <TVector2> fEventSignal;   //2D vector with the time and the charge of the signal
        				  //Convention: TVector2 p(t,c) first time and then the charge
        				  
           
    public:

	void Reset(){fEventSignal.clear(); }
        
        //Setters
        void AddPoint(TVector2 p){fEventSignal.push_back(p);}
        void SetSignalID(Int_t sID){fSignalID=sID;}
        //Getters
        Int_t GetNumberOfPoints(){return fEventSignal.size();}
        TVector2 *GetPoint(Int_t n){
        return &fEventSignal[n];
        }
        Int_t GetSignalID(){return fSignalID;}
        
        //Construtor
        TRestSignal();
        //Destructor
        ~TRestSignal();
        
        ClassDef(TRestSignal, 1);



};
#endif



