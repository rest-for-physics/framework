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

