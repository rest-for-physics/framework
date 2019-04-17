# The REST Framework

The REST (Rare Event Searches with TPCs Software) Framework is mainly written in C++ and it is fully integrated with [ROOT](root.cern.ch) I/O interface.
REST was born as a collaborative software effort to provide common tools for acquisition, simulation, and data analysis of gaseous Time Projection Chambers (TPCs).
The REST Framework provides 3 interfaces that prototype the use of `event types`, `metadata` and `event processes` through TRestEvent, TRestMetadata and TRestEventProcess abstract class definitions.
Any REST library will implement `specific objects` that inherit from those 3 basic interfaces. 
Different `event processes` can be combined to build complex event processing chains with full traceability. 
The `metadata` structures will allow us to provide input parameters or information to the framework using an XML like format.
REST integrates a special `metadata` class named `TRestManager` that encapsulates all the required information to launch the processing of a particular data chain.
REST will produce output using ROOT format. Any REST file will always contain a TRestRun metadata object.
TRestRun is a `metadata` object responsible to encapsulate and give access to all the objects stored inside the ROOT file; 
i.e. the resulting `event data` output, the TRestAnalysisTree, any `specific` metadata object used during a processing chain.



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

