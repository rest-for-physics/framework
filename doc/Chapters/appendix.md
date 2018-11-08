## Appendix


### List of REST processes

We list here the different event data processes implemented in REST together with a brief funcionality description.

#### Data transformation processes

These processes are in charged of transforming data between different basic data types in REST. 

As soon as we transform one data type to another we can make use of the dedicated data type processes. For example, 
if we have a geant4 event (TRestG4Event), we may transform it to a basic hits event (TRestHitsEvent) and continue 
processing the event using the basic *hit processes*.

REST process | Input type | Output type | Description
-------------|------------|-------------|------------
TRestRawSignalToSignalProcess               | TRestRawSignal    | TRestSignalEvent  | Transforms a rawsignal into a signal event.
TRestSignalToHitsProcess                    | TRestSignalEvent  | TRestHitsEvent    | Converts a signal event into a hits event using TRestReadout.
TRestHitsToSignalProcess                    | TRestHitsEvent    | TRestSignalEvent  | Transforms a HitsEvent into SignalEvent using TRestReadout.
TRestHitsToTrackProcess                     | TRestHitsEvent    | TRestTrackEvent   | Hit clusterization into tracks by proximity (Accurate).
TRestFastHitsToTrackProcess                 | TRestHitsEvent    | TRestTrackEvent   | Hit clusterization into tracks by proximity (Aproximation).
TRestG4toHitsProcess                        | TRestG4Event      | TRestHitsEvent    | Transforms a geant4 event into a hits event.

#### Analysis processes

These are pure analysis processes. They do not transform the event data itself but add new observables/branches to 
TRestAnalysisTree. They may apply cuts to the event data, somehow deciding if an event should continue being processed. 
Any process can decide to stop processing an event by just returning a NULL pointer. This should be documented 
in each process class.

REST process | Input type | Output type | Description
-------------|------------|-------------|------------
TRestRawSignalAnalysisProcess               | TRestRawSignal    | TRestRawSignal    | Adds analysis observables from raw signal event.
TRestHitsAnalysisProcess                    | TRestHitsEvent    | TRestHitsEvent    | Adds analysis observables from hits event.
TRestGeant4AnalysisProcess                  | TRestG4Event      | TRestG4Event      | Adds analysis observables from a geant4 event.
TRestTrackAnalysisProcess                   | TRestTrackEvent   | TRestTrackEvent   | Adds analysis observables from a track event.
TRestTriggerAnalysisProcess                 | TRestSignalEvent  | TRestSignalEvent  | Applies cuts using time window and energy threshold trigger definition.
TRestFindG4BlobAnalysisProcess              | TRestG4Event      | TRestG4Event      | Finds the electron end blobs in a TRestG4Event. For events with at least 2-electron tracks.

#### Signal processes

These processes just modify the data inside a signal event, returning again a signal event data type. These kind 
of processes add signal noise to simulated data, filter noise from rawdata, shape the input signal, or suppress 
signal data points which are under threshold.

REST process | Input type | Output type | Description
-------------|------------|-------------|------------
TRestAddSignalNoiseProcess                  | TRestSignalEvent      | TRestSignalEvent  | Adds random noise to a signal event.
TRestSignalDeconvolutionProcess             | TRestSignalEvent      | TRestSignalEvent  | Deconvolutes a signal using a given input response signal.
TRestSignalGaussianConvolutionProcess       | TRestSignalEvent      | TRestSignalEvent  | Convolutes the input signal with a gaussian.
TRestSignalShapingProcess                   | TRestSignalEvent      | TRestSignalEvent  | Shapes the input signal with a given input response signal.
TRestFindResponseSignalProcess              | TRestSignalEvent      | TRestSignalEvent  | Selects clean signals from input to be used as response for deconvolution.
TRestSignalZeroSuppresionProcess            | TRestRawSignalEvent   | TRestSignalEvent  | Keeps only points which are found over threshold.

#### Hit processes

