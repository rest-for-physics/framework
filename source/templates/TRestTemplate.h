///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTemplate.h
///
///             A simple template class for copy/paste
///
///             jul 2015:   First concept
///                 J. Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestTemplate
#define RestCore_TRestTemplate

#include <iostream>
using namespace std;

#include "TObject.h"

class TRestTemplate:public TObject {
 protected:

 public:

   //Construtor
   TRestTemplate();
   //Destructor
   virtual ~ TRestTemplate();

   ClassDef(TRestTemplate, 1);     // REST event superclass
};
#endif
