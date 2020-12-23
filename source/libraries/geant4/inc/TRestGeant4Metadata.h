/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestGeant4Metadata
#define RestCore_TRestGeant4Metadata
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include <TMath.h>
#include <TString.h>
#include <TVector2.h>
#include <TVector3.h>

#include <TRestGeant4BiasingVolume.h>
#include <TRestGeant4PrimaryGenerator.h>
#include <TRestMetadata.h>

//------------------------------------------------------------------------------------------------------------------------
//
// * This section was added by Luis A. Obis (lobis@unizar.es) on 17/06/2019
//
// Here we add all the possible options for different configurations such as all the types of generators, etc.
// We use a structure called 'enum' and a function to clean the strings so that we can easily implement case
// insensitivity or more options such as ignoring underscores.
//

namespace g4_metadata_parameters {
string CleanString(string);

enum class generator_types {
    CUSTOM,
    VOLUME,
    SURFACE,
    POINT,
    VIRTUAL_WALL,
    VIRTUAL_BOX,
    VIRTUAL_SPHERE,
    VIRTUAL_CIRCLE_WALL,
    VIRTUAL_CYLINDER,
};
extern std::map<string, generator_types> generator_types_map;

enum class energy_dist_types {
    TH1D,
    MONO,
    FLAT,
    LOG,
};
extern std::map<string, energy_dist_types> energy_dist_types_map;

enum class angular_dist_types {
    TH1D,
    ISOTROPIC,
    FLUX,
    BACK_TO_BACK,
};
extern std::map<string, angular_dist_types> angular_dist_types_map;
}  // namespace g4_metadata_parameters

//------------------------------------------------------------------------------------------------------------------------

/// The main class to store the *Geant4* simulation conditions that will be used by *restG4*.
class TRestGeant4Metadata : public TRestMetadata {
   private:
    void Initialize();

    void InitFromConfigFile();

    void ReadGenerator();

    void ReadStorage();
    void ReadBiasing();

    void ReadEventDataFile(TString fName);

    Int_t ReadNewDecay0File(TString fileName);

    Int_t ReadOldDecay0File(TString fileName);

    void ReadParticleSource(TiXmlElement* sourceDefinition);

    /// The version of Geant4 used to generate the data
    TString fGeant4Version;

    /// The local path to the GDML geometry
    TString fGeometryPath;  //!

    /// The filename of the GDML geometry
    TString fGDML_Filename;  //!

    /// A GDML geometry reference introduced in the header of the GDML main setup
    TString fGDML_Reference;

    /// A GDML materials reference introduced in the header of the GDML of materials definition
    TString fMaterialsReference;

    /// Type of spatial generator (surface, volume, point, virtualWall, etc)
    TString fGenType;

    /// The volume name where the events are generated, in case of volume or
    /// surface generator types.
    TString fGenFrom;

    /// The position of the generator for virtual, and point, generator types.
    TVector3 fGenPosition;

    /// \brief A 3d-vector with the angles, measured in degrees, of a XYZ rotation
    /// applied to the virtual generator. This rotation is used by virtualWall,
    /// virtualCircleWall and virtualCylinder generators.
    TVector3 fGenRotation;

    /// \brief The primary length of the virtual generator. I.e. the radius of a virtual
    /// sphere, or the X-side size of a virtual wall.
    Double_t fGenDimension1;

    /// \brief An additional dimension required by some virtual generators. I.e. the
    /// lenght of a virtual cylinder or the Y-side size of a virtual wall.
    Double_t fGenDimension2;

    /// \brief The name of a Decay0 generated file used to produce a collection of
    /// primary sources, definning energy, momentum, particle type.
    TString fGeneratorFile;

    /// \brief A 2d-vector storing the energy range, in keV, to decide if a particular
    /// event should be written to disk or not.
    TVector2 fEnergyRangeStored;

    /// \brief A vector to store the names of the active volumes.
    std::vector<TString> fActiveVolumes;

    /// \brief A vector to store the probability value to write to disk the hits in a
    /// particular event.
    std::vector<Double_t> fChance;

