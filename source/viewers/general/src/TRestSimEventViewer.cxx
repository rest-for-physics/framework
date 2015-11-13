///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSimEventViewer.cxx
///
///             nov 2015:   First concept
///                 Generic class for visualization of simulated events using gEveManager
///                 JuanAn
///_______________________________________________________________________________

#include "TRestSimEventViewer.h"

ClassImp(TRestSimEventViewer)
//______________________________________________________________________________
TRestSimEventViewer::TRestSimEventViewer()
{
  Initialize();
}


//______________________________________________________________________________
TRestSimEventViewer::~TRestSimEventViewer()
{
   // TRestSimEventViewer destructor
   DeleteCurrentEvent( );
   DeleteGeometry( );
}

//______________________________________________________________________________
void TRestSimEventViewer::Initialize()
{
       
   gEve = TEveManager::Create();
   gEve->GetBrowser()->DontCallClose();
   
   fMinRadius = 1.5;
   fMaxRadius = 3.;
   
   gEve->AddEvent(new TEveEventManager("Event", "Event"));
   
   MultiView( );
   DrawTab();
   
}

void TRestSimEventViewer::SetGeometry(TGeoManager *geo){
    
    TRestEventViewer::SetGeometry(geo);
    
    if(fGeometry==NULL)return;
    
    for(int i=0;i< fGeometry->GetNNodes();i++)
    fGeometry->GetVolume(i)->SetTransparency(85);
    
    TEveGeoTopNode* en = new TEveGeoTopNode( fGeometry, fGeometry->GetTopNode());

    gEve->AddGlobalElement(en);
    
    viewer3D->AddScene(gEve->GetGlobalScene());
    
}

void TRestSimEventViewer::DeleteCurrentEvent(  ){

   cout<<"Removing event"<<endl;
   gEve->GetViewers()->DeleteAnnotations();
   gEve->GetCurrentEvent()->DestroyElements();
      
}

void TRestSimEventViewer::DeleteGeometry(  ){

   cout<<"Removing geometry"<<endl;
   gEve->GetGlobalScene()->DestroyElements();
   
}

void TRestSimEventViewer::MultiView( ){


    slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    
    pack = slot->MakePack();
    pack->SetElementName("Tab");
    pack->SetHorizontal();
    pack->SetShowTitleBar(kFALSE);
    pack->NewSlot()->MakeCurrent();
    
    viewer3D = gEve->SpawnNewViewer("3DView");
    viewer3D->AddScene(gEve->GetEventScene());
    
    pack = pack->NewSlot()->MakePack();
    pack->SetShowTitleBar(kFALSE);
    pack->NewSlot()->MakeCurrent();
    rphiViewer = gEve->SpawnNewViewer("RPhi View");
    rphiViewer->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    
    rphiScene  = gEve->SpawnNewScene("RPhi");
    rphiViewer->AddScene(rphiScene);
    
    rphi = new TEveProjectionManager(TEveProjection::kPT_RPhi);
    rphiScene->AddElement(rphi);
    
    rphiAxes = new TEveProjectionAxes(rphi);
    rphiScene->AddElement(rphiAxes);
    
    pack = pack->NewSlot()->MakePack();
    pack->SetShowTitleBar(kFALSE);
    pack->NewSlot()->MakeCurrent();
    rhozViewer = gEve->SpawnNewViewer("RhoZ View");
    rhozViewer->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    
    rhozScene  = gEve->SpawnNewScene("RhoZ");
    rhozViewer->AddScene(rhozScene);
    
    rhoz = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
    rhozScene->AddElement(rhoz);
    
    rhozAxes = new TEveProjectionAxes(rhoz);
    rhozScene->AddElement(rhozAxes);


}

void TRestSimEventViewer::DrawTab( ){
         
   gEve->FullRedraw3D(kTRUE);
   
   TGLViewer *v = gEve->GetDefaultGLViewer();
   //v->GetClipSet()->SetClipType(TGLClip::EType(1));
   v->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
   v->SetStyle(TGLRnrCtx::kOutline);
   v->RefreshPadEditor(v);

   //v->CurrentCamera().RotateRad(-1.2, 0.5);
   v->DoDraw();
   
   TGLViewer *v2 = viewer3D->GetGLViewer();
   v2->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
   v2->SetStyle(TGLRnrCtx::kOutline);
   v2->RefreshPadEditor(v);
   v2->DoDraw();
   
   gEve->GetBrowser()->GetTabRight()->SetTab(1);


}

void TRestSimEventViewer::Update( ){

rphi->ImportElements( gEve->GetCurrentEvent());
rhoz->ImportElements( gEve->GetCurrentEvent());

rphiViewer->Redraw(kTRUE);
rhozViewer->Redraw(kTRUE);

}


void TRestSimEventViewer::AddSphericalHit( double x, double y, double z, double radius, double en )
{
    TEvePointSet* ps = new TEvePointSet();
    ps->SetOwnIds(kTRUE);
    ps->SetNextPoint(x*GEOM_SCALE,y*GEOM_SCALE,z*GEOM_SCALE);
    sprintf(pointName,"Edep %.2lf",en);
    ps->SetElementName(pointName);
    ps->SetMarkerColor(kYellow);
    ps->SetMarkerSize(radius);
    ps->SetMarkerStyle(4);
    
    gEve->AddElement( ps);
    
}

