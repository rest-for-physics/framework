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

If you are asked for a password, this is because you did not add your local computer account public ssh key to your Gitlab account. 
Check the following [instructions](https://git-scm.com/book/en/v2/Git-on-the-Server-Generating-Your-SSH-Public-Key) to generate your key.
Then, paste the key into your GitLab account (top-right icon of Gitlab site), go to "settings", and access the "ssh keys" in the left menu.

We provide mirror repositories for users in different location. If you find it slow or unreachable
to the previous address, try with the alternatives:

```
git clone https://gitlab.pandax.sjtu.edu.cn/pandax-iii/REST_v2.git
```

As soon as REST is under strong development phase the repository will be private, and access to the REST repository will be only granted on demand.
Before granting access, an account must be registered at the [Unizar Gitlab site](https://lfna.unizar.es). 
Then, you will need to contact the authors to request access to the code.

### Installing

The only mandatory prerequisite of REST is ROOT6. Details on the installation of ROOT will be found at the [ROOT's official site](https://root.cern.ch). 
One may directly find binary distributions on its [download page](https://root.cern.ch/downloading-root). 
If not, try to compile and install it manually.

Before starting the REST installation, make sure you are running the desired ROOT version and binary.

```
root-config --version
which root
```

After ROOT6 has been installed in the system, the compilation of REST should be straight forward. 
Note that it is recommended to compile REST using the same version of g++ compiler used to compile ROOT.
The detailed installation guide could be found in [REST documentation](doc/Chapters/2-installing-rest.md).

Go to the root directory of your local REST repository, assume it is `~/REST_v2`, and execute the following commands.

```
cd ~/REST_v2  
mkdir build
cd build
cmake .. -DINSTALL_PREFIX=../install/master/ 
make -j4 install
```

We collected some common problems one may get into while installing REST. Find it [here](doc/Chapters/2-installing-rest.md#trouble-shooting).

After all the compilation and installation process ends, you will end up with an installed REST version at `~/REST_v2/install/master/`.

Execute the following command to configure your `.bashrc` to load REST in your system environment each time you open a new shell terminal.

 ```
 echo "source ~/REST_v2/install/master/thisREST.sh" >> .bashrc
 ```

## REST libraries

The REST framework provides only the structure and support to create REST user libraries. 
The detailed documentation can be found [here](doc/Chapters/4-the-rest-framework.md). 

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

- PandaX-III: Searching for neutrinoless double beta decay with high pressure 136Xe gas time projection chambers. [X. Chen et al., Science China Physics, Mechanics & Astronomy 60, 061011 (2017)](https://doi.org/10.1007/s11433-017-9028-0) [arXiv:1610.08883](https://arxiv.org/abs/1610.08883).
- Background assessment for the TREX Dark Matter experiment. [Castel, J., Cebrián, S., Coarasa, I. et al. Eur. Phys. J. C 79, 782 (2019)](https://doi.org/10.1140/epjc/s10052-019-7282-6). [arXiv:1812.04519](https://arxiv.org/abs/1812.04519).
- Topological background discrimination in the PandaX-III neutrinoless double beta decay experiment. [J Galan et al 2020 J. Phys. G: Nucl. Part. Phys. 47 045108](https://doi.org/10.1088/1361-6471/ab4dbe). [arxiv:1903.03979]( https://arxiv.org/abs/1903.03979).

## License

This project is licensed under the GNU License - see the [LICENSE](https://lfna.unizar.es/rest-development/REST_v2/blob/master/LICENCE) file for details

## Acknowledgments

`TOBE written`

* Hat tip to anyone whose code was used
* Inspiration
* etc

