## Using REST

REST in all provides two main executables, several ROOT scripts, several alias calling the scripts, plus a shell
script containing REST system infomation. 

![alt](Image/executables.png)

`restManager` is the main program of REST. It runs with the first argument specifying the rml config file or ROOT
script name. And with the following arguments giving some parameters. In rml config mode, the usage is like:

`restManager --c CONFIG_FILE [--i INPUT_FILE] [--o OUTPUT_FILE] [--j THREADS] [--e EVENTS_TO_PROCESS] [--v VERBOSELEVEL]`.

Here we must give the rml config file to the program. Other arguments with squared brackets are optional. If given,
they will overwrite the corresponding parameters in rml config file. "restManager" calls TRestManager object to
parse rml config file and handle the objects and tasks defined in the xml section. 

In scripts executing mode, the usage is: `restManager TASK_NAME ARG1 ARG2 ARG3`. User just needs to specify
the script name and REST will automatically find and run it from the "macro" directory in installation path. 
Shell alias are set together with those ROOT scripts. For example, the command `restXXX` is actually alias
of the command "restManager XXX". The usage is like:

`restViewEvents abc.root TRestRawSignalEvent` or `restManager ViewEvents abc.root TRestRawSignalEvent`

The other executable is `restRoot`. It is identically ROOT with additional REST libraries and macros loaded.
The loaded library makes it able to operate REST objects or data trees saved in TFile. The macros are ones we
mentioned above. So we can directly run the methods in those pre-defined ROOT scripts. For example, calling 
`TASK_NAME(ARG1,ARG2,ARG3)`  
in restRoot prompt will be equal to calling: 
`restManager TASK_NAME ARG1 ARG2 ARG3`  
in the shell.

Note that REST output file can only be opened by "restRoot". Trees and metadata objects can be reterived
successfully. TBrowser also works for REST objects.

Finally we have a shell script `rest-config`. It provides some basic infomation of REST, including
installation date/directories, branch, commit id, compilation flags, etc.

### Running with a ROOT script

Pre-defined ROOT scripts are also called REST macros. They can be found in ./macros directory. In a ROOT 
script we define a C++ function. Then this function can be used directly at restRoot prompt and bash prompt.

When using `restRoot`, the program will load all the ROOT scripts and user can get access to all the defined C++ 
functions in prompt. When using `restManager`, REST will try to instantiate a TRestTask class to wrap the 
target macro file. It forms a C++ command and invokes it by calling gInterpreter->ProcessLine().
Arguments of this C++ command are constructed according to the input arguments. 

Our task running strategy makes different callings resulting the same, as they both referrs to a same ROOT 
script. It also saves a lot of time for developer adapting these different usage.

TRestTask provides basic functionalities of macro file wrapping, helping message output and rml parsing.
If that's not enough, we can define TRestTask-inherted class ourselves inside the scripts, and override
the helping or rml parsing methods in it. This will be talked later.

For example, we can write a ROOT script named "REST_ViewEvents.hh" in the directory ./macros. Inside the script
we define a C++ function "REST_Viewer_GenericEvents()" and a class "REST_ViewEvents". When we use the line 
`<addTask type="ViewEvents" filename="abc.root" value="ON"/>` in an rml file, TRestManager will try to
instantiate a TRestTask-inherted class with class name "REST_ViewEvents". The class will set its 
datamember "filename" to the value "abc.root", and then do the task of showing an event. It calls 
the defined function "REST_Viewer_GenericEvents()" in file, giving the argument of the input file name.
This is equivelent to the call `REST_Viewer_GenericEvents("abc.root")` inside restRoot prompt, and the call
`restManager ViewEvents abc.root` at bash.

### Rml introduction

Though encoded in standard xml format, all REST config files use .rml extension to identify themselves. rml file
makes a clear hierarchy of C++ objects used during analysis. This kind of configuration file not only tells
the newcomer the job it is carrying, but also gives him a good view of the entire REST framework.

Firstly, the user needs to study a little xml stuff. We start with a template rml file.

