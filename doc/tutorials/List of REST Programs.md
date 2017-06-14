\brief A list of the different executables compiled with REST together with a brief functionality description.

This is just an indexing list of existing programs included with REST compilation. In order to get details about how to use this programs just execute it in the command line without any arguments.

## Core executables

**restG4** : The executable from *restG4* package. It takes as argument a RML configuration file with a TRestG4Metadata section definition.

**restManager** : The manager program of REST allowing to execute the REST event processes defined in TRestManager. Additional definitions allow to include additional metadata structures inside REST.

**restPlots** : It uses the plot definitions given by RML configuration in a TRestAnalysisPlot section. It creates the plots from the variables at the TRestAnalysisTree and creates a PDF report and a ROOT file including the histograms created.

## Histogram and integration executables

**restCreateHisto** : A histogram will be created from a variable inside TRestAnalysisTree. Histogram range and number of bins can be specified by argument. Several input files can be merged into one single histogram. The generated histogram can be added to an existing root file, if the ROOT file does not exist it will be created.

**restFit** : This program does a gausian fit of an existing variable inside TRestAnalysisTree. The fit range must be given by argument.

**restIntegrate** : This program will return the integrated number of entries of an existing variable name inside TRestAnalysisTree. The integration range must be given as argument.

**restIntegrateSmearing** : It integrates the entries inside a variable name from TRestAnalysisTree after smearing the input variable.

## Printing info executables 

**restPrintRunInfo** : It prints the metadata information inside a REST file.

**restPrintAllMetadata** : It prints on screen all the metadata information containned in a REST file. TRestRun metadata, readout, gas, processes metadata, etc.

**restPrintFileContents** : It prints on screen the ROOT objects stored in a REST file.

**restPrintAnaTreeVariables** : It prints on screen the existing variables inside the TRestAnalysisTree.

**restPrintEvents** : It prints on screen the content of the event data inside a REST file.

**restDrawEvents** : It draws the events one by one, using the specific TRestEvent::Draw method implemented in the specific event data type found inside the REST file. 

**restG4PrintParticles** : It prints the name of all the different particles registered inside a REST file containning a TRestG4Event data type.
