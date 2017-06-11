\brief Instructions to setup your environment, compile and install REST in your local system

The following instructions assume you have a linux system with ubuntu or debian based distribution. This will basically affect only to the installation of required packages using apt-get command.

## Downloading REST from the Git repository

The [Git repository of REST](https://sultan.unizar.es:4443/) is hosted at the server at the University of Zaragoza. Git offers web access to the repository, that you may access with your Git account credentials.

To be able to download the code in your local machine you will need to install a [Git](https://git-scm.com/) client in your local machine, by doing

\code
sudo apt-get install git
\endcode

or downloading an alternative git client following the instructions [here](https://git-scm.com/downloads/guis).

**Note:** If you are new to Git, you may start reading this [simple Git introduction] (http://rogerdudler.github.io/git-guide/). You can find more info on Git usage at the [REST Git web interface] (https://sultan.unizar.es:4443/rest-development/REST_v2/wikis/rest-git) wiki pages.

To get access to the repository via SSH you need to generate an SSH key at your local machine you need to follow [these instructions] (https://git-scm.com/book/en/v2/Git-on-the-Server-Generating-Your-SSH-Public-Key). Then, add the SSH key to your Git account through the Git web interface.

You have to set up your git client to recognize your credentials 

\code
 git config --global user.name "your name"
 git config --global user.email "your e-mail"
\endcode

And, finally, clone the repository in a local folder: 

\code
 git clone git@sultan.unizar.es:rest-development/REST_v2.git
\endcode

You will have downloaded the code into a new folder named <code>REST_v2</code>, and you should be now in the master branch

\warning if you get a request for a password here is because you didnt add your previoustly generated SSH key to your git profile. Go to the [REST Git web interface] (https://sultan.unizar.es:4443/), go to your profile, *profile settings* at the top-right corner, go to *SSH Keys* tab, and add the public SSH key you generated previously.

## Preparing the environment

If you plan to install RESTsoft in your computer and work locally, you need to have installed the following software previously: [GEANT4](http://geant4.cern.ch/), [ROOT](https://root.cern.ch/), [Garfield++](https://garfieldpp.web.cern.ch/garfieldpp/) and some other external dependencies. The steps to do that are described here.

A script exists in the directory <code> REST_v2/scripts/install/ </code> allowing to install required software and libraries to compile *Geant4*, *ROOT* and *Garfield*.

\code
cd REST_v2/scripts/install/
./installRequiredSoftware.sh
\endcode

\warning By using this script you will be prompted for a password, in order to gain the rights to install packages on your system.

\warning If you are not using ubuntu/debian, or apt-like software management, you will need to check the packages being installed by the installRequiredSoftware.sh, and install them following your system packaging system.

## Quick ROOT, Geant4 and Garfield installation

If you installed all the required software in the previous step, and if you are feeling lucky, you can try to launch the following three scripts to install ROOT, Garfield++ and Geant4. These script will always install the recommended version to be used with REST.

\code
./installROOT.sh
source $HOME/apps/root_v5.34.32/install/bin/thisroot.sh
./installGarfield.sh
./installGeant4.sh
\endcode

If you succeed to execute these scripts without any error, you will end up with the installation of ROOT, Garfield, and Geant4 under your $HOME/apps.

You should add the following lines to your $HOME/.bashrc file,

\code

source /programas/root34.32/root/bin/thisroot.sh

export GARFIELD_HOME=$HOME/apps/garfield
export HEED_DATABASE=$GARFIELD_HOME/Heed/heed++/database
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GARFIELD_HOME/lib

source $HOME/apps/geant4_10_02_p02-install/bin/geant4.sh

\endcode

then re-launch your terminal. You can directly proceed to the compilation and installation of REST, described in the section "Building and installing REST".

### Install ROOT

The installation of ROOT 5.34/32 with GDML support (and gcc compiler version 4.8) is the recommended setup to be used with REST.

#### Option 1. Using the installation script provided in the repository (recommended)

A script definning the recommended compiler and ROOT versions, and compilation options (as i.e. GDML) can be used to install ROOT in our system. By default this script will install ROOT in our $HOME, at $HOME/apps directory.

\code
cd REST_v2/scripts/install/
./installROOT.sh
\endcode

once ROOT has been installed you will need to add the following line to your <code>.bashrc</code> file.

\code

source $HOME/apps/root_5.34.32/install/thisROOT.sh

\endcode

Now restart your terminal, and verify that root has been properly installed by executing

\code
root
.q
\endcode

#### Option 2. Manual installation

Alternativelly you can follow the instructions found at the official site:

* [ROOT build prerequisites] (http://root.cern.ch/drupal/content/build-prerequisites).

* [Download ROOT] (http://root.cern.ch/drupal/content/downloading-root).

* [ROOT User-guide] (http://root.cern.ch/drupal/content/users-guide).

REST uses [GDML] (http://gdml.web.cern.ch/GDML/) to encode geometry information. To enable import/export GDML files with ROOT you need to enable it before building ROOT, by using the option <code>-enable-gdml</code>.

**Note:** It is a good option to install the latest consolidated release of ROOT. For the moment, REST has been successfully tested with the last ROOT5 version (*ROOT version 5.34/32*). Working with ROOT6 has not been tested yet.

\warning You should choose **cmake** compilation scheme in ROOT so that REST compilation recognizes the ROOT installation.

### Install Garfield++

REST provides access to Garfield++ objects and some metadata structures as TRestGas use Garfield++ to generate gas properties using Magboltz. You can follow the official instructions provided in this link.
*  [Garfield++ getting started] (http://garfieldpp.web.cern.ch/garfieldpp/getting-started/).

\warning It is mandatory to follow the alternative installation scheme using cmake which generates the dynamic libGarfield.so library:

It is **highly recommended** to install the release <code>v1r0</code> that can be downloaded by executing:

\code
svn co http://svn.cern.ch/guest/garfield/tags/v1r0 $GARFIELD_HOME
\endcode

And define in your .bashrc file the following environmental variables (according to your installation):

\code
 #Garfield++ env variables
 export GARFIELD_HOME=/path/to/your/garfield/installation
 export HEED_DATABASE=$GARFIELD_HOME/Heed/heed++/database
 export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GARFIELD_HOME/lib
\endcode

You can use the script <code> REST_v2/scripts/install/ </code> to launch the Garfield++ download, compilation and installation. In this case Garfield will be installed by default at $HOME/apps/garfield (which will be your GARFIELD_HOME).

\code
cd REST_v2/scripts/install
./installGarfield.sh
\endcode

\warning Make sure you load the Garfield++ env variables in your environment by openning a new terminal.

### Install Geant4 (optional)

This step is necessary if you want to do MonteCarlo simulation using REST Geant4 package (restG4).

Follow instructions in the links:

* [Geant4 general web page] (http://geant4.cern.ch/).

* [Geant4 download page] (http://geant4.cern.ch/support/download.shtml).

* [Geant4 installation guide] (http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/InstallationGuide/html/).

Geant 4 version >= 4.10.1.p02 is required due to compatibility problems with ROOT detected in previous versions of Geant4.

In order to activate the use of GDML, add the option -DGEANT4_USE_GDML=ON when installing Geant4 (in the call to cmake).
It is also recommended the addition of option -DGEANT4_USE_QT=ON

#### Using the installation script in the repository to install Geant4

You can use the script <code> REST_v2/scripts/install/installGeant4.sh </code> to launch the Geant4 download, compilation and installation. This script will install Geant4.10.02p02 version under <code>$HOME/apps/</code>.

\code
cd REST_v2/scripts/install/
./installGeant4.sh
\endcode

After the installation you should add the following line to your <code> .bashrc </code> file.

\code
source $HOME/apps/geant4_10_02_p02-install/bin/geant4.sh
\endcode

\warning Make sure you load the Geant4 env variables in your environment by openning a new terminal.

## Building and installing REST

The REST repository contains the source code of the REST libraries and programs that need to be compiled.

We assume in the following that the REST sources have been downloaded to a local path $REST_v2 in your home (substitute by your particular case in the following instructions). 

First of all, create a build directory wherever you want, here we assume it is done inside the $REST_v2 folder:

\code
 cd $REST_v2
 mkdir build
 cd build
\endcode

We then create the compilation environment using cmake (i.e., we define the REST install path, the compiler version, etc). If we do not specify the installation path, the default is <code>/usr/local/REST</code> (which requires admin privileges). We use cmake for this, with the following syntax:

\code
 cmake -DCMAKE_CXX_COMPILER=g++-4.8 -DINSTALL_PREFIX=/full/path/to/install/destination/ $REST_v2
\endcode

Where *DINSTALL_PREFIX* will be the directory where headers, libraries and other required files will be installed. You can choose for example $REST_v2/install. Since *../* points to $REST_v2, you could execute

\code
 cmake -DCMAKE_CXX_COMPILER=g++-4.8 -DINSTALL_PREFIX=$REST_v2/install ../
\endcode

being $REST_v2 the full path directory pointing to your REST_v2 directory.

\warning It is **important** that full path to installation is specified in the INSTALL_PREFIX definition.

After this is done, you can launch the compilation of REST,

\code
 make -j4
\endcode

and install of REST at the previously specified *INSTALL_PREFIX*,

\code
 make install
\endcode

\warning You will need to have write access to the final installation destination.


Finally, you must add the following line to your .bashrc so that REST environment is loaded 

\code
 source $REST_v2/install/thisREST.sh 
\endcode

Where $REST_v2 must be sustituted by the full path pointing your REST directory. 

Then restart a new terminal to load REST. If everything went fine you should be able to launch a root session with REST libraries/scripts loaded,

\code
restRoot
\endcode

and a root terminal should open without any message errors.

If you reached this point you have REST libraries and binaries operational... Congratulations!


### "REST packages" compilation: restG4

To do REST packages installation you should have already installed REST and loaded it in your environment by sourcing thisREST.sh.
 
Now you need to compile "REST packages" that can be found under the <code>$REST_v2/packages</code> directory. At the moment the sole example is <code>restG4</code>.

As before, you need to create a build directory:

\code
 cd $REST_v2/packages/restG4
 mkdir build
 cd build
\endcode

The geometry and macros are copied to this directory every time you execute <code>make</code>. The geometry files should be considered as a template. You should produce your own geometry file (so that it is not overwritten in case you do a <code>make</code>). And give to the <code>config/simulation.rml</code> file inside REST_v2 the name of the geometry file you will be using (i.e. <code>myGeometry.gdml</code>).

Now execute <code>cmake</code> using the Geant4 installation path and the restG4 source path:

\code
 cmake -DCMAKE_CXX_COMPILE=g++-4.8 -DGeant4_DIR="$G4_INSTALL_CMAKE" $REST_v2/packages/restG4
\endcode

Where <code>$G4_INSTALL_CMAKE</code> is the path to the cmake package configuration file of Geant4 

For example (if you used ./installGeant4.sh) you should do:

\code
 cmake -DCMAKE_CXX_COMPILE=g++-4.8 -DGeant4_DIR=$HOME/apps/geant4_10_02_p02-install/lib/Geant4-10.2.0/ ../
\endcode

If <code>cmake</code> executed with no errors you should be able now to execute <code>make</code> to build the restG4 executable

\code
 make -j4
\endcode

And to install it in <code>$REST_v2/install</code>:

\code
 make install 
\endcode

<code> restG4 </code> executable should be now available.
