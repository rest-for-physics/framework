///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEveEventViewer.cxx
///
///             nov 2015:   First concept
///                 Generic class for visualization of simulated events using
///                 gEveManager JuanAn/Javier Galan
///_______________________________________________________________________________

#include "TRestEveEventViewer.h"

#include "TRestStringOutput.h"

using namespace std;

ClassImp(TRestEveEventViewer);

TRestEveEventViewer::TRestEveEventViewer() {
    Initialize();
    fEnergyDeposits = new TEvePointSet();
    fEnergyDeposits->SetElementName("Energy deposits");

    RESTWarning << "There are some issues with geometry representation in Eve 3D scenes!" << RESTendl;
    RESTWarning
        << "We use a geometry scaling factor to place the hits in the scene and correct a placement problem"
        << RESTendl;
    RESTWarning << "Presently the default value of fGeomScale is " << fGeomScale << RESTendl;
    RESTWarning << "Please, report to the dev team if you experience problems visualizing the geometry."
                << RESTendl;
    RESTWarning << "For example: when hit positions seem to do not match geometry positions" << RESTendl;

    RESTWarning << " " << RESTendl;
    RESTWarning << "You may try to fix this using TRestEventViewer::SetGeomScale(1.0);" << RESTendl;
}

TRestEveEventViewer::~TRestEveEventViewer() {
    delete fEnergyDeposits;
    // TRestEveEventViewer destructor
    DeleteCurrentEvent();
    DeleteGeometry();
}

void InitializePerspectiveView() {
    const std::map<TString, TGLViewer::ECameraType> projectionsMap = {
        {"Projection XY", TGLViewer::kCameraOrthoXOY},  //
        {"Projection XZ", TGLViewer::kCameraOrthoXnOZ},  //
        {"Projection YZ", TGLViewer::kCameraOrthoZOY}    //
    };
    for (const auto& [name, cameraType] : projectionsMap) {
        auto slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
        auto pack = slot->MakePack();
        pack->SetElementName(name);
        pack->SetHorizontal();
        pack->SetShowTitleBar(kFALSE);
        pack->NewSlot()->MakeCurrent();
        auto f3DView = gEve->SpawnNewViewer("3D View", pack->GetName());
        f3DView->AddScene(gEve->GetGlobalScene());
        f3DView->AddScene(gEve->GetEventScene());
        f3DView->GetGLViewer()->SetCurrentCamera(cameraType);
    }
}

void TRestEveEventViewer::Initialize() {
    gEve = TEveManager::Create();
    gEve->GetBrowser()->DontCallClose();

    gEve->AddEvent(new TEveEventManager("Event", "Event"));

    auto singleViewer = gEve->GetDefaultViewer();
    singleViewer->SetElementName("SingleView");

    singleViewer->GetGLViewer()->SetClearColor(kBlack);

    InitializePerspectiveView();
    MultiView();
    DrawTab();

    singleViewer->GetGLViewer()->SetStyle(TGLRnrCtx::kOutline);

    gEve->GetBrowser()->GetTabRight()->SetTab(0);  // select the "SingleView" viewer as active

    singleViewer->GetGLViewer()->CurrentCamera().Reset();
    gEve->FullRedraw3D(kTRUE);
}

void TRestEveEventViewer::SetGeometry(TGeoManager* geo) {
    TRestEventViewer::SetGeometry(geo);

    if (fGeometry == nullptr) {
        return;
    }

    const unsigned int nVolumes = fGeometry->GetListOfVolumes()->GetEntries();
    double transparencyLevel = 85;
    if (nVolumes > 30) {
        transparencyLevel = 90;
    }
    for (int i = 0; i < fGeometry->GetListOfVolumes()->GetEntries(); i++) {
        auto volume = fGeometry->GetVolume(i);
        auto material = volume->GetMaterial();
        if (material->GetDensity() <= 0.01) {
            volume->SetTransparency(99);
            if (material->GetDensity() <= 0.001) {
                // We consider this vacuum for display purposes
                volume->SetVisibility(kFALSE);
            }
        } else {
            volume->SetTransparency(transparencyLevel);
        }
    }

    TEveGeoTopNode* eveGeoTopNode = new TEveGeoTopNode(fGeometry, fGeometry->GetTopNode());
    eveGeoTopNode->SetVisLevel(5);

    gEve->AddGlobalElement(eveGeoTopNode);

    viewer3D->AddScene(gEve->GetGlobalScene());
}

void TRestEveEventViewer::DeleteCurrentEvent() {
    cout << "Removing event" << endl;
    delete fEnergyDeposits;
    fEnergyDeposits = new TEvePointSet();
    fEnergyDeposits->SetElementName("Energy deposits");
    gEve->GetViewers()->DeleteAnnotations();
    gEve->GetCurrentEvent()->DestroyElements();
}

void TRestEveEventViewer::DeleteGeometry() {
    cout << "Removing geometry" << endl;
    gEve->GetGlobalScene()->DestroyElements();
}

void TRestEveEventViewer::MultiView() {
    slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());

    pack = slot->MakePack();
    pack->SetElementName("MultiView");
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

    rphiScene = gEve->SpawnNewScene("RPhi");
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

    rhozScene = gEve->SpawnNewScene("RhoZ");
    rhozViewer->AddScene(rhozScene);

    rhoz = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
    rhozScene->AddElement(rhoz);

    rhozAxes = new TEveProjectionAxes(rhoz);
    rhozScene->AddElement(rhozAxes);
}

void TRestEveEventViewer::DrawTab() {
    TGLViewer* v2 = viewer3D->GetGLViewer();
    v2->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, nullptr);
    v2->SetStyle(TGLRnrCtx::kOutline);
}

void TRestEveEventViewer::Update() {
    gEve->AddElement(fEnergyDeposits);
    rphi->ImportElements(gEve->GetCurrentEvent());
    rhoz->ImportElements(gEve->GetCurrentEvent());

    rphiViewer->Redraw(kTRUE);
    rhozViewer->Redraw(kTRUE);
}

void TRestEveEventViewer::AddSphericalHit(double x, double y, double z, double radius, double en) {
    fEnergyDeposits->SetOwnIds(kTRUE);
    fEnergyDeposits->SetNextPoint(x * fGeomScale, y * fGeomScale, z * fGeomScale);
    fEnergyDeposits->SetMarkerColor(kYellow);
    fEnergyDeposits->SetMarkerSize(Size_t(radius));
    fEnergyDeposits->SetMarkerStyle(4);
}
