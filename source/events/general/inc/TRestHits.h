///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHits.h
///
///             Event class to store hits 
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///		nov 2015:
///		    Changed vectors fX fY fZ and fEnergy from <Int_t> to <Float_t>
///	            JuanAn Garcia
///_______________________________________________________________________________


#ifndef TRestSoft_TRestHits
#define TRestSoft_TRestHits

#include <iostream>
using namespace std;

#include "TObject.h"
#include "TMath.h"
#include "TArrayI.h"
#include <TVector3.h>

//! Storage class

//! It let save an event as a set of punctual deposition.
//! It saves a 3-coordinate position and an energy for each punctual deposition.
class TRestHits : public TObject
{
    public:
        Int_t     fNHits;	///< Number of punctual energy depositions, it is the lenght for all the array
        Double_t  fTotEnergy;	///< Event total energy

        vector <Float_t>   fX;		// [fNHits] Position on X axis for each punctual deposition (units microms)
        vector <Float_t>   fY;		// [fNHits] Position on Y axis for each punctual deposition (units microms)
        vector <Float_t>   fZ;		// [fNHits] Position on Z axis for each punctual deposition (units microms)
        vector <Float_t>   fEnergy;	// [fNHits] Energy deposited at each 3-coordinate position (units eV)

        //! Changes the orgin of the Cartesian coordinate system
        void ChangeOrigin(double origx, double origy, double origz);

        void AddHit( Double_t x, Double_t y, Double_t z, Double_t en );
        void AddHit( TVector3 pos, Double_t en );
        void RemoveHits( );

        Int_t GetNumberOfHits( ) { return fNHits; }

        Double_t GetX( int n ) { return ( (Double_t) fX[n]); } // return value in mm
        Double_t GetY( int n ) { return ( (Double_t) fY[n]); } // return value in mm
        Double_t GetZ( int n ) { return ( (Double_t) fZ[n]); } // return value in mm
        Double_t GetEnergy( int n ) { return ( (Double_t) fEnergy[n]); } //return value in keV

        TVector3 Get( int n ) {
            return TVector3 ( ( (Double_t) fX[n]), ((Double_t) fY[n]), ((Double_t) fZ[n]) ) ; 
        }


        Double_t GetTotalDepositedEnergy() { return fTotEnergy; }


        //Construtor
        TRestHits();
        //Destructor
        ~TRestHits();

        ClassDef(TRestHits, 1);
};
#endif
