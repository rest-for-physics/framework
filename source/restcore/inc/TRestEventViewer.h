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
#include <TVector3.h>
#include <TGeoMaterial.h>
#include <TGeoManager.h>
#include <TPad.h>
#include <TGLViewer.h>

#include "TRestEvent.h"

#include <TNamed.h>
#include <TEveManager.h>
#include <TEveEventManager.h>
#include <TEveProjectionManager.h>
#include <TEveProjectionAxes.h>
#include <TEveViewer.h>
#include <TEveArrow.h>
#include <TGeoNode.h>
#include <TEveLine.h>
#include <TEveText.h>
#include <TEveTrans.h>
#include <TEveGeoNode.h>
#include <TEveScene.h>
#include <TEveBrowser.h>
#include <TGTab.h>

#define GEOM_SCALE 0.1

class TRestEventViewer:public TNamed {

    private:
    TEveManager *gEve;
    
    TEveWindowSlot  *slot;
    TEveWindowPack *pack;
    
    TEveViewer *viewer3D;
    TEveViewer *rphiViewer;
    TEveViewer *rhozViewer;
    
    TEveScene *rphiScene;
    TEveScene *rhozScene;
    
    TEveProjectionManager* rphi;
    TEveProjectionManager* rhoz;
    
    TEveProjectionAxes* rphiAxes;
    TEveProjectionAxes* rhozAxes;
    
    Bool_t isMultiView;
    
    char pointName[256];
 
    vector <TEveLine *> fHitConnectors;
    
    protected:
        
    Int_t nDeposits;
    
    void AddMarker( TVector3 at, TString name );

    public:


    virtual void Initialize();

    virtual void AddEvent( TRestEvent *ev ) = 0;
    
    void DeleteCurrentEvent(  );

    virtual void Draw(  );
        
    void AddGeometry( TGeoManager *geo );
    void AddGeometry( TString fName );
    
    void DeleteGeometry( );
    
    void AddSphericalHit( double x, double y, double z, double radius );
    void AddSphericalHit( double x, double y, double z, double radius, double en );
    void NextTrackVertex( Int_t trkID, TVector3 to );
    void AddTrack( Int_t trkID, Int_t parentID, TVector3 from, TString name );
    void AddParentTrack( Int_t trkID, TVector3 from, TString name );

    void AddText( TString text, TVector3 at );
    void AddMarker( Int_t trkID, TVector3 at, TString name );
    
    void AddPoints( Int_t npoints, TVector3 at[ ] , Double_t radius);
    
    void MultiView( );
    void AddProjectedEvent( );
    
    void Update( );
    
    //Construtor
    TRestEventViewer();
    //Destructor
    virtual ~ TRestEventViewer();

    ClassDef(TRestEventViewer, 1);     // REST event superclass
};
#endif

