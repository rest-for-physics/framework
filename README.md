# The REST Framework

The REST (Rare Event Searches with TPCs) Framework is mainly written in C++ and it is fully integrated with [ROOT](https://root.cern.ch) I/O interface.
REST was born as a collaborative software effort to provide common tools for acquisition, simulation, and data analysis of gaseous Time Projection Chambers (TPCs).
The REST Framework provides 3 interfaces that prototype the use of **event types**, **metadata** and **event processes** through `TRestEvent`, `TRestMetadata` and `TRestEventProcess` abstract class definitions.
Any REST library will implement **specific objects** that inherit from those 3 basic interfaces. 

Different **event processes** can be combined to build complex event processing chains with full traceability. 
The **metadata** objects will allow us to provide input parameters or information to the framework using a XML-like format.
REST integrates a special **metadata** object named `TRestManager` that encapsulates all the required information to launch the processing of a particular data chain.
REST will produce output using ROOT format. Any REST file will always contain a `TRestRun` metadata object.
`TRestRun` is a **metadata** object responsible to encapsulate and give access to all the objects stored inside the REST/ROOT file; 
i.e. the **specific** resulting `TRestEvent` output, the `TRestAnalysisTree`, and any **specific** `TRestMetadata` object used during a processing chain.

The REST Framework provides additionally different interfaces to **browse data**, `TRestBrowser`, and to **plot data**, `TRestAnalysisPlot`, by accessing `TRestEvent` and `TRestAnalysisTree` ROOT-based drawing methods.
Other objects included in the framework will help to add unit definitions, `REST_Units`, define physical constants `REST_Physics`, or provide methods to help on text formatting `TRestStringHelper` or define output styles, `TRestStringOutput`.


## Getting Started

These instructions will get you a copy of the project up and running on your local machine in your home directory.
More info can be found in the [documentation](doc/Chapters/2-installing-rest.md) 

The recommended way to download a copy of REST will be to clone it using the corresponding git command.

```
cd
git clone git@lfna.unizar.es:rest-development/REST_v2.git
```

As soon as REST is under strong development phase the repository will be private, and access to the REST repository will be only granted on demand.
Before granting access, an account must be registered at the [Unizar Gitlab site](https://lfna.unizar.es). 
Then, you will need to contact the authors to request access to the code.

### Prerequisites

The only mandatory prerequisite of REST is ROOT6. Details on the installation of ROOT will be found at the [ROOT's official site](root.cern.ch). 
One may directly find binary distributions on its [download page](https://root.cern.ch/downloading-root). 
If not, try to compile and install it manually.

We provide a script inside `scripts/installation/installROOT.sh`, to automatize the process of downloading, compiling and installing a predefined version of ROOT in your local system.
If your system comes installed with all the [ROOT prerequisites](https://root.cern.ch/build-prerequisites) the installation using this script should be quite smooth.

Example of installing ROOT6 using REST installation script:

```
cd REST_v2/scripts/installation/  
./installROOT.sh  
```

The script will install a particular version of ROOT defined inside the script, and it will add to your bashrc a line to load ROOT each time you start a new terminal session.
ROOT will be installed at `$HOME/apps`. Feel free to modify the `installROOT.sh` script to choose a different installation directory or ROOT version.

Before starting the REST installation, make sure you are running the desired ROOT version and binary.

```
root-config --version
which root
```

### Installing

After ROOT6 has been installed in the system, the compilation of REST should be straight forward. 
Note that it is recommended to compile REST using the same version of g++ compiler used to compile ROOT.

Go to the root directory of your local REST repository, lets name it here `REST_SOURCE_PATH` and execute the following commands.

```
cd  
cd REST_v2  
mkdir build
cd build
cmake .. -DINSTALL_PREFIX=../install/master/ 
make -j4 install
```

After all the compilation and installation process ends, you will end up with an installed REST version at `~/REST_v2/install/master/`.

Execute the following command to configure your `.bashrc` to load REST in your system environment each time you open a new shell terminal.

 ```
 echo "source ~/REST_v2/install/master/thisREST.sh" >> .bashrc
 ```


### Basic tests of the REST installation

After sourcing `thisREST.sh` you should see a message on screen similar to the following one.

```
  *****************************************************************************
  W E L C O M E   to  R E S T

  Commit  : d1a37c6c (2019-08-27 16:11:18 +0800)
  Branch/Version : v2.2.12_dev/v2.2.12
  Compilation date : 2019-08-27 18:53

  Installed at : /home/daq/REST_v2/install

  REST forum site : ezpc10.unizar.es (New!)
  *****************************************************************************
```

You can test now that REST libraries are loaded without errors inside the ROOT environment using the `restRoot` command:

```
restRoot

   ------------------------------------------------------------
  | Welcome to ROOT 6.14/06                http://root.cern.ch |
  |                               (c) 1995-2018, The ROOT Team |
  | Built for macosx64                                         |
  | From tags/v6-14-06@v6-14-06, Dec 11 2018, 12:58:00         |
  | Try '.help', '.demo', '.license', '.credits', '.quit'/'.q' |
   ------------------------------------------------------------

Loading library : libRestCore.dylib
Loading library : libRestProcesses.dylib
Loading library : libRestEvents.dylib
Loading library : libRestMetadata.dylib
Loading library : libRestTools.dylib

```

### Compilation options

Different options can be passed to the `cmake` command to personalize the REST installation. The following options are available in REST.

* **INSTALL_PREFIX** : Allows to define the destination of the final REST install directory. The default value is either "REST_v2/install/" (if you haven't installed REST) or the current REST path (if you already installed REST).
* **REST_WELCOME** (Default: ON) : If dissabled no message will be displayed each time we call thisREST.sh.
* **REST_GARFIELD** (Default: OFF) : Enables access to [Garfield++](https://garfieldpp.web.cern.ch/garfieldpp/) libraries in REST.
* **REST_G4** (Default: OFF) : Adds executable `restG4` which carries out simulation with [Geant4++](http://geant4.web.cern.ch/) using REST style config file.

To pass the options to cmake, one need to append "-DXXX=XXX" in the cmake command, for example: `cmake .. -DREST_WELCOME=OFF -DREST_G4=ON`.
Once you explicitly set an option, the default value of this option will be changed to the new value when you run `cmake` in the next time.

## REST libraries

The REST framework provides only the structure and support to create REST user libraries. 
The detailed documentation can be found [here](doc/Chapters/4-the-rest-framework) 

Few official REST libraries maintained by the REST team will be installed as a module and maintained in a separate Git repository.
Additional information on the functionality provided by those libraries will be found on that repository.

* RestDetectorLib : 
* RestRawLib : 
* etc

`TOBE written`

## Contributing, versioning and documentation

Please read [CONTRIBUTING.md](CONTRIBUTING.md) to get some guidelines on how to contribute to this project.
Before any contribution, those guidelines must be assimilated and accepted. 
In any case, changes, improvements, or addons, to [CONTRIBUTING.md](CONTRIBUTING.md) are aceptable after proposal and discussion with other authors at the [REST Framework forum](https://ezpc10.unizar.es/).

REST exploits the Git tagging system to produce its own versioning system. Details on how the REST version number is produced are given in [CONTRIBUTING.md](CONTRIBUTING.md).
Any **metadata** object written with REST **will contain this version number**, making it possible to determine the REST version used to write any particular ROOT file.
If different REST versions were used to write a ROOT file, e.g. at different steps of the data processing chain, the historic metadata objects will preserve their original version.
However, the `TRestRun` metadata object **will always store** the version used to write the ROOT file.

After REST release 2.2.1., REST implements correctly the `ROOT schema evolution`. Therefore, any new REST version should always be backwards compatible.
I.e. Any file written after v2.2.1 should be readable without problems with any future version.

The [REST API documentation](https://sultan.unizar.es/rest/) (WIP) will provide details on the use of the different REST objects, together with few tutorials and basic examples on the use of REST in different scenarios. 


## Main authors and contributing institutions

* **Javier Galan, Igor G. Irastorza, Gloria Luzon** - *University of Zaragoza (Spain)*
* **Ke Han, Kaixiang Ni** - *Shanghai Jiao Tong University (China)*
* **Yann Bedfer, Damien Neyret** - *CEA Saclay (France)*

See also the list of [contributors]() who participated in this project.

## Publications

`TOBE written`

## License

This project is licensed under the GNU License - see the [LICENSE](https://lfna.unizar.es/rest-development/REST_v2/blob/master/LICENCE) file for details

## Acknowledgments

`TOBE written`

* Hat tip to anyone whose code was used
* Inspiration
* etc

