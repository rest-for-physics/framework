#include <iostream>

#include "TRestBrowser.h"
#include "TRestTask.h"
using namespace std;

#ifndef RestTask_ViewGeometry
#define RestTask_ViewGeometry

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Geant4_ViewGeometry(TString fName, TString option = "") {
    cout << "Filename : " << fName << endl;

    TGeoManager* geo = NULL;
    if (TRestTools::isRootFile((string)fName)) {
        TFile* fFile = new TFile(fName);

        TIter nextkey(fFile->GetListOfKeys());
        TKey* key;
        while ((key = (TKey*)nextkey()) != NULL) {
            if (key->GetClassName() == (TString) "TGeoManager") {
                if (geo == NULL)
                    geo = (TGeoManager*)fFile->Get(key->GetName());
                else if (key->GetName() == (TString) "Geometry")
                    geo = (TGeoManager*)fFile->Get(key->GetName());
            }
        }
    } else if (((string)fName).find(".gdml") != string::npos) {
        TRestGDMLParser* p = new TRestGDMLParser();
        p->Load((string)fName);
        geo = p->CreateGeoM();
    } else {
        cout << "File is not supported!" << endl;
    }

    if (option == "") {
        geo->GetMasterVolume()->Draw();
    } else if (ToUpper((string)option) == "EVE") {
        TRestEventViewer* view = (TRestEventViewer*)REST_Reflection::Assembly("TRestGeant4EventViewer");
        if (view == NULL) return -1;
        view->SetGeometry(geo);
        view->AddEvent(new TRestGeant4Event());

        // TEveManager::Create();

        // TGeoNode* node = geo->GetTopNode();

        // TObjArray* arr = geo->GetListOfVolumes();
        // Int_t nVolumes = arr->GetEntries();
        // for (int i = 0; i < nVolumes; i++) geo->GetVolume(i)->SetTransparency(50);

        // geo->CheckOverlaps(0.0000001);
        // geo->PrintOverlaps();

        // TEveGeoTopNode* vol = new TEveGeoTopNode(geo, node);

        // vol->SetVisLevel(3);

        // gEve->AddGlobalElement(vol);

        // gEve->FullRedraw3D(kTRUE);

        // TGLViewer* v = gEve->GetDefaultGLViewer();
        // v->GetClipSet()->SetClipType((TGLClip::EType)1);
        // v->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
        // v->SetStyle(TGLRnrCtx::kOutline);
        // v->RefreshPadEditor(v);

        //// v->CurrentCamera().RotateRad(-.7, 0.5);
        // v->DoDraw();
    }
// when we run this macro from restManager from bash,
// we need to call TRestMetadata::GetChar() to prevent returning,
// while keeping GUI alive.
#ifdef REST_Geant4_MANAGER
    TRestRun* run = new TRestRun();
    GetChar("Running...\nPress a key to exit");
#endif

    return 0;
}

#endif
