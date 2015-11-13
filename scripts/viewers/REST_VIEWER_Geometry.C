//#include <TObject.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TBranch.h>

#include <iostream>
using namespace std;


Int_t REST_VIEWER_Geometry( TString fName)
{

    TEveManager::Create();
   
    cout << "Filename : " << fName << endl;

    
    TRestRun *run = new TRestRun( );
    run->OpenInputFile(fName);
        
    TGeoManager *geo = run->GetGeometry();
        
    TGeoNode* node = geo->GetTopNode();
    for(int i=0;i< geo->GetNNodes();i++)
    geo->GetVolume(i)->SetTransparency(50);
    
    geo->CheckOverlaps(0.0000001);
    geo->PrintOverlaps();
    
    TEveGeoTopNode* vol =new TEveGeoTopNode(geo, node);
    
    vol->SetVisLevel(3);
    
    gEve->AddGlobalElement(vol);
    
    gEve->FullRedraw3D(kTRUE);
    
    TGLViewer *v = gEve->GetDefaultGLViewer();
    v->GetClipSet()->SetClipType(1);
    v->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
    v->SetStyle(TGLRnrCtx::kOutline);
    v->RefreshPadEditor(v);

    //v->CurrentCamera().RotateRad(-.7, 0.5);
    v->DoDraw();
    
    
    /////////////////////////////

}
