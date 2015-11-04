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
///                 JuanAn Garcia/Javier Galan
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
        
        vector <TVector2> fSignalData;   //2D vector with the time and the charge of the signal
        				  //Convention: TVector2 p(t,c) first time and then the charge
        				  
           
    public:

        //Getters
        Int_t GetNumberOfPoints(){return fEventSignal.size();}
        TVector2 *GetPoint(Int_t n){
        return &fEventSignal[n];
        }
        Int_t GetSignalID( ) { return fSignalID; }
        Int_t GetTimeIndex( Double_t t );

        Int_t GetNumberOfPoints() { return fSignalData.size(); }

        Double_t GetIntegral( );

        Double_t GetData( Int_t index ) { return fSignalData[index].Y(); }
        Double_t GetTime( Int_t index ) { return fSignalData[index].X(); }
        
        //Setters
        void SetSignalID(Int_t sID) { fSignalID = sID; }

        void AddPoint( Double_t t, Double_t d );
        void AddCharge( Double_t t, Double_t d );
        void AddDeposit( Double_t t, Double_t d );

        void AddPoint(TVector2 p);

        Bool_t isSorted( );
        void Sort();


        void Reset() { fSignalData.clear(); }

        void Print( );
                
        //Construtor
        TRestSignal();
        //Destructor
        ~TRestSignal();
        
        ClassDef(TRestSignal, 1);



};
#endif



