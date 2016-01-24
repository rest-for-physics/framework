///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestVolumeHits.h
///
///             Event class to store signals form simulation and acquisition events 
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Gracia
///_______________________________________________________________________________


#ifndef RestCore_TRestVolumeHits
#define RestCore_TRestVolumeHits

#include <iostream>
using namespace std;

#include <TObject.h>
#include <TVector3.h>
#include <TRestHits.h>

class TRestVolumeHits: public TRestHits {

    protected:
        
     vector <Int_t>   fSigmaX;		// [fNHits] Sigma on X axis for each volume hit (units microms)
     vector <Int_t>   fSigmaY;		// [fNHits] Sigma on Y axis for each volume hit (units microms)
     vector <Int_t>   fSigmaZ;		// [fNHits] Sigma on Z axis for each volume hit (units microms)  				  
           
    public:

       void AddHit( Double_t x, Double_t y, Double_t z, Double_t en, Double_t sigmax, Double_t sigmay, Double_t sigmaz );
       void AddHit( TVector3 pos, Double_t en, TVector3 sigma );
        
        void RemoveHits( );
        void MergeHits( Int_t n, Int_t m );

        void RemoveHit( int n );
        //Setters

        //Getters

        Double_t GetSigmaX( int n ) { return ( (Double_t) fSigmaX[n])/1000.; } // return value in mm
        Double_t GetSigmaY( int n ) { return ( (Double_t) fSigmaY[n])/1000.; } // return value in mm
        Double_t GetSigmaZ( int n ) { return ( (Double_t) fSigmaZ[n])/1000.; } // return value in mm

        void PrintHits();

        Double_t GetClusterSize( int n) { return TMath::Sqrt( fSigmaX[n] * fSigmaX[n] + fSigmaY[n]*fSigmaY[n] + fSigmaZ[n]*fSigmaZ[n]); }
        Double_t GetXYSize( int n) { return TMath::Sqrt( fSigmaX[n] * fSigmaX[n] + fSigmaY[n]*fSigmaY[n]); }

        //Construtor
        TRestVolumeHits();
        //Destructor
        ~TRestVolumeHits();
        
        ClassDef(TRestVolumeHits, 1);

};
#endif