`<?xml version="1.0" encoding="UTF-8" standalone="no" ?>`  
`<A_ROOT_SECTION>`  
&emsp;`<ClassName name="userGivenName" title="User given title" >`  
&emsp;&emsp;`<parameter name="parName" value="parVal" />` 
&emsp;&emsp;`<SomeCommand field1="value1" field2="value2"/>`  
&emsp;&emsp;`<ContainedClassName field1="value1" field2="value2" ... >`  
&emsp;&emsp;&emsp;`<SomeCommand field1="value1" field2="value2" /> `   
&emsp;&emsp;&emsp;`comments or words`  
&emsp;&emsp;`</ContainedClassName>`  
&emsp;`</ClassName>`  
&emsp;`<AnotherClass ...>`  
&emsp;&emsp;`...`  
&emsp;`</AnotherClass>`  
&emsp;`<SomeCommand field1="value1" field2="value2"/>`  
`</A_ROOT_SECTION>`

The first line is universal, telling the text viewer this file is xml encoded. Then here comes a **section**.
An xml section is a sealed text structure starting with `<decalre` and ending with `</decalre>` or `/>`. 
Sections in an xml file usually have multiple nesting relationship. Here as the indentation suggests, we have a root
section decalred "A_ROOT_SECTION". It has three child sections "ClassName", "AnotherClass", "SomeCommand".
In tinyxml xml sections are also called **element**, and the declaration are also called "value". In old 
version xml sections are also called **KeyDefinition** or **KeyStructure**.

Xml sections can also have some **field values** (also called xml **attributes**), like in the template 
`field1="value1"` or `name="userGivenName"`. In REST these attributes can also be written in a child section
declared with **parameter**. In the template if we add an attribute `parName="parVal"` in the thrid line, 
this is equivalent to the forth line: `<parameter name="parName" value="parVal" />`.

The symbol `<`, `>`, `&`, `"` shall appear in the main text of xml encoded file. Use escape string 
`&lt;`, `&gt;`, `&amp;`, `&quot;` respectively.

In REST, the section decalration `include`, `for`, `variable` and `myParameter` and the symbols `${` `$ENV{` and `}`
are reserved for the software. They will be preprocessed by the software before the method "InitFromConfigFile()"
in matadata classes.

#### variable and myParameter

The xml sections "variable" and "myParameter" are for the keyword replacement. They are defined with a line
like: `<variable name="PITCH" value="3" overwrite="false" />`. xml sections with same or lower hierarchy than
this definition section will have its attributes replaced. If this definition section is in the "globals"
section, then all xml sections in the file can see it.

To replace "variables", we must mark the corresponding keyword with `${}`, while to replace "myParameter" we
need not. For example, we add a line after the previous "variable" definition: 
`<myParameter name="pitch" value="${PITCH}" />`.
This marks out the keyword "PITCH" and it will be replaced by the word "3". Now we defined a "myParameter" with 
name "pitch" and value "3". Then we add another line: 
`<addPixel id="0" origin="(pitch,pitch/4+pitch)" size="(20,20)" rotation="45" />`.
In this line all the apperrance of "pitch" will be replced by the word "3". The expression will be executed.

REST works together with system environmental variable. By switching true or false for the "overwrite" attribute,
a variable definition will use the text defined vale or the system environmental variable. By marking the keyword
with `$ENV{}` REST will search for system environmental variable to replace it. 

#### include definition

It is possible to link to other rml files in any section. The included file must also be xml encoded.
REST will open the file and searches for the section with the same declaration(or type attribute) and name 
attribute as the current section. If found, the external section will be expanded into the current section.
Variable in that file will be imported together.

There are two ways to make a include definition. One can either specify the external file in the element 
attribute:  
`<addProcess type="TRestRawSignalAnalysisProcess" name="sAna" value="ON" file="processes.rml"/>`  
or in the element's child:  
`<addProcess type="TRestRawSignalAnalysisProcess" name="sAna" value="ON">`  
`&emsp;<include file = "processes.rml" />`  
`</addProcess>`  

These two include definitions will order REST to find a section in the file process.rml declared as 
"addProcess" or "TRestRawSignalAnalysisProcess" and named with "sAna". The section can both be a root element
or a child element of root element(cannot be grand-child element). If found, REST will expand its attributes
and child elements to the element "addProcess".

#### for loop expansion

The definition of FOR loop is implemented in RML in order to allow extense
definitions, where many elements may need to be added to an existing array in
our metadata structure. The use of FOR loops allows to introduce more
versatil and extense definitions. Its implementation was fundamentally triggered 
by its use in the construction of complex, multi-channel generic readouts by
TRestReadout.

The for loop definition is as follows, where *pitch* and *nChannels* are previously 
defined myParameters, and *nCh* and *nPix* are the *for* loop iteration variables.

