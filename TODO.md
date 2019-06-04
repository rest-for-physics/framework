TODO list
---------

This is our TODO list. A TODO bullet might be connected to a post in the forum site. Use it to discuss about options to solve a particular point, or to inform others about a fix commited to the development branch.

* [ ] A first process, if declared as `external`, cannot access the `analysisTree`. We should override this limitation. Furthermore, it would be desirable that if the first process is `external` and the process itself does not stamp the `eventID`, `timestamp`, etc, an automatic numbering will be assigned by `TRestEventProcess`. E.g. just using the entry number on the `analysisTree`.

* [ ] Implement a way to define a materials.xml file common to every-one. Avoiding `materials.xml` file to propagate on different geometry setups. Instead, geometry setups will use always a common one that can be validated and maintained by REST users. In summary, find a way that the GDML file will give a reference to `REST_PATH/data/definitions/materials.xml`.

* [ ] `restG4GammaTransfer` independent calculation.

* [ ] Plot an analysis tree observable with different cuts using `TRestAnalysisPlot`.

* [ ] Add `IF` condition option to `RML` files.

* [ ] New metadata `TRestSpectrumCalibration` containing members allowing to identify different peaks in the spectrum, including energy, width, tagName, etc. Perhaps an auxiliar `TRestPeak::TObject` can be used to store those values. This metadata structure might be filled by a process `TRestMultiPeakFinderProcess` that fits a branch variable from the `analysisTree`.

* [ ] Implement a new process `TRestRawSignalToHitsProcess` that allows to fit detector signals and obtain the TRestHitsEvent directly.

* [ ] Integrate the use of `TRestDetectorSetup` inside `TRestMultiFEMINOSToSignal` to retrieve the information of run number, detector settings, run tag, etc. `TRestDetectorSetup` should be added to the metadata structures in the data flow, and at the same time the values of `TRestRun` and `TRestAnalysisTree` should be updated according to the information in `TRestDetectorSetup`.

* [ ] `Gas file server` web interface.

* [ ] `restExplorer` package.

* [ ] `restSQL` REST metadata indexing/database population + re-implement `restWeb` to search and access the REST file server database.

