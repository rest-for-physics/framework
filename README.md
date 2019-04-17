# REST (Rare Event Searches with TPCs) Framework

RESTSoft (Rare Event Searches with TPCs Software) was born as a collaborative software effort to provide common tools for acquisition, simulation, and data analysis of gaseous Time Projection Chambers (TPCs). The REST Framework is mainly written in C++ and it is fully integrated in [ROOT](root.cern.ch).

 i.e. all REST classes inherit from TObject and can be read/accessed/written using the ROOT I/O interface. The only structural dependence is related to ROOT libraries, while other packages, as Geant4 [32] or
Topological background discrimination in the PandaX-III experiment 31
Garfield++ [19], can be optionally integrated and used within the REST framework when generating or processing Monte Carlo data.
During the last years, major upgrades took place on the REST core libraries [33]. An important feature introduced in REST is that metadata and event data are stored together in a unique file. We understand by metadata any information required to give meaning to the data registered in the event data, as it can be the initial run data taking conditions, the geometry of the detector, the gas properties, or the detector readout topology. Additionally, any input or output parameters, required during the processing or transformation of event data, using event processes, will be stored as metadata. Any previous existing metadata structures inside the REST input file will be transferred to any future output, assuring full traceability, as well as reproducibility of results obtained with a particular dataset.
An ambitious feature of REST is its capability to analyze Monte Carlo and experimental data using common event processes. This is possible by using existing REST event processes to condition the input data generated, for example, by a Geant4 Monte Carlo simulation. After an appropriate event data conditioning, our Monte Carlo generated event will reproduce the rawdata of the detector (as it is shown in section 4). Once we are at that stage, we can benefit from using the same event processes to analyze Monte Carlo and real experimental data. A realistic Monte Carlo rawdata reconstruction will allow us to assess, validate and optimize the processes that will be involved in the real event reconstruction and analysis even before the start of the physics run of the experiment.
In the following subsections, we recall the definitions of the different components of REST, viz. event types, event processes, and analysis tree. These definitions will serve as a reference for the article. Note that we do this having in mind the case where the physics variables of interest are local energy deposits, called hits, and the signal is digitized by a sampling ADC. The REST software is versatile enough, though, to handle many other cases. We include here only those components of REST that are relevant to our study.

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

### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```

## Deployment

Add additional notes about how to deploy this on a live system

## Built With

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - The web framework used
* [Maven](https://maven.apache.org/) - Dependency Management
* [ROME](https://rometools.github.io/rome/) - Used to generate RSS Feeds

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Billie Thompson** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone whose code was used
* Inspiration
* etc