    /// \brief It the defines the primary source properties, particle type, momentum,
    /// energy, etc.
    TRestGeant4PrimaryGenerator fPrimaryGenerator;

    /// \brief The number of biasing volumes used in the simulation. If zero, no biasing
    /// technique is used.
    Int_t fNBiasingVolumes;

    /// A vector containning the biasing volume properties.
    std::vector<TRestGeant4BiasingVolume> fBiasingVolumes;

    /// \brief The maximum target step size, in mm, allowed in Geant4 for the target
    /// volume (usually the gas volume).
    Double_t fMaxTargetStepSize;

    /// \brief A time gap, in us, determinning if an energy hit should be considered (and
    /// stored) as an independent event.
    Double_t fSubEventTimeDelay;

    /// \brief Defines if a radioactive isotope decay is simulated in full chain
    /// (fFullChain=true), or just a single decay (fFullChain=false).
    Bool_t fFullChain;

    /// \brief The volume that serves as trigger for data storage. Only events that
    /// deposit a non-zero energy on this volume will be registered.
    TString fSensitiveVolume;

    /// The number of events simulated, or to be simulated.
    Int_t fNEvents;

    /// \brief The seed value used for Geant4 random event generator. If it is zero
    /// its value will be assigned using the system timestamp.
    Long_t fSeed = 0;

    /// \brief If this parameter is set to 'true' it will save all events even if they leave no energy in the
    /// sensitive volume (used for debugging pourposes). It is set to 'false' by default.
    Bool_t fSaveAllEvents = 0;

    /// If this parameter is set to 'true' it will print out on screen every time 10k events are reached.
    Bool_t fPrintProgress = 0;  //!

    /// \brief If this parameter is enabled it will register tracks with no hits inside. This is the default
    /// behaviour. If it is disabled then empty tracks will not be written to disk at the risk of loosing
    /// traceability, but saving disk space and likely improving computing performance for extense events.
    Bool_t fRegisterEmptyTracks = 1;

    /// The world magnetic field
    TVector3 fMagneticField = TVector3(0, 0, 0);

   public:
    /// \brief Returns the random seed that was used to generate the corresponding
    /// geant4 dataset.
    Long_t GetSeed() { return fSeed; }

    /// \brief Returns a string with the version of Geant4 used on the event data
    /// simulation
    TString GetGeant4Version() { return fGeant4Version; }

    /// Returns the local path to the GDML geometry
    TString GetGeometryPath() { return fGeometryPath; }

    /// Returns the main filename of the GDML geometry
    TString Get_GDML_Filename() { return fGDML_Filename; }

    /// Returns the reference provided at the GDML file header
    TString Get_GDML_Reference() { return fGDML_Reference; }

    /// Returns the reference provided at the materials file header
    TString GetMaterialsReference() { return fMaterialsReference; }

    /// \brief Returns a string specifying the generator type (volume, surface, point,
    /// virtualWall, etc )
    TString GetGeneratorType() { return fGenType; }

    /// \brief Returns the name of the GDML volume where primary events are
    /// produced. This value has meaning only when using volume or surface
    /// generator types.
    TString GetGeneratedFrom() { return fGenFrom; }

    /// \brief Returns the name of the GDML volume where primary events are
    /// produced. This value has meaning only when using volume or surface
    /// generator types.
    TString GetGDMLGeneratorVolume() { return fGenFrom; }

    /// \brief Returns a 3d-vector with the position of the primary event
    /// generator. This value has meaning only when using point and virtual
    /// generator types.
    TVector3 GetGeneratorPosition() { return fGenPosition; }

    /// \brief Returns a 3d-vector, fGenRotation, with the XYZ rotation angle
    /// values in degrees. This value is used by virtualWall, virtualCircleWall
    /// and virtualCylinder generator types.
    TVector3 GetGeneratorRotation() { return fGenRotation; }

    /// \brief Returns the main spatial dimension of virtual generator.
    /// It is the size of a  virtualBox.
    Double_t GetGeneratorSize() { return fGenDimension1; }

