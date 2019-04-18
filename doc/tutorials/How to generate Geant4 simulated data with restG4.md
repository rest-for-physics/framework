\brief This tutorial shows how to define specific simulation conditions to generate Geant4 event data in REST format.

The package restG4 provides a Geant4 code that can be interfaced with REST using TRestMetadata specific structures to be used in Geant4 simulation conditions. All the simulation behaviour is defined through a REST metadata language file (RML). TRestG4Metadata is the main class collecting all the initial simulation conditions, such as which particules will be generated, from where they will be generated, what data will be stored in the output file, and few other options. All this information is stored by TRestG4Metadata by using helper classes, such as TRestPhysicsLists, TRestG4PrimaryGenerator, TRestBiasingVolume, etc.

restG4 will launch the simulation, that we described in the simulation RML file, and it will automatically create a ROOT file using the classes structure of REST libraries.

## The required user input to start a restG4 simulation

The required input files to use *restG4* as a simulation toolkit inside REST are

1. A RML file definning the run, simulation conditions, and physics lists to be used in Geant4,

2. and a GDML describing the geometry of the detector.

### 1. The RML file definitions

In general terms, an RML file to be used with *restG4* must define the following sections, and structures.

~~~{.xml}
<!-- We must, as usual, define the location where the REST output files will be stored -->
<globals>
   <parameter name="mainDataPath" value="${REST_DATAPATH}" />
</globals>

<!-- A TRestRun section to define few run parameters with a general run description. -->
<TRestRun>
    ...
</TRestRun>

<!-- A TRestG4Metadata section definning few parameters, generator, and storage. -->
<TRestG4Metadata>
    ...
</TRestG4Metadata>

<!-- A TRestPhysicsLists section definning the physics processes active. -->
<TRestPhysicsLists>
    ...
</TRestPhysicsLists>
~~~


\note Wherever 3 dots (`...`) are provided means a redundant code format, or that additional fields might be required. 

Few basic working examples can be found at REST_PATH/config/template/restG4.rml. Those examples will be used to illustrate step by step the execution of *restG4* and the obtention of few parameters for analysis. The documentation found at TRestG4Metadata class will help you construct a generic RML file to allow you define your particular simulation setup.

### 2. The GDML detector geometry

The geometry of the detector is definned using GDML. Check the GDML official website http://gdml.web.cern.ch/GDML/ for further details on how to create or modify an existing GDML geometry. The GDML file defines the most revelevant parts of the detector, active region, vessel, shielding, etc.

Inside the REST repository you will find an example geometry definition to help you start with a simple working geometry. You will find this example at REST_PATH/config/template/geometry/. This geometry consists of two basic elements,

1. a gas volume which is the active detector component,
2. and a vessel volume that contains the gas. 

Our simple demo geometry will be organized in few files, as described in the following list.

- **mySetupTemplate.gdml**: This is the main geometry file. It defines all the parameters you will use later to define the dimensions and/or material properties of the geometrical solids, volumes and physical volumes used by Geant4 (I.e. the size of the vessel, the gas volume, etc). This file includes the two other files that are used to define materials (materials.xml) and the definition of solids, volumes and physical volumes (myGeometryTemplate.gdml).

- **materials.xml**: This file defines all the materials you need to define your geometry volumes (including gas mixtures). The materials.xml defined in the repository is just used as a basic example, if there are new materials you need for the definition of your own setup you will need to include them your personal geometry files. In any case, this file will be kept in the repository as a library of materials that are commonly used in simulations.

- **myGeometryTemplate.gdml**: This file defines the solids, volumes and physical volumes using the parameter values defined in `mySetupTemplate.gdml`, and materials defined at `materials.xml`.

In principle, you can follow any other file scheme (i.e. using a unique file for all material and geometry definitions), as soon as the geometry is compatible with ROOT (i.e. it can be imported into TGeoManager ROOT class). 

