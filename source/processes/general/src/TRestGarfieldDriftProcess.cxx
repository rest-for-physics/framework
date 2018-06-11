///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             Process which create ionization electrons and drift them toward
///             the readout plane (stop 1mm above), giving output timed hit events
///
///             TRestGarfieldDriftProcess.cxx
///
///             march 2017:   Damien Neyret
///_______________________________________________________________________________


#include <TRandom3.h>
#include <TGeoBBox.h>

#include "TRestGarfieldDriftProcess.h"

#if defined USE_Garfield
#include "TGDMLParse.h"
#include "ComponentConstant.hh"
using namespace Garfield;
#endif

#include <stdio.h>

using namespace std;



const double cmTomm = 10.;

ClassImp(TRestGarfieldDriftProcess)

    //______________________________________________________________________________
#if defined USE_Garfield
    TRestGarfieldDriftProcess::TRestGarfieldDriftProcess() : fRandom(0), fGfSensor(0)  {

        Initialize();

    }

// __________________________________________________________
//     TODO : Perhaps this constructor should be removed
//            since we will always load the config from TRestRun
//            when we use AddProcess. It would be necessary only if we use the process stand alone
//            but even then we could just call LoadConfig
//            __________________________________________________________

TRestGarfieldDriftProcess::TRestGarfieldDriftProcess( char *cfgFileName ) : fRandom(0), fGfSensor(0) 
{
    Initialize();

    if ( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );

    PrintMetadata();

    if ( fReadout == NULL ) fReadout = new TRestReadout( cfgFileName );

    // TRestGarfieldDriftProcess default constructor
}


//______________________________________________________________________________

// TRestGarfieldDriftProcess destructor
TRestGarfieldDriftProcess::~TRestGarfieldDriftProcess()
{
    if( fReadout != NULL ) delete fReadout;
    if (fGeometry) delete fGeometry;
    fGeometry = NULL;

    delete fInputHitsEvent;
    delete fOutputHitsEvent;
}


void TRestGarfieldDriftProcess::LoadDefaultConfig( ) 
{
    SetName( "garfieldDriftProcess-Default" );
    SetTitle( "Default config" );

    cout << "TRestGarfieldDriftProcess metadata not found. Loading default values" << endl;

    fDriftPotential = 1000;
    fGasPressure = 10;
}


void TRestGarfieldDriftProcess::LoadConfig( string cfgFilename, string name ) 
{
    if ( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );

    if ( fDriftPotential == PARAMETER_NOT_FOUND_DBL ) {
        fDriftPotential = 1000; // V
    }
}

//______________________________________________________________________________
#endif


void TRestGarfieldDriftProcess::Initialize() 
{
    SetSectionName( ClassName() );

    fRandom = new TRandom3(0);
	fInputHitsEvent = new TRestHitsEvent();
	fOutputHitsEvent = new TRestHitsEvent();

#if defined USE_Garfield
    fReadout = NULL;
    fGas = NULL;
    fGeometry= NULL;
    fPEReduction = 1.;
    fStopDistance = 2; // default distance from readout to stop drift set to 2mm



    fInputEvent = fInputHitsEvent;
    fOutputEvent = fOutputHitsEvent;
#endif
}


//______________________________________________________________________________
#if defined USE_Garfield
void TRestGarfieldDriftProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();


    // Getting gas data
    fGas = (TRestGas *) this->GetGasMetadata( );
    if ( fGas != NULL )  
    {
        if ( fGasPressure <=0 ) 
            fGasPressure = fGas->GetPressure();
		else
			fGas->SetPressure( fGasPressure );

    } 
    else 
    {
        cout << "REST_WARNING. No TRestGas found in TRestRun." << endl;
    }

    // Getting readout data
    fReadout = (TRestReadout *) this->GetReadoutMetadata( );
    if ( fReadout == NULL ) 
    {
        cout << "REST ERRORRRR : Readout has not been initialized" << endl;
        exit(-1);
    }

    // Getting geometry data
    if (fGDML_Filename == "") 
    {
        cout << "REST ERROR no geometry GDML file name parameter (gdml_file) given in TRestGarfieldDriftProcess "<< endl;
        exit(-1);
    }

    register TGeoVolume *geovol = NULL;
    fGeometry = new TRestGeometry();
    fGeometry->InitGfGeometry();

    geovol = TGDMLParse::StartGDML(fGDML_Filename);
    if (!geovol) 
    {
        cout << "REST ERROR when TRestGeometry read GDML file "<<fGDML_Filename << endl;
        exit(-1);
    }

    fGeometry->SetTopVolume(geovol);
    fGeometry->CloseGeometry();
    fGeometry->DefaultColors();
    //         fGeometry->UpdateElements();

    cout<< "TRestGarfieldDriftProcess  GetVerboseLevel : " << this->GetVerboseLevel() << endl;

    // analyze GDML geometry to find major elements (gas volume, electrodes, readout)
    TObjArray* thenodes = geovol->GetNodes();
    for (TIter it = thenodes->begin(); it != thenodes->end(); ++it) 
    {
        register TGeoNode *itnode = (TGeoNode*)(*it);
        if ( GetVerboseLevel() >= REST_Info ) {
            cout << "****** itnode " << itnode->GetName() << endl;
            itnode->PrintCandidates();
        }
        cout << "****** itnode " << itnode->GetName() << endl;
        itnode->PrintCandidates();

        register TGeoVolume *itvol = itnode->GetVolume();
        if ( GetVerboseLevel() >= REST_Info ) {
            cout << "  *  *  itvolume " << itvol->GetName() << endl;
            itvol->Print();
        }
        cout << "  *  *  itvolume " << itvol->GetName() << endl;
        itvol->Print();

        register TGeoMedium *itmed = itvol->GetMedium();
        if ( GetVerboseLevel() >= REST_Info ) cout  <<  "  *  *  itmed "  <<  itmed->GetName( )  <<  endl;

        // gas volume
        if (fGas->GetGDMLMaterialRef() == itmed->GetName()) {
            fGeometry->SetGfGeoMedium(itmed->GetName(), fGas);
            if ( GetVerboseLevel() >= REST_Info ) {
                cout << "  -> gas volume   SetMedium itmed " << itmed->GetName() << "  fGas " << fGas->GetGasMedium()->GetName() << endl;
                fGas->GetGasMedium()->PrintGas();
            }
            cout << "  -> gas volume   SetMedium itmed " << itmed->GetName() << "  fGas " << fGas->GetGasMedium()->GetName() << endl;
            fGas->GetGasMedium()->PrintGas();
        }

        // drift electrode (it should be called a cathode, not anode, no ?)
        if ( (strncmp(itvol->GetName(), "anodeVol", 8) == 0) || (strncmp(itvol->GetName(), "cathodeVol", 10) == 0)) {
            fGeometry->SetDriftElecNode(itnode);
            if ( GetVerboseLevel() >= REST_Info ) cout << "  -> cathode volume " << endl;
            cout << "  -> cathode volume " << endl;
        }

        // micromegas readout electrode 
        if ( (strncmp(itvol->GetName(), "micromegasVol", 13) == 0) ) {
            fGeometry->AddReadoutElecNode(itnode);
            if ( GetVerboseLevel() >= REST_Info ) cout << "  -> readout volume " << endl;
            cout << "  -> readout volume " << endl;
        }

    }

    // For the moment we set only constant electric field in Garfield
    register double matrixZpos=0, planeZpos=0, planeZvec=0;
    cout << "fGeometry " << fGeometry << " nb node " << fGeometry->GetNReadoutElecNodes() << endl;
    for (register int ii=0; ii<fGeometry->GetNReadoutElecNodes(); ii++) {
        bool rdPlaneFound = false;

        TGeoNode* readoutnode = fGeometry->GetReadoutElecNode(ii);
        cout << "ii " << ii << " readoutnode " << readoutnode << " visible " << readoutnode->IsVisible() << endl;
        if (! readoutnode) continue;
        cout << "readoutnode visible " << readoutnode->IsVisible() << endl;
        readoutnode->ls(); readoutnode->PrintCandidates();
        TGeoMatrix* readoutmatrix = readoutnode->GetMatrix();
        if (! readoutmatrix) continue;
        cout << "readoutmatrix " << endl; readoutmatrix->Print();
        matrixZpos = ( readoutmatrix->IsTranslation() ? 10*readoutmatrix->GetTranslation()[2] : 0); // converted to mm
        for (register int jj=0; jj<fReadout->GetNumberOfReadoutPlanes(); jj++) {
            register TRestReadoutPlane* readoutplane = fReadout->GetReadoutPlane(jj);
            planeZpos = readoutplane->GetPosition().Z();
            cout << "    jj " << jj << " matrixZpos " << matrixZpos << " planeZpos " << planeZpos << endl;
            if (fabs(planeZpos - matrixZpos) > 1) continue;  // we search for fReadout entry at same Z position
            rdPlaneFound = true;
            planeZvec = readoutplane->GetPlaneVector().z();
            cout << "     planeZvec " << planeZvec << endl;
        }

        if (rdPlaneFound) {
            register double field = fDriftPotential / (fGeometry->GetDriftElecNode()->GetMatrix()->GetTranslation()[2] - planeZpos);
            ComponentConstant* cmp = new ComponentConstant();
            //         cmp->SetElectricField(0, 0, fElectricField);  // assuming V/cm
            cmp->SetElectricField(0, 0, field);  // assuming V/cm
            cmp->SetPotential(0, 0, planeZpos/10., 0);  // potential at readout level set to 0
            cmp->SetWeightingField(0, 0, planeZvec, "m");
            fGeometry->AddGfComponent(cmp);
            cout << "add component field " << field << " planeZpos " << planeZpos << " planeZvec " << planeZvec
                 << " drift pos " << fGeometry->GetDriftElecNode()->GetMatrix()->GetTranslation()[2] << endl;
        }
    }

    // create Sensor corresponding to readout plane
    // only the first component used, so 1 sensor only for the moment
    if ( fGeometry->GetNbOfGfComponent() > 0) {
        fGfSensor = new Sensor();
        fGfSensor->AddComponent(fGeometry->GetGfComponent(0));
        fGfSensor->AddElectrode(fGeometry->GetGfComponent(0), "m");
        fGfSensor->SetTimeWindow(0., 0.1, 500);

        double xmin = -1e9, xmax = 1e9, ymin = -1e9, ymax = 1e9;
        TGeoShape* readoutshape = fGeometry->GetReadoutElecNode(0)->GetVolume()->GetShape();
        TGeoMatrix* readoutmatrix = fGeometry->GetReadoutElecNode(0)->GetMatrix();
        double xmid = 10.*readoutmatrix->GetTranslation()[0], ymid = 10.*readoutmatrix->GetTranslation()[1];
        if (readoutshape->InheritsFrom("TGeoBBox")) {
            // keep drifting electrons 10cm around the readout volume in x and y
            TGeoBBox* readoutbox = (TGeoBBox*) readoutshape;
            xmin = xmid - 10.*readoutbox->GetDX() - 100; xmax = xmid + 10.*readoutbox->GetDX() + 100;
            ymin = ymid - 10.*readoutbox->GetDY() - 100; ymax = ymid + 10.*readoutbox->GetDY() + 100;
        }
        register double driftelecZpos = fGeometry->GetDriftElecNode()->GetMatrix()->GetTranslation()[2]; // hope that all these objects are really there...
        // drift area defined from bouding box of readout shape
        fGfSensor->SetArea(xmin/10., ymin/10., planeZpos/10.+fStopDistance, xmax/10., ymax/10., driftelecZpos); // drift stops fStopDistance above the readout plane
        fGeometry->AddGfSensor(fGfSensor);
        printf("  GfSensor area x- %lf y- %lf z- %lf x+ %lf y+ %lf z+ %lf\n", xmin/10., ymin/10., planeZpos/10. + 0.2, xmax/10., ymax/10., driftelecZpos);

        fGfDriftMethod = new DRIFT_METHOD();
        //       fGfDriftMethod->EnableDebugging();
        //       fGfDriftMethod->EnableVerbose();
        fGfDriftMethod->SetSensor(fGfSensor);
        //       fGfDriftMethod->SetTimeSteps(0.2);
        fGfDriftMethod->SetCollisionSteps(100);
    }

    if ( !fGfDriftMethod || !fGfSensor ) {
        cout  <<  "REST ERROR Garfield not fully initialized in TRestGarfieldDriftProcess::InitProcess " << endl;
        cout  <<  "  fGfDriftMethod " << fGfDriftMethod << " fGfSensor " << fGfSensor << " GetNbOfGfComponent " << fGeometry->GetNbOfGfComponent() << endl;
        cout  <<  " matrixZpos " << matrixZpos << " planeZpos " << planeZpos << " planeZvec " << planeZvec << endl;
        exit(-1);
    }


    // for (register double ix=-3000; ix<3000; ix+=10) {
    // printf("  ix %lf medium %p is_inside %d", ix, fGeometry->GetGfGeometry()->GetMedium(ix/10.,5,0),
    //               fGeometry->GetGfGeometry()->IsInside(ix/10.,5,0));
    //   cout << " moduleId " << fReadout->GetReadoutPlane(0)->isInsideDriftVolume( ix, 50, 0 );
    // fGeometry->SetCurrentPoint(ix/10., 5,0);
    //   if (fGeometry->IsOutside()) cout << " is outside " << endl; else cout << " is inside" << endl;
    //   TGeoNode* cnode =fGeometry ->GetCurrentNode();
    // TGeoNode* cnode2= fGeometry->FindNode(ix/10., 5,0);
    //   std::string name(cnode->GetMedium()->GetMaterial()->GetName());
    //   register Medium* tmed = fGeometry->GetGfMedium(ix/10., 5,0);
    //   cout << "    node " << cnode->GetName() << " material " << name;
    //   cout << " node2 " << cnode2->GetName() << " material " << cnode2->GetMedium()->GetMaterial()->GetName();
    //   if (tmed) cout << " medium " << tmed->GetName();
    //   cout << endl;
    // }

}


