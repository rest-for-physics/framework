
#ifndef TRestSoft_TRestHitsEvent
#define TRestSoft_TRestHitsEvent

#include <iostream>
using namespace std;

#include "TObject.h"
#include "TMath.h"
#include "TArrayI.h"
#include <TVector3.h>

#include <TRestEvent.h>
//! Storage class

//! It let save an event as a set of punctual deposition.
//! It saves a 3-coordinate position and an energy for each punctual deposition.
class TRestHitsEvent : public TRestEvent
{
	public:
	Int_t     fNHits;	///< Number of punctual energy depositions, it is the lenght for all the array
	Double_t  fTotEnergy;	///< Event total energy

	vector <Int_t>   fX;		// [fNHits] Position on X axis for each punctual deposition (units microms)
	vector <Int_t>   fY;		// [fNHits] Position on Y axis for each punctual deposition (units microms)
	vector <Int_t>   fZ;		// [fNHits] Position on Z axis for each punctual deposition (units microms)
	vector <Int_t>   fEnergy;	// [fNHits] Energy deposited at each 3-coordinate position (units eV)

	//! Changes the orgin of the Cartesian coordinate system
	void ChangeOrigin(double origx, double origy, double origz);

    void AddHit( Double_t x, Double_t y, Double_t z, Double_t en );
    void AddHit( TVector3 pos, Double_t en );
    void RemoveHits( );

    Int_t GetNumberOfHits( ) { return fNHits; }

     Double_t GetX( int n ) { return ( (Double_t) fX[n])/1000.; } // return value in mm
     Double_t GetY( int n ) { return ( (Double_t) fY[n])/1000.; } // return value in mm
     Double_t GetZ( int n ) { return ( (Double_t) fZ[n])/1000.; } // return value in mm
     Double_t GetEnergy( int n ) { return ( (Double_t) fEnergy[n])/1000.; } //return value in keV


     TVector3 Get( int n ) {
        return TVector3 ( ( (Double_t) fX[n])/1000., ((Double_t) fY[n])/1000., ((Double_t) fZ[n])/1000. ) ; 
      }


	Double_t GetTotalDepositedEnergy() { return fTotEnergy; }
	

	//Construtor
	TRestHitsEvent();
	//Destructor
	~TRestHitsEvent();

	ClassDef(TRestHitsEvent, 1);
};
#endif
