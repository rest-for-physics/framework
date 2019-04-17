# The REST Framework

The REST (Rare Event Searches with TPCs Software) Framework is mainly written in C++ and it is fully integrated with [ROOT](root.cern.ch) I/O interface.
REST was born as a collaborative software effort to provide common tools for acquisition, simulation, and data analysis of gaseous Time Projection Chambers (TPCs).
The REST Framework provides 3 interfaces that prototype the use of **event types**, **metadata** and **event processes** through `TRestEvent`, `TRestMetadata` and `TRestEventProcess` abstract class definitions.
Any REST library will implement **specific objects** that inherit from those 3 basic interfaces. 

Different **event processes** can be combined to build complex event processing chains with full traceability. 
The **metadata** objects will allow us to provide input parameters or information to the framework using a XML like format.
REST integrates a special **metadata** object named `TRestManager` that encapsulates all the required information to launch the processing of a particular data chain.
REST will produce output using ROOT format. Any REST file will always contain a `TRestRun` metadata object.
`TRestRun` is a **metadata** object responsible to encapsulate and give access to all the objects stored inside the REST/ROOT file; 
i.e. the **specific** resulting `TRestEvent` output, the `TRestAnalysisTree`, and any **specific** `TRestMetadata` object used during a processing chain.

The REST Framework provides additionally different interfaces to **browse data**, `TRestBrowser`, and to **plot data**, `TRestAnalysisPlot`, by accessing `TRestEvent` and `TRestAnalysisTree` ROOT-based drawing methods.
Other objects included in the framework will help to add unit definitions, `REST_Units`, define physical constants `REST_Physics`, or provide methods to help on text formatting `TRestStringHelper` or define output styles, `TRestStringOutput`.


## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

What things you need to install the software and how to install them

```
Give examples
```

### Installing

A step by step series of examples that tell you how to get a development env running

Say what the step will be

```
Give the example
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo

## Running the tests

Explain how to run the automated tests for this system


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

This project is licensed under the GNU License - see the [LICENSE](LICENSE) file for details

## Acknowledgments

TOBE written

* Hat tip to anyone whose code was used
* Inspiration
* etc

