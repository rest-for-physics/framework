# The REST Framework

[![DOI](https://zenodo.org/badge/324291710.svg)](https://zenodo.org/badge/latestdoi/324291710)

The REST-for-Physics (Rare Event Searches Toolkit) Framework is mainly written in C++ and it is fully integrated with [ROOT](https://root.cern.ch) I/O interface.
REST was initially born as a collaborative software effort to provide common tools for acquisition, simulation, and data analysis of gaseous Time Projection Chambers (TPCs). However, the framework is already extending its usage to be non-exclusive of detector data analysis. The possibilities of the framework are provided by the different libraries and packages written for REST in our community.

The REST Framework provides 3 interfaces that prototype the use of **event types**, **metadata** and **event processes** through `TRestEvent`, `TRestMetadata` and `TRestEventProcess` abstract class definitions.
Any REST library will implement **specific objects** that inherit from those 3 basic interfaces. 

Different **event processes** can be combined to build complex event processing chains with full traceability. 
The **metadata** objects will allow us to provide input parameters or information to the framework using a XML-like format.
REST integrates a special **metadata** object named `TRestManager` that encapsulates all the required information to launch the processing of a particular data chain.
REST will produce output using ROOT format. Any REST file will always contain a `TRestRun` metadata object.
`TRestRun` is a **metadata** object responsible to encapsulate and give access to all the objects stored inside the REST/ROOT file; 
i.e. the **specific** resulting `TRestEvent` output, the `TRestAnalysisTree`, and any **specific** `TRestMetadata` object used during a processing chain.

This framework provides additionally different interfaces to **browse data**, `TRestBrowser`, **event visualization** `TRestEventViewer`, define a **event data processing** infraestructure, `TRestProcessRunner`, **event analysis and metadata plotting**, `TRestAnalysisPlot` or `TRestMetadataPlot`, a common access **analysis tree** based on `TTree` ROOT object, `TRestAnalysisTree`, and centralizing the use of REST through a manager `TRestManager` are few of the features the framework offers when used standalone.

Other objects included in the framework will help to add unit definitions, `REST_Units`, define physical constants and basic physical routines, `REST_Physics` or access to geometrical calculations, `TRestMesh`. Additional objects provide methods to help on text formatting as `TRestStringHelper` or define output styles, `TRestStringOutput`.

Basic pure analysis tasks will also be included in this framework, such as a processes performing fundamental routines, such as performing generic fits on observables/branches found inside the analysis tree, producing a summary report, creating data quality rules definitions, or basic interfaces to external databases.

## Getting Started

This repository, *rest-framework*, centralizes all the activity or software ecosystem in the form of REST libraries, packages and projects. All official projects associated to `REST-for-Physics` will be usually linked to this repository as a **git submodule**. Cloning this repository and following these instructions will let you obtain all the REST libraries and other modules that are publicly accessible, and in addition those submodules where your GitHub user account was granted access.

These instructions will get you a copy of the project up and running on your local machine. Additional details can be found in the [documentation](doc/Chapters/2-installing-rest.md) 

### Mirror repositories

REST is mirrored to the following repositories where pipelines are executed, and where code can also be retrieved.

- https://gitlab.cern.ch/rest-for-physics
- https://lfna.unizar.es/rest-for-physics

Code can be pulled for read-access from those mirrors, however, development is centralized at the main GitHub public repository.

### Downloading REST
The recommended way to download a copy of REST will be *cloning it* using the corresponding git command.

The following commands will download the master branch to the `rest-framework` directory including all public submodules (libraries, packages, etc).

```
cd $HOME
mkdir git
cd git
git clone https://github.com/rest-for-physics/framework.git rest-framework
cd rest-framework
## It is known to work with python version 3.5 but feel free to use a later version.
python3.5 pull-submodules.py
```

If you have pulled changes in a particular submodule, or added your own commits, be aware that calling again to `python3.5 pull-submodules.py` will bring the state of submodules to the official ones at the main repository. REMOVING! any commits you may have at the submodule and that were not pushed to a remote.

If you have access to private repositories, related to projects or experiments inside the REST community you may pull those executing an additional command.

```
python3.5 pull-submodules.py --lfna (or --sjtu)
```

on top of that, you might get the latest state of each submodule by executing

```
python3.5 pull-submodules.py --lfna --latest
```

But, if you wish to remain at the reference/official release, and get the latest state from a particular submodule, it is possible to move to the given submodule and checkout its `master` branch.

```
cd source/libraries/xlib
git checkout master
git pull
```

### Recovering a clean git state of rest-framework and submodules

If you have added modifications to the rest-framework code and/or submodules. It is possible to fully clean your repository to be an identical copy to the one found at the remote repository.

Notice that executing the following script (placed at the rest-framework root repository) will completely remove any changes or addons you have done at your local installation.

```
python3.5 pull-submodules.py --clean
```

If there is any untracked content you might still need to `git clean -d -f` to remove untracked items. If any untracked items or modified files are found at your source repository during compilation the `fCleanState` flag at `TRestMetadata` will be set to `OFF`.

### Installing

The only mandatory prerequisite of REST is ROOT6. Details on the installation of ROOT will be found at the [ROOT's official site](https://root.cern.ch). 
One may directly find binary distributions on its [download page](https://root.cern.ch/downloading-root), although compiling and installing on your system from source is recommended since it is important that ROOT and REST have been compiled for the same system architecture and compiler version.

Before starting the REST installation, make sure you are running the desired ROOT version and binary.

```
root-config --version
which root
```

After ROOT6 has been installed in the system, the compilation of REST should be straight forward. 
Note that it is recommended to compile REST using the same version of g++ compiler used to compile ROOT.
The detailed [installation guide](doc/Chapters/2-installing-rest.md), including compilation options, will be found at the [REST documentation](doc/Chapters/0-contents.md).

Go to the root directory of your local REST repository, assume it is `~/rest-framework`, and execute the following commands.

```
cd ~/rest-framework
mkdir build
cd build
cmake .. -DREST_ALL_LIBS=ON -DREST_G4=ON -DREST_GARFIELD=ON -DINSTALL_PREFIX=../install/ 
make -j4 install
```

If you do not need to link REST to `Geant4` and/or `Garfield` funtionalities, just set the compilation options to `OFF` or do not include them at the `cmake` command.

**Remark**: If there is no structural changes in REST, such as new files or libraries, then `cmake` command needs to be executed only once. If you modify a source file and exectue `make -j4 install` the compilation system will recognize that it requires to compile that file. If you execute `cmake` again, then the full project will be compiled from scratch.

We collected some [common problems](doc/Chapters/2-installing-rest.md#trouble-shooting) one may get into while installing REST.

After all the compilation and installation process ends, you will end up with an installed REST version at `~/REST_v2/install/`. Test it now by doing:

```
source ../install/thisREST.sh
rest-config --welcome
rest-config --version
restRoot
```

**IMPORTANT**: Execute the following command to configure your `.bashrc` to load REST in your system environment each time you open a new shell terminal.

 ```
 echo "source ~/REST_v2/install/master/thisREST.sh" >> .bashrc
 ```
 
### Troubleshooting

- One of the major bottlenecks on REST installation is ROOT installation itself. It is recommended to compile ROOT from source. To help on this task we provide the following script [installROOT.py](scripts/installation/installROOT.py).
- We recommend to have a look or just execute the script [installRequiredSoftware.py](scripts/installation/installRequiredSoftware.py) at Debian based Linux distributions, to install recommended software.
- If ROOT compilation complains about Python libraries, you may need to install python development libraries in your system, and/or update your default python to V3. The following executions should help:
```
 sudo apt-get install libpython3.7-dev
 sudo update-alternatives --install /usr/bin/python python /usr/bin/python3 1
 ```


## REST libraries

The REST framework provides only the structure and support to create and use REST libraries. Few official REST libraries are maintained by the REST community at the [REST-for-Physics](https://github.com/rest-for-physics) namespace. Please, refer to the respective repositories and README.md documentation to get more insights about the features and functionalities of each library.

By listing the contents of the *library* directory inside `rest-framework` (once you executed `pull-submodules.py`) you will quickly identify the available libraries. In order to enable a particular library, just get the library directory name, and use it to define a compilation flag as `-DRESTLIB_NAME`.

For example, in order to compile REST including the `detector` and `raw` libraries, you should update the compilation system set-up by moving again to the build directory and executing:

```
cd build
cmake .. -DRESTLIB_DETECTOR=ON -DRESTLIB_RAW=ON
make -j4 install
```

**Remark:** Notice that once we pass an option to cmake, that option will be cached inside the cmake system. I.e. we do not need to provide the installation path we provided the first time, and any future calls to `cmake` will assume `detector` and `raw` libraries are enabled.

## Useful links or references

- A REST overview guide can be found [here](doc/Chapters/4-the-rest-framework.md). 
- An API doxygen documentation is frequently updated [here](https://sultan.unizar.es/rest/).
- The REST Framework forum for open discussions is available [here](ezpc10.unizar.es).
- ROOT naming convention and coding rules are [Taligent rules](https://root.cern/TaligentDocs/TaligentOnline/DocumentRoot/1.0/Docs/books/WM/WM_63.html#HEADING77).

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) to get some guidelines on how to contribute to this project.
Before any contribution, those guidelines must be assimilated and accepted. 
In any case, changes, improvements, or addons, to [CONTRIBUTING.md](CONTRIBUTING.md) are aceptable after proposal and discussion with other authors at the [REST Framework forum](https://ezpc10.unizar.es/).

The framework exploits the Git tagging system to produce its own versioning system. It is important to emphasize that the REST framework centralizes the versioning of all the submodules (libraries, packages, ...) that it contains. Details on how the REST version number is produced are given in [CONTRIBUTING.md](CONTRIBUTING.md).

## Versioning

Any **metadata** object written with REST **will be stamped** with few metadata members that will allow to identify the state of the code when the object was produced. Those data members are:
  - *fVersion*: A string containning the human version number.
  - *fCommit*: The latest commit hash value when the compilation took place.
  - *fLibraryVersion*: The human version library. It is fixed by CMakeLists at the library submodules.
  - *fOfficialRelease*: It will be true if the commit was tagged at the repository.
  - *fCleanState*: It will be true if there are no local modifications (including submodules). To remove any local modifications and recover a clean state we may execute `source clean-state.sh` at the project root.

If different REST versions were used to write a ROOT file, e.g. at different steps of the data processing chain, the historic metadata objects will preserve their original version.
However, the `TRestRun` metadata object **will always store** the version used to write the ROOT file.

After REST release 2.2.1., REST implements correctly the `ROOT schema evolution`. Therefore, any new REST version should always be backwards compatible.
I.e. Any file written after v2.2.1 should be readable without problems with any future version.

A major change at 2.3 will prevent from backwards compatibility, since class names have been reviewed.

## Main authors and contributing institutions

* **Javier Galan, Igor G. Irastorza, Gloria Luzon** - *University of Zaragoza (Spain)*
* **Ke Han, Kaixiang Ni** - *Shanghai Jiao Tong University (China)*
* **Yann Bedfer, Damien Neyret** - *CEA Saclay (France)*
* **Eduardo Picatoste, Cristian Cogollos** - *Universitat de Barcelona (Spain)*

See also the list of [contributors]() who participated in this project.

## Publications

- PandaX-III: Searching for neutrinoless double beta decay with high pressure 136Xe gas time projection chambers. [X. Chen et al., Science China Physics, Mechanics & Astronomy 60, 061011 (2017)](https://doi.org/10.1007/s11433-017-9028-0) [arXiv:1610.08883](https://arxiv.org/abs/1610.08883).
- Background assessment for the TREX Dark Matter experiment. [Castel, J., Cebrián, S., Coarasa, I. et al. Eur. Phys. J. C 79, 782 (2019)](https://doi.org/10.1140/epjc/s10052-019-7282-6). [arXiv:1812.04519](https://arxiv.org/abs/1812.04519).
- Topological background discrimination in the PandaX-III neutrinoless double beta decay experiment. [J Galan et al 2020 J. Phys. G: Nucl. Part. Phys. 47 045108](https://doi.org/10.1088/1361-6471/ab4dbe). [arxiv:1903.03979]( https://arxiv.org/abs/1903.03979).

## License

This project is licensed under the GNU License - see the [LICENSE](https://lfna.unizar.es/rest-development/REST_v2/blob/master/LICENCE) file for details

## Acknowledgments

We acknowledge support from the the European Research Council (ERC) under the European Union’s Horizon 2020 research and innovation programme, grant agreement ERC-2017-AdG788781 (IAXO+), and from the Spanish Agencia Estatal de Investigaci\ ́on under grant FPA2016-76978-C3-1-P



