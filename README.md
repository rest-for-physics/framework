# The REST Framework

The REST (Rare Event Searches with TPCs) Framework is mainly written in C++ and it is fully integrated with [ROOT](root.cern.ch) I/O interface.
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

These instructions will get you a copy of the project up and running on your local machine.

The recommended way to download a copy of REST will be to clone it using the corresponding git command.

```
git clone git@lfna.unizar.es:rest-development/REST_v2.git
```

As soon as REST is under strong development phase the repository will be private, and access to the REST repository will be only granted on demand.
Before granting access, an account must be registered at the [Unizar Gitlab site](https://lfna.unizar.es). 
Then, you will need to contact the authors to request access to the code.

### Prerequisites

The only mandatory prerequisite of REST is ROOT6. Details on the installation of ROOT will be found at the [ROOT's official site](root.cern.ch). 
To avoid problems during the installation it is recommended that REST libraries will be compiled using the same version of g++ compiler used to compile ROOT.

We provide a script inside `scripts/installation/installROOT.sh`, to automatize the process of downloading, compiling and installing a predefined version of ROOT in your local system.
If your system comes installed with all the [ROOT prerequisites](https://root.cern.ch/build-prerequisites) the installation using this script should be quite smooth.

Example of installing ROOT6 using REST installation script:

```
cd scripts/installation/
./installROOT.sh
```

The script will install a particular version of ROOT defined inside the script, and it will add to your bashrc a line to load ROOT each time you start a new terminal session.
ROOT will be installed at `$HOME/apps`. Feel free to modify the `installROOT.sh` script to choose a different installation directory or ROOT version.

### Installing

After ROOT6 has been installed in the system, the compilation of REST should be straight forward. 
Before starting the REST installation process make sure you are running the desired ROOT version and binary.

```
root-config --version
which root
```

REST will recognize those and use them to link to your ROOT installation.

Then, go to the root directory of your local REST repository, lets name it here `REST_SOURCE_PATH` and execute the following commands.

```
cd REST_SOURCE_PATH
mkdir build
cd build
cmake -DINSTALL_PREFIX=../install/master/ ../
make -j4 install
```

After all the compilation and installation process ends, you will end up with an installed REST version at your `REST_SOURCE_PATH/install/master/`.

Add the following line ```source REST_SOURCE_PATH/install/master/thisREST.sh``` to your `.bashrc` file in order to load REST in your system environment each time you open a new shell terminal.

### Basic tests of the REST installation

After sourcing `thisREST.sh` you should see a message on screen similar to the following one.

```
  *****************************************************************************
  W E L C O M E   to  R E S T  
  
  Commit  : b8474752 (2019-04-13 11:00:22 +0200)  
  Branch/Version : v2.2.10/v2.2.10  
  Compilation date : 2019-04-16 21:51  
  
  Installed at : /Users/jgalan/git/REST_v2/install/v2.2.10  
  
  REST releases announcement : rest-dev@cern.ch  
  
  Self-subscription policy is open at egroups.cern.ch  
  
  REST forum site : rest-forum.gifna.unizar.es
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

* **REST_WELCOME** (Default: ON) : If dissabled no message will be displayed each time we call thisREST.sh.
* **INSTALL_PREFIX** : Allows to define the destination of the final REST install directory.
* **REST_GARFIELD** (Default: OFF) : Enables access to [Garfield++](https://garfieldpp.web.cern.ch/garfieldpp/) libraries in REST.

### Installing a particular REST version release

When we download/clone REST repository on our local system, the latest release will be downloaded. 
We can switch and install any specific REST release by cloning any particular *git tag*.

**Note!:** Save all your changes before continuing. You need to be in a clean copy to be able to change to a new release.

For example, to create a git branch connected to the REST release v2.2.6, you will do the following.

```
git reset --hard
git checkout tags/v2.2.6 -b v2.2.6
```

You may make sure the change took place by checking the status and commit history.

```
git status
git log
```

Then, you may re-use the build directory to compile and install the new version. We recommend to remove your build directory before.

```
cd REST_SOURCE_PATH
cd build
rm -rf *
cmake -DINSTALL_PREFIX=../install/v2.2.6/ ../
make -j4 install
```

## REST libraries

The REST framework provides only the structure and support to create REST user libraries. 
Few official REST libraries maintained by the REST team will be installed as a module and maintained in a separate Git repository.
Additional information on the functionality provided by those libraries will be found on that repository.

* RestDetectorLib : 
* RestRawLib : 
* etc

TOBE written

## Contributing, versioning and documentation

Please read [CONTRIBUTING.md](https://lfna.unizar.es/rest-development/REST_v2/blob/master/CONTRIBUTING.md) to get 


## Main authors and contributing institutions

* **Javier Galan, Igor G. Irastorza, Gloria Luzon** - *University of Zaragoza (Spain)*
* **Ke Han, Kaixiang Ni** - *Shanghai Jiao Tong University (China)*
* **Yann Bedfer, Damien Neyret** - *CEA Saclay (France)*

See also the list of [contributors]() who participated in this project.

## Publications



## License

This project is licensed under the GNU License - see the [LICENSE.md](https://lfna.unizar.es/rest-development/REST_v2/blob/master/LICENCE.md) file for details

## Acknowledgments

TOBE written

* Hat tip to anyone whose code was used
* Inspiration
* etc

