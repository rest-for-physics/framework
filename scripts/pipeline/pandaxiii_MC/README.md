The following scripts are used to validate the PandaX-III simulation & topological analysis chain.

step 1: Use Xe136bb0n.rml to tun restG4 for Geant4 simulation. Use ValidateG4.C to check if the simulation 
goes smoothly. i.e. all 500 events are simulated and metadata is correctly recorded. The output file will 
be used later on

step 2: Use AllProcesses.rml to run restManager for REST simulation and topological analysis. processes_2D.rml 
and readout-140kg.rml is included by it.

step 3: Use plots.rml to check the plots can be successfully drawn by TRestAnalysisPlot

If geant4 version is not 10.4.2, we will use testInput.root instead of the one generated in step 1 as the input 
of step 2. This file is generated manually with 10.4.2 with only 10 events saved.