These processes just modify the data inside a hits event, returning again a hits event data type. These kind of 
processes normalize hits energy, fiducialize hits in a given volume, drift and diffuse hits, or reduce the hit 
number using merging algorithms.

REST process | Input type | Output type | Description
-------------|------------|-------------|------------
TRestElectronDiffusionProcess               | TRestHitsEvent        | TRestHitsEvent    | Spatially diffuses input hits using gas properties defined in TRestGas and the active TPC volume/geometry defined in TRestReadout.
TRestFiducializationProcess                 | TRestHitsEvent        | TRestHitsEvent    | Only hits inside readout active volume definition survive.
TRestAvalancheProcess                       | TRestHitsEvent        | TRestHitsEvent    | Statistical gain increase per hit.
TRestHitsNormalizationProcess               | TRestHitsEvent        | TRestHitsEvent    | Re-scales the hits energy by a constant factor.
TRestHitsRotateAndTraslateProcess           | TRestHitsEvent        | TRestHitsEvent    | Rotates and translates a hit distribution along its energy center.
TRestHitsReductionProcess                   | TRestHitsEvent        | TRestHitsEvent    | Merges hits by proximity reducing the effective number of hits.
TRestHitsShuffleProcess                     | TRestHitsEvent        | TRestHitsEvent    | Hits are disordered in the hits queue.
TRestSmearingProcess                        | TRestHitsEvent        | TRestHitsEvent    | Hits energy is re-scaled with a random factor by a given resolution.

#### Track processes

These processes operate over track event data, or a set of hits that have been grouped into tracks. We may find 
the minimum path between the hits inside each track, reduce the number of hits inside each track, find the track 
ends, or project the hits over the main trajectory effectively linearizing the track.

REST process | Input type | Output type | Description
-------------|------------|-------------|------------
TRestFindTrackBlobsProcess                  | TRestTrackEvent       | TRestTrackEvent       | Finds the track end blobs in a TrackEvent .Tracks should have been pre-processed with path minimization and reconnection processes.
TRestTrackLinearizationProcess              | TRestTrackEvent       | TRestLinearTrackEvent | Projects the hits into the track to get dE/dx profile.
TRestTrackPathMinimizationProcess           | TRestTrackEvent       | TRestTrackEvent       | Finds the minimum path between hits inside each track.
TRestTrackReconnectionProcess               | TRestTrackEvent       | TRestTrackEvent       | Improves physical track description after track minimization.
TRestTrackReductionProcess                  | TRestTrackEvent       | TRestTrackEvent       | Reduces the number of hits inside a track by merging closer hits.
TRestTrackToHitsProcess                     | TRestTrackEvent       | TRestHitsEvent        | It recovers back a track event into a hits event.

#### Rawdata processes

These processes read rawdata written in binary format and extract the signal event data to write it into a 
TRestRawSignalEvent.

REST process | Input type | Output type | Description
-------------|------------|-------------|------------
TRestRawToSignalProcess                     | rawdata               | TRestRawSignalEvent   | Used to encapsulate rawdata to signal processes.
TRestAFTERToSignalProcess                   | rawdata               | TRestRawSignalEvent   | Transforms AFTER data into raw signal event.
TRestCoBoAsAdToSignalProcess                | rawdata               | TRestRawSignalEvent   | Transforms CoBoAsAd data into raw signal event.
TRestMultiCoBoAsAdToSignalProcess           | rawdata               | TRestRawSignalEvent   | Transforms CoBoAsAd data into raw signal event. General version using several CoBoAsAd cards. Event data might be splitted between different data files. The process receives a list of all the files in a given run.
TRestFEMINOSToSignalProcess                 | rawdata               | TRestRawSignalEvent        | Transforms FEMINOS data into SignalEvent.
TRestMultiFEMINOSToSignalProcess            | rawdata               | TRestRawSignalEvent        | Transforms FEMINOS data into SignalEvent.  General version using several Feminos cards. Full event data is containned in one single file.

#### Viewer processes