`<for variable = "nCh" from = "0" to = "nChannels-2" step = "1" >`  
&emsp;`<readoutChannel id = "${nCh}" >`  
&emsp;&emsp;`<for variable = "nPix" from = "0" to = "nChannels-1" step = "1" >`  
&emsp;&emsp;&emsp;`<addPixel id = "${nPix}" origin = "((1+${nCh})*pitch,pitch/4+${nPix}*pitch)" size = "(pixelSize,pixelSize)" rotation = "45" />`  
&emsp;&emsp;`</for>`  
&emsp;&emsp;`<addPixel id = "nChannels" origin = "(${nCh}*pitch,pitch/4+(nChannels-1)*pitch+pitch/2)" size = "(pitch+pitch/2,pitch/2)" rotation = "0" />`  
&emsp;`</readoutChannel>`  
`</for>`  

REST will recongize the fields "variable", "from", "to", "step" in the header of the for loop definition. 
The variable "nCh", definded at the header of the for loop definition, will be updated in each loop and be used
to replace values of the loop content. During the loop, REST will add the new content element at the 
front of the for loop element(add a new sibling). After the loop, REST will delete the for loop element, leaving
purely the loop content.

#### an example

We used generateReadoutFile.rml in the ./example directory to generate a readout file. We now open it and see what
it did.

`...`
`<TRestManager ...>`  
&emsp;`<globals>...</globals>`  
&emsp;`<TRestRun>...</TRestRun>`  
&emsp;`<addTask .../>`  
&emsp;`<addTask .../>`  
`</TRestManager>` 

The root section is declared "TRestManager". It contains a scetion declared "globals", a scetion declared 
"TRestRun" and two sections declared "addTask". This section has a corresponding class in REST with same class name. 

In the "TRestManager" section, obviously, the "globals" section are providing some global setting for 
others. The "TRestRun" section has also a corresponding class in REST. This class mainly deals with file IO
and data transmission. So we are adding TRestRun class inside TRestManager class. Lets see what's in it.

`<TRestRun ...>`  
&emsp;`<TRestReadout ...>`  
&emsp;&emsp;`<readoutModule .../>`  
&emsp;&emsp;`<readoutPlane ...>`  
&emsp;&emsp;&emsp;`<addReadoutModule .../>`  
&emsp;&emsp;&emsp;`<addReadoutModule .../>`  
&emsp;&emsp;&emsp;`<addReadoutModule .../>`  
&emsp;&emsp;&emsp;`...`  
&emsp;&emsp;`</readoutPlane ...>`  
&emsp;`</TRestReadout>`  
`</TRestRun>`  

Its easy guess that we are adding a TRestReadout class inside TRestRun class. This class is what contains
real readout definition. Inside its section, there are several operations. First we define a readout module,
which in our experiment is the MicroMegas. And then we define a readoutplane, adding several of this kind of
readout module in it, giving their physical position and some other infomation. All together they form 
an one-plane readout system. This TRestReadout class is now saved inside TRestRun class.

Previous work is done within the initialization of these classes. Now these classes are ready and we need 
to tell REST what to do. So finally, in "addTask" section, we give the command readoutrun->FormOutputFile().
and readoutrun->CloseFile(). Here "readoutrun" is the name(not class name) of the previously defind TRestRun 
class. Obviously here we are telling REST to "save file and close". The two lines of command are actually
a method in the class TRestRun which TRestManager will invoke.


### Writing an rml

Here we will talk about the detailed options of rml config file when using REST. We usually have a 
"TRestRun" section, a "TRestProcessRunner" section, a "globals" section and some "addTask" section under 
the root section "TRestManager" in an rml file to be loaded by `restManager`. "TRestRun" section and 
"TRestProcessRunner" section corresponds to the two REST application objects managed by TRestManager.
They cooperate with each other.

#### name, title and verbose level

TNamed class introduces name and title as datamembers for the objects of its inherted class. In addition,
TRestMetadata introduces verbose level option. We need to set all three of them as basic information for
any TRestMetadata inherted class objects. Use xml attributes or child sections "parameter" to set them. 
For example:

`<TRestProcessRunner name="TemplateEventProcess" verboseLevel="info">`
&emsp;`<parameter name="title" value="A Template of REST Analysis" />`  
&emsp;`...`
`</TRestProcessRunner>`

