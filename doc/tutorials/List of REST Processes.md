\brief A list of the different event data processes implemented in REST together with a brief funcionality description.

## Data transformation processes

These processes are in charged of transforming data between different basic data types in REST. 

As soon as we transform one data type to another we can make use of the dedicated data type processes. For example, if we have a geant4 event (TRestG4Event), we may transform it to a basic hits event (TRestHitsEvent) and continue processing the event using the basic *hit processes*.

REST process | Input type | Output type | Description
-------------|------------|-------------|------------
TRestRawSignalToSignalProcess               | TRestRawSignal    | TRestSignalEvent  | Transforms a rawsignal into a signal event.
TRestSignalToHitsProcess                    | TRestSignalEvent  | TRestHitsEvent    | Converts a signal event into a hits event using TRestReadout.
TRestHitsToSignalProcess                    | TRestHitsEvent    | TRestSignalEvent  | Transforms a HitsEvent into SignalEvent using TRestReadout.
TRestHitsToTrackProcess                     | TRestHitsEvent    | TRestTrackEvent   | Hit clusterization into tracks by proximity (Accurate).
TRestFastHitsToTrackProcess                 | TRestHitsEvent    | TRestTrackEvent   | Hit clusterization into tracks by proximity (Aproximation).
TRestG4toHitsProcess                        | TRestG4Event      | TRestHitsEvent    | Transforms a geant4 event into a hits event.

## Analysis processes

These are pure analysis processes. They do not transform the event data itself but add new observables/branches to TRestAnalysisTree. They may apply cuts to the event data, somehow deciding if an event should continue being processed. Any process can decide to stop processing an event by just returning a NULL pointer. This should be documented in each process class.

REST process | Input type | Output type | Description
-------------|------------|-------------|------------
TRestRawSignalAnalysisProcess               | TRestRawSignal    | TRestRawSignal    | Adds analysis observables from raw signal event.
TRestHitsAnalysisProcess                    | TRestHitsEvent    | TRestHitsEvent    | Adds analysis observables from hits event.
TRestGeant4AnalysisProcess                  | TRestG4Event      | TRestG4Event      | Adds analysis observables from a geant4 event.
TRestTrackAnalysisProcess                   | TRestTrackEvent   | TRestTrackEvent   | Adds analysis observables from a track event.
TRestTriggerAnalysisProcess                 | TRestSignalEvent  | TRestSignalEvent  | Applies cuts using time window and energy threshold trigger definition.
TRestFindG4BlobAnalysisProcess              | TRestG4Event      | TRestG4Event      | Finds the electron end blobs in a TRestG4Event. For events with at least 2-electron tracks.

## Signal processes

These processes just modify the data inside a signal event, returning again a signal event data type. These kind of processes add signal noise to simulated data, filter noise from rawdata, shape the input signal, or suppress signal data points which are under threshold.

REST process | Input type | Output type | Description
-------------|------------|-------------|------------
TRestAddSignalNoiseProcess                  | TRestSignalEvent      | TRestSignalEvent  | Adds random noise to a signal event.
TRestSignalDeconvolutionProcess             | TRestSignalEvent      | TRestSignalEvent  | Deconvolutes a signal using a given input response signal.
TRestSignalGaussianConvolutionProcess       | TRestSignalEvent      | TRestSignalEvent  | Convolutes the input signal with a gaussian.
TRestSignalShapingProcess                   | TRestSignalEvent      | TRestSignalEvent  | Shapes the input signal with a given input response signal.
TRestFindResponseSignalProcess              | TRestSignalEvent      | TRestSignalEvent  | Selects clean signals from input to be used as response for deconvolution.
TRestSignalZeroSuppresionProcess            | TRestRawSignalEvent   | TRestSignalEvent  | Keeps only points which are found over threshold.

## Hit processes

These processes just modify the data inside a hits event, returning again a hits event data type. These kind of processes normalize hits energy, fiducialize hits in a given volume, drift and diffuse hits, or reduce the hit number using merging algorithms.

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

## Track processes

These processes operate over track event data, or a set of hits that have been grouped into tracks. We may find the minimum path between the hits inside each track, reduce the number of hits inside each track, find the track ends, or project the hits over the main trajectory effectively linearizing the track.

REST process | Input type | Output type | Description
-------------|------------|-------------|------------
TRestFindTrackBlobsProcess                  | TRestTrackEvent       | TRestTrackEvent       | Finds the track end blobs in a TrackEvent .Tracks should have been pre-processed with path minimization and reconnection processes.
TRestTrackLinearizationProcess              | TRestTrackEvent       | TRestLinearTrackEvent | Projects the hits into the track to get dE/dx profile.
TRestTrackPathMinimizationProcess           | TRestTrackEvent       | TRestTrackEvent       | Finds the minimum path between hits inside each track.
TRestTrackReconnectionProcess               | TRestTrackEvent       | TRestTrackEvent       | Improves physical track description after track minimization.
TRestTrackReductionProcess                  | TRestTrackEvent       | TRestTrackEvent       | Reduces the number of hits inside a track by merging closer hits.

## Rawdata processes

These processes read rawdata written in binary format and extract the signal event data to write it into a TRestRawSignalEvent.

REST process | Input type | Output type | Description
-------------|------------|-------------|------------
TRestRawToSignalProcess                     | rawdata               | TRestRawSignalEvent   | Used to encapsulate rawdata to signal processes.
TRestAFTERToSignalProcess                   | rawdata               | TRestRawSignalEvent   | Transforms AFTER data into raw signal event.
TRestCoBoAsAdToSignalProcess                | rawdata               | TRestRawSignalEvent   | Transforms CoBoAsAd data into raw signal event.
TRestMultiCoBoAsAdToSignalProcess           | rawdata               | TRestRawSignalEvent   | Transforms CoBoAsAd data into raw signal event. General version using several CoBoAsAd cards. Event data might be splitted between different data files. The process receives a list of all the files in a given run.
TRestFEMINOSToSignalProcess                 | rawdata               | TRestRawSignalEvent        | Transforms FEMINOS data into SignalEvent.

## Viewer processes

These processes can be connected during intermediate event processes to visualize the input/output of different processes. Still many missing viewer processes should be developed in this section.

REST process | Input type | Output type | Description
-------------|------------|-------------|------------
TRestRawSignalViewerProcess                 | TRestRawSignalEvent   | TRestRawSignalEvent   | Visualizes a raw-signal event
TRestSignalViewerProcess                    | TRestSignalEvent      | TRestSignalEvent      | Visualizes a signal event