These processes can be connected during intermediate event processes to visualize the input/output of 
different processes. Still many missing viewer processes should be developed in this section.

REST process | Input type | Output type | Description
-------------|------------|-------------|------------
TRestRawSignalViewerProcess                 | TRestRawSignalEvent   | TRestRawSignalEvent   | Visualizes a raw-signal event
TRestSignalViewerProcess                    | TRestSignalEvent      | TRestSignalEvent      | Visualizes a signal event


### List of ROOT scripts

Call them in restRoot prompt by "Function name". Some can directly be in bash with restXXX. Use tab completion
to check the supported rest bash command

The macros could be changed frequently in future.

#### general macros

These macros provides basic data analysis functionality, including fit, integrate, etc, for observables in 
the anslysis tree

Function name  |Input arguments | Description
------------|-------------|------------
REST_General_CreateHisto()   | TString varName<br>TString rootFileName<br>TString histoName<br>int startVal = 0<br>int endVal = 1000<br>int bins = 1000<br>Double_t normFactor = 1 | Creates and saves a histogram for a given observable in the data file.
REST_General_Fit() |  |  Fits a given observable in the file and prints the result on screen.vis
REST_General_Integrate() |  |  Integrates a given observable in the file and prints the result on screen.
REST_General_IntegrateSmearing() |  |  Integrates with smearing a given observable in the file and prints the result on screen.


#### viewer macros

Function name  |Input arguments | Description
------------|-------------|------------
REST_Viewer_GenericEvents() | TString fName<br>TString EventType = "" | Shows a TRestBrowser which visualizes events in file by calling TRestGenericEventViewer
REST_Viewer_G4Event() | TString fName |  Shows a TRestBrowser which visualizes TRestG4Event by calling TRestG4EventViewer
REST_Viewer_Geometry() | TString fName |  Shows Geometry info saved in the given file by calling TGeoManager
REST_Viewer_HitsEvent() | TString fName |  Shows a TRestBrowser which visualizes TRestHitsEvent by calling TRestHitsEventViewer
REST_Viewer_Readout() | TString rootFile<br>TString name<br>Int_t plane = 0 |  Draw a figure of readout definition according to the save TRestReadout object in the file
REST_Viewer_ReadoutEvent() | TString fName<br>TString cfgFilename = "template/config.rml"  | Shows a TRestBrowser which visualizes TRestReadoutEvent by calling TRestReadoutEventViewer

#### printer macros

Function name  |Input arguments | Description
------------|-------------|------------
REST_Printer_GenericEvents()   | TString fName<br>TString EventType=""<br>Int_t firstEvent = 0 | Print the event in file with given type. It calls the method TRestEvent::Print() 
REST_Printer_FileContents() | TString fName | Print name and title of Metadata/Application/Trees saved in the file.(Anything inherited from TNamed)
REST_Printer_Metadata() | TString fName |  Print the metadata content of metadata objects in the file.
REST_Printer_Trees() | TString fName |  Print EventTree, AnalysisTree and observables in the file.

#### geant4 macros

Function name  |Input arguments | Description
------------|-------------|------------
REST_Geant4_FindGammasEmitted()   | TString fName | 
REST_Geant4_FindIsotopes() | TString fName<br>TString fIsotope |  
REST_Geant4_GetBiasingError() | TString fName<br>Int_t finalEvents = 0 |  
REST_Geant4_GetROIEvents() | TString fName<br>Double_t mean=2457.83<br>Double_t fwhm=0.03 |  
REST_Geant4_GetROIEvents_Fiducial() |  TString fName<br>Double_t zMin<br>Double_t zMax<br>Double_t radius<br>Double_t mean=2457.83<br>Double_t fwhm=25 |  
REST_Geant4_ListIsotopes() | TString fName<br>TString fOutName |  
REST_Geant4_MeanTrackLength() | TString fName |  
REST_Geant4_QuickLookAnalysis() | TString fName |  
REST_Geant4_ReadNEvents() | TString fName<br>int n1<br>int n2 |  