There are five verbose levels one can choose, and the parameter "verboseLevel" can either be a number value 
or a string value.

level | number | string | Description
-------------|------------|-----------------|------------
REST_Silent       | 0      | silent           | show minimized information of the software, as well as error messages
REST_Essential    | 1      | essential        | +show some essential information, as well as warnings
REST_Info         | 2      | info             | +show most of the infomation
REST_Debug        | 3      | debug            | +show the debug messages, pause at each processes to show the details
REST_Extreme      | 4      | extreme          | show everything

The default verbose level for a TRestMetadata inherted class is silent/info before/after
loading the rml file(calling the method "LoadConfigFromFile()").

#### setting run information

Physically, a "run" is a continuous data taking during which all the detector's configuration maintains
a constant. We can set run information in "TRestRun" section and they can be saved in output file.

Below is a list of run info datamember in the class TRestRun. They can be set with a line like:
`<parameter name="experiment" value="PandaX-III"/>` under the section "TRestRun".


item | type | rml parameter name | Description
-------------|------------|-----------------|------------
fRunNumber       | int      | runNumber           | first identificative number
fParentRunNumber | int      | ~~~~                | ~~~~
fRunClassName    | TString  | ~~~~                | ~~~~
fRunType         | TString  | runType             | Stores bit by bit the type of run. e.g. calibration, background, pedestal, simulation, datataking 
fRunUser         | TString  | user                | To identify the author it has created the run. It might be also a word describing the origin of the run (I.e. REST_Prototype, T-REX, etc)
fRunTag          | TString  | runTag              | A tag to be written to the output file
fRunDescription  | TString  | runDescription      | A word or sentence describing the run (I.e. Fe55 calibration, cosmics, etc)
fExperimentName  | TString  | experiment          | The experiment name


#### adding metadata

REST metadata objects is managed by TRestRun. They provides, for example, gas definition or readout definition
to the processes to use. So inside the "TRestRun" section we need to add metadata. It is possible to either 
import them from a ROOT file or to generate new. 

To import them from file we need to use a line like:  
`<TRestRun ...>`  
&emsp;`<addMetadata name="PandaReadout_MxM" file="readouts.root"/>`,  
`</TRestRun ...>`  
where we input the name of the matadata object and the name of the file. This is a recommended way to add metadata
as it is faster. One can spend some time generate definition files for the detector, then other users need not to do 
it again.

To generate a new instance, we need to add a full definition of the class. The section declaration must 
be the metadata class name. The content of the section should follow the rule of the class. For example:

`<TRestRun ...>`  
&emsp;`<TRestReadout ...>`  
&emsp;&emsp;`<readoutModule .../>`  
&emsp;&emsp;`<readoutPlane ...>`  
&emsp;&emsp;&emsp;`<addReadoutModule .../>`  
&emsp;&emsp;&emsp;`<addReadoutModule .../>`  
&emsp;&emsp;&emsp;`<addReadoutModule .../>`  
&emsp;&emsp;&emsp;`...`  
&emsp;&emsp;`</readoutPlane ...>`  
&emsp;`</TRestReadout>`  
`</TRestRun>` 

#### adding process and its observables

Now we are going to add processes and define the needed output observables. In the section "TRestProcessRunner"
we add sections like `<addProcess type="TRestRawSignalAnalysisProcess" name="sAna" value="ON" file="processes.rml"/>`.
We have to specify the type and name. In addition, we can use the option "value" to switch on/off the process 
in the analysis chain. 

To add observables or to set parameters, we need to write lines like: `<observable name="FirstX" value="ON" />` 
or `<parameter name="resolutionReference" value="1.0" />` inside the "addProcess" section (as a child section
of it). In case there are too many observables and parameter to define, which may cause a mess in the rml file,
we can use an include definition here. REST already defines some useful observable/parameter sets for its process.
For example for TRestSmearingProcess we have two parameter sets defined in the file "processes.rml", their names
are "smear_1FWHM" and "smear_3FWHM" respectively. Use an attribute like: `file="processes.rml` to include and 
expand this rml file. To change the parameter set just change the "name" attribute.

#### input file and external process

The input file must be given to TRestRun to run an analysis. If the input file has .root extension, then 
it will be regarded as REST data file and will be directly opened. If not, we must have an external process
in TRestRun to extract events from it. The "addProcess" section for external file processes can both be in 
section "TRestRun" and section "TRestProcessRunner". There can only be one external process added, and it
is running under single threaded mode.

