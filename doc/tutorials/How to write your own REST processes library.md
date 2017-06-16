\brief This tutorial shows how to implement external processes and metadata structures by generating a ROOT-ified external library.

## Introduction

This tutorial shows how to produce your own processes (specific implementations of TRestEventProcess) and/or new metadata structures (specific implementations of TRestMetadata). An event process itself is a metadata structure allowing to receive input parameters from a RML file.

All you need to integrate processes or metadata structures inside REST is to write a C++ class that derives your processes class from the TRestEventProcess class, and your metadata class from the TRestMetadata class. You will then need to generate a shared library (.so) and install it to a path that can be found by LD_LIBRARY_PATH environment variable.

As soon as the library name contains REST or Rest (case sensitive) your new library will be automatically recognized by *restRoot* environment and *restManager* executable. I.e. you will be able to load your classes into a ROOT sesion started with *restRoot* or add your processes to the processing queue definning in the TRestManager RML section.

This document will show you step by step to write new processes/metadata classes, compile them into a new library, and use them in ROOT or REST. 

## Compiling and installing the example included in REST

Inside the REST repository you will find a directory named *example*. This directory is a *dummy* library containning a *dummy* TRestSignalEvent process (mySignalProcess) and a *dummy* TRestMetadata structure (myMetadata). This is a fully working example, therefore you can just compile it, and install to verify how the integration with REST is done.

The following code shows how to compile and install the library, we assume the working directory is now *REST_v2*.

\code

~/REST_v2$ cd example 

// We create the build directory inside 
~/REST_v2/example/$ mkdir build

// We enter the build directory and create the compilation environment
~/REST_v2/example/build$ cmake -DINSTALL_PREFIX=../ ../

// We have defined the library installation at the same example directory.
// If not specified, by default it will install it at the REST_PATH location.
// But if you have no rights you must specify the install path using -DINSTALL_PREFIX

// We compile and install the library
~/REST_v2/example/build$ make -j4
~/REST_v2/example/build$ make install

\endcode

After following those steps, we should have a *lib* directory inside the *example* directory, and inside a library named *libMyRESTLibrary.so*. In order to use the generated library the system needs to be able to find it looking at LD_LIBRARY_PATH.

Add the following line to your *.bashrc* file and launch a new terminal.

\code

// We assume here the REST_v2 source directory is at your home. 
// Change it according to your system!
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/REST_v2/example/lib

\endcode

Once you launch a new terminal and enter inside *restRoot* environment you should see on the output the name of your new library.

\code

~ restRoot

...
Loading library : libRestTools.so
Loading library : libMyRESTLibrary.so
...

\endcode

## Using the new classes inside libMyRESTLibrary

You can use now the new compiled classes as any other class of REST inside ROOT. The first step to make use of the new classes is to define a new RML file for each class, since both of these new classes derive from a TRestMetadata class.

\warning Even if your new class does not contain any metadata member you need to provide a config file with a valid section with the process section name. This might change in the future to avoid empty section definitions.

### Using the class *myMetadata*

The class *myMetadata* defines one single parameter (or metadata member) named *dummy*. It is an integer value stored in the class member *fDummy*. We can initialize the value from a config file defining the following RML section.

\code
<section myMetadataSection name="dummyMeta" >

    <parameter name="dummy" value="7" />

</section>
\endcode

Then we can test the new metadata class inside *restRoot* environment. Assuming that the section is defined inside a file named *test.rml*, the following code should intialize the class using the RML info.

\code

~ restRoot

// We give the filename, and the name of the RML section we want to use
[0] myMetadata *mD = new myMetadata( "test.rml", "dummyMeta" );

// This will print on screen the result of executing TRestMetadata::PrintMetadata method.
+++++++++++++++++++++++++++++++++++++++++++++
myMetadata content
+++++++++++++++++++++++++++++++++++++++++++++
 Dummy number : 7
 --------------------------------------------
root [1] .q

\endcode

### Using the class *mySignalProcess*

The class *mySignalProcess* can be connected to the processing queue inside the TRestManager processes definition as any other REST process. If you have a signal processing chain coming for example from rawdata you might connect it inside the REST manager definition, as follows.

\code

<section TRestManager

...

    <addProcess type="TRestCoBoAsAdToSignalProcess" name="virtualDAQ" value="ON" >
    <addProcess type="TRestSignalZeroSuppresionProcess" name="zS" value="ON" >

    // The only condition is that the output of the previous process is a TRestSignalEvent
    <addProcess type="mySignalProcess" name="dummySignal" value="ON" >

    <addProcess type="TRestSignalToHitsProcess" name="sgnlToHit" value="ON" >

...

</section>


...

\endcode

The new *mySignalProcess* will need a specific RML section definition inside the *processes.rml* file defined in TRestManager. Our new process does nothing but transfer the input signal event into the output signal event. An output message will be displayed on screen only if the verbose level of the process is set to *debug*.

\code 
<section mySignalProcess name="dummySignal" verboseLevel="debug" >

    <parameter name="aDummyParameter" value="13" />

</section>
\endcode

\warning If we define "silent" as verbose level our process should output nothing on the screen.

## Creating a new project library from scratch

You can use the example directory as a template for your new library. The *CMakeLists.txt* has been designed to list all the classes found under *src/* and *inc/* directories. Therefore, if you add a new header and source files of a class inside those directories, the compilation process will consider them to build the new library.

To start from scratch you could just migrate the basic parts of the example directory into your new project directory.

\code

// We create a new library project directory
~ mkdir myRESTproject

// We copy the cmake directory into the project directory
~ cp -r example/cmake myRESTproject

// We copy the CMakeLists.txt file into the project directory
~ cp example/CMakeLists.txt myRESTproject

// We create the src/, inc/, and build directories
~ cd myRESTproject
~ mkdir src
~ mkdir inc
~ mkdir build

\endcode

One last step is required to define the name of the new library. Just open the *CMakeLists.txt* file for editing, and replace the following line inside,

\code 
set( MY_LIBRARY "MyRESTLibrary" )
\endcode

by the name of your library containning REST, or Rest, inside its name. For example,

\code 
set( MY_LIBRARY "MyProjectRESTLibrary" )
\endcode

\warning It is important to change the name of the library inside CMakeLists.txt to avoid conflicts. As it was already mentioned, the name of the library should contain *REST* or *Rest* inside its name so that *restManager* and *restRoot* load the library on the REST environment.

Now you could add new classes and build a new library following the same steps as for the generation of *libMyRESTLibrary*.

## Details on the implamentation of a REST class

This section describes the details you must be aware when implementing a new class in REST, as the necessary pure virtual methods that need to be implemented inside the class code.

### Details on the implementation of a new metadata class

This section relates to the implementation of metadata methods that *must* be implemented in a class deriving from a TRestMetadata class. This applies to a pure TRestMetada specific metadata class, as *myMetadata* class defined before, 

### Details on the implementation of a new process class