#### tool macros

Function name  |Input arguments | Description
------------|-------------|------------
REST_Tools_CheckReadout()   | TRestReadoutPlane *p<br>Int_t mask[4]<br>Double_t region[4]<br>Int_t N | 
REST_Tools_CheckRunFileList() | TString namePattern<br>Int_t N = 100000 |  
REST_Tools_DrawCombinedGasCurves() | / |  
REST_Tools_DrawResponseSignal() | TString fName |  
REST_Tools_GenerateGasFile() | TString cfgFile |  
REST_Tools_MergeFiles() |  TString pathAndPattern<br>TString outputFilename | Merge ROOT files with given pattern
REST_Tools_ProduceResponseSignal() | TString inputFileName<br>TString outputFileName<br>Int_t nPoints = 512<br>Double_t threshold = 1 |  
REST_Tools_ValidateGeometry() | TString gdmlName | 


### REST input data

REST input data is in the directory ./inputdata. It will be installed together with REST. An
environment variable $REST_INPUTDATA will be set pointing to this directory. 

The directory contains pre-generated data which may be used in analysis/simulaiton. 

#### acqusition

#### decoding

Contains several decoding files for electronics. The first line is the daq channel, the section line is
the channel in TRestReadout. For example, for each AGET chip, we have 68 channels. One MicroMegas needs
two AGET chips. Their 136 channels are connected with 128 MicroMegas channels with certain mapping. So 
we need decoding file to process readout file. 

#### definitions

Contains several rml files with pre-defined sections. Other rml files can directly use these sections with
include definition.

#### distributions

Energy and angular distribution of different radios, including cosmic ray, high energy gamma, etc

#### gasFiles

Contains pre-generated gas file of many conditions. The files correspond to the sections in file gases.rml 
in "definitions" directory.

#### generator

Geant4 generator file for restG4

#### signal

Contains several root files saving TRestSignal as response signal.

### V2.2 changelog

The most basic change is that we migrate from ROOT5 to ROOT6. Thanks to this, we can write our code in a 
more mordern way.

#### Basic rml concept change

1. change to standard xml format, remove `<section TRestXXX...`, use `<TRestXXX ...`
2. environment section are merged into globals section. This section will be expanded into other sections, we can set "variable", "parameter" here
3. field value is also a prarmeter. GetFieldValue() and GetParameter are now a same thing.

e.g. GetParameter("parName")/GetFieldValue("parName") will find in:  
`<TRestXXX parName="parVal" ...>`  
`</TRestXXX>`  
and  
`<TRestXXX ...>`  
&emsp;`<parameter name="parName" value="parVal" />`  
`</TRestXXX>`  

4. removed processFile, readoutFile definition, use universal include definition:

//to expand section "TRestXXX" with name "sAna" in file "processes.rml" into this section,  
//we can write  
`<TRestXXX name="sAna" file="processes.rml"/>`  
or  
`<TRestXXX name="sAna">`  
&emsp;`<include file = "processes.rml" />`  
`</TRestXXX>`  

#### Output file format change 

1. tree name is changed. EventTree + AnalysisTree + Metadata objects
2. event branch name is changed. In EventTree we have multiple branches named TRestXXXEventBranch, no more TRestXXXEventTree--evtBranch
3. observables are named like "sAna_BaseLineSigmaMean"(use "_" instead of ".")

#### Update directory order 
1. merge ./config into ./packages/restG4/example
2. change ./example to ./packages/userRESTLibrary, the old example folder now contains example rml file
3. move ./source/viewers to ./source/events/viewers
4. move TRestFFT and TRestMesh to ./source/events/addons, move TRestManager to ./source/restcore
5. directory ./source/tools contains new classes
6. move .C scripts in ./scripts into ./macros. ./macros now contians some python script for installation/updating

