///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTemplateEventProcess.cxx
///
///             Template to use to design "event process" classes inherited from 
///             TRestTemplateEventProcess
///             How to use: replace TRestTemplateEventProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________


#include "TRestTemplateEventProcess.h"

ClassImp(TRestTemplateEventProcess)
//______________________________________________________________________________
TRestTemplateEventProcess::TRestTemplateEventProcess()
{
   // TRestTemplateEventProcess default constructor
}

//______________________________________________________________________________
TRestTemplateEventProcess::~TRestTemplateEventProcess()
{
   // TRestTemplateEventProcess destructor
}

//______________________________________________________________________________
void TRestTemplateEventProcess::InitProcess()
{
   // Function to be executed once at the beginning of process
   // (before starting the process of the events)

	//Start by calling the InitProcess function of the abstract class. 
	//Comment this if you don't want it.
	TRestEventProcess::InitProcess();
}

//______________________________________________________________________________
void TRestTemplateEventProcess::Execute()
{
   // Function to be executed for every event to be processed
}

//______________________________________________________________________________
void TRestTemplateEventProcess::EndProcess()
{
   // Function to be executed once at the end of the process 
   // (after all events have been processed)

   //Start by calling the EndProcess function of the abstract class. 
   //Comment this if you don't want it.
   TRestEventProcess::EndProcess();
}