#### changing saved branches

Some parameters in section "TRestProcessRunner" changes the branches to save in the two output trees. They are:  
`<parameter name="inputAnalysis" value="on"/>`  
`<parameter name="inputEvent" value="on"/>`  
`<parameter name="outputEvent" value="on"/>`  

Output analysis is saved in whatever settings. If the user turns on "outputEvent", then the output event 
will be saved. It is by default on. If the user turns on "inputEvent", then "outputEvent" will be automatically 
turned on. All the events with different types will be saved. If the user turns on "inputAnalysis", then when 
the input file is a root file, tree observables in it will be copied to the output file. 

Events may have overlaps. For example, the output events of two processes are of the same type when a process is a 
pure anslysis process. In this situation, only the later one will be saved.

By default all these three settings are on in REST. If the user wants to save some disk space, he can choose to
save analysis items only(turn off events). Or if he only wants a view of the last processed event, he can choose 
to save output event only.

#### changing event region

Three parameters in section "TRestProcessRunner" changes the region of events to process. They are:
"firstEntry", "lastEntry" and "eventsToProcess". If they are all zero, then the input file will have its all
events processed. If the parameter "eventsToProcess" is non-zero, then the process will be stopped after it 
reaches the number. The parameter "firstEntry" and "lastEntry" is only effective when the input file is a
REST data file. They determines the entry region in the tree to extract events to process. "lastEntry" will
be overwritten by a non-zero "eventsToProcess".

REST counts processed events number by the number of saved events. If there is a cut in some process and the
event is not saved, we need to read more events than the given number. If there is many threads working together,
the actual saved event number will be silghtly bigger than required.

#### search path for definition files

Usually we put include rml files and pre-defined root files in a same directory as the main rml file. This
makes it simple to specify the file name. On the other hand, when the target file is in a remote directory 
and we don't want to input a long absolute path in rml, we can add additional paths for REST to find files. 
The following is an example:  
`<globals>`  
&emsp;`<searchPath value="$ENV{REST_PATH}/inputData/definitions/:$ENV{REST_PATH}/inputData/gasFiles/"/>`  
`</globals>`  

Putting the parameter in "globals" section will make it visible for all the sections in rml. The value is in
linux env style with ":" separating multiple paths. Alternativally on can define multiple "searchPath sections,
this is also vaild.

The directry "inputData" is where REST saves its universal reference files. For example here in the directoty 
"definitions" REST saves many pre-defined rml files for readout, processes and gases. The detailed organization 
of the directory "inputdata" can be found in the appendix. [REST pre-definition data](#rest-pre-definition-data)  

#### output file: naming and saving

REST also enables auto-naming of the output file. By using square brackets in the "outputFile" parameter in TRestRun
section, the user can have REST trying to replace the strings. For example, we can have a the parameter written like:

`<parameter name="outputFile" value="RUN[RunNumber]_[Time]_[LastProcess].root" />`  

The replace work is done as follows:  
1. replace with system environmental variable(done in rml parsing step)
2. replace with the file info  
3. replace with the process info  
4. replace with the run info  

There are some public keywords for replacement in REST:

source | item | Description
-------------|------------|------------
file info       | Time                 | The last write time of the input file
file info       | Date                 | The last write date of the input file 
file info       | Size                 | The size of input file  
file info       | Entries              | The total entries of input file (if is REST data file, otherwise = 2e9)        
process info    | FirstProcess         | The name of first event process         
process info    | LastProcess          | The name of last event process
process info    | ProcNumber           | The number of processes
run info        | [see the table above ](#setting-run-information)  | ~~~~

In future we may add more keywords. The user can also add his own file info keywords from the input file name.
What he needs is to add another parameter line like:  
`<parameter name="inputFormat" value="run[RunNumber]_cobo[CoBoId]_[Fragment].graw"/>`  
Therefore REST will match the input file name with the given format. For example, when file name is:
"run00042_cobo1_0000.graw", then item "RunNumber" with value "00042", item "CoBoId" with value "1", and item
"Fragment" with value "0000" will be added into file info. 

Let's assume that the graw file is created in 2018-01-30 16:30, and the last event process is a 
TRestRawSignalAnalysisProcess with a name "sAna", then the output file name will be:
"RUN00042_16:42_sAna.root".

Another parameter "mainDataPath" defines the path of saving output file. By default REST saves the output file 
at current directory. When "mainDataPath" is specified, the output file is saved in this directory. We can set 
it in "globals" section. For example:

`<globals>`  
&emsp;`<parameter name="mainDataPath" value="../" />`  
`</globals>`  

then all the output file will be saved in the parent directory.

#### the "addTask" command

Don't forget to add an "addTask" section after all the sections are completed. We invoke the method RunProcess()
in TRestProcessRunner to run the analysis. Here we need a line in "TRestManager" section like:  
`<addTask command="TemplateEventProcess->RunProcess()" value="ON"/>`.  
Here "TemplateEventProcess" is the name of the TRestProcessRunner object defined previously.

It is also possible to directly use a line:  
`<addTask type="processEvents" value="ON" />`.  
This asks TRestManager to directly call start of TRestProcessRunner object.

### During processing

When we have prepared the rml file, we can start the process! The command is like following:

`restManager --c multiCoboAnalysis.rml --i /data2/7MM/graw/CoBo_AsAd0_2017-12-23T17\:24\:04.657_0000.graw --o abc.root`

REST provides a progress bar and a pause menu during the process. Pressing "p" and then "enter" will pause the process and
a pause menu will be shown. Functionalities like changing verbose level, printing current event or exiting with saving,
etc. is provided in the menu. As shown in the following figures.

![alt](Image/progressbar.png)


![alt](Image/pausemenu.png)

### REST data format

REST saves an event tree, an analysis tree, some metadata or application objects, and some ROOT analysis 
objects in a same output file of the analysis run, as shown in the figure.

![alt](Image/datafile.png)

The trees are of the type TRestAnalysisTree, which is typically a ROOT tree plus a list of observables. Inside 
these trees there are six branches saving some 

The event tree is purely TTree. The event branches inside it are C++ object branches with a same structure 
as the class definition. Here in the figure we can see three event branches are saved: 
TRestRawSignalEventBranch, TRestSignalEventBranch, and TRestHitsEventBranch. In TBrowser we can open them
and draw their data members by double clicking on it.

For the analysis tree, we first save six event branches(in the first red circle). The event branches contains
basic information of one event, they are:

* run ID
* sub run ID
* event ID
* sub event ID
* time stamp
* event tag

Then we save process branches(in the second red circle). They are the snapshot of data member of each 
processes. In the figure there are four process branches called "sAna", "zS", "signalToHits" and "hitsAna".
If we open them, we can see some leaves corresponding to the processes' data member. 

Finally there is observable branches(in the thrid red circle). Observable is a concept in REST which means
"direct analysis result". They are of double type and put directly in the analysis tree. Each process
can yield observable. They are named after the process's name.

AnalysisTree is separated from the EventTree for the convenience of drawing. Usually event tree 
contains large amount of data and we needs to go through all the entries when drawing some thing.
As a result, drawing objects in AnalysisTree can be much faster than drawing in EventTree.

Several metadata and application objects are also saved in file. They are used for recovering the setup
of that analysis run. For example, REST can read the saved TRestRawSignalAnalysisProcess object (here named 
"sAna") and get the parameters used at that time. For metadata objects, REST can directly use them again in 
the next run.

In addition, REST allows processes to save some ROOT analysis objects in the file. Here the TH1D
"readoutChannelActivity" is saved by the process "sAna" (of type TRestRawSignalAnalysisProcess). We can 
directly draw it.

### Browsing and viewing events

Events in REST data files are managed by TRestRun, whose graphical interface, in turn, is shown by
by TRestBrowser. This class shows a TBrowser window during initialization. In the window there is a 
canvas showing the current event, and a control panel to switch between events. 

In restRoot prompt, by using TRestBrowser, one can easily get accsess to the file's events, and don't
need to manually instantiate a TRestEvent object and set the tree's branch address. He just needs to type:  
`restRoot abc.root`  
`TRestBrowser a`  
`TRestxxxEvent*eve=(TRestxxxEvent*)a.GetInputEvent()`,  
and will be free to operate this event.

By default TRestBrowser extracts the last event in file, and draws it in the canvas by using the viewer
class TRestGenericEventViewer. This viewer just calls the default method TRestEvent::Draw(). Other viewers
like TRestHitsEventViewer or TRestG4EventViewer are also available. Some pre-defined ROOT scripts can be 
used to draw these events in differently. The commands like: "restViewEvents abc.root", 
"restManager ViewHitsEvents hits.root", "REST_Viewer_LinearTrackEvent("track.root")" and 
"restManager --c Viewabc.rml" shall all work.

Here for example, we use the generated file in [example](process-a-raw-data-file), and call the command 
`restViewEvents abc.root`. The last event is TRestRawSignalEvent type in this file, and a TRestBrowser
window will show up with some observable values on prompt.

![alt](Image/restViewEvents.png)

In the right side it shows a combined plot of the event, which consists from many individual signals. 
In the left side we have a control panel which helps to switch next/previous/specific event/signal 
and open a new file. Different event viewers will define different interfaces of the control panel 
and the plot window.

Some viewer processes are also available in REST. The user can have a view of the events during 
the process. All the viewer processes are single thread only, and TRestProcessRunner will automatically
roll back to single thread mode with a viewer process in process chain. 

### Plot the analysis result

It is also allowed to plot histograms for observables in output file. REST has an application class 
called TRestAnalysisPlot. It generates plot string according to an rml config file and calls the 
TTree::Draw() method to draw the histogram. It can also save the plots to a pdf file or ROOT file afterwards.

To use it, a "TRestAnalysisPlot" section is needed in "TRestManager" section. The template of rml config file 
for TRestAnalysisPlot can also be found in ./examples. It shall follow the rules below. The command calling it
is like:  
`restManager --c plots.rml --i abc.root --p ouput.pdf`  

#### add input file and set plot mode

To add input files just use a section like: `<addFile name="filename.root" />` in the "TRestAnalysisPlot"
section. Multiple input file is allowed. If the "addFile" section does not exist, TRestAnalysisPlot will
ask the sibling TRestRun object for its output or input file as the input file. 

In most cases REST saves a single output file in an analysis run. So these multiple files are from different 
runs, or analysis with different configurations. Usually we are interested in the difference of one observable 
between different runs. In this case we just set the plot mode to "compare" with section:
`<parameter name="plotMode" value="compare" />`. Then REST will plot these same-observable-from-different-file
in a same figure with different color. The "compare" plot mode is also the default plot mode.

In rare cases the multiple files are from a same run with same analysis configuration, then we need to set 
plot mode to "add". This will make REST plot the observables into a single histogram.

#### define a canvas

It is needed to define a canvas for TRestAnalysisPlot. Use a section like:  
`<canvas size="(1000,800)" divide="(2,2)" save="plot.pdf" />`  
to define it. The canvas can be devided into several sub-canvas and each of them will contain a plot figure.
It can also be saved into a file. Most of the common figure formats are supported, as REST calls 
TCanvas::Print() method to make the save. The list of supported file formats shall be found in ROOT website. 

#### add a plot with cut

Now we define and add a plot in "TRestAnalysis" plot section.

`<plot name="Baseline" title="Baseline average" value="ON" >`  
&emsp;`<parameter name="xlabel" value="BaselineRms [ADC units]" />`  
&emsp;`<parameter name="ylabel" value="Counts" />`  
&emsp;`<parameter name="logscale" value="false" />`  
&emsp;`<parameter name="option" value="" />`  
&emsp;`<source name="sAna_BaseLineSigmaMean" range="(0,1000)" nbins="100" />`  
&emsp;`<cut source="sAna_NumberOfGoodSignals" condition="&gt;1" value="ON" />`
`</plot>`

Here we can set xlabel, ylabel and logscale with corresponding parameters. We can also add additional
options with parameter "option". 

Then we add the source of the plot. Its name is "sAna_BaseLineSigmaMean", which is an observable in the analysis
tree. Here we got a TH1 histogram of this observable. We can also define the region of this histogram, just
as the template shows. TH2 and TH3 are also supported, by adding more lines of this kind of "source" section.

We also add cut for the plot. Here in the "plot" section we add a cut that the observable
"sAna_NumberOfGoodSignals" should be greater than 1. Note that standard xml needs escape string to express
the symbol `>`. Though this symbol still works out of some reason, we suggest using `&gt;` instead of `>` 
for a good habit.

Finally with an "addTask" section in "TRestManager" section, we can make a plot like below

![alt](Image/plot.png)


[**prev**](4-the-rest-framework.md)
[**contents**](0-contents.md)
[**next**](6-some-metadata-classes.md)