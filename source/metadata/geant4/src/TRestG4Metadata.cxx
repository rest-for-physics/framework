
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4Metadata.cxx
///
///             G4 class description
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#include "TRestG4Metadata.h"
using namespace std;

ClassImp(TRestG4Metadata)
//______________________________________________________________________________
TRestG4Metadata::TRestG4Metadata() : TRestMetadata()
{

   // TRestG4Metadata default constructor
   Initialize();
}


//______________________________________________________________________________
TRestG4Metadata::TRestG4Metadata( char *cfgFileName) : TRestMetadata (cfgFileName)
{

    Initialize();

    LoadConfigFromFile( fConfigFileName );

    PrintMetadata();
}


//______________________________________________________________________________
TRestG4Metadata::~TRestG4Metadata()
{
    // TRestG4Metadata destructor
}

void TRestG4Metadata::Initialize()
{
    SetName( "restG4" );
    cout << __PRETTY_FUNCTION__ << endl;

    fChance.clear();
    fActiveVolumes.clear();
    fBiasingVolumes.clear();

    fNBiasingVolumes = 0;

    fGeneratorFile = "";

    fPrimaryGenerator.Reset();

    fSensitiveVolume = "gas";

    fEnergyRangeStored.Set( 0, 1.E20 );
}

//______________________________________________________________________________
void TRestG4Metadata::InitFromConfigFile()
{
    cout << __PRETTY_FUNCTION__ << endl;

    this->Initialize();

    // Initialize the metadata members from a configfile
    fGDML_Filename = GetParameter( "gdml_file" );

    fGeometryPath = GetParameter( "geometryPath" );

    fMaxTargetStepSize = GetDblParameterWithUnits( "maxTargetStepSize" );

    ReadGenerator();

    ReadStorage();

    ReadBiasing();

    // should return sucess or fail

}


void TRestG4Metadata::ReadBiasing()
{
    string biasingString = GetKEYStructure( "biasing" );

    if ( biasingString == "NotFound" )
    {
        // Some problem in GetKEYDefinition (If we do not define biasing it will crash
        cout << "REST WARNNING : Biasing structure has not been defined" << endl;
        fNBiasingVolumes = 0;
        return;
    }

    string biasingDefinition = GetKEYDefinition( "biasing", biasingString );

    TString biasEnabled = GetFieldValue( "value", biasingDefinition );
    TString biasType =  GetFieldValue( "type", biasingDefinition );

    cout << "Bias : " << biasEnabled << endl;

    if( biasEnabled == "on" || biasEnabled == "ON" || biasEnabled == "On" || biasEnabled == "oN" )
    {
        cout << "Biasing is enabled" << endl;

        size_t position = 0;
        string biasVolumeDefinition;

        Int_t n = 0;
        while( ( biasVolumeDefinition = GetKEYDefinition( "biasingVolume", position, biasingString ) ) != "" )
        {
            TRestBiasingVolume biasVolume;

            cout << "Def : " << biasVolumeDefinition << endl;
            
            biasVolume.SetBiasingVolumePosition( Get3DVectorFieldValueWithUnits( "position", biasVolumeDefinition ) );
            biasVolume.SetBiasingFactor( StringToDouble( GetFieldValue( "factor", biasVolumeDefinition ) ) );
            biasVolume.SetBiasingVolumeSize( GetDblFieldValueWithUnits( "size", biasVolumeDefinition ) );
            biasVolume.SetEnergyRange( Get2DVectorFieldValueWithUnits( "energyRange", biasVolumeDefinition ) );
            biasVolume.SetBiasingVolumeType( biasType ); // For the moment all the volumes should be same type

            /* TODO check that values are right if not printBiasingVolume with getchar() 
            biasVolume.PrintBiasingVolume();
            getchar();
            */

            fBiasingVolumes.push_back( biasVolume );

            n++;
        }
        fNBiasingVolumes = n;
    }


}

