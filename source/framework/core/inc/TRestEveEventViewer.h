///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEveEventViewer.h inherited from TRestEventViewer
///
///             nov 2015:   First concept
///                 Generic class for visualization of simulated events using
///                 gEveManager JuanAn/Javier Galan
///_______________________________________________________________________________

#ifndef RestCore_TRestEveEventViewer
#define RestCore_TRestEveEventViewer

#include <TEveArrow.h>
#include <TEveBrowser.h>
#include <TEveEventManager.h>
#include <TEveGeoNode.h>
#include <TEveLine.h>
#include <TEveManager.h>
#include <TEveProjectionAxes.h>
#include <TEveProjectionManager.h>
#include <TEveScene.h>
#include <TEveText.h>
#include <TEveTrans.h>
#include <TEveViewer.h>
#include <TGLViewer.h>
#include <TGTab.h>
#include <TGeoManager.h>
#include <TGeoMaterial.h>
#include <TGeoNode.h>
#include <TObject.h>
#include <TPad.h>
#include <TVector3.h>

#include "TRestEventViewer.h"

#define GEOM_SCALE 0.1

class TRestEveEventViewer : public TRestEventViewer {
   protected:
    TEveManager* gEve;

    TEveWindowSlot* slot;
    TEveWindowPack* pack;

    TEveViewer* viewer3D;
    TEveViewer* rphiViewer;
    TEveViewer* rhozViewer;

    TEveScene* rphiScene;
    TEveScene* rhozScene;

    TEveProjectionManager* rphi;
    TEveProjectionManager* rhoz;

    TEveProjectionAxes* rphiAxes;
    TEveProjectionAxes* rhozAxes;

    TEvePointSet* fEnergyDeposits;

    char pointName[256];

    Double_t fMinRadius = 0.2;
    Double_t fMaxRadius = 3.0;

   public:
    virtual void Initialize();

    virtual void Embed(TBrowser* b) {}

    virtual void DeleteCurrentEvent();
    void DeleteGeometry();

    virtual void AddEvent(TRestEvent* ev) = 0;

    virtual void Plot(const char* option) {}

    void AddSphericalHit(double x, double y, double z, double radius, double en);

    void MultiView();
    void DrawTab();
    void SetGeometry(TGeoManager* geo);
    void Update();

    void SetMinRadious(Double_t rmin) { fMinRadius = rmin; }
    void SetMaxRadious(Double_t rmax) { fMaxRadius = rmax; }

    // Constructor
    TRestEveEventViewer();
    // Destructor
    virtual ~TRestEveEventViewer();

    ClassDef(TRestEveEventViewer, 1);  // class inherited from TRestEventViewer
};
#endif
