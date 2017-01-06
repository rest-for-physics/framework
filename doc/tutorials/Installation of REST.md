\brief Instructions to setup your local environment, compile and install REST in your local system

## Preparing the environment

If you plan to install RESTsoft in your computer and work locally, you need to have installed the following software previously: [Git](https://git-scm.com/), [GEANT4](http://geant4.cern.ch/), [ROOT](https://root.cern.ch/), [Garfield++](https://garfieldpp.web.cern.ch/garfieldpp/) and some other external dependencies. The steps to do that are described here.

The following instructions assume you have a linux system with debian or ubuntu distribution.

### Install Git

By doing this:

\code
 sudo apt-get install git-core
\endcode

Or download an alternative git client and follow instructions [here](https://git-scm.com/downloads/guis).

### Install ROOT

Follow the instructions in the following links:

* [ROOT build prerequisites] (http://root.cern.ch/drupal/content/build-prerequisites).

* [Download ROOT] (http://root.cern.ch/drupal/content/downloading-root).

* [ROOT User-guide] (http://root.cern.ch/drupal/content/users-guide).

Usually it is a good option to install the latest consolidated release of ROOT. 

**Note:** REST has been successfully tested with the last ROOT5 version (*ROOT version 5.34/32*). Working with ROOT6 has not been tested yet.

REST uses [GDML] (http://gdml.web.cern.ch/GDML/) to encode geometry information. To enable import/export GDML files with ROOT you need to enable it before building ROOT, by using the option ''–enable-gdml'' when calling the ''./configure'' script:

\code
 ./configure -–enable-gdml
 make
\endcode

### Install GEANT4 (optional)

This step is necessary if you want to do MonteCarlo simulation using REST Geant4 package (restG4).

Follow instructions in the links:

* [GEANT4 general web page] (http://geant4.cern.ch/).

* [GEANT4 download page] (http://geant4.cern.ch/support/download.shtml).

* [GEANT4 installation guide] (http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/InstallationGuide/html/).

Geant 4 version >= 4.10.1.p02 is required due to compatibility problems with ROOT detected in previous versions of Geant4.

In order to activate the use of GDML, add the option -DGEANT4_USE_GDML=ON when installing Geant4 (in the call to cmake).
It is also recommended the addition of option -DGEANT4_USE_QT=ON

Finally, it might be interesting to set a variable for the Geant4 install since you will need it later on for the installation of REST (cmake). Add this line to your .bashrc file:

\code
 export G4_INSTALL_CMAKE="YOUR_G4_INSTALL_DIR/lib/Geant4.10.XX"
\endcode

### Install Garfield++

REST provides access to Garfield++ objects and some metadata structures as TRestGas use Garfield++ to generate gas properties using Magboltz. Follow instructions in the link, it is recommended to follow the alternative installation scheme using cmake which generates the dynamic libGarfield.so library:

*  [Garfield++ getting started] (http://garfieldpp.web.cern.ch/garfieldpp/getting-started/).

And define in your .bashrc file the following environmental variables (according to your installation):

\code
 #Garfield++ env variables
 export GARFIELD_HOME=/path/to/your/garfield/installation
 export HEED_DATABASE=$GARFIELD_HOME/Heed/heed++/database
 export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GARFIELD_HOME/lib
\endcode

### FFTW3 

FFTW3 is an external package that is used to perform Fast Fourier Transforms. See [the official FFTW page] (http://www.fftw.org/).
 
You can just install it by typing:
 
\code
 sudo apt-get install libfftw3-dev
\endcode

## Downloading and building REST

### Downloading REST from the Git repository

The [Git repository of REST](https://sultan.unizar.es:4443/) is hosted at the server at the University of Zaragoza. Git offers web access to the repository. You may access with your Git account credentials.

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

**Note:** if you get a request for a password here is because you didnt add your previoustly generated SSH key to your git profile. Go to the [REST Git web interface] (https://sultan.unizar.es:4443/), go to your profile, *profile settings* at the top-right corner, go to *SSH Keys* tab, and add the public SSH key you generated previously.

### Some tips on using Git

If you are new to Git, you may start reading this [simple Git introduction] (http://rogerdudler.github.io/git-guide/). You can find more info on Git usage at the [REST Git web interface] (https://sultan.unizar.es:4443/rest-development/REST_v2/wikis/rest-git) wiki pages.

### Building RESTsoft

The REST repository contains the source code of the REST libraries and programs that need to be compiled.

We assume in the following that the REST sources have been downloaded to a local path $REST_v2 in your home (substitute by your particular case in the following instructions). 

First of all, create a build directory wherever you want, here we assume it is done inside the $REST_v2 folder:

\code
 cd $REST_v2
 mkdir build
 cd build
\endcode

We then create the compilation environment, i.e., the REST install path. If we do not specify it, the default is <code>/usr/local/REST</code> (which requires admin privileges). We use cmake for this, with the following syntax:

\code
 cmake -DINSTALL_PREFIX=/full/path/to/install/destination/ $REST_v2
\endcode

Where *DINSTALL_PREFIX* will be the directory where headers, libraries and other required files will be installed. You can choose for example $REST_v2/install. Since *../* points to $REST_v2, you could execute

\code
 cmake -DINSTALL_PREFIX=$REST_v2/install ../
\endcode

being $REST_v2 the full path directory pointing to your REST_v2 directory. It is important that **full path** is specified in the INSTALL_PREFIX definition.

After this is done, you can launch the compilation of REST,

\code
 make -j4
\endcode

and install of REST at the previously specified *INSTALL_PREFIX*,

\code
 make install
\endcode

**Note :** You will need to have write access to the final installation destination.


Finally, you must add the following line to your .bashrc so that REST environment is loaded 

\code
 source $REST_v2/install/thisREST.sh 
\endcode

Where $REST_v2 must be sustituted by the full path pointing your REST directory. 

Then restart a new terminal to load REST.

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
 cmake -DGeant4_DIR="$G4_INSTALL_CMAKE" $REST_v2/packages/restG4
\endcode

Where <code>$G4_INSTALL_CMAKE</code> is the path to the cmake package configuration file of Geant4 (you may have defined it previously - see above).

For example:

\code
 cmake -DGeant4_DIR=/usr/local/geant4.10.02.b01-install/lib/Geant4-10.2.0/ ../
\endcode

** Note : ** In LXPLUS problems have been experienced because the default compiler is CC. To define the compiler by hand you may use an additional option in cmake compilation

\code
 -DCMAKE_CXX_COMPILER=g++
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
