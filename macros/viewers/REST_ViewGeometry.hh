#include <iostream>
#include "TRestTask.h"
#include "TRestG4EventViewer.h"
#include "TRestBrowser.h"
using namespace std;

Int_t REST_Viewer_Geometry( TString fName)
{

    TEveManager::Create();
   
    cout << "Filename : " << fName << endl;
    
    TRestRun *run = new TRestRun( );
    run->OpenInputFile(fName);
        
    TFile *fFile = new TFile( fName );
    TGeoManager *geo = (TGeoManager *) fFile->Get( "Default" );

    TGeoNode* node = geo->GetTopNode();

    TObjArray *arr = geo->GetListOfVolumes();
    Int_t nVolumes = arr->GetEntries();
    for( int i=0; i< nVolumes; i++ )
        geo->GetVolume(i)->SetTransparency(50);
    
    geo->CheckOverlaps(0.0000001);
    geo->PrintOverlaps();
    
    TEveGeoTopNode* vol =new TEveGeoTopNode(geo, node);
    
    vol->SetVisLevel(3);
    
    gEve->AddGlobalElement(vol);
    
    gEve->FullRedraw3D(kTRUE);
    
    TGLViewer *v = gEve->GetDefaultGLViewer();
    v->GetClipSet()->SetClipType( (TGLClip::EType) 1);
    v->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
    v->SetStyle(TGLRnrCtx::kOutline);
    v->RefreshPadEditor(v);

    //v->CurrentCamera().RotateRad(-.7, 0.5);
    v->DoDraw();
    
    /////////////////////////////

    return 0;
}
