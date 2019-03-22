#ifndef __RestCore_TRestPhysics_H__
#define __RestCore_TRestPhysics_H__

#include <iostream>
using namespace std;

#include <TString.h>

//! This namespace defines the unit conversion for different units which are understood by REST

//! TODO: Write a detailed description HERE
namespace REST_Physics {

	const double vacuumPermitivity	=	8.854E-12;	// F/m
	const double vacuumPermeability =	4E-7 * 3.141592653589793;   // H/m	
	const double speedLight =		2.998E8;	// m/s
	const double qElectron  =		1.602E-19;	// C
	const double mElectron  =		9.107E-31;	// Kg
	const double kBoltzman  =		1.380E-23;	// J/K
	const double hPlanck    =		1.054E-34;	// J*s

	/* Physical constants */
	const double PhMeterIneV = 5067731.236453719; // 8.0655447654281218E5;// 506.773123645372;
	const double lightSpeed = 2.99792458E8;
	const double naturalElectron = 0.302822120214353;
}

//dummy class
class TRestPhysics {};
#endif
