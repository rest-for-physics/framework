///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTemplateEventProcess.h
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


#ifndef RestCore_TRestTemplateEventProcess
#define RestCore_TRestTemplateEventProcess

#include "TRestEventProcess.h"

class TRestTemplateEventProcess:public TRestEventProcess {
 protected:
   //add here the members of your event process
   Int_t fDummyMember1;			// change this by your members

 public:
   void InitProcess();
   void Execute();
   void EndProcess();

   //Constructor
   TRestTemplateEventProcess();
   //Destructor
   ~TRestTemplateEventProcess();

   ClassDef(TRestTemplateEventProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif
