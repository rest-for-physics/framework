#include "DetectorConstruction.hh"

#include "G4SystemOfUnits.hh"
#include "G4Isotope.hh"
#include "G4IonTable.hh"
#include "G4Material.hh"
#include <G4UserLimits.hh>

extern TRestG4Event *restG4Event;
extern TRestG4Metadata *restG4Metadata;

//_____________________________________________________________________________
DetectorConstruction::DetectorConstruction()
{

    G4cout << "Detector Construction" << G4endl;

    parser = new G4GDMLParser();

}
//_____________________________________________________________________________
DetectorConstruction::~DetectorConstruction()
{
    delete parser;
}
//_____________________________________________________________________________
G4VPhysicalVolume* DetectorConstruction::Construct()
{
    cout << "Isotope table " << endl;
    cout << *(G4Isotope::GetIsotopeTable()) << endl;
     
    G4cout << "Producing geometry" << G4endl;

    // Reading the geometry
    TString geometryFile = restG4Metadata->Get_GDML_Filename();

    char originDirectory[255];
    sprintf( originDirectory, "%s", get_current_dir_name() );

    char buffer[255];
    sprintf( buffer, "%s", (char *) restG4Metadata->GetGeometryPath().Data() );
    chdir( buffer );

    parser->Read( (string) geometryFile, false );

    chdir( originDirectory );


    G4VPhysicalVolume *W = parser->GetWorldVolume();

    // TODO : Take the name of the sensitive volume and use it here to define its StepSize
    G4LogicalVolume *gas = parser->GetVolume("gasVolume");
    G4Material *mat = gas->GetMaterial();
    gas->SetUserLimits( new G4UserLimits( restG4Metadata->GetMaxTargetStepSize() * mm ) );
    G4cout << "Gas properties" << G4endl;
    G4cout << "==============" << G4endl;
    G4cout << "Gas name : " << mat->GetName() << G4endl;
    G4cout << "Gas temperature : " << mat->GetTemperature() << G4endl;
    G4cout << "Gas density : " << mat->GetDensity()/(g/cm3) << " g/cm3" <<  G4endl;

    // Getting generation volume
    string volume = (string) restG4Metadata->GetGeneratedFrom();
    cout << "Generated from volume : " << volume << endl;
    string type = (string) restG4Metadata->GetGeneratorType();
    cout << "Generator type : " << type << endl;


    // TODO if we do not find the volume given in the config inside the geometry we should RETURN error
    if( type == "volume" && volume != "Not defined" )
    {
        G4VPhysicalVolume *pVol = GetPhysicalVolume( volume );

        if( pVol == NULL ) { cout << "ERROR : The generator volume was not found in the geometry" << endl; exit(1);  return W; }

        fGeneratorTranslation = pVol->GetTranslation();

        // We set in TRestG4Metadata the center of the generator. If it is a point we just want the value from the config file.
        // TODO : make this kind of keyword comparisons case insensitive?
        if( type == "surface" || type == "volume" )
        {
            restG4Metadata->SetGeneratorPosition( fGeneratorTranslation.x(), fGeneratorTranslation.y(), fGeneratorTranslation.z() );
        }

        generatorSolid = pVol->GetLogicalVolume()->GetSolid();

        //while ( fDetector->GetGeneratorSolid()->Inside( G4ThreeVector( x, y, z) ) != kInside );

        // We estimate the maximum distance of our volume
        // The code below returns a value bigger than expected
        // If we try with a cylinder the maximum distance should be sqrt(R*R+L*L)
        // But the value returned by this is bigger TODO check this
        boundBox_xMax = -1.e30;
        boundBox_yMax = -1.e30;
        boundBox_zMax = -1.e30;
        boundBox_xMin = 1.e30;
        boundBox_yMin = 1.e30;
        boundBox_zMin = 1.e30;
        if( type == "volume" )
        {
            cout << "Optimizing REST volume generation (Please wait. This might take few minutes depending on geometry complexity) " << flush;

            for( int n = 0; n < 100000; n++ )
            {
                G4ThreeVector point = generatorSolid->GetPointOnSurface();

                if( point.x() > boundBox_xMax ) boundBox_xMax = point.x(); 
                if( point.y() > boundBox_yMax ) boundBox_yMax = point.y(); 
                if( point.z() > boundBox_zMax ) boundBox_zMax = point.z(); 

                if( point.x() < boundBox_xMin ) boundBox_xMin = point.x(); 
                if( point.y() < boundBox_yMin ) boundBox_yMin = point.y(); 
                if( point.z() < boundBox_zMin ) boundBox_zMin = point.z(); 
            }

            boundBox_xMin = boundBox_xMin * 1.1;
            boundBox_xMax = boundBox_xMax * 1.1;

            boundBox_yMin = boundBox_yMin * 1.1;
            boundBox_yMax = boundBox_yMax * 1.1;

            boundBox_zMin = boundBox_zMin * 1.1;
            boundBox_zMax = boundBox_zMax * 1.1;

        }
    }

    for( int id = 0; id < restG4Metadata->GetNumberOfActiveVolumes(); id++ )
    {
        G4VPhysicalVolume *pVol = GetPhysicalVolume( (G4String) restG4Metadata->GetActiveVolumeName( id )  );

        cout << "Activating volume : " << restG4Metadata->GetActiveVolumeName( id ) << endl;
        restG4Event->AddActiveVolume( );
        if( pVol == NULL )
        {
            cout << "REST Warning : " <<  restG4Metadata->GetActiveVolumeName( id ) << " is not defined in the geometry" << endl;
            exit(1);

        }

    }


    cout << "Detector construced : " << W << endl;

    return W;
}
//_____________________________________________________________________________
G4VPhysicalVolume *DetectorConstruction::GetPhysicalVolume( G4String physVolName )
{
    G4PhysicalVolumeStore *physVolStore = G4PhysicalVolumeStore::GetInstance();
    std::vector<G4VPhysicalVolume*>::const_iterator physVol;
    for( physVol = physVolStore->begin(); physVol != physVolStore->end(); physVol++ )
    {
        if ((*physVol)->GetName() == physVolName)
        { 
            return *physVol;
        }
    }

    return NULL;
}
