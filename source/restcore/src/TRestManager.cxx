#include "TRestManager.h"

ClassImp(TRestManager)


TRestManager::TRestManager()
{
	// default constructor of TRestManager
	// these lines below are temporary. Just to reproduce a typical call to a template "dummy" task...

//TRestManager loads the main metadata (TRestMainConfig) from config files or command line.
//TRestManager creates the appropiate “task” object (TRestFileTask)
//TRestFileTask creates the appropiate TRestMetadata objects
//TRestMetadata objects are initialized with metadata from config files or are “loaded” from data files
//TRestFileTask creates the process objects that it needs (TRestEventProcess) and initializes them. 
//TRestFileTask opens the input files (if any) and browse through them.
//For each of the targeted events the targeted TRestEventProcess are executed. The final output TRestEvent (if any) of the processes is saved on a Ttree on memory.
//TRestFileTask saves the output TTree and closes output file(s) (if any)
//TRestFileTask executes “ending” routinges for each of the processes, and saves the TMetadata objects to the output file (if any). Objects are deleted.

	Int_t resultOfTask = 0;
	
	// the main config file name must be in the command line (for the  moment we hardwire it here...
	fMainConfigFileName = (TString)"RestTemplateMainConfig.cfg";
	
	// FIRST STEP: Create main metadata object, attach it to an external file and initialize with data from the file...
	fMainConfig = new TRestTemplateMetadata( );
    /*
	if (fMainConfig->CheckConfigFile() == 0) {
		cout << "Manager stops." << endl;
	}
    */
	fMainConfig->InitFromConfigFile();

	//SECOND STEP: Create the appropiate FileTask object and Launch it.
//	if (fMainConfig->GetName() == "TemplateFileTask") { // create and launch task
		fFileTask = new TRestTemplateFileTask();  
		resultOfTask = fFileTask->LaunchFileTask(); 
//	}

	return;
}

/*
TRestManager::TRestManager(TRestConfig *config) : TRestFiles(config)
{
  rawTree = new TTree(); daqTree = new TTree(); physTree = new TTree(); g4Tree = new TTree(); clusterTree=new TTree();
	rawBranch = new TBranch(); daqBranch = new TBranch(); physBranch = new TBranch(); g4Branch = new TBranch(); clusterBranch= new TBranch();
	rawEvent = new T2KrawEvent(); daqEvent = new TRestDaqEvent(); physEvent = new TRestPhysEvent(); g4Event = new TRestG4Event(); 
      clusterEvent = new TRestClusterEvent();

	if(restConfig->fTypeOfWork == "Histo2Daq")
	{
		cout << "Running HistoEvent2DaqEvent..." << endl;
		HistoEvent2DaqEvent();
		exit(0);
	}
	else if(restConfig->fTypeOfWork == "Histo2Phys")
	{
		cout << "Running HistoEvent2PhysEvent..." << endl;
		HistoEvent2PhysEvent();
		exit(0);
	}
	else if(restConfig->fTypeOfWork == "Sim2Root")
	{
		cout << "Running G4Simulated event to G4Event..." << endl;
		G4Sim2G4Event();
		exit(0);
	}
	else if(restConfig->fTypeOfWork == "ConversionPhys")
	{
		cout << "Running ConversionPhys..." << endl;
		ConversionPhys();
		exit(0);
	}
	else if(restConfig->fTypeOfWork == "ClusterFinder")
	{
		cout << "Running ClusterFinder..." << endl;
		ClusterFinder();
		exit(0);
	}
	else if(restConfig->fTypeOfWork == "PhysBrowser")
	{
		cout << "Running PhysBrowser..." << endl;
		PhysBrowser();
	}
}
*/
TRestManager::~TRestManager()
{
/*	delete rawTree;		delete daqTree;		delete physTree;	delete g4Tree;   delete clusterTree;
	delete rawBranch;	delete daqBranch;	delete physBranch;	delete g4Branch;  delete clusterBranch;
	delete rawEvent;	delete daqEvent;	delete physEvent;	delete g4Event;   delete clusterEvent;
*/}

