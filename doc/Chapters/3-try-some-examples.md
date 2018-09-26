## Try Some Examples

The main executable of REST is restManager and restRoot. By typing directly `restManager` it will show 
its usage. By typing restRoot the user can access to REST libraries and macros inside ROOT prompt. We 
have some example files for restManager in the directory ./example. We first switch to that 
directory.

### Save a metadata file

We generate here a matadata file saving several definitions of the detector's readout and gas metadata. 
The readout metadata includes readout plane geometry, daq channel mapping, strip gain, etc. The gas 
metadata includes infomation of gas component, pressure, diffusion and drift speed. This file will be 
very important for running the processes. Generate it by typing :

`restManager --c saveMetadata.rml --o meta.root`

This command will make REST to save two TRestGas classes and one TRestReadout class to the file 
"meta.root". This file can be used in the later data analysis work. The content of gas and readout 
definition can be observed the rml file. Later we will explain what it did. 

### Process a raw data file

If you are in pandax-iii daq server, you can try to process some raw data file with the readout file we just 
generated. For example, to process a 7MM run data file, we can type the command:

`restManager --c multiCoboAnalysis.rml --i /data2/7MM/graw/CoBo_AsAd0_2017-12-23T17\:24\:04.657_0000.graw --o abc.root`

This gives REST names of the config file, input file and output file. If works smoothly, you can see numbers of
processed events increasing.

When it is done, use command: `restRoot abc.root` to see the content of the generated ROOT file. The file should 
contain two trees and some saved classes. We will talk it later.


[**prev**](2-installing-rest.md)
[**contents**](0-contents.md)
[**next**](4-the-rest-framework.md)