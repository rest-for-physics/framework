## Installing REST

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