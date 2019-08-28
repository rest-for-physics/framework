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

* **INSTALL_PREFIX** : Allows to define the destination of the final REST install directory. The default value is either "REST_v2/install/" (if you haven't installed REST) or the current REST path (if you already installed REST).
* **REST_WELCOME** (Default: ON) : If dissabled no message will be displayed each time we call thisREST.sh.
* **REST_GARFIELD** (Default: OFF) : Enables access to [Garfield++](https://garfieldpp.web.cern.ch/garfieldpp/) libraries in REST.
* **REST_G4** (Default: OFF) : Adds executable `restG4` which carries out simulation with [Geant4++](http://geant4.web.cern.ch/) using REST style config file.

To pass the options to cmake, one need to append "-DXXX=XXX" in the cmake command, for example: `cmake .. -DREST_WELCOME=OFF -DREST_G4=ON`.
Note: Once you explicitly set an option, the default value of this option will be changed to the new value when you run `cmake` in the next time.

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

#### go without git

You can directly download the source code from the website and install REST.
In this case the git functionality is disabled, and you cannot update/siwtch the version of REST.
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

#### cannot find -lXXX

Sometimes during compilation ld will report error like "/usr/bin/ld: cannot find -lEve". 
This could be due to the partial installation of ROOT, which lacks libEve.so, libMathMore.so, libGdml.so, etc. 
This problem is found in some linux distributions where ROOT6 is available as packages.
First confirm the missing by finding the library file in ROOT library directory, which can be found with `root-config --libdir`. 
If so, switch another installation of ROOT.

After this, we need to clear build path and re-run the previous command.

#### undefined symbol XXX

If this happens during installation, this may be a bug of REST code. Contact the developers

If this happens when launching restManager, this may be a problem of ROOT libraries. 
We found that in some platform the ROOT binary are using unmatched version of interface of some system library. 
Try to complie ROOT manually, or change another ROOT binary distribution.




































We first start with the installation of REST. First check the environment. The followings are some 
necessary development tools on linux. 

* unzip
* cmake
* g++
* gcc
* python
* git
* subversion

Most of the Linux distributions should already contain these softwares. One can check if they are installed
by typing their names directly in bash(for subversion type "svn"). If not installed, use `sudo apt-get install ...`
or `sudo yum install ...` to install them.

### Dependency

The only dependence of REST(mainbody) is

* ROOT6

Its [official website](https://root.cern.ch/downloading-root) provides downloads of binaries for most of
the Linux system. If not, try to manually compile it. After finishing installing ROOT6, we can directly 
install REST, just by using cmake.

For those who is new to git commands, we recommend using the python script "scriptsGUI.py". It gives a 
GUI wizard for installation and updating. The package

* python-tk (or tkinter in centos)

is needed. To install it, use `sudo apt-get install python-tk` or `sudo yum install Tkinter`. Then,
to launch the installer, change to directory ./scripts and call this script by `./scriptsGUI.py`. Note 
that python should be in version 2.7. If the default python is python3, try `python2.7 scriptsGUI.py`.

![alt](Image/installer.png)

### Add-ons

Here in the GUI we can see some optional checkboxes like restGas, restDataBaseImpl, restG4. They provides 
extra functionality upon the fundamental REST mainbody. They depends on different 

option | dependency | Description
-------------|------------|-----------------
restGas          | Garfield (compiled with ROOT6)    | Enables gas simulation (drift speed, diffusion), gas file reading/saving/generation for TRestGas class. Therefore TRestGas can provide gas information for processes.
restDataBaseImpl | PostgreSQL (tested version: 9.5)  | Adds a library libRestDataBaseImpl. Provides access to pgsql database on local machine. TRestRun will be able to find input files from database. restG4 will be able to record its run in the database.
restG4           | geant4                            | Adds a executable restG4. Generates events and saves them in REST data format. Reads configuration from REST's rml file. 

### Using the GUI

After checking the needed options, we can click on "update!" or "install!" button on the window.
Sometimes during update/installation, a window will show up asking for gitlab account. The account will
be used in linking the authorized-only git repository. Note that during update, any modified local files 
will be overwritten. 

After the installation has finished, the user needs to source the shell script before using REST 
software. The shell script is in the installation path named "thisREST.sh". It is recommended to add
a line "source .../thisREST.sh" in user's ".bashrc" file. Finally after restarting the bash, REST will
be ready to use.

Check if REST is successfully installed by typing `rest-config --welcome` in the command line. A welcome
message should show up. Then type `restManager` to check if the exectuable can run. The usage of 
restManager will be shown if all succeeds.

### Trouble shooting

#### TKinter is not available

Sometimes we have to install REST without GUI. First make a directory "build" in REST main directory 
and then enter it. Then run "cmake" followed by "make":

:REST_v2$ `mkdir build`  
:REST_v2$ `cd build`  
:build$ `cmake ..`  
:build$ `make install`

If all proceed normally, REST will be installed in the directory "install" in the source directory. 
We provide multiple cmake flags for the users:

* **-DINSTALL_PREFIX=(install path)** : change the installation path of REST  
* **-DREST_WELCOME=(ON/OFF)** : switch on/off the welcome message when logging in. The message contains 
information about the versions, install path, install date, etc. Default is on.  
* **-DREST_Garfield=(ON/OFF)** : switch on/off the garfield accessibility. Default is off.  
* **-DREST_G4=(ON/OFF)** : switch on/off the installation of restG4. Default is off.  
* **-REST_DATABASE=(ON/OFF)** : switch on/off the installation of restDataBaseImpl. Default is off.  
* **-DREST_SE=(ON/OFF)** : switch on/off the ROOT auto schema evolution(on means, generating a linkdef.h 
for each class). We don't need to care about this. Default is on.  

For example, to install REST in softwares/REST in your home directory, one can type:

:build$ `cmake .. -DINSTALL_PREFIX=~/softwares/REST/`

restG4 and restDataBaseImpl can be installed after REST mainbody installation and `source thisREST.sh`
We just switch to their main directory and do similar. 

:REST_v2$ `cd packages/restG4/`  
:restG4$ `mkdir build`  
:restG4$ `cd build`  
:build$ `cmake ..`  
:build$ `make install`  

#### ROOT library missing

Sometimes during compilation ld will report error like "/usr/bin/ld: cannot find -lEve". This is because
ROOT is not installed properly. In some operation system the self-contained ROOT6 lacks some library, 
including libEve.so, libMathMore.so, libGdml.so, etc. First confirm the missing by checking those files in 
ROOT's library directory, which can be found with `root-config --libdir`. If so, we need to install ROOT 
manually. Turn to their official website for more information.

After this, we need to clear build path and re-run the previous command.

#### Cannot update with the python script

Maybe you have typed a wrong password. Check the corresponding output.

Some servers forbids git to use https connection to online repository. This will cause the GUI installer
to fail. Ask administrator to open the premission, or just use ssh conncection, manually. 

[**prev**](1-introduction.md)
[**contents**](0-contents.md)
[**next**](3-getting-started.md)