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

        std::vector <Float_t>   fX;		// [fNHits] Position on X axis for each punctual deposition (units microms)
        std::vector <Float_t>   fY;		// [fNHits] Position on Y axis for each punctual deposition (units microms)
        std::vector <Float_t>   fZ;		// [fNHits] Position on Z axis for each punctual deposition (units microms)
        std::vector <Float_t>   fEnergy;	// [fNHits] Energy deposited at each 3-coordinate position (units eV)

        //! Changes the orgin of the Cartesian coordinate system
        void Translate( Double_t x, Double_t y, Double_t z);

        void AddHit( Double_t x, Double_t y, Double_t z, Double_t en );
        void AddHit( TVector3 pos, Double_t en );
        void RemoveHits( );
	
        void MergeHits( int n, int m );
        void SwapHits( Int_t i, Int_t j );
        virtual void RemoveHit( int n );

        Bool_t areXY();
        Bool_t areXZ();
        Bool_t areYZ();
        Bool_t areXYZ();

        void GetXArray( Float_t *x );
        void GetYArray( Float_t *y );
        void GetZArray( Float_t *z );

        Double_t GetDistanceToNode( Int_t n );

        Bool_t isSortedByEnergy( );
	
        Int_t GetNumberOfHits( ) { return fNHits; }

        Double_t GetX( int n ) { return ( (Double_t) fX[n]); } // return value in mm
        Double_t GetY( int n ) { return ( (Double_t) fY[n]); } // return value in mm
        Double_t GetZ( int n ) { return ( (Double_t) fZ[n]); } // return value in mm

        TVector3 GetPosition( int n )
        {
            return TVector3 ( ( (Double_t) fX[n]), ((Double_t) fY[n]), ((Double_t) fZ[n]) ) ; 
        }

        TVector3 GetVector( int n, int m )
        {
            TVector3 vector = GetPosition(n) - GetPosition(m);
            return vector;
        }

        Double_t GetEnergy( int n ) { return ( (Double_t) fEnergy[n]); } //return value in keV

        Double_t GetMaximumHitEnergy( );
        Double_t GetMinimumHitEnergy( );
        Double_t GetMeanHitEnergy( );

        Double_t GetEnergyIntegral();
        Double_t GetTotalDepositedEnergy() { return fTotEnergy; }
        Double_t GetTotalEnergy() { return fTotEnergy; }
        Double_t GetDistance2( int n, int m );
        Double_t GetTotalDistance();

        void PrintHits();
	
        //Construtor
        TRestHits();
        //Destructor
        ~TRestHits();

        ClassDef(TRestHits, 1);
};
#endif
