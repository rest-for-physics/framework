///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestBiasingVolume.h
///
///             A template class for copy/paste
///
///             jul 2015:   First concept
///                 J. Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestBiasingVolume
#define RestCore_TRestBiasingVolume

#include <iostream>

#include "TObject.h"
#include "TVector3.h"

class TRestBiasingVolume:public TObject {
    protected:
        TVector3 fVolumePosition;
        Double_t fVolumeSize;
        TString fBiasingVolumeType;
        Double_t fBiasingFactor;
        TVector2 fEnergyRange;
        TString fVolumeType;

    public:

        Double_t GetBiasingFactor() { return fBiasingFactor; }
        Double_t GetBiasingVolumeSize() { return fVolumeSize; }
        TString GetBiasingVolumeType() { return fVolumeType; }
        TVector3 GetBiasingVolumePosition() { return fVolumePosition; }
        TVector2 GetEnergyRange() { return fEnergyRange; }
        Double_t GetMaxEnergy() { return fEnergyRange.Y(); }
        Double_t GetMinEnergy() { return fEnergyRange.X(); }


        void SetBiasingVolumeSize ( Double_t size ) { fVolumeSize = size; }
        void SetBiasingVolumeType( TString type ) { fVolumeType = type; }
        void SetBiasingVolumePosition( TVector3 pos ) { fVolumePosition = pos; }
        void SetBiasingFactor ( Double_t factor ) { fBiasingFactor = factor; }
        void SetEnergyRange( TVector2 eRange ) { fEnergyRange = eRange; }

        // Check if it is inside the sphere
        Int_t isInside( Double_t x, Double_t y, Double_t z )
        {
            if( fVolumeType.Contains( "virtualBox" ) )
            {
                if( x < fVolumeSize/2. && x > -fVolumeSize/2. )
                    if( y < fVolumeSize/2. && y > -fVolumeSize/2. )
                        if( z < fVolumeSize/2. && z > -fVolumeSize/2. )
                            return 1;
            }
            else if( fVolumeType == "virtualSphere" )
            {
                Double_t r2 = x*x + y*y + z*z;
                if( r2 < fVolumeSize*fVolumeSize )
                    return 1;
            }
	    else
	    {
		cout << "TRestBiasingVolume::isInside. Biasing volume type not found" << endl;
	    }
            return 0;
        }

        void PrintBiasingVolume();

        //Construtor
        TRestBiasingVolume();
        //Destructor
        virtual ~ TRestBiasingVolume();

        ClassDef(TRestBiasingVolume, 2);     // REST event superclass
};
#endif