#### Update software logic
1. removed dependence of Garfield, tinyxml. Only based on ROOT6
2. add package restDataBaseImpl. modularized installation.
3. optimized compilation logic and library hierarchy, add a macro CompileDir.cmake
4. keep only two executables, restManager and restRoot. Others are now the alias of calling restManager with macro files  
(e.g. restViewEvents = restManager ViewEvents)
5. add a shell script rest-config
6. changed some codes. windows compilation is now passed, though it cannot run.

#### Other class update

##### Added class: TRestStringHelper
1. string methods from TRestMetadata are move here.
2. added method Spilt(), ToUpper(), ExecuteShellCommand()

**Spilt()**: input a target string and a separator, returns a vector of sub string  
**ToUpper()**: change string to upper string, used when checking values, i.e. "ON", "On", "on"  
**ExecuteShellCommand()**: execute a shell command and returns the output string. No more temp files generated in GetFileNamesMatchingPattern()

3. provides many inline methods which one can directly use. Just include the header and he can use them in any codes(not just TRestMetadata classes)

##### Added class: TRestStringOutput
1. this class can be used similarly like cout.

`TRestStringOutput fout;`  
`fout.setborder("==");`  
`fout<<"hellow world"<<endl;`  
`//result is in middle of your screen:`  
`//==                     hellow world                       ==`  

2. provides auto formatting, cursor action, colored display
3. provides console tools that can tell current width and height of your console.

##### Changed class: TRestMetadata
1. uses TinyXml to parse rml config file, not the home-made string parser
2. initializes with TiXmlElement* object 
3. changed verbose level name
4. provides beautiful string output tools which activates according to verbose level

`int a =0;`  
`fout<<"a is : "<<a<<endl;//show message at any condition`  
`essential<<"a is : "<<a<<endl;//show message when verbose level is >= REST_ESSENTIAL`  
`info<<"a is : "<<a<<endl;//show message when verbose level is >= REST_INFO`  
`debug<<"a is : "<<a<<endl;//show message when verbose level is >= REST_DEBUG`  
`extreme<<"a is : "<<a<<endl;//show message when verbose level is >= REST_EXTREME`  
`warning<<"a is : "<<a<<endl;//show message when verbose level is >= REST_ESSENTIAL, color is yellow`  
`error<<"a is : "<<a<<endl;//show message at any condition, color is red`  

5. keeps a pointer to TRestManager
6. added reflection support(see document)
7. Initialize() is no more a pure virtual method, now we have InitFromConfigFile()
8. Unified search path definition and file finding

`//in rml file`  
`<searchPath value="/home/nkx/abc/"/>`  

`//in the code`  
`SearchFile("readouts.root");`  

##### Changed class: TRestManager
1. manages all application objects, run tasks with the help of TRestTask. 
2. rml sections in TRestManager secrion keeps same hierarchy as class order. 
3. use TClass reflection to instantiate these managed classes. use child rml sections to initialize these managed classes.
4. allows C++ sytle command task and pre-defined task

`<addTask command="TemplateEventProcess->RunProcess()" value="ON"/>`  
`<addTask type="ProcessEvents" value="ON"/>`  

5. currently we have three application classes: TRestProcessRunner, TRestRun and TRestAnalysisPlot. 
6. Applications can refer to each other by asking their manager for their friends

##### Changed class: TRestRun
1. manages all metadata objects, provides data from objects or external file
2. external process is handled here.
3. other processes are handled in TRestProcessRunner
4. rml section "addMetadata" now should be written inside "TRestRun" section, not in "TRestManager section"
5. auto naming now collects information from multiple source: fileinfo map, processinfo map and TRestRun datamember.
6. naming key words changed from like:  
`Tracks_[EXPERIMENT]_[USER]_[RUNTYPE]_[RUNTAG]_[RUN]_V[VERSION].root`  
to  
`Tracks_[fExperimentName]_[fRunUser]_[fRunType]_[fRunTag]_[fRunNumber]_V[fVersion].root`  