* [x] Fix required in TRestBrowser. See [forum post](http://ezpc10.unizar.es/t/error-when-using-trestbrowser-trestbrowser-inherits-from-trestrun/41)

* [x] Give the event tree the name of the specific event stored. See [forum post](http://ezpc10.unizar.es/t/eventtree-should-be-stored-using-the-name-of-the-specific-event-output/42).

* [x] Fix duplicated metadata entries. See [forum post](http://ezpc10.unizar.es/t/duplicated-metadata-entries-in-last-version-v2-2-10/38).

* [x] Integrate `Decay0` generator code into `libRestG4`. See [forum post](http://ezpc10.unizar.es/t/problem-reading-decay0-generator-with-restg4/47/7).

* [ ] Optimize TRestReadout initialization See [forum post](http://ezpc10.unizar.es/t/optimize-trestreadout-startup-time/52).

* [ ] We need to work on the `TRestEvent::DrawEvent()` methods of the different event types. See implementation of `TRestG4Event::DrawEvent` to be used as reference. We should create first the documentation of this method in `TRestG4Event` and then use it in other `TRestEvent::DrawEvent` methods.

* [ ] Work on `basic RML's examples` and validation for REST new comers. Add a README.md giving a brief description of the examples.

* [ ] Work on having a `README.md` description **on each data directory**.

* [ ] Documentation of all classes using doxygen.

   * General guidelines for documentation (TODO).
    
        - CXX File should include a LICENCE header, and general class description. As for example TRestG4Metadata.

        - Methods should be documented where ever they are implemented. If implemented in TRestXX.h file they must be documented in the TRestXX.h file. Use \brief to have a quick description on the method list.

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

### tools/resttools/ --> framework/common and framework/tools

* [ ] TRestDataBase. Documentation exists, but seems wrong formating. Needs revision.

* [ ] TRestDataBase. Validation.

* [ ] TRestSystemOfUnits. Needs to be documented. Description, default units, how to use, etc.

* [ ] TRestPhysics. Document variables, description, units, reference.

* [ ] TRestStringHelper. Few methods to be documented.

* [ ] TRestStringOutput. Methods, members and brief documentation to be written.

* [ ] TRestTools. A couple of methods, detailed description, and brief description to be written.

### restcore/ --> framework/tools and framework/core

* [ ] RmlUpdateTool. What we do with this? This is only necessary to migrate RMLs from 2.1 to 2.2? 
      Perhaps we may remove it from 2.3?

* [ ] TRestTask. It should describe how to fully integrate a macro into REST, and extend functionalities. Add header.

* [ ] TRestAnalysisPlot. Write documentation.

* [ ] TRestAnalysisPlot. Allow to combine several histograms in a plot, i.e. after we apply some cuts. Add <histo> scheme to <plot>.

* [ ] TRestAnalysisPlot. Add a fitting attribute to <histo> so that it performs a hit using ROOT fitting functions and plots it in.

* [ ] TRestAnalysisTree. To be documented. Detailed description, use, examples, etc.

* [ ] TRestBrowser. To be documented. Detailed description, use, examples, etc. Discuss a little the connection with TRestEventViewer.

* [ ] TRestEvent. Validate documentation, complete if needed.

* [ ] TRestEventProcess. Validate documentation, complete if needed.

* [ ] TRestEventViewer. To be documented.

* [ ] TRestManager. Documentation/class to be reviewed/validated.

* [ ] TRestMetadata. Documentation/class to be reviewed/validated.

* [ ] TRestProcessRunner. Documentation/class to be reviewed/validated.

* [ ] TRestProcessRunner. Documentation/class to be reviewed/validated.

* [ ] TRestRun. Documentation to be completed, detailed description, few methods.

* [ ] TRestThread. Documentation/class to be reviewed/validated.

### metadata/daq --> libraries/raw/

* [ ] TRestDAQMetadata. Documentation review/validation. This class should be used to store information relative to the configuration of the electronics, shaping, electronic gain, trigger delay, etc, etc.

### metadata/geant4 --> libraries/geant4/

* [ ] TRestBiasingVolume. To become obsolete? See task XX.

* [ ] TRestG4Metadata. Rename to TRestGeant4Metadata. Validate documentation. Remove biasing volumes.

* [ ] TRestG4PrimaryGenerator. Rename to TRestGeant4PrimaryGenerator. Write documentation.

* [ ] TRestParticle. Rename to TRestGeant4Particle. Write documentation.

* [ ] TRestParticleCollection. Rename to TRestGeant4ParticleCollection. Write documentation.

* [ ] TRestPhysicsList. Rename to TRestGeant4PhysicsList. Write documentation.

### metadata/general --> libraries/detector/

* [ ] TRestDetectorSetup. Should serve to define conditions of the detector, mesh voltages, etc. However, could we have those also on TRestDetectorReadout? There is anyway a clear connection of TRestDetectorSetup, TRestGas and TRestReadout.

* [ ] TRestGas. To be renamed to TRestDetectorGas.

* [ ] TRestGeometry. It is used?

### metadata/readout --> libraries/detector/

* [ ] TRestReadout. To be renamed to TRestDetectorReadout. Documentation/class validation.

* [ ] TRestReadoutPlane. To be renamed to TRestDetectorReadoutPlane. Documentation/class validation.

* [ ] TRestReadoutModule. To be renamed to TRestDetectorReadoutModule. Documentation/class validation.

* [ ] TRestReadoutChannel. To be renamed to TRestDetectorReadoutChannel. Documentation/class validation.

* [ ] TRestReadoutPixel. To be renamed to TRestDetectorReadoutPixel. Documentation/class validation.

* [ ] TRestReadoutMapping. To be renamed to TRestDetectorReadoutMapping. Documentation/class validation.

### events/addons --> libraries/raw/ and framework/tools/

* [ ] TRestFFT. It helps to perform an FFT in a TRestRawSignal. However, actually is working in a TRestSignal. The methods need to be migrated to TRestRawSignal. FFT makes only sense on fixed time interval data as TRestRawSignal. Helper methods inside TRestFFT are only related to signal conditioning related to raw signal processing. This class should end up in RestRawLib, and renamed to TRestRawFFT.

* [ ] TRestMesh. A quite generic class, exploited mainly by TRestDetectorReadoutMapping. However, this class could end up in `framework/tools/`. It might be used for detector readout, but also, for example, for regular field definition or similar.

### events/analysis/ --> libraries/track, libraries/projectedTrack, libraries/

* [ ] TRest2DHitsEvent. To be documented. Could this fit in an existing library? What is the main purpose? --> Library name? Muon track analysis? RestMuonLib?

* [ ] TRestLinearTrack. To be renamed to TRestProjectedTrack.

* [ ] TRestLinearTrackEvent. To be renamed to TRestProjectedTrackEvent.

* [ ] TRestTrack. To be documented.

* [ ] TRestTrackEvent. To be documented.

### events/geant4/ --> libraries/geant4/

* [ ] TRestG4Event. To be renamed to TRestGeant4Event. Write documentation, detailed description and specially drawing methods and options.

* [ ] TRestG4Hits. To be renamed to TRestGeantHits. Write documentation.

* [ ] TRestG4Track. To be renamed to TRestGeant4Track. Write documentation.

### events/general/ --> libraries/detector/ and libraries/raw/

* [ ] TRestSignalEvent. To be renamed to TRestDetectorSignalEvent. Write documentation.

* [ ] TRestSignal. To be renamed to TRestDetectorSignal. Write documentation.

* [ ] TRestHitsEvent. To be renamed to TRestDetectorHitsEvent. Write documentation.

* [ ] TRestHits. To be renamed to TRestDetectorHits. Write documentation.

* [ ] TRestVolumeHits. OBSOLETE? This class is connected to TRestTrack. Do we really need volume hits? Should we just use DetectorHits.

* [ ] TRestRawSignalEvent. Write documentation.

* [ ] TRestRawSignal. Write documentation.

### events/viewers/

* [ ] TRestEveEventViewer. To be moved to framework/tools? Documentation to be written.

* [ ] TRestGenericEventViewer. To be moved to framework/tools? Documentation to be written.

* [ ] TRestG4EventViewer. To be renamed to TRestGeant4EventViewer --> libraries/geant4/. Documentation to be written.

* [ ] TRestHitsEventViewer. To be renamed to TRestDetectorHitsEventViewer. --> libraries/detector. Documentation to be written.

* [ ] TRestReadoutEventViewer. To be renamed to TRestDetetorSignalEventViewer? --> libraries/detector. Documentation to be written.

### processes/general/

* [ ] TRestAvalancheProcess. Rename to TRestDetectorAvalancheProcess. Write documentation.

* [ ] TRestElectronDiffusionProcess. Rename to TRestDetectorDriftDiffusionProcess? Write documentation.

* [ ] TRestFastHitsToTrackProcess. Rename to TRestTrackDetectorToTrackProcess. Write documentation.

* [ ] TRestFastHitsToTrackProcess. Rename to TRestTrackFastDetectorToTrackProcess. Write documentation.

* [ ] TRestFiducializationProcess. Rename to TRestDetectorFiducializationProcess. Write documentation.

* [ ] TRestG4toHitsProcess. Rename to TRestGeant4ToDetectorProcess. Write documentation.

* [ ] TRestGarfieldDriftProcess. Rename to TRestDetectorDriftGarfieldProcess. Write documentation.

* [ ] TRestHitsNormalizationProcess. Rename to TRestDetectorHitsNormalizationProcess. Write documentation.

* [ ] TRestHitsReductionProcess. Rename to TRestDetectorHitsReductionProcess. Write documentation.

* [ ] TRestRotateAndTranslateProcess. Rename to TRestDetectorHitsTransformProcess. Write documentation.

* [ ] TRestHitsShuffleProcess. Rename to TRestDetectorHitsShuffleProcess. Write documentation.

* [ ] TRestHitsToSignalProcess. Rename to TRestDetectorHitsToSignalProcess. Write documentation.

* [ ] TRestHitsToTrackProcess. Rename to TRestDetectorHitsToTrackProcess. Write documentation.

* [ ] TRestRawSignalTo2DHitsProcess. Should be moved to the same library (same name convention) as TRest2DHits. Write documentation.

* [ ] TRestRawSignalToSignalProcess. Rename to TRestRawSignalToDetectorProcess. Write documentation.

* [ ] TRestSignalToHitsProcess. Rename to TRestDetectorSignalToHitsProcess. Write documentation.

* [ ] TRestSignalToRawSignalProcess. Rename to TRestRawDetectorToRawSignalProcess. Write documentation.

* [ ] TRestSignalZeroSuppressionProcess. Rename to TRestRawZeroSuppresionProcess. Write documentation.

* [ ] TRestSmearingProcess. Rename to TRestDetectorHitsSmearingProcess. Write documentation.

* [ ] TRestTrackDetachIsolatedNodes. Write documentation.

* [ ] TRestTrackLinearizationProcess. Rename to TRestProjectedTrackGenerateProjectionProcess. Write documentation.

* [ ] TRestTrackPathMinimizationProcess. Write documentation.

* [ ] TRestTrackReconnectionProcess. Write documentation.

* [ ] TRestTrackReductionProcess. Write documentation.

* [ ] TRestTrackToHitsProcess. Rename to TRestTrackToDetectorHitsProcess. Write documentation.

### processes/raw/ --> libraries/raw

Note : Rename these processes to TRestRawToSignalAFTERProcess, TRestRawToSignalCoBoAsAdProcess, TRestRawToSignalUSTCProcess? So that all import files who inherit from TRestRawToSignal share a common root name?

* [ ] TRestAFTERToSignalProcess. Rename to TRestRawAFTERToSignalProcess. Write documentation.

* [ ] TRestCoboAsAdToSignalProcess. Rename to TRestRawCoBoAsAdToSignalProcess. Write documentation.

* [ ] TRestMultiCoBoAsAdToSignalProcess. Rename to TRestRawMultiCoBoAsAdToSignalProcess. Write documentation.

* [ ] TRestMultiFEMINOSToSignalProcess. Rename to TRestRawMultiFEMINOSToSignalProcess. Write documentation.

* [ ] TRestRawToSignalProcess. Write documentation. Main class from which all other classes inherit.

* [ ] TRestSharedMemoryBufferToRawSignalProcess. Rename to TRestRawSharedMemToSignalProcess. Write documentation.

* [ ] TRestUSTCElectronicsProcess. Rename to TRestRawUSTCElectronicsToSignalProcess. Write documentation.

* [ ] TRestUSTCElectronicsProcess. Rename to TRestRawUSTCElectronicsToSignalProcess. Write documentation.

### processes/signal --> libraries/raw

* [ ] TRestFindResponseSignalProcess. Rename to TRestRawGenerateSignalResponse. Review/validate and write documentation. Filter/select pulses with particular characteristics then this process generates a response out of an average.

* [ ] TRestSignalAddNoiseProcess. Rename to TRestRawAddSignalNoiseProcess. Validate/write documentation.

* [ ] TRestRawSignalRecoverChannelsProcess. Rename to TRestRawRecoverChannelsProcess. A process to try to add/recover charge on dead channels using information on neighbout channels. Validate/write documentation.

* [ ] TRestRawSignalRemoveChannelsProcess. Rename to TRestRawRemoveChannelsProcess. A process to artificially remove channels to produce the effect of dead channel on mock Montecarlo data. Validate/write documentation.

* [ ] TRestRawSignalShapingProcess. Validate/write documentation.

* [ ] TRestSignalDeconvolutionProcess. OBSOLETE. Should be replaced by TRestRawSignalEvents.

### processes/analysis --> libraries/raw

* [ ] TRestAnalysisHistogramViewerProcess. --> libraries/track/ OBSOLETE? Rename to TRestTrackHistogramViewerProcess? Looks as a user custom oriented process.

* [ ] TRestFindG4BlobProcess. --> libraries/geant4/. Rename to TRestGeant4FindEndTracksProcess. It only works on NLDBD tracks where we have 2-electrons. But it could be generalized if needed. We should add a message in case the G4Event does not have this feature (exactly 2-electrons) and prompt a warnning message telling that the feature to adapt any number needs to be implemented.

* [ ] TRestGeant4AnalysisProcess. --> libraries/geant4/. Write documentation.

* [ ] TRestHitsAnalysisProcess. --> libraries/detector/. Rename to TRestDetectorHitsAnalysisProcess. Write documentation.

* [ ] TRestMuonAnalysisProcess. --> libraries/muon/? Write documentation.

* [ ] TRestPointLikeTrackAnaProcess --> libraries/detector/. TRestDetectorSignalAnalysisProcess? Understand/review and write documentation. Someone is using it?

* [ ] TRestPointLikeTrackAnaProcess. --> libraries/track/. OBSOLETE? Someone using this? Looks as a short version of TRestTrackAnalysisProcess.

* [ ] TRestRawSignalAnalysisProcess --> libraries/raw/. Write documentation.

* [ ] TRestRawSignalViewerProcess --> libraries/raw/. A process that takes the event type and draws it in a canvas. It can be used as online viewer during data chain processing. Could we create a c++ template process that accepts any event type and calls draw event with the options provided through RML?

* [ ] TRestReadoutAnalysisProcess. --> libraries/detector/. Perhaps this could be merged with TRestDetectorSignalAnalysisProcess/. Write documentation.

* [ ] TRestSignalChannelActivityProcess. --> libraries/detector/. Rename to TRestDetectorChannelActivityProcess. Write documentation.

* [ ] TRestSignalViewerProcess. --> libraries/detector/. Same case as TRestRawSignalViewerProcess. Should we have a template process?

---

Structure of directories README.md

