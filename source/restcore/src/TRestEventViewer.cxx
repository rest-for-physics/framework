///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEventViewer.cxx
///
///             Base geometry class used to visualize events
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 J. Galan
///_______________________________________________________________________________


#include "TRestEventViewer.h"


ClassImp(TRestEventViewer)
//______________________________________________________________________________
    TRestEventViewer::TRestEventViewer()
{
   // TRestEventViewer default constructor
   Initialize();
}

//______________________________________________________________________________
TRestEventViewer::~TRestEventViewer()
{
    // TRestEventViewer destructor
    DeleteCurrentEvent(  );
    DeleteGeometry( );
    
    //delete slot;
    //gEve->GetBrowser()->CloseTabs();
    //delete gEve;
    
}

void TRestEventViewer::Initialize()
{
    nDeposits = 0;
    
    gEve = TEveManager::Create();
    gEve->GetBrowser()->DontCallClose();
    fHitConnectors.clear();
    fHitConnectors.push_back( NULL );
    
    gEve->AddEvent(new TEveEventManager("Event", "Event"));
    
    isMultiView=kFALSE;
    
    //MultiView( );
    
}

void TRestEventViewer::AddGeometry( TGeoManager *geo ){

    for(int i=0;i< geo->GetNNodes();i++)
    geo->GetVolume(i)->SetTransparency(85);
    
    TEveGeoTopNode* en = new TEveGeoTopNode( geo, geo->GetTopNode());

    gEve->AddGlobalElement(en);
    
    viewer3D->AddScene(gEve->GetGlobalScene());
    
}

void TRestEventViewer::AddGeometry( TString fName ){

    TGeoManager *geo =gEve->GetGeometry(fName.Data());
    AddGeometry(geo);
    
}


void TRestEventViewer::AddSphericalHit( double x, double y, double z, double radius )
{
    TEvePointSet* ps = new TEvePointSet();
    ps->SetOwnIds(kTRUE);
    ps->SetNextPoint(x*GEOM_SCALE,y*GEOM_SCALE,z*GEOM_SCALE);
    sprintf(pointName,"Point %d",nDeposits);
    ps->SetElementName(pointName);
    ps->SetMarkerColor(kYellow);
    ps->SetMarkerSize(radius);
    ps->SetMarkerStyle(4);
    
    gEve->AddElement( ps);
    
    nDeposits++;
}

void TRestEventViewer::AddSphericalHit( double x, double y, double z, double radius, double en )
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
    
    nDeposits++;
}



void TRestEventViewer::AddText( TString text, TVector3 at )
{
    TEveText *evText = new TEveText( text );
    evText->SetName( "Event title" );
    evText->SetFontSize(16);
    evText->RefMainTrans().SetPos( (at.X()+15)*GEOM_SCALE, (at.Y()+15)*GEOM_SCALE, (at.Z()+15)*GEOM_SCALE );

    gEve->AddElement( evText );
}

void TRestEventViewer::AddMarker( TVector3 at, TString name )
{
    TEvePointSet *marker = new TEvePointSet(1);
    marker->SetName( name );
    marker->SetMarkerColor( kMagenta );
    marker->SetMarkerStyle( 3 );
    marker->SetPoint( 0, at.X()*GEOM_SCALE, at.Y()*GEOM_SCALE, at.Z()*GEOM_SCALE );
    marker->SetMarkerSize(0.2);

    gEve->AddElement( marker );
}

void TRestEventViewer::AddMarker( Int_t trkID, TVector3 at, TString name )
{
    TEvePointSet *marker = new TEvePointSet(1);
    marker->SetName( name );
    marker->SetMarkerColor( kMagenta );
    marker->SetMarkerStyle( 3 );
    marker->SetPoint( 0, at.X()*GEOM_SCALE, at.Y()*GEOM_SCALE, at.Z()*GEOM_SCALE );
    marker->SetMarkerSize(0.2);
    fHitConnectors[trkID]->AddElement( marker );
}

void TRestEventViewer::NextTrackVertex( Int_t trkID, TVector3 to )
{

    fHitConnectors[trkID]->SetNextPoint( to.X()*GEOM_SCALE, to.Y()*GEOM_SCALE, to.Z()*GEOM_SCALE );
}

void TRestEventViewer::AddTrack( Int_t trkID, Int_t parentID, TVector3 from, TString name )
{
    TEveLine *evLine = new TEveLine( );
    evLine->SetName( name );
    fHitConnectors.push_back( evLine );

    fHitConnectors[trkID]->SetMainColor( kWhite );
    fHitConnectors[trkID]->SetLineWidth( 2 );

    if( name.Contains("gamma") ) fHitConnectors[trkID]->SetMainColor( kGreen );
    if( name.Contains("e-") ) fHitConnectors[trkID]->SetMainColor( kRed );


    fHitConnectors[trkID]->SetNextPoint( from.X()*GEOM_SCALE, from.Y()*GEOM_SCALE, from.Z()*GEOM_SCALE );

    fHitConnectors[parentID]->AddElement( fHitConnectors[trkID] );

}

void TRestEventViewer::AddParentTrack( Int_t trkID, TVector3 from, TString name )
{
    TEveLine *evLine = new TEveLine( );
    evLine->SetName( name );
    fHitConnectors.push_back( evLine );

    fHitConnectors[trkID]->SetMainColor( kWhite );
    fHitConnectors[trkID]->SetNextPoint( from.X()*GEOM_SCALE, from.Y()*GEOM_SCALE, from.Z()*GEOM_SCALE );

    gEve->AddElement( fHitConnectors[trkID] );
}

void TRestEventViewer::AddPoints( Int_t npoints, TVector3 at[ ] , Double_t radius){

TEvePointSet* ps = new TEvePointSet(npoints);
ps->SetOwnIds(kTRUE);

for(int n=0;n<npoints;n++)ps->SetNextPoint(at[n].X()*GEOM_SCALE,at[n].Y()*GEOM_SCALE,at[n].Z()*GEOM_SCALE);

    ps->SetMarkerColor(kYellow);
    ps->SetMarkerSize(radius);
    ps->SetMarkerStyle(4);
    
    gEve->AddElement( ps);
}


void TRestEventViewer::Draw( )
{
         
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


void TRestEventViewer::DeleteCurrentEvent(  ){

   cout<<"Removing event"<<endl;
   gEve->GetViewers()->DeleteAnnotations();
   gEve->GetCurrentEvent()->DestroyElements();
       
   fHitConnectors.clear();
   fHitConnectors.push_back( NULL );
   
   nDeposits=0;
}

void TRestEventViewer::DeleteGeometry(  ){

   cout<<"Removing geometry"<<endl;
   gEve->GetGlobalScene()->DestroyElements();
   
}

void TRestEventViewer::AddProjectedEvent( ){

	if(isMultiView){
	rphi->ImportElements( gEve->GetCurrentEvent());
	rhoz->ImportElements( gEve->GetCurrentEvent());
	}

}

void TRestEventViewer::MultiView( ){


    slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    
    pack = slot->MakePack();
    pack->SetElementName(this->GetName());
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

isMultiView=kTRUE;

}

void TRestEventViewer::Update( ){

	if(isMultiView){
	rphiViewer->Redraw(kTRUE);
	rhozViewer->Redraw(kTRUE);
	}


}