//______________________________________________________________________________

void TRestGarfieldDriftProcess::BeginOfEventProcess() {
    //cout  <<  "Begin of event process"  <<  endl;
    fOutputHitsEvent->Initialize(); 
}


//------------------------------------------------------------------------------

Int_t TRestGarfieldDriftProcess::FindModule( Int_t readoutPlane, Double_t x, Double_t y ) {
    // TODO verify this
    TRestReadoutPlane *plane = fReadout->GetReadoutPlane( readoutPlane );
    for ( int md = 0; md < plane->GetNumberOfModules(); md++ )
        if( plane->GetReadoutModule( md )->isInside( x, y ) ) return md;

    return -1;
}


//______________________________________________________________________________
#endif


TRestEvent* TRestGarfieldDriftProcess::ProcessEvent( TRestEvent *evInput ) {

#if defined USE_Garfield
    fInputHitsEvent = (TRestHitsEvent *) evInput;

    double x, y, z, energy;
    double xi, yi, zi, ti, xf, yf, zf, tf, energyf;
    int status;

    if ( GetVerboseLevel() >= REST_Debug ) 
    {
        cout  <<  "Number of hits : "  <<  fInputHitsEvent->GetNumberOfHits()  <<  endl;
        cout  <<  "--------------------------"  <<  endl;
        fInputHitsEvent->PrintEvent(20);
    }

    for ( register int hit = 0; hit < fInputHitsEvent->GetNumberOfHits(); hit++ ) 
    {
        x = fInputHitsEvent->GetX( hit );
        y = fInputHitsEvent->GetY( hit );
        z = fInputHitsEvent->GetZ( hit );
        energy = fInputHitsEvent->GetEnergy( hit );
        //         if (hit%10==0) printf("hit: x %lf y %lf z %lf Energy %lf\n", x, y, z, energy);
        //         cout  <<  "hit: x "  <<  x  <<  " y "  <<  y  <<  " z "  <<  z  << " Energy " << energy <<  endl;

        // assuming energy in keV, W factor in eV
        register Medium* tmed = fGeometry->GetGfMedium(x,y,z);
        if (!tmed) {
            cout << "no medium found at x " << x << " y " << y << " z " << z << endl;
            continue; 
            //         } else {
            //           cout << "  medium " << tmed->GetName() << " W " << tmed->GetW() << " at x " << x << " y " << y << " z " << z << endl;
    }
    double Wfactor = tmed->GetW();
    register double nb_electrons_real = energy * 1000. / Wfactor / fPEReduction;
    register unsigned int nb_electrons = fRandom->Poisson(nb_electrons_real);  // we need an int number of electrons to drift...
    //         if (nb_electrons>0) printf("hit: x %lf y %lf z %lf Energy %lf\n", x, y, z, energy);
    //         if (nb_electrons>0) cout  <<  "   : energy " << energy << " nb_electrons_real " << nb_electrons_real << " nb_electrons " << nb_electrons << endl;

    // lets drift all those electrons
    for (register unsigned int iel = 0; iel < nb_electrons; iel++) {
        // drift an electron
        fGfDriftMethod->DriftElectron(x/10., y/10., z/10., 0);
        //           fGfDriftMethod->GetEndPoint(xf, yf, zf, tf, status);
        if ( fGfDriftMethod->GetNumberOfElectronEndpoints() == 0) {
            cout << " TRestGarfieldDriftProcess::ProcessEvent:  no electron end point ???" << endl;
            continue;
        }

        // create an hit for the drifted electron
        fGfDriftMethod->GetElectronEndpoint(0, xi, yi, zi, ti, xf, yf, zf, tf, status); // works with AvalancheMC
        // xi *= 10; yi *= 10; zi *= 10; // cm to mm
        xf *= cmTomm; yf *= cmTomm; zf *= cmTomm; // cm to mm

        tf /= 1000.;  // ns to us
        energyf = fPEReduction * Wfactor / 1000.;  // back to keV
        fOutputHitsEvent->AddHit( xf, yf, zf, energyf, tf );

        //           if (nb_electrons>0) cout  <<  "   : Nendpoint " << fGfDriftMethod->GetNumberOfElectronEndpoints() << " xi "  <<  xi  <<  " yi "  <<  yi  <<  " zi "  <<  zi  << " ti " << ti
        //                 << " xf "  <<  xf  <<  " yf "  <<  yf  <<  " zf "  <<  zf  << " tf " << tf << " status " << status ;

    }
    }

    if( fOutputHitsEvent->GetNumberOfHits() == 0 ) return NULL;

    //fSignalEvent->PrintEvent();

    if ( GetVerboseLevel() >= REST_Debug ) 
    {
        cout << "TRestElectronDiffusionProcess. Hits added : " << fOutputHitsEvent->GetNumberOfHits() << endl;
        cout << "TRestElectronDiffusionProcess. Hits total energy : " << fOutputHitsEvent->GetEnergy() << endl;
        cout << " fTimedHitsEvent " << fOutputHitsEvent << " class " << fOutputHitsEvent->ClassName() << endl;
        fOutputHitsEvent->PrintEvent(20);
    }

    return fOutputHitsEvent;
#else
	fInputHitsEvent = (TRestHitsEvent *)evInput;
	debug << "null process" << endl;
	return evInput;

#endif
}

//______________________________________________________________________________
#if defined USE_Garfield

void TRestGarfieldDriftProcess::EndOfEventProcess()  {

}

//______________________________________________________________________________

void TRestGarfieldDriftProcess::EndProcess() {
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________


void TRestGarfieldDriftProcess::InitFromConfigFile( ) {
    fGasPressure = StringToDouble( GetParameter( "gasPressure", "-1" ) );
    //     fElectricField = GetDblParameterWithUnits( "electricField" );
    fDriftPotential = GetDblParameterWithUnits( "driftPotential" );
    fPEReduction = StringToDouble( GetParameter( "primaryElectronReduction", "1" ) );
    fStopDistance = StringToDouble( GetParameter( "stopDistance", "2" ) );

    // TODO : Still units must be implemented for velocity quantities

    fGDML_Filename = GetParameter( "geometryPath", "" ) + GetParameter( "gdml_file", "" );
}

#endif