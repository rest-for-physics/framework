///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSimEventViewer.h
///
///             nov 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn
///_______________________________________________________________________________


#ifndef RestCore_TRestSimEventViewer
#define RestCore_TRestSimEventViewer

#include "TRestEventViewer.h"

#include <TObject.h>
#include <TVector3.h>
#include <TGeoMaterial.h>
#include <TGeoManager.h>
#include <TPad.h>
#include <TGLViewer.h>

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

class TRestSimEventViewer:public TRestEventViewer {

 protected:
   
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
    
    char pointName[256];
    
    Double_t fMinRadius;
    Double_t fMaxRadius;
    
 public:
 
   virtual void Initialize();
   
   virtual void DeleteCurrentEvent(  );
   void DeleteGeometry( );
   
   virtual void AddEvent( TRestEvent *ev ) = 0;
   
   void AddSphericalHit( double x, double y, double z, double radius, double en );
   
   void MultiView( );
   void DrawTab( );
   void SetGeometry(TGeoManager *geo);
   void Update( );
   
   void SetMinRadious(Double_t rmin){fMinRadius=rmin;}
   void SetMaxRadious(Double_t rmax){fMaxRadius=rmax;}
   
   //Constructor
   TRestSimEventViewer();
   //Destructor
   virtual ~TRestSimEventViewer();

   ClassDef(TRestSimEventViewer, 1);      //class inherited from TRestEventViewer
};
#endif