    /// \brief Returns the main spatial dimension of virtual generator.
    /// It is the length along the x-axis of a virtualWall.
    Double_t GetGeneratorLenX() { return fGenDimension1; }

    /// \brief Returns the main spatial dimension of virtual generator.
    /// It is the radius for virtualSphere, virtualCylinder and virtualCircleWall.
    Double_t GetGeneratorRadius() { return fGenDimension1; }

    /// \brief Returns the secondary spatial dimension of a virtual volume
    /// generator. It is used to define the length of virtualCylinder generator.
    Double_t GetGeneratorLength() { return fGenDimension2; }

    /// \brief Returns the secondary spatial dimension of a virtual volume
    /// generator. It is the length along the y-axis of a virtualWall.
    Double_t GetGeneratorLenY() { return fGenDimension2; }

    /// \brief Returns true in case full decay chain simulation is enabled.
    Bool_t isFullChainActivated() { return fFullChain; }

    /// \brief Returns the filename used as manual generator. Decay0 files are can
    /// be understood by restG4.
    TString GetGeneratorFile() { return fGeneratorFile; }

    /// \brief Returns the value of the maximum Geant4 step size in mm for the
    /// target volume.
    Double_t GetMaxTargetStepSize() { return fMaxTargetStepSize; }

    /// \brief Returns the time gap, in us, required to consider a Geant4 hit as a
    /// new independent event. It is used to separate simulated events that in
    /// practice will appear as such in our detector. I.e. to separate multiple
    /// decay products (sometimes with years time delays) into independent events.
    Double_t GetSubEventTimeDelay() { return fSubEventTimeDelay; }

    /// It returns true if save all events is active
    Bool_t GetSaveAllEvents() const { return fSaveAllEvents; }

    /// It returns true if `printProgress` parameter was set to true
    Bool_t PrintProgress() const { return fPrintProgress; }

    /// It returns false if `registerEmptyTracks` parameter was set to false.
    Bool_t RegisterEmptyTracks() const { return fRegisterEmptyTracks; }

    /// \brief Used exclusively by restG4 to set the value of the random seed used on
    /// Geant4 simulation.
    void SetSeed(Long_t seed) { fSeed = seed; }

    /// Enables or disables the save all events feature
    void SetSaveAllEvents(const Bool_t value) { fSaveAllEvents = value; }

    /// Sets the value of the Geant4 version
    void SetGeant4Version(TString g4Version) { fGeant4Version = g4Version; }

    ///  \brief Sets the generator type. I.e. volume, surface, point, virtualWall,
    ///  virtualCylinder, etc.
    void SetGeneratorType(TString type) { fGenType = type; }

    ///  \brief Sets the generator main spatial dimension. In a virtual generator is the
    ///  radius of cylinder, size of wall, etc.
    void SetGeneratorSize(Double_t size) { fGenDimension1 = size; }

    ///  Enables/disables the full chain decay generation.
    void SetFullChain(Bool_t fullChain) { fFullChain = fullChain; }

    ///  Sets the position of the virtual generator using a TVector3.
    void SetGeneratorPosition(TVector3 pos) { fGenPosition = pos; }

    ///  Sets the position of the virtual generator using x,y,z coordinates.
    void SetGeneratorPosition(double x, double y, double z) { fGenPosition = TVector3(x, y, z); }

    /// Sets the number of events to be simulated.
    void SetNEvents(Int_t n) { fNEvents = n; }

    /// It sets the location of the geometry files
    void SetGeometryPath(string path) { fGeometryPath = path; }

    /// It sets the main filename to be used for the GDML geometry
    void Set_GDML_Filename(string gdmlFile) { fGDML_Filename = gdmlFile; }

    /// Returns the reference provided at the GDML file header
    void Set_GDML_Reference(string reference) { fGDML_Reference = reference; }

    /// Returns the reference provided at the materials file header
    void SetMaterialsReference(string reference) { fMaterialsReference = reference; }

    /// Returns the number of events to be simulated.
    Int_t GetNumberOfEvents() { return fNEvents; }
    ///////////////////////////////////////////////////////////

    // Direct access to sources definition in primary generator
    ///////////////////////////////////////////////////////////
    /// Returns the number of primary event sources defined in the generator.
    Int_t GetNumberOfSources() { return fPrimaryGenerator.GetNumberOfSources(); }

    /// Returns the number of primary event sources defined in the generator.
    Int_t GetNumberOfPrimaries() { return GetNumberOfSources(); }

    /// Returns the particle source specified with index n.
    TRestGeant4ParticleSource GetSource(int n) { return fPrimaryGenerator.GetParticleSource(n); }

    /// Returns the name of the particle source with index n (Geant4 based names).
    TRestGeant4ParticleSource GetParticleSource(int n) { return fPrimaryGenerator.GetParticleSource(n); }

    /// \brief Returns the primary generator object containning information about
    /// particle sources.
    TRestGeant4PrimaryGenerator GetPrimaryGenerator() { return fPrimaryGenerator; }

    /// \brief Places in fPrimaryGenerator the source definition, with index n,
    /// from a TRestGeant4ParticleCollection. This will be used by restG4 to pick up
    /// randomly a primary source definition from a pre-generated sources
    /// collection. The particle collection needs to be previously populated using
    /// i.e. an input Decay0 file.
    void ReadParticleCollection(Int_t n) { fPrimaryGenerator.UpdateSourcesFromParticleCollection(n); }

    /// Removes all the sources from fPrimaryGenerator.
    void RemoveSources() { fPrimaryGenerator.RemoveSources(); }

    /// Adds a new source to fPrimaryGenerator.
    void AddSource(TRestGeant4ParticleSource src) { fPrimaryGenerator.AddSource(src); }
    ///////////////////////////////////////////////////////////

    // Direct access to biasing volumes definition
    //////////////////////////////////////////////
    /// Returns the number of biasing volumes defined
    Int_t GetNumberOfBiasingVolumes() { return fBiasingVolumes.size(); }

    /// Return the biasing volume with index n
    TRestGeant4BiasingVolume GetBiasingVolume(int n) { return fBiasingVolumes[n]; }

    /// \brief Returns the number of biasing volumes defined. If 0 the biasing technique
    /// is not being used.
    Int_t isBiasingActive() { return fBiasingVolumes.size(); }

    /// Returns a string with the name of the sensitive volume.
    TString GetSensitiveVolume() { return fSensitiveVolume; }

    /// Sets the name of the sensitive volume
    void SetSensitiveVolume(TString sensVol) { fSensitiveVolume = sensVol; }
    ///////////////////////////////////////////////////////////

    /// \brief Returns the probability per event to register (write to disk) hits in the
    /// storage volume with index n.
    Double_t GetStorageChance(Int_t n) { return fChance[n]; }

    /// Returns the probability per event to register (write to disk) hits in a
    /// GDML volume given its geometry name.
    Double_t GetStorageChance(TString vol);

    /// Returns the minimum event energy required for an event to be stored.
    Double_t GetMinimumEnergyStored() { return fEnergyRangeStored.X(); }

    /// Returns the maximum event energy required for an event to be stored.
    Double_t GetMaximumEnergyStored() { return fEnergyRangeStored.Y(); }

    /// \brief Returns the number of active volumes, or geometry volumes that have been
    /// selected for data storage.
    Int_t GetNumberOfActiveVolumes() { return fActiveVolumes.size(); }

    /// Returns a string with the name of the active volume with index n
    TString GetActiveVolumeName(Int_t n) { return fActiveVolumes[n]; }

    /// Returns the world magnetic field in Tesla
    TVector3 GetMagneticField() { return fMagneticField; }

    Int_t GetActiveVolumeID(TString name);

    Bool_t isVolumeStored(TString volName);

    void SetActiveVolume(TString name, Double_t chance);

    void PrintMetadata();

    TRestGeant4Metadata();
    TRestGeant4Metadata(char* cfgFileName, std::string name = "");

    ~TRestGeant4Metadata();

    ClassDef(TRestGeant4Metadata, 7);
};
#endif  // RestCore_TRestGeant4Metadata