\warning For few geometry primitives, it might happen that are supported in Geant4 but not in ROOT, or viceversa. You will need to load the GDML geometry into ROOT TGeoManager class to verify the geometry will be properly imported into REST.

If the geometry is valid then we can load it into ROOT and visualize it using the following lines of code.

~~~
cd $REST_PATH/config/template/geometry/
~ root
[0] TGeoManager *geo = TGeoManager::Import("mySetupTemplate.gdml");
[1] geo->GetTopVolume()->Draw("ogl");
~~~

## Producing, visualizing and printing Geant4 REST generated data using the examples

Inside the file `REST_PATH/config/template/restG4.rml`, you will find three different examples that will allow you to generate and store event data. The file `restG4.rml` contains a unique TRestRun and TRestPhysicsLists sections, common to each simulation case, and different TRestG4Metadata sections each of them definning different simulation conditions. All these examples use the basic geometry described previously.

The following list describes briefly the examples available,

1. **NLDBD**: It produces neutrinoless double beta decay mode of 136Xe isotope. The events are launched using a volume generator in the gas.
2. **MuonShower**: It produces a cosmic muon shower using the measured energy and angular spectrum of muons on earth. The muons are randomly generated from a virtual wall placed on top of the detector.
3. **Cd109**: It simulates the radiactive decay of Cadmium 109 isotope at a fixed position inside the detector. Low energy gammas from the de-excitation of the daugther isotope interact inside the detector.

These working examples can be launched with *restG4* command. In these examples, the number of primaries to generate is only 100, but you can modify it to make a longer simulation by definning the environment variable *NEVENTS*. We must also assure that the *REST_DATAPATH* variable is defined and points to an existing directory with write access.

The restG4 command receives up to two arguments,

1. the RML file containning the TRestG4Metadata, TRestRun and TRestPhysicsLists sections,
2. and the name of the TRestG4Metadata section to be used.

The execution of *restG4* follows this scheme,

~~~
restG4 cfgFile.rml sectionName
~~~

being the first argument, `cfgFile.rml` mandatory, and the second argument, `sectionName`, optional. If the name of the section is omitted when launching *restG4*, the first TRestG4Metadata section found inside `cfgFile.rml` will be considered.

The following lines of code illustrate how to generate the example files.

~~~
// Change directory to the place where the RML files are located
cd $REST_PATH/config/template/

// Define your output data path
export REST_DATAPATH=/path/to/your/data/directory

// Define the number of events to be simulated
export NEVENTS=1000

// Launching example 1 (NLDBD)
restG4 restG4.rml 

// Launching example 2
restG4 restG4.rml MuonShower

// Launching example 3
restG4 restG4.rml Cd109
~~~

After, the proper execution of the *restG4* command we will find our results file, containning TRestG4Event data, inside the directory *REST_DATAPATH* we have defined in our system.

You can visualize the generated Geant4 events, that produced an energy deposit in the sensitive volume of the detector using the `REST_VIEWER_G4Event` script available when launching *restG4*, or print detailed event information using `REST_UTILS_PrintG4Event`.

The following code shows how to access and visualize the data.

~~~
// We launch ROOT interface with REST libraries and scripts
restRoot

// We print the event entry 1
[0] REST_UTILS_PrintG4Event("/path/to/your/data/directory/Run_xx_xx_xx.root", 1);

// We print the event entry 21
[1] REST_UTILS_PrintG4Event("/path/to/your/data/directory/Run_xx_xx_xx.root", 21);

// We open the event browser to visualize Geant4 events
[2] REST_VIEWER_G4Event( "/path/to/your/data/directory/Run_xx_xx_xx.root");
~~~


`REST_VIEWER_G4Event` will present a screen created by TEveViewer class. On the left size we have a tree viewer, where we can access to different details, as the particle name and energy deposited, from the particles that are involved in the event that we are visualizing.

![Left. A neutrino less double beta decay event. Right. A simulated cosmic muon. Both visualizations were produced using REST_VIEWER_G4Event.](Geant4Events.png) 


