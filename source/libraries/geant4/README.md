# The REST Geant4 Library

TOBE written

This file should include basic information to understand the main use trend of the library. It should include a brief description of this class. What is intended for, features, functionalities, connection to other REST libraries, etc. And a description of the main use of this library.
    
NOTE: The use of this library should be documented through examples and doxygen class description.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine.

TODO: Add a description and explain that the recommended way is to install it as a git submodule in RestFramework project.

Additionaly you may download a copy of this library cloning it using the corresponding git command.

```
git clone git@lfna.unizar.es:rest/RestGeant4Lib.git
```

### Prerequisites

The only mandatory prerequisite is that REST and ROOT6 are installed in your system. Details on the installation of ROOT will be found at the [ROOT's official site](root.cern.ch). 
To avoid problems during the installation it is recommended that REST libraries will be compiled using the same version of g++ compiler used to compile ROOT and REST.


### Installing

After ROOT6 and REST haven been installed in the system, the compilation of this REST library should be straight forward. 
Before starting the installation process make sure you are running the desired ROOT version and binary,

```
root-config --version
which root
```

and the corresponding REST version and binary,

```
rest-config --version
which restRoot
```

The cmake compilation environment will recognize those and use them to link to your ROOT installation.

Then, go to the root directory of your local REST repository, lets name it here `REST_SOURCE_PATH` and execute the following commands.

```
cd THIS_LIB_SRC_PATH
mkdir build
cd build
cmake ../
make -j4 install
```

After all the compilation and installation process ends, you will end up with a new installed REST library inside `REST_PATH/lib/`.

Three other directories, data, macros and examples - common to any REST library - will be installed.

### Basic tests of this library

You can test now that REST libraries are loaded without errors inside the ROOT environment using the `restRoot` command. After launching `restRoot` you should see a message on screen similar to the following one, where the recently compiled library will be found in the list of loaded libraries.

```
   ------------------------------------------------------------
  | Welcome to ROOT 6.14/06                http://root.cern.ch |
  |                               (c) 1995-2018, The ROOT Team |
  | Built for macosx64                                         |
  | From tags/v6-14-06@v6-14-06, Dec 11 2018, 12:58:00         |
  | Try '.help', '.demo', '.license', '.credits', '.quit'/'.q' |
   ------------------------------------------------------------

Loading library : libRestGeant4.dylib
Loading library : libRestFramework.dylib
Loading library : libRestDetector.dylib
```

### A brief description of the examples provided

### Compilation options

TO define any library compilation options if necessary.

Different options can be passed to the `cmake` command to personalize the REST installation. The following options are available.

* **INSTALL_PREFIX** (Default: REST_PATH) : Allows to define the destination of the final REST install directory.
* **REST_DUMMY** (Default: ON) : A dummy option that might be defined in the CMakeLists.

## License

This project is licensed under the GNU License - see the [LICENSE](https://lfna.unizar.es/rest-development/REST_v2/blob/master/LICENCE) file for details

## Acknowledgments

`TOBE written`

* Hat tip to anyone whose code was used
* Inspiration
* etc
