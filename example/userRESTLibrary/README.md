====================================

Author : J. Galan
Date : 7-June-2016
Updated : 12-December-2016
Description : An example implementing a new process and a new metadata structure inside REST.

===================================

Class implementation :

    - src/myMetadata.cxx and inc/myMetadata.h

                This class defines a new metadata section named "myMetadataSection". We only define a member inside, named "dummy".
                A section example is defined inside config/myMetadata.rml

    - src/mySignalProcess.cxx and inc/mySignalProcess.h

                This class defines a new process. This process defines a TRestSignalEvent as input and a TRestSignalEvent as output.
                The process is doing nothing, just transfering the input event to the output event.
                It has a process metadata member named fMyDummyParameter that is connected with the configuration parameter "aDummyParameter".
                

Compilation :

    mkdir build

    cd build

    cmake ../

    make

    make install


Use:

    * As soon as the library is installed at a path seen by LD_LIBRARY_PATH, and the name of the library contains REST (or Rest), the processes will be automatically seen by restManager binary.

    * We can add the process to TRestManager using the name of the class inside the TRestManager section of an RML file.

    <addProcess type="mySignalProcess" name="myPcs" value="ON" />

    * Then, we will need to provide also the process definition. For example:

    <section mySignalProcess name="myPcs" title="A process title">
        <parameter name="myDummyParameter" value="40" />
    </section>
    
