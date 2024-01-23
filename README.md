# The REST Framework
[![DOI](https://zenodo.org/badge/324291710.svg)](http://doi.org/10.5281/zenodo.4528985)
[![pipeline status](https://gitlab.cern.ch/rest-for-physics/framework/badges/master/pipeline.svg)](https://gitlab.cern.ch/rest-for-physics/framework/-/commits/master)
[![website](https://img.shields.io/badge/user-guide-E8B6FF.svg)](https://rest-for-physics.github.io)
[![api](https://img.shields.io/badge/user-API-FFCA78.svg)](https://rest-for-physics.github.io/framework/)
[![forum](https://img.shields.io/badge/user-forum-AAFF90.svg)](https://rest-forum.unizar.es/)

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

### Mirror repositories

REST is mirrored to the following repositories where pipelines are executed, and where code can also be retrieved.

- https://gitlab.cern.ch/rest-for-physics

Code can be pulled for read-access from those mirrors, however, development is centralized at the main [GitHub public repository](https://github.com/rest-for-physics/).

### Installing

Please, visit the [REST-for-Physics userguide](https://rest-for-physics.github.io) for installation instructions.


## Useful links or references

- [Doxygen documentation](https://rest-for-physics.github.io/framework/).
- The REST Framework forum for open discussions is available [here](https://rest-forum.unizar.es).
- ROOT naming convention and coding rules are [Taligent rules](https://root.cern/TaligentDocs/TaligentOnline/DocumentRoot/1.0/Docs/books/WM/WM_63.html#HEADING77).

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) to get some guidelines on how to contribute to this project.
Before any contribution, those guidelines must be assimilated and accepted.
In any case, changes, improvements, or addons, to [CONTRIBUTING.md](CONTRIBUTING.md) are aceptable after proposal and discussion with other authors at the [REST Framework forum](https://ezpc10.unizar.es/).

The framework exploits the Git tagging system to produce its own versioning system. It is important to emphasize that the REST framework centralizes the versioning of all the submodules (libraries, packages, ...) that it contains. Details on how the REST version number is produced are given in [CONTRIBUTING.md](CONTRIBUTING.md).

## Versioning

Any **metadata** object written with REST **will be stamped** with few metadata members that will allow to identify the state of the code when the object was produced. Those data members are:
  - *fVersion*: A string containing the human version number.
  - *fCommit*: The latest commit hash value when the compilation took place.
  - *fLibraryVersion*: The human version library. It is fixed by CMakeLists at the library submodules.
  - *fOfficialRelease*: It will be true if the commit was tagged at the repository.
  - *fCleanState*: It will be true if there are no local modifications (including submodules). To remove any local modifications and recover a clean state we may execute `source clean-state.sh` at the project root.

If different REST versions were used to write a ROOT file, e.g. at different steps of the data processing chain, the historic metadata objects will preserve their original version.
However, the `TRestRun` metadata object **will always store** the version used to write the ROOT file.

After REST release 2.2.1., REST implements correctly the `ROOT schema evolution`. Therefore, any new REST version should always be backwards compatible.
I.e. Any file written after v2.2.1 should be readable without problems with any future version.

A major change at 2.3 will prevent from backwards compatibility, since class names have been reviewed.

## Publications

- AlphaCAMM, a Micromegas-based camera for high-sensitivity screening of alpha surface contamination, [K. Altenmüller et al., Journal of Instrumentation, Volume 17, August 2022](https://doi.org/10.1088/1748-0221/17/08/P08035), [arXiv:2201.01859](https://doi.org/10.48550/arXiv.2201.01859).
- REST-for-Physics, a ROOT-based framework for event oriented data analysis and combined Monte Carlo response, [K. Altenmüller, S. Cebrián, T. Dafni et al., , Computer Physics Communications, 108281, April 2022](https://doi.org/10.1016/j.cpc.2021.108281).
- Topological background discrimination in the PandaX-III neutrinoless double beta decay experiment, [J Galan et al 2020 J. Phys. G: Nucl. Part. Phys. 47 045108](https://doi.org/10.1088/1361-6471/ab4dbe), [arxiv:1903.03979](https://doi.org/10.48550/arXiv.1903.03979).
- Background assessment for the TREX Dark Matter experiment. [Castel, J., Cebrián, S., Coarasa, I. et al. Eur. Phys. J. C 79, 782 (2019)](https://doi.org/10.1140/epjc/s10052-019-7282-6). [arXiv:1812.04519](https://arxiv.org/abs/1812.04519).
- PandaX-III: Searching for neutrinoless double beta decay with high pressure 136Xe gas time projection chambers. [X. Chen et al., Science China Physics, Mechanics & Astronomy 60, 061011 (2017)](https://doi.org/10.1007/s11433-017-9028-0) [arXiv:1610.08883](https://arxiv.org/abs/1610.08883).

## Presentations
- REST-for-Physics, Luis Obis, [2022-May, ROOT Users Workshop, FermiLab](https://indico.fnal.gov/event/23628/contributions/240755/).
- REST v2.0 : A data analysis and simulation framework for micro-patterned readout detectors., Javier Galan, [2016-Dec, 8th Symposium on Large TPCs for low-energy rare event detection, Paris](https://indico.cern.ch/event/473362/contributions/2334838/).


## License

This project is licensed under the GNU License - see the [LICENSE](LICENCE) file for details

## Acknowledgments

We acknowledge support from the the European Research Council (ERC) under the European Union’s Horizon 2020 research and innovation programme, grant agreement ERC-2017-AdG788781 (IAXO+), and from the Spanish Agencia Estatal de Investigacion under grant FPA2016-76978-C3-1-P

![Insitution logos](miscellaneous/institution_logos.png)
