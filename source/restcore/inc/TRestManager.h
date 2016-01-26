#ifndef TRestSoft_TRestManager
#define TRestSoft_TRestManager

//C/C++ libraries
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

//ROOT libraries
#include "TROOT.h"
#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TRandom.h"
#include "TString.h"

//T2Kelectronics libraries
//#include "T2KrawEvent.h"

//TRestSoft libraries
#include "TRestMetadata.h"
#include "TRestFileTask.h"
#include "TRestTemplateMetadata.h"
#include "TRestTemplateFileTask.h"


class TRestManager : public TObject
{
	public:
	TRestTemplateMetadata *fMainConfig; // pointer to the main config metadata object
	TRestTemplateFileTask *fFileTask; // pointer to the file task object to be launched;
	TString fMainConfigFileName; // name of the main config file

/*	TRestCalibration *restCalibration;
	TRestDrift *restDrift;
	TRestPixel *restPixel;
	TRestShaper *restShaper;
	TRestClusterFinder *restClusterFinder;

	void HistoEvent2DaqEvent();
	void HistoEvent2PhysEvent();
	void G4Sim2G4Event();
	void ConversionPhys();
        void ClusterFinder();
	void PhysBrowser();
	void PhysEvent2HistoEvent();
	void ShowProcess(int j, int total);
*/
	//Constructors
	TRestManager();
//	TRestManager(TRestConfig *config);
	//Destructor
	~TRestManager();

	ClassDef(TRestManager, 2);
};

typedef struct
{
	double posx;
	double posy;
	double posz;
	double Ehit;
	int    process_flag;
	int    pID;
} G4hit_struct;

typedef struct
{
	int process, place, particle, auxint, type;
	double energy, auxdouble, edep;
} G4int_struct;
#endif