##### Added class: TRestProcessRunner
1. old process functionality are move here. 
2. "addProcess" section now should be written in "TRestProcessRunner" section, not in "TRestRun" section(for external process both are ok)
3. add multi-thread support
4. added class TRestThread for multi-thread run.
5. supports tree branch choosing(switch on/off: input event, output event, input analysis)

`<TRestProcessRunner ...>`  
&emsp;`<parameter name="inputAnalysis" value="on"/>`  
&emsp;`<parameter name="inputEvent" value="on"/>`  
&emsp;`<parameter name="outputEvent" value="on"/>`  
&emsp;`...`  
`</TRestProcessRunner>`  

-- inputAnalysis: observables from input file. if input file is not REST file, this parameter
can be ignored  
-- inputEvent: inputEvent from input file, or the inputEvent of the first process. 
if input file is not REST file, it is also the outputEvent of the external process  
-- outputEvent: outputEvent of each process. if several of them is of the same type, only the 
last one is kept  

6. added progressbar
7. added pause/terminate functionality. a pause menu is provided
8. added test run functionality, which could be useful for debuging. 
9. now in processes one can simply copy the pointer to output event

`TRestEvent* TRestRawSignalAnalysisProcess::ProcessEvent( TRestEvent *evInput ){`  
&emsp;`fSignalEvent = (TRestRawSignalEvent *)evInput;`  
&emsp;`fOutputEvent = fSignalEvent;`  
&emsp;`fInputEvent = fSignalEvent;`  
&emsp;`//some analysis for fSignalEvent`  
&emsp;`...`  
&emsp;`return fSignalEvent;`  
`}`  

10. we don't necessarily need processes.rml to setup processes. 

e.g.  
`//uses include definition`  
`<addProcess type="TRestXXX" name="sAna" file="processes.rml"/>`  
or  
`//directly set values here`  
`<addProcess type="TRestXXX" name="sAna">`  
&emsp;`<parameter name="myDummyParameter" value="10"/>`  
&emsp;`<observable name="X1:X2:X3" value="on"/>`  
`</addProcess>`  


