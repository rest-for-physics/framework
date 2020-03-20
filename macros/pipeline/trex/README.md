The following scripts are used to validate the TREX-DM processing chain being read with Feminos cards in TCM mode.

The following RML config files, aqs and readout.root are used to generate a result.root file containning a basic TTree.

The generated TTree inside results.root contains the double observables extracted from the analysisTree from the last ROOT file generated in the RML processing chain.

A validation.root file, corresponding to the results.root file generated at some point of the state of the code, will be used to validate future pushes to the code repository.

The validation.root file was generated using command:

```
source generateBasicTree.C
```