void TRestG4Metadata::ReadGenerator()
{

    // TODO if some fields are defined in the generator but not finally used
    // i.e. <generator type="volume" from="gasTarget" position="(100,0,-100)
    // here position is irrelevant since the event will be generated from the volume defined in the geometry
    // we should take care of these values so they are not stored in metadata (to avoid future confusion)
    // In the particular case of position, the value is overwritten in DetectorConstruction by the center of the volume (i.e. gasTarget)
    // but if i.e rotation or side are defined and not relevant we should set it to -1

    string generatorString = GetKEYStructure( "generator" );

    string generatorDefinition = GetKEYDefinition( "generator", generatorString );

    fGenType = GetFieldValue( "type", generatorDefinition );
    fGenFrom = GetFieldValue( "from", generatorDefinition );
    fGenSize = GetDblFieldValueWithUnits( "size", generatorDefinition );

    // TODO : If not defined (and required to be) it just returns (0,0,0) we should make a WARNING. Inside StringToVector probably
    fGenPosition = Get3DVectorFieldValueWithUnits( "position", generatorDefinition );

    fGenRotation = StringTo3DVector ( GetFieldValue( "rotation", generatorDefinition ) );

    fGenLength = GetDblFieldValueWithUnits ( "length", generatorDefinition );

    size_t position = 0;
    string sourceString;

    Int_t n = 0;
    while( ( sourceString = GetKEYStructure( "source", position, generatorString ) ) != "" )
    {
        TRestParticleSource source;

        // Source parameters
        string sourceDefinition = GetKEYDefinition( "source", sourceString );

        fGeneratorFile = GetFieldValue( "fromFile", sourceDefinition );

        if( fGeneratorFile != "Not defined" )
        {
            cout << "Reading sources from generator file : " << fGeneratorFile << endl;
            ReadGeneratorFile( fGeneratorFile );

            break;
        }

        source.SetParticleName( GetFieldValue( "particle", sourceDefinition ) );

        TString fullChain = GetFieldValue( "fullchain", sourceDefinition );

        if( fullChain == "on" || fullChain == "ON" || fullChain == "On" || fullChain == "oN" )
        {
            SetFullChain( true );
        }
        else
        {
            SetFullChain( false );
        }

        // Angular distribution parameters
        string angularDefinition = GetKEYDefinition( "angularDist", sourceString );

        source.SetAngularDistType( GetFieldValue( "type", angularDefinition ) );

        if( source.GetAngularDistType() == "TH1D" )
        {

            source.SetAngularFilename( GetFieldValue( "file", angularDefinition ) ); 
            source.SetAngularName( GetFieldValue( "spctName", angularDefinition ) ); 
        }

        if( n == 0 && source.GetAngularDistType() == "backtoback" ) 
        {
            cout << "WARNING: First source cannot be backtoback. Setting it to isotropic" << endl;
            source.SetAngularDistType( "isotropic");

        }

        source.SetDirection( StringTo3DVector( GetFieldValue( "direction", angularDefinition ) ) );

        // Energy distribution parameters
        string energyDefinition = GetKEYDefinition( "energyDist", sourceString );

        source.SetEnergyDistType( GetFieldValue( "type", energyDefinition ) );

        if( source.GetEnergyDistType() == "TH1D" )
        {
            source.SetSpectrumFilename ( GetFieldValue( "file", energyDefinition ) );
            source.SetSpectrumName ( GetFieldValue( "spctName", energyDefinition ) );
        }

        source.SetEnergyRange( Get2DVectorFieldValueWithUnits ( "range", energyDefinition ) );

        if( source.GetEnergyDistType() == "mono" )
        {
            Double_t en;
            en = GetDblFieldValueWithUnits( "energy", energyDefinition );
            source.SetEnergyRange ( TVector2 (en,en) ); 
            source.SetEnergy( en );
        }

        fPrimaryGenerator.AddSource( source );

        n++;
    }

}

void TRestG4Metadata::ReadStorage( )
{

    string storageString = GetKEYStructure( "storage" );
    fSensitiveVolume = GetFieldValue( "sensitiveVolume", storageString  );
    cout << "Sensitive volume : " << fSensitiveVolume << endl;
    if( fSensitiveVolume == "Not defined" ) 
    {
        cout << "REST WARNNING : Sensitive volume not defined. Setting it to gas!!!!" << endl;
        fSensitiveVolume = "gas";
    }

    cout << fSensitiveVolume << endl;
    
    size_t pos = 0;
    fEnergyRangeStored = Get2DVectorParameterWithUnits( "energyRange", pos, storageString  );

    pos = 0;
    string volumeDefinition;
    while( (volumeDefinition = GetKEYDefinition( "activeVolume", pos, storageString )) != "" )
    {
            Double_t chance = StringToDouble( GetFieldValue( "chance", volumeDefinition ) );
            TString name = GetFieldValue( "name", volumeDefinition );
            SetActiveVolume( name, chance );
            cout << "Adding active volume" << endl;
    }

}

void TRestG4Metadata::PrintMetadata( )
{

        cout << endl;
        cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
        cout << "TRestG4Metadata content" << endl;
        cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
        cout << "Config file : " << fConfigFileName << endl;
        cout << "Section name : " << fSectionName << endl;        // section name given in the constructor of TRestSpecificMetadata
        cout << "---------------------------------------" << endl;
        cout << "Name : " << GetName() << endl;
        cout << "Title : " << GetTitle() << endl;
        cout << "---------------------------------------" << endl;
        cout << "Geometry File : " << Get_GDML_Filename() << endl;
        cout << "Max. Step size : " << GetMaxTargetStepSize() << " mm" << endl;
        cout << "---------------------------------------" << endl;
        cout << "Generator type : " << GetGeneratorType() << endl;
        cout << "Generated from : " << GetGeneratedFrom() << endl;
        TVector3 a = GetGeneratorPosition();
        cout << "Generator center : (" << a.X() << "," << a.Y() << "," << a.Z() << ") mm" << endl; 
        TVector3 b = GetGeneratorRotation();
        cout << "Generator rotation : (" << b.X() << "," << b.Y() << "," << b.Z() << ") mm" << endl; 
        cout << "Generator size : " << GetGeneratorSize() << " mm" << endl; 
        cout << "Generator length : " << GetGeneratorLength() << " mm" << endl; 
        cout << "---------------------------------------" << endl;
        cout << "Number of primary particles : " << GetNumberOfPrimaries() << endl;
        cout << "Generator file : " << GetGeneratorFile() << endl;
        cout << "***************************************" << endl;

        for( int n = 0; n < GetNumberOfPrimaries(); n++ )
        {
            TRestParticleSource src = GetParticleSource(n);
            src.PrintParticleSource();
        }
        
        cout << "Storage." << endl;
        cout << "***************************************" << endl;
        cout << "Energy range : Emin = " << GetMinimumEnergyStored() << " Emax : " << GetMaximumEnergyStored() << endl;
        cout << "Sensitive volume : " << GetSensitiveVolume() << endl;
        cout << "Active volumes : " << GetNumberOfActiveVolumes() << endl;
        cout << "---------------------------------------" << endl;
        for( int n = 0; n < GetNumberOfActiveVolumes(); n++ )
        {
            cout << "Name : " << GetActiveVolumeName(n) << " ID : " << GetActiveVolumeID( GetActiveVolumeName(n) ) << " chance : " << GetStorageChance( GetActiveVolumeName(n) ) << endl;

        }
        cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
        cout << "List of biasing volumes" << endl;
        cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
        for( int n = 0; n < GetNumberOfBiasingVolumes(); n++ )
        {
            GetBiasingVolume( n ).PrintBiasingVolume();
        }

}

void TRestG4Metadata::ReadGeneratorFile( TString fName )
{
    TString fullFilename = (TString) getenv("REST_PATH") +  "/inputData/generator/" + fName;

    char cadena[256];
    int tmpInt;

    FILE *fFileIn;

    if ( (fFileIn = fopen(fullFilename.Data(),"rt") ) == NULL ) {
        printf("Error en el archivo %s\n", fullFilename.Data());
        return;
    }

    // First lines are discarded.
    for( int i = 0 ; i < 20 ; i++)
    {
        fscanf( fFileIn, "%[^\n]\n", cadena);
        if( strcmp( cadena, "First event and full number of events:" ) == 0 ) break;
    }

    Int_t fGeneratorEvents;
    fscanf( fFileIn, "%d %d\n", &tmpInt, &fGeneratorEvents); // The number of events.

   // cout << "i : " << tmpInt << " fN : " << fGeneratorEvents << endl;

    TRestParticleCollection particleCollection;

    TRestParticle particle;


    cout << "Reading generator file : " << fName << endl;
    cout << "Total number of events : " << fGeneratorEvents << endl;

    for( int n = 0; n < fGeneratorEvents; n++ )
    {
        particleCollection.RemoveParticles();

        Int_t nParticles;
        Int_t evID;
        Double_t time;


        fscanf(fFileIn,"%d %lf %d\n", &evID, &time, &nParticles);

        for( int i = 0; i < nParticles; i++)
        {
            Int_t pID;
            Double_t momx, momy, momz, mass;
            Double_t energy, momentum2;

            fscanf( fFileIn, "%d %lf %lf %lf %lf\n", &pID, &momx, &momy, &momz, &time);

            if( pID == 3)
            {
                momentum2 = (momx * momx) + (momy * momy) + (momz * momz);
                mass = 0.511;

                energy = TMath::Sqrt( momentum2 + mass*mass ) - mass;
                particle.SetParticleName( "e-" );

            }
            else
            {
                cout << "Particle id " << pID << " not recognized" << endl;

            }

            TVector3 momDirection( momx, momy, momz );
            momDirection = momDirection.Unit();

            particle.SetEnergy( 1000. * energy );
            particle.SetDirection( momDirection );

            particleCollection.AddParticle( particle );
        }

        fPrimaryGenerator.AddParticleCollection( particleCollection );
    }

    fPrimaryGenerator.SetSourcesFromParticleCollection( 0 );

}

Int_t TRestG4Metadata::GetActiveVolumeID( TString name )
{
    Int_t id;
    for( id = 0; id < (Int_t) fActiveVolumes.size(); id++ )
    {
        if( fActiveVolumes[id] == name ) return id;
    }
    return -1;
}

void TRestG4Metadata::SetActiveVolume( TString name, Double_t chance )
{
    fActiveVolumes.push_back( name );
    fChance.push_back( chance );
}

Double_t TRestG4Metadata::GetStorageChance( TString vol )
{
    Int_t id;
    for( id = 0; id < (Int_t) fActiveVolumes.size(); id++ )
    {
        if( fActiveVolumes[id] == vol ) return fChance[id];
    }
    cout << "STORAGE VOLUME NOT FOUND" << endl;

    return 0;
}