##### Changed class: TRestEvent
1. added a method CloneTo(), which is very useful in process(don't need to manually copy the event data)

`TRestEvent* TRestRawSignalAnalysisProcess::ProcessEvent( TRestEvent *evInput ){`
&emsp;`fSignalEvent = (TRestRawSignalEvent *)evInput;`
&emsp;`fInputEvent = fSignalEvent;`
&emsp;`fSignalEvent->CloneTo(fOutputEvent);`
&emsp;`//some analysis for fSignalEvent`
&emsp;`...`
&emsp;`return fSignalEvent;`
`}`

##### Changed class: TRestEventProcess
1. added analysistree output level control:

No_Output : no analysistree in the output file, processes' self output is still available. e.g. a histogram in the file.  
Observable : +saving observables in the root directory of the tree  
Internal_Var : +saving processes as branches. internal variables can be found inside the branch  
Full_Output : +saving output event together in the process branch  

2. we can save non-double type values as analysis result thanks to ouput level Internal_Var

`//inside class definition(TRestRawSignalAnalysisProcess.h)`  
`map<int, Double_t> baseLineSigma;//signal id, bsl value`  
`//we scan baselinesigma of signal id 100 in AnalysisTree`  
`AnalysisTree->Scan("sAna.baseLineSigma->second","sAna.baseLineSigma->first==100","")`  

3. added class-member-observable support

`//inside class definition(TRestRawSignalAnalysisProcess.h), add class member`  
`Double_t baseLineSigmaMean;`  
`//inside function ProcessEvent(), set the value once.`  
`baseLineSigmaMean= fSignalEvent->GetBaseLineSigmaAverage( fBaseLineRange.X(), fBaseLineRange.Y() );`  
`//inside rml`  
`<addProcess type="TRestRawSignalAnalysisProcess" name="sAna">`  
&emsp;`<observable name="baseLineSigmaMean" value="on"/>`  
`</addProcess>`

4. coding in the inherited class is simplified:

Now we can use:

`TRestEvent* TRestRawSignalAnalysisProcess::ProcessEvent( TRestEvent *evInput ){`  
&emsp;`fSignalEvent = (TRestRawSignalEvent *)evInput;`    
&emsp;`fOutputEvent = fSignalEvent;`    
`...`  
`}`  

Or:

`TRestEvent* TRestRawSignalAnalysisProcess::ProcessEvent( TRestEvent *evInput ){`  
&emsp;`fSignalEvent->Initialize();`  
&emsp;`evInput->CloneTo(fSignalEvent);`  
&emsp;`fOutputEvent = fSignalEvent;`  
`...`  
`}`  

Previously:

`TRestEvent* TRestRawSignalAnalysisProcess::ProcessEvent( TRestEvent *evInput ){`  
&emsp;`fSignalEvent->Initialize();`  
&emsp;`TRestRawSignalEvent *fInputSignalEvent = (TRestRawSignalEvent *)evInput;`  
&emsp;`fSignalEvent->SetID(fInputSignalEvent->GetID());`  
&emsp;`fSignalEvent->SetSubID(fInputSignalEvent->GetSubID());`  
&emsp;`fSignalEvent->SetTimeStamp(fInputSignalEvent->GetTimeStamp());`  
&emsp;`fSignalEvent->SetSubEventTag(fInputSignalEvent->GetSubEventTag());`  
&emsp;`Int_t N = fInputSignalEvent->GetNumberOfSignals();`  
&emsp;`for (int sgnl = 0; sgnl < N; sgnl++)`  
&emsp;&emsp;`fSignalEvent->AddSignal(*fInputSignalEvent->GetSignal(sgnl));`  
&emsp;`fOutputEvent = fSignalEvent;`  
`...`  
`}`

We can directly copy the pointer, thanks to the test run strategy adpoted.

##### Changed class: TRestBrowser
1. embeded into TBrowser.
2. TRestEventViewer is allowed to customize control bar

##### Changed class: TRestReadout
1. Add operator [] in each class, which uses "Index" (fReadoutChannel[i])
2. add a method GetHitsDaqChannel(), GetPlaneModuleChannel(), GetXXXWithID(), GetX(), GetY()
3. add channel type definition in TRestReadoutChannel. This is for the convenience of GetX(), GetY() method

##### Changed class: TRestGas
1. it saves garfield's gas file as string inside its class menber. This string will be saved inside metadata file.
2. no need for gasFile! it will retrieve its content from metadata file or download it from website
3. added several c++ preprocesser in the code. turn on/off garfield functionality by DREST_GARFIELD=ON/OFF

##### Changed processes
1. TRestRawSignalAnalysisProcess: added several non-double analysis output
2. TRestSignalZeroSuppresionProcess: added baseline subtraction correction for previous 
TRestRawSignalAnalysisProcess
3. Update the cut definition in TRestTrackAnalysisProcess and TRestRawSignalAnalysisProcess

e.g.
`<parameter name="cutsEnabled" value="false" />`
`<cut name="MeanBaseLineCut" value="(200,800)" />`
`<cut name="MeanBaseLineSigmaCut" value="(0,6)" />`

(old)
`<parameter name="cutsEnabled" value="false" />`
`<parameter name="meanBaseLineCutRange" value="(200,800)" />`
`<parameter name="meanBaseLineSigmaCutRange" value="(0,6)" />`

4. Add processes: TRest2DHitsEvent, TRestRawSignalTo2DHitsProcess, TRestMuonAnalysisProcess, 
TRestReadoutAnalysisProcess, TRestUSTCElectronicsProcess
5. Multiple processes will now transfer event info
6. TRestMultiCoBoAsAdToSignalProcess: added package-loss handling
7. TRestSignalToHitsProcess: fix a bug in triple max method
8. TRestHitsToSignalProcess: simplified the code using new TRestReadout method
9. TRestHitsShuffleProcess: fix a bug when nHits is too small


[**prev**](9-rest-packages.md)
[**contents**](0-contents.md)
[**next**](0-contents.md)