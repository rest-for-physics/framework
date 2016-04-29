///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4Metadata.h
///
///             G4 class to be used to design REST metadata classes to be 
///             inherited from TRestMetadata
///             How to use: replace TRestG4Metadata by your class name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             aug 2015:   First concept. Javier Galan
//  
///_______________________________________________________________________________


#ifndef RestCore_TRestG4Metadata
#define RestCore_TRestG4Metadata

#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>

#include <TMath.h>
#include <TString.h>
#include <TVector2.h>
#include <TVector3.h>

#include <TRestMetadata.h>
#include <TRestG4PrimaryGenerator.h>
#include <TRestBiasingVolume.h>

class TRestG4Metadata:public TRestMetadata {
    private:
        void Initialize();

        void InitFromConfigFile();

        void ReadGenerator();
        void ReadStorage();
        void ReadBiasing();

        void ReadGeneratorFile( TString fName );

        TString fGeometryPath;
        TString fGDML_Filename;	

        TString fGenType;  // Type of spatial generator (surface, volume, point)
        TString fGenFrom;  // Definition of surface or volume surface->sphere,wall  volume->GeoVolumeName - point
        TVector3 fGenPosition; // center of the generator geometry
        TVector3 fGenRotation; // Generator rotation (i.e. for wall and cylinder generators)
        Double_t fGenSize;    // Size of the generating geometry (Sphere ->radius Wall->side)
        Double_t fGenLength;  // For generators that require an additional length (i.e. cylinder)

        TString fGeneratorFile;

        TVector2 fEnergyRangeStored;
        std::vector <TString> fActiveVolumes;
        std::vector <Double_t> fChance;

        TRestG4PrimaryGenerator fPrimaryGenerator;

        Int_t fNBiasingVolumes;
        std::vector <TRestBiasingVolume> fBiasingVolumes;
        
        Double_t fMaxTargetStepSize;
        Double_t fSubEventTimeDelay;


        Bool_t fFullChain;

        TString fSensitiveVolume;

    public:

        TString GetGeometryPath() { return fGeometryPath; }
        TString Get_GDML_Filename() { return fGDML_Filename; }
        TString GetGeneratorType() { return fGenType; }
        TString GetGeneratedFrom() { return fGenFrom; }
        TVector3 GetGeneratorPosition() { return fGenPosition; }
        TVector3 GetGeneratorRotation() { return fGenRotation; }
        Double_t GetGeneratorSize() { return fGenSize; }
        Double_t GetGeneratorLength() { return fGenLength; }
        Bool_t isFullChainActivated() { return fFullChain; }
        TString GetGeneratorFile() { return fGeneratorFile; }

        Double_t GetMaxTargetStepSize() { return fMaxTargetStepSize; }
        Double_t GetSubEventTimeDelay() { return fSubEventTimeDelay; }

        void SetGeneratorType( TString type ) { fGenType = type; } 
        void SetGeneratorSize( Double_t size ) { fGenSize = size; }
        void SetFullChain( Bool_t fullChain ) { fFullChain = fullChain; }

        void SetGeneratorPosition( TVector3 pos ) { fGenPosition = pos; }
        void SetGeneratorPosition( double x, double y, double z ) { fGenPosition = TVector3( x, y, z ); }

        // Direct access to sources in primary generator
        Int_t GetNumberOfSources() { return fPrimaryGenerator.GetNumberOfSources(); }
        Int_t GetNumberOfPrimaries() { return GetNumberOfSources(); }
        TRestParticleSource GetSource( int n ) { return fPrimaryGenerator.GetParticleSource(n); }
        TRestParticleSource GetParticleSource( int i ) { return fPrimaryGenerator.GetParticleSource( i ); }
        TRestG4PrimaryGenerator GetPrimaryGenerator() { return fPrimaryGenerator; }

        void SetParticleCollection ( Int_t n ) { fPrimaryGenerator.SetSourcesFromParticleCollection( n ); }

        void RemoveSources() { fPrimaryGenerator.RemoveSources(); }
        void AddSource( TRestParticleSource src ) { fPrimaryGenerator.AddSource( src ); }

        Int_t GetNumberOfBiasingVolumes() { return fBiasingVolumes.size(); }
        TRestBiasingVolume GetBiasingVolume( int i ) { return fBiasingVolumes[i]; }
        Int_t isBiasingActive() { return fBiasingVolumes.size(); }


        TString GetSensitiveVolume() { return fSensitiveVolume; }

        void SetSensitiveVolume( TString sensVol ) { fSensitiveVolume = sensVol; }


        Double_t GetStorageChance( Int_t n ) { return fChance[n]; }
        Double_t GetStorageChance( TString vol );

        Double_t GetMinimumEnergyStored() { return fEnergyRangeStored.X(); }
        Double_t GetMaximumEnergyStored() { return fEnergyRangeStored.Y(); }

        Int_t GetNumberOfActiveVolumes( ) { return fActiveVolumes.size(); }
        Int_t GetActiveVolumeID( TString name );

        TString GetActiveVolumeName( Int_t id ) { return fActiveVolumes[id]; }

        Bool_t isVolumeStored( TString volName )
        {
            for( int n = 0; n < GetNumberOfActiveVolumes(); n++ )
                if( GetActiveVolumeName( n ) == volName ) return true;
            return false;
        }

        void SetActiveVolume( TString name, Double_t chance );

        void PrintMetadata( );

        //Constructors
        TRestG4Metadata();
        TRestG4Metadata( char *cfgFileName, std::string name = "");
        //Destructor
        ~TRestG4Metadata();


        ClassDef(TRestG4Metadata, 1); 
};
#endif
