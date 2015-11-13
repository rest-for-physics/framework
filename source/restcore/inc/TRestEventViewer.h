///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEventViewer.h
///
///             A geometry class to store detector geometry 
///
///             jul 2015:   First concept
///                 J. Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestEventViewer
#define RestCore_TRestEventViewer

#include <iostream>
using namespace std;

#include <TObject.h>
#include <TGeoManager.h>
#include <TGLViewer.h>

#include "TRestEvent.h"

class TRestEventViewer {

    protected:
     
    TGeoManager *fGeometry;
    TRestEvent *fEvent;
        
    public:


    virtual void Initialize();

    virtual void AddEvent( TRestEvent *ev ) = 0;
            
    virtual void DeleteCurrentEvent(  );
    //Setters
    virtual void SetGeometry( TGeoManager *geo){ fGeometry=geo;}
    //Getters
    TGeoManager *GetGeometry( ){return fGeometry;}
    TRestEvent *GetEvent( ){return fEvent;}

            
    //Construtor
    TRestEventViewer();
    //Destructor
    virtual ~ TRestEventViewer();

    ClassDef(TRestEventViewer, 1);     // REST event superclass
};
#endif

