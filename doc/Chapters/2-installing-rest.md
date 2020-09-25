## Installing REST

These instructions will get you a copy of the project up and running on your local machine in your home directory.

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
cd ~/REST_v2  
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

* REST Options/features:
    * **INSTALL_PREFIX** : Allows to define the destination of the final REST install directory. The default value is either "REST_v2/install/" (if you haven't installed REST) or the current REST path (if you already installed REST).
    * **REST_WELCOME** (Default: ON) : If dissabled no message will be displayed each time we call thisREST.sh.
    * **REST_GARFIELD** (Default: OFF) : Enables access to [Garfield++](https://garfieldpp.web.cern.ch/garfieldpp/) libraries in REST. Garfield code inside REST will be encapsulated inside `#if defined USE_Garfield` statements.
    * **SQL** (Default: OFF) : Enables the use of mysql libraries in REST. SQL code inside REST will be encapsulated inside `#if defined USE_SQL`.
    * **REST_EVE** (Default: ON) : Enables the use of libEve of ROOT which provides hardware accelerated 3D view of detector model and events within.

* REST Packages:
    * **REST_G4** (Default: OFF) : Adds executable `restG4` which carries out simulation with [Geant4++](http://geant4.web.cern.ch/) using REST style config file.
    * **REST_SQL** (Default: OFF) : Adds executable `restSQL` which allows to populate and update a SQL database extracting the metadata information from a REST file database.

* REST Libraries:
    * **REST_GEANT4_LIB** (Default: ON) : Enables the use of Geant4 event type and analysis processes in REST. It does not require *Geant4*. But it allows to access previous `restG4` generated data.
    * **REST_AXION_LIB** (Default: OFF) : Enables the use of Axion event type and Axion signal calculation processes in REST.
    * **REST_DETECTOR_LIB** (Default: ON) : Enables the use of Detector event type and event reconstruction processes in REST.
    * **REST_RAW_LIB** (Default: ON) : Enables the use of Raw signal event type and Raw signal conditioning processes in REST.
    * **REST_TRACK_LIB** (Default: ON) : Enables the use of Track event type and Track identification processes in REST.

To pass the options to cmake, one need to append "-DXXX=XXX" in the cmake command, for example: `cmake .. -DREST_WELCOME=OFF -DREST_G4=ON`. Once you explicitly set an option, your option choice will become the default choice for future `cmake` executions.

### Adding libraries

The concrete analysis tasks and experiment setups of REST are kept in individual libraries or packages. 
The main framework of REST only keeps general event types and analysis algorithms. We need to install
concrete library(package) to enable the workload, after installing REST mainbody.

Both `Library` and `Package` are kind of c++ projects that based on REST. The concept is to make 
that part of code envolve independently with REST framework, reducing the changing of REST framework.
They are usually kept in different git repositories. `Library` provides a library with new event types and 
analysis algorithms. `Package` provides not only libraries but also executables. They are installed to
REST installation path.

Some of the libraries/packages are provided as git submodule of REST, and can be installed together 
with REST framework, just by adding compilation options to cmake.

The following is a list of REST libraries/packages:

Name         | Type       |  cmake flag (=default)  | repository
-------------|------------|-------------------------|------------
restMuonLib  |   library  |                         | https://gitlab.pandax.sjtu.edu.cn/pandax-iii/restmuonlib
restDecay0   |   library  |   -DREST_DECAY0=OFF     | 
RestAxionLib |   library  |   -DREST_AXION_LIB=OFF  | https://lfna.unizar.es/iaxo/RestAxionLib
RestGeant4Lib|   library  |   -DREST_GEANT4_LIB=ON  | https://lfna.unizar.es/rest-development/RestGeant4Lib
restG4       |   package  |   -DREST_G4=OFF         | 
restP3DB     |   package  |                         | https://gitlab.pandax.sjtu.edu.cn/pandax-iii/restp3db
restSQL      |   package  |   -DREST_SQL=ON         | https://lfna.unizar.es/rest-development/restsql
restWeb      |   package  |                         | https://lfna.unizar.es/rest-development/restWeb

### Updating REST

We recommend updating REST to the latest version if possible. 
The remote branch "master" is always our target to synchronize the latest stable code.
The update can be done with:

```
cd ~/REST_v2  
git fetch origin master
git reset --hard FETCH_HEAD
cd build
cmake ..
make -j4 install
```

Note that if you changed the code, the **file modification** and **file deletion** will be reverted, but the **file creation** will be kept. 

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

### Trouble shooting

#### without git

You can directly download the source code from the website and install REST, with the same command
described [previously](#installing). Without git, you cannot see the version information on the 
welcome message. You cannot update or siwtch the version of REST either.

At some point if you want to enable the git access, you can type the following commands:

```
cd ~/REST_v2  
rm -rf .git
git init
git remote add origin git@lfna.unizar.es:rest-development/REST_v2.git
git checkout -b master
git fetch
git reset --hard origin/master
cd build
cmake ..
make -j4 install
```

This operation also updates your REST to the latest version.

#### Garfield not found

During cmake, sometimes it says cannot find Garfield. If you are not necessary with drift speed
and diffusion calculation functionality, you can turn it off: `cmake .. -DREST_GARFIELD=OFF`.

To use Garfield, one must set env "GARFIELD_HOME", "HEED_DATABASE" in the bash. He also needs
to add Garfield's library dir to "LD_LIBRARY_PATH". The garfield library "libGarfield.so" must 
be placed in "$GARFIELD_HOME/lib", and the garfield headers must be placed in "$GARFIELD_HOME/Include".
Garfield is also based on ROOT, and it must be compiled with same ROOT for REST.

Take a look at `installGarfield.sh` for more details.

#### cannot find -lGeom, -lGdml, -lSpectrum, -lEve, -lRGL

During compilation, if it reports error "/usr/bin/ld: cannot find -lXXX" of **more than five 
libraries**, this means your ROOT installation is incomplete. It is most possible that you 
are using the OS provided ROOT distribution, which lackes several libraries. Check the 
ROOT installaion with `root-config --libdir`. If the ROOT library directory is /usr/lib64/root, 
then it is the case. You need to manually install ROOT. If not, check also the output of
`cmake` command if it is using ROOT in the unwanted path. If so, source the correct thisROOT.sh,
clear the build dir, and re-run cmake and make.

#### cannot find -lGdml

During compilation, if it reports error "/usr/bin/ld: cannot find -lXXX" **including Gdml 
library**, this means your ROOT installation is incomplete. When installing ROOT, you 
must turn on the compilation flag for ROOT to generate gdml library, as in `installROOT.sh`: 
`cmake ../source -Dgdml=ON -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/install`

#### cannot find -lEve, -lRGL

During compilation, if it reports error "/usr/bin/ld: cannot find -lXXX" of **those two
libraries**, this means ROOT really lacks them. Sometimes the manual installed ROOT won't compile
those two libraries because the openGL libraries are not installed in the system. You may need 
to install at least mesa-libGL-devel and glew-devel (and/or xlibmesa-glu-dev and libglew1.5-dev) 
and re-install ROOT. Another solution is to disable eve libraries dependence in REST, by adding 
cmake flags like: `cmake .. -DREST_EVE=OFF`

#### error: ¡®set¡¯ was not declared in this scope

In the some releases of gcc, header <set> is added through a different include chain, and must
be manually added. Since REST 2.2.19 we fixed this problem. One can update the REST version or
switch to a different gcc version.

#### libtbb.so.2, needed by XXX/libImt.so, not found; undefined reference to `TParticle::Sizeof3D() const'

This happens when one wants to install REST with `sudo make install`. It will together report many 
similar messages. This is because the `LD_LIBRARY_PATH` is cleared when you temporary 
switched to administrator user. The solutions is: 

1. login as administrator to do all the operation. Remember to source the needed files before 
opeartion.

2. run `make` first. After compilation, run `sudo make install` to install.

#### undefined symbol XXX

If this happens during installation, this may be a bug of REST code. Contact the developers

If this happens when launching restManager, this may be a problem of ROOT libraries. 
We found that in some platform the ROOT binary are using unmatched version of interface of some system library. 
Try to complie ROOT manually, or change another ROOT binary distribution.

[**prev**](1-introduction.md)
[**contents**](0-contents.md)
[**next**](3-getting-started.md)
