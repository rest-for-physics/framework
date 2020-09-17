TODO list
---------

This is our TODO list. A TODO bullet might be connected to a post in the forum site (An identifying symbol/character is given at the beginning of each line to help referencing in the forum). 
Use it to discuss about options to solve a particular point, or to inform others about a fix commited to the development branch.

## A. Isolated topics

* A.1 [x] A first process, if declared as `external`, cannot access the `analysisTree`. We should override this limitation. Furthermore, it would be desirable that if the first process is `external` and the process itself does not stamp the `eventID`, `timestamp`, etc, an automatic numbering will be assigned by `TRestEventProcess`. E.g. just using the entry number on the `analysisTree`.

* A.2 [x] Implement a way to define a materials.xml file common to every-one. Avoiding `materials.xml` file to propagate on different geometry setups. Instead, geometry setups will use always a common one that can be validated and maintained by REST users. In summary, find a way that the GDML file will give a reference to `REST_PATH/data/definitions/materials.xml`.

* A.3 [ ] `restG4GammaTransfer` independent calculation.

* A.4 [x] Plot an analysis tree observable with different cuts using `TRestAnalysisPlot`. See [forum post](http://ezpc10.unizar.es/t/new-feature-to-be-implemented-in-trestanalysisplot/24)

* A.5 [x] Add `IF` condition option to `RML` files. See [forum post](https://gifna.unizar.es/rest-forum/?question=adding-if-conditions-in-rml-files)

* A.6 [ ] New metadata `TRestSpectrumCalibration` containing members allowing to identify different peaks in the spectrum, including energy, width, tagName, etc. Perhaps an auxiliar `TRestPeak::TObject` can be used to store those values. This metadata structure might be filled by a process `TRestMultiPeakFinderProcess` that fits a branch variable from the `analysisTree`.

* A.7 [ ] Implement a new process `TRestRawSignalToHitsProcess` that allows to fit detector signals and obtain the TRestHitsEvent directly.

* A.8 [x] Integrate the use of `TRestDetectorSetup` inside `TRestMultiFEMINOSToSignal` to retrieve the information of run number, detector settings, run tag, etc. `TRestDetectorSetup` should be added to the metadata structures in the data flow, and at the same time the values of `TRestRun` and `TRestAnalysisTree` should be updated according to the information in `TRestDetectorSetup`.

* A.9 [x] `Gas file server` web interface.

* A.10 [ ] `restExplorer` package.

* A.11 [ ] `restSQL` REST metadata indexing/database population + re-implement `restWeb` to search and access the REST file server database.

* A.12 [x] Fix required in TRestBrowser. See [forum post](http://ezpc10.unizar.es/t/error-when-using-trestbrowser-trestbrowser-inherits-from-trestrun/41)

* A.13 [x] Give the event tree the name of the specific event stored. See [forum post](http://ezpc10.unizar.es/t/eventtree-should-be-stored-using-the-name-of-the-specific-event-output/42).

* A.14 [x] Fix duplicated metadata entries. See [forum post](http://ezpc10.unizar.es/t/duplicated-metadata-entries-in-last-version-v2-2-10/38).

* A.15 [ ] Review restG4 and TRestG4Metadata to make a better generator definition, either for geant4 built-in generator or custom generator like restDecay0. See [forum post](http://ezpc10.unizar.es/t/confusion-of-particle-source-collection/352).

* A.16 [x] Optimize TRestReadout initialization See [forum post](http://ezpc10.unizar.es/t/optimize-trestreadout-startup-time/52).

* A.17 [ ] We need to work on the `TRestEvent::DrawEvent()` methods of the different event types. See implementation of `TRestG4Event::DrawEvent` to be used as reference. 
  We should create first the documentation of this method in `TRestG4Event` and then use it in other `TRestEvent::DrawEvent` methods.

* A.18 [x] Work on `basic RML's examples` and validation for REST new comers. Add a README.md giving a brief description of the examples.

## B. README.md description on each data directory, library, etc

TODO: This section will evolve in a section to determine README files or directories that require documentation.

* B.1 [ ] Write a README.md for X directory.

* B.2 [ ] Review README.md at Y directory.

## C. Examples

TODO: Write a list of existing examples, which examples require validation or documentation? 
I.e. describe how to launch the example, example description, what it does, what is required, etc.

We should place permanent data files in a public server for those examples that require to access a particular input data file.

Examples brainstorming. restG4 examples, analysis plot examples, event reconstruction, readout generation, etc.


## D. Documentation of all classes using doxygen.

   * General guidelines for documentation.
    
        - CXX File should include a LICENCE header, and general class description. See for example TRestG4Metadata.

        - Methods should be documented where ever they are implemented. If implemented in TRestXX.h file they must be documented in the TRestXX.h file. 
        Use \brief to have a quick description on the method list on the HTML result.

        - Members of the class should be documented using /// on top of the definition. Check TRestG4Metadata.h

        - Authors and date should be kept updated from now on.

            - Authors for different type of contributions: First concept, upgrades, documentation, review and/or validation.
                - First concept: Author who created the file and created the first definitions.
                - Upgrades: Author who added new methods, upgrades or fixes.
                - Documentation: Author who wrote the initial documentation.
                - Review: Author who reviewed/modified/fixed/updated the documentation.
                - Validation: Author - different from first concept one - who certifies the class is operating as it should.
            
            - For example: 
                History of developmens:
                2018-May - First concept - any comments if necessary
                \author J.Galan
                2018-Aug - Documentation - any comments if necessary
                \author J.Galan
                2019-May - Review - any comments if necessary
                \author K.Ni
                2019-May - Validation - any comments if necessary
                \author K.Ni

        - REST processes generating/transforming event output should include a figure ilustrating the output using `DrawEvent`.

        - Detailed description should include examples of use of the class.

        - Any options should be also included/detailed in the detailed description. I.e. drawing option of the event type.



The future proposed structure, naming of classes, will be applied once we have finished documenting the classes.
Re-naming of classes is not good for compatibility. 
Thus, although this TODO section proposes re-naming classes this should only apply at the last moment.
New structure will be further detailed in point E.
This should be applied in a REST 2.3 version.

### D.1 tools/resttools/ --> framework/common and framework/tools

* D.1.1 [ ] TRestDataBase. Documentation exists, but seems wrong formating. Needs revision.

* D.1.2 [ ] TRestDataBase. Validation.

* D.1.3 [ ] TRestSystemOfUnits. Needs to be documented. Description, default units, how to use, etc.

* D.1.4 [ ] TRestPhysics. Document variables, description, units, reference.

* D.1.5 [ ] TRestStringHelper. Few methods to be documented.

* D.1.6 [ ] TRestStringOutput. Methods, members and brief documentation to be written.

* D.1.7 [ ] TRestTools. A couple of methods, detailed description, and brief description to be written.

### D.2 restcore/ --> framework/tools and framework/core

* D.2.1 [ ] RmlUpdateTool. What we do with this? This is only necessary to migrate RMLs from 2.1 to 2.2? 
      Perhaps we may remove it from 2.3?

* D.2.2 [ ] TRestTask. It should describe how to fully integrate a macro into REST, and extend functionalities. Add header.

* D.2.3 [ ] TRestAnalysisPlot. Write documentation.

* D.2.4 [ ] TRestAnalysisPlot. Allow to combine several histograms in a plot, i.e. after we apply some cuts. Add <histo> scheme to <plot>.

* D.2.5 [ ] TRestAnalysisPlot. Add a fitting attribute to <histo> so that it performs a hit using ROOT fitting functions and plots it in.

* D.2.6 [ ] TRestAnalysisTree. To be documented. Detailed description, use, examples, etc.

* D.2.7 [ ] TRestBrowser. To be documented. Detailed description, use, examples, etc. Discuss a little the connection with TRestEventViewer.

* D.2.8 [ ] TRestEvent. Validate documentation, complete if needed.

* D.2.9 [ ] TRestEventProcess. Validate documentation, complete if needed.

* D.2.10 [ ] TRestEventViewer. To be documented.

* D.2.11 [ ] TRestManager. Documentation/class to be reviewed/validated.

* D.2.12 [ ] TRestMetadata. Documentation/class to be reviewed/validated.

* D.2.13 [ ] TRestProcessRunner. Documentation/class to be reviewed/validated.

* D.2.14 [ ] TRestProcessRunner. Documentation/class to be reviewed/validated.

* D.2.15 [ ] TRestRun. Documentation to be completed, detailed description, few methods.

* D.2.16 [ ] TRestThread. Documentation/class to be reviewed/validated.

### D.3 metadata/daq --> libraries/raw/

* D.3.1 [ ] TRestDAQMetadata. Documentation review/validation. This class should be used to store information relative to the configuration of the electronics, shaping, electronic gain, trigger delay, etc, etc.

### D.4 metadata/geant4 --> libraries/geant4/

* D.4.1 [ ] TRestBiasingVolume. To become obsolete? See task XX.

* D.4.2 [ ] TRestG4Metadata. Rename to TRestGeant4Metadata. Validate documentation. Remove biasing volumes.

* D.4.3 [ ] TRestG4PrimaryGenerator. Rename to TRestGeant4PrimaryGenerator. Write documentation.

* D.4.4 [ ] TRestParticle. Rename to TRestGeant4Particle. Write documentation.

* D.4.5 [ ] TRestParticleCollection. Rename to TRestGeant4ParticleCollection. Write documentation.

* D.4.6 [ ] TRestPhysicsList. Rename to TRestGeant4PhysicsList. Write documentation.

### D.5 metadata/general --> libraries/detector/

* D.5.1 [ ] TRestDetectorSetup. Should serve to define conditions of the detector, mesh voltages, etc. However, could we have those also on TRestDetectorReadout? There is anyway a clear connection of TRestDetectorSetup, TRestGas and TRestReadout.

* D.5.2 [ ] TRestGas. To be renamed to TRestDetectorGas.

* D.5.3 [ ] TRestGeometry. It is used?

### D.6 metadata/readout --> libraries/detector/

* D.6.1 [ ] TRestReadout. To be renamed to TRestDetectorReadout. Documentation/class validation.

* D.6.2 [ ] TRestReadoutPlane. To be renamed to TRestDetectorReadoutPlane. Documentation/class validation.

* D.6.3 [ ] TRestReadoutModule. To be renamed to TRestDetectorReadoutModule. Documentation/class validation.

* D.6.4 [ ] TRestReadoutChannel. To be renamed to TRestDetectorReadoutChannel. Documentation/class validation.

* D.6.5 [ ] TRestReadoutPixel. To be renamed to TRestDetectorReadoutPixel. Documentation/class validation.

* D.6.6 [ ] TRestReadoutMapping. To be renamed to TRestDetectorReadoutMapping. Documentation/class validation.

### D.7 events/addons --> libraries/raw/ and framework/tools/

* D.7.1 [ ] TRestFFT. It helps to perform an FFT in a TRestRawSignal. However, actually is working in a TRestSignal. The methods need to be migrated to TRestRawSignal. FFT makes only sense on fixed time interval data as TRestRawSignal. Helper methods inside TRestFFT are only related to signal conditioning related to raw signal processing. This class should end up in RestRawLib, and renamed to TRestRawFFT.

* D.7.2 [ ] TRestMesh. A quite generic class, exploited mainly by TRestDetectorReadoutMapping. However, this class could end up in `framework/tools/`. It might be used for detector readout, but also, for example, for regular field definition or similar.

### D.8 events/analysis/ --> libraries/track, libraries/projectedTrack, libraries/

* D.8.1 [ ] TRest2DHitsEvent. To be documented. Could this fit in an existing library? What is the main purpose? --> Library name? Muon track analysis? RestMuonLib?

* D.8.2 [ ] TRestLinearTrack. To be renamed to TRestProjectedTrack.

* D.8.3 [ ] TRestLinearTrackEvent. To be renamed to TRestProjectedTrackEvent.

* D.8.4 [ ] TRestTrack. To be documented.

* D.8.5 [ ] TRestTrackEvent. To be documented.

### D.9 events/geant4/ --> libraries/geant4/

* D.9.1 [ ] TRestG4Event. To be renamed to TRestGeant4Event. Write documentation, detailed description and specially drawing methods and options.

* D.9.2 [ ] TRestG4Hits. To be renamed to TRestGeantHits. Write documentation.

* D.9.3 [ ] TRestG4Track. To be renamed to TRestGeant4Track. Write documentation.

### D.10 events/general/ --> libraries/detector/ and libraries/raw/

* D.10.1 [ ] TRestSignalEvent. To be renamed to TRestDetectorSignalEvent. Write documentation.

* D.10.2 [ ] TRestSignal. To be renamed to TRestDetectorSignal. Write documentation.

* D.10.3 [ ] TRestHitsEvent. To be renamed to TRestDetectorHitsEvent. Write documentation.

* D.10.4 [ ] TRestHits. To be renamed to TRestDetectorHits. Write documentation.

* D.10.5 [ ] TRestVolumeHits. OBSOLETE? This class is connected to TRestTrack. Do we really need volume hits? Should we just use DetectorHits.

* D.10.6 [ ] TRestRawSignalEvent. Write documentation.

* D.10.7 [ ] TRestRawSignal. Write documentation.

### D.11 events/viewers/

* D.11.1 [ ] TRestEveEventViewer. To be moved to framework/tools? Documentation to be written.

* D.11.2 [ ]  TRestGenericEventViewer. To be moved to framework/tools? Documentation to be written.

* D.11.3 [ ] TRestG4EventViewer. To be renamed to TRestGeant4EventViewer --> libraries/geant4/. Documentation to be written.

* D.11.4 [ ] TRestHitsEventViewer. To be renamed to TRestDetectorHitsEventViewer. --> libraries/detector. Documentation to be written.

* D.11.5 [ ] TRestReadoutEventViewer. To be renamed to TRestDetetorSignalEventViewer? --> libraries/detector. Documentation to be written.

### D.12 processes/general/

* [ ] D.12.1 TRestAvalancheProcess. Rename to TRestDetectorAvalancheProcess. Write documentation.

* [ ] D.12.2 TRestElectronDiffusionProcess. Rename to TRestDetectorDriftDiffusionProcess? Write documentation.

* [ ] D.12.3 TRestFastHitsToTrackProcess. Rename to TRestTrackDetectorToTrackProcess. Write documentation.

* [ ] D.12.4 TRestFastHitsToTrackProcess. Rename to TRestTrackFastDetectorToTrackProcess. Write documentation.

* [ ] D.12.5 TRestFiducializationProcess. Rename to TRestDetectorFiducializationProcess. Write documentation.

* [ ] D.12.6 TRestG4toHitsProcess. Rename to TRestGeant4ToDetectorProcess. Write documentation.

* [ ] D.12.7 TRestGarfieldDriftProcess. Rename to TRestDetectorDriftGarfieldProcess. Write documentation.

* [ ] D.12.8 TRestHitsNormalizationProcess. Rename to TRestDetectorHitsNormalizationProcess. Write documentation.

* [ ] D.12.9 TRestHitsReductionProcess. Rename to TRestDetectorHitsReductionProcess. Write documentation.

* [ ] D.12.10 TRestRotateAndTranslateProcess. Rename to TRestDetectorHitsTransformProcess. Write documentation.

* [ ] D.12.11 TRestHitsShuffleProcess. Rename to TRestDetectorHitsShuffleProcess. Write documentation.

* [ ] D.12.12 TRestHitsToSignalProcess. Rename to TRestDetectorHitsToSignalProcess. Write documentation.

* [ ] D.12.13 TRestHitsToTrackProcess. Rename to TRestDetectorHitsToTrackProcess. Write documentation.

* [ ] D.12.14 TRestRawSignalTo2DHitsProcess. Should be moved to the same library (same name convention) as TRest2DHits. Write documentation.

* [ ] D.12.15 TRestRawSignalToSignalProcess. Rename to TRestRawSignalToDetectorProcess. Write documentation.

* [ ] D.12.16 TRestSignalToHitsProcess. Rename to TRestDetectorSignalToHitsProcess. Write documentation.

* [ ] D.12.17 TRestSignalToRawSignalProcess. Rename to TRestRawDetectorToRawSignalProcess. Write documentation.

* [ ] D.12.18 TRestSignalZeroSuppressionProcess. Rename to TRestRawZeroSuppresionProcess. Write documentation.

* [ ] D.12.19 TRestSmearingProcess. Rename to TRestDetectorHitsSmearingProcess. Write documentation.

* [ ] D.12.20 TRestTrackDetachIsolatedNodes. Write documentation.

* [ ] D.12.21 TRestTrackLinearizationProcess. Rename to TRestProjectedTrackGenerateProjectionProcess. Write documentation.

* [ ] D.12.22 TRestTrackPathMinimizationProcess. Write documentation.

* [ ] D.12.23 TRestTrackReconnectionProcess. Write documentation.

* [ ] D.12.24 TRestTrackReductionProcess. Write documentation.

* [ ] D.12.25 TRestTrackToHitsProcess. Rename to TRestTrackToDetectorHitsProcess. Write documentation.

### D.13 processes/raw/ --> libraries/raw

Note : Rename these processes to TRestRawToSignalAFTERProcess, TRestRawToSignalCoBoAsAdProcess, TRestRawToSignalUSTCProcess? So that all import files who inherit from TRestRawToSignal share a common root name?

* D.13.1 [ ] TRestAFTERToSignalProcess. Rename to TRestRawAFTERToSignalProcess. Write documentation.

* D.13.2 [ ] TRestCoboAsAdToSignalProcess. Rename to TRestRawCoBoAsAdToSignalProcess. Write documentation.

* D.13.3 [ ] TRestMultiCoBoAsAdToSignalProcess. Rename to TRestRawMultiCoBoAsAdToSignalProcess. Write documentation.

* D.13.4 [ ] TRestMultiFEMINOSToSignalProcess. Rename to TRestRawMultiFEMINOSToSignalProcess. Write documentation.

* D.13.5 [ ] TRestRawToSignalProcess. Write documentation. Main class from which all other classes inherit.

* D.13.6 [ ] TRestSharedMemoryBufferToRawSignalProcess. Rename to TRestRawSharedMemToSignalProcess. Write documentation.

* D.13.7 [ ] TRestUSTCElectronicsProcess. Rename to TRestRawUSTCElectronicsToSignalProcess. Write documentation.

* D.13.8 [ ] TRestUSTCElectronicsProcess. Rename to TRestRawUSTCElectronicsToSignalProcess. Write documentation.

### D.14 processes/signal --> libraries/raw

* D.14.1 [ ] TRestFindResponseSignalProcess. Rename to TRestRawGenerateSignalResponse. Review/validate and write documentation. Filter/select pulses with particular characteristics then this process generates a response out of an average.

* D.14.2 [ ] TRestSignalAddNoiseProcess. Rename to TRestRawAddSignalNoiseProcess. Validate/write documentation.

* D.14.3 [ ] TRestRawSignalRecoverChannelsProcess. Rename to TRestRawRecoverChannelsProcess. A process to try to add/recover charge on dead channels using information on neighbout channels. Validate/write documentation.

* D.14.4 [ ] TRestRawSignalRemoveChannelsProcess. Rename to TRestRawRemoveChannelsProcess. A process to artificially remove channels to produce the effect of dead channel on mock Montecarlo data. Validate/write documentation.

* D.14.5 [ ] TRestRawSignalShapingProcess. Validate/write documentation.

* D.14.6 [ ] TRestSignalDeconvolutionProcess. OBSOLETE. Should be replaced by TRestRawSignalEvents.

### D.15 processes/analysis --> libraries/raw

* D.15.1 [ ] TRestAnalysisHistogramViewerProcess. --> libraries/track/ OBSOLETE? Rename to TRestTrackHistogramViewerProcess? Looks as a user custom oriented process.

* D.15.2 [ ] TRestFindG4BlobProcess. --> libraries/geant4/. Rename to TRestGeant4FindEndTracksProcess. It only works on NLDBD tracks where we have 2-electrons. But it could be generalized if needed. We should add a message in case the G4Event does not have this feature (exactly 2-electrons) and prompt a warnning message telling that the feature to adapt any number needs to be implemented.

* D.15.3 [ ] TRestGeant4AnalysisProcess. --> libraries/geant4/. Write documentation.

* D.15.4 [ ] TRestHitsAnalysisProcess. --> libraries/detector/. Rename to TRestDetectorHitsAnalysisProcess. Write documentation.

* D.15.5 [ ] TRestMuonAnalysisProcess. --> libraries/muon/? Write documentation.

* D.15.6 [ ] TRestPointLikeTrackAnaProcess --> libraries/detector/. TRestDetectorSignalAnalysisProcess? Understand/review and write documentation. Someone is using it?

* D.15.7 [ ] TRestPointLikeTrackAnaProcess. --> libraries/track/. OBSOLETE? Someone using this? Looks as a short version of TRestTrackAnalysisProcess.

* D.15.8 [ ] TRestRawSignalAnalysisProcess --> libraries/raw/. Write documentation.

* D.15.9 [ ] TRestRawSignalViewerProcess --> libraries/raw/. A process that takes the event type and draws it in a canvas. It can be used as online viewer during data chain processing. Could we create a c++ template process that accepts any event type and calls draw event with the options provided through RML?

* D.15.10 [ ] TRestReadoutAnalysisProcess. --> libraries/detector/. Perhaps this could be merged with TRestDetectorSignalAnalysisProcess/. Write documentation.

* D.15.11 [ ] TRestSignalChannelActivityProcess. --> libraries/detector/. Rename to TRestDetectorChannelActivityProcess. Write documentation.

* D.15.12 [ ] TRestSignalViewerProcess. --> libraries/detector/. Same case as TRestRawSignalViewerProcess. Should we have a template process?


## E. New directory structure. Towards 2.3.0 release.

After previous points have been addressed. We start to integrate the new directory structure. This topic might be discussed in between.

Preliminary proposed structure, this is just a very open proposal to be discussed.
- framework/tools : Like tools, string helper, anlysis tools, etc
- framework/physics : REST_Units, REST_Physics, TRestParticle interface, TRestFieldMap interface, TRestHits, TRestSignal, TRestMesh.
- framework/core : TRestEvent, TRestEventProcess, TRestMetadata, TRestRun, TRestManager, TRestProcessRunner, TRestThread

- libraries/detector : TRestSignalEvent, TRestHitsEvent, and TRestReadout related processes and metadata.
- libraries/raw : TRestRawSignalEvent related processes and metadata.
- libraries/geant4 : TRestGeant4Event related processes and metadata.
- libraries/track : TRestTrackEvent related processes and metadata.
- libraries/projectedTrack : Related TRestProjectedTrack processes and metadata.

- libraries/dummy : It will be used as a reference for all other libraries, CMakeLists and directory structure, data/ examples/ etc/ macros/ src/ inc/.

