## Getting Started

The main executable of REST is restManager and restRoot. By typing directly `restManager` it will show 
its usage. By typing restRoot the user can access to REST libraries and macros inside ROOT prompt. We 
have some example files for restManager in the directory `$REST_PATH/example`. We first switch to that 
directory.

### Save a metadata file

We generate here a matadata file saving several definitions of the detector's readout and gas metadata. 
The readout metadata includes readout plane geometry, daq channel mapping, strip gain, etc. The gas 
metadata includes infomation of gas component, pressure, diffusion and drift speed. This file will be 
very important for running processes later on. Generate it by typing :

`restManager --c saveMetadata.rml --o meta.root`

This command will make REST to save two TRestGas classes and one TRestReadout class to the file 
"meta.root", which will be used in the later data analysis work. The content of gas and readout 
definition can be observed the rml file. Later we will explain what it did. 

### Process a raw data file

If you are in pandax-iii daq server, you can try to process some raw data file with the readout file we just 
generated. For example, to process a 7MM run data file, we can type the command:

`restManager --c multiCoboAnalysis.rml --i /data2/7MM/graw/CoBo_AsAd0_2017-12-23T17\:24\:04.657_0000.graw --o abc.root`

This gives REST names of the config file, input file and output file. If works smoothly, you can see numbers of
processed events increasing.

When it is done, use command: `restRoot abc.root` to see the content of the generated ROOT file. The file should 
contain two trees and some saved classes. We will talk it later.


### The concept of rml

rml file is encoded in standerd xml(Extensible Markup Language) but adds several additional features 
or key words for it.

#### a little xml

We start with a template xml file.

`<?xml version="1.0" encoding="UTF-8" standalone="no" ?>`  
`<A_ROOT_SECTION>`   
&emsp;`Hello World!`  
&emsp;`<!--This is a comment-->`  
&emsp;`<SectionName name="abc" title="a section title" >`  
&emsp;&emsp;`<parameter name="parName" value="parVal" />`  
&emsp;&emsp;`<variable field1="value1" field2="value2"/>`  
&emsp;&emsp;`<ChildSection field1="value1" field2="value2" ... >`  
&emsp;&emsp;&emsp;`...`   
&emsp;&emsp;`</ChildSection>`  
&emsp;`</SectionName>`  
&emsp;`<AnotherSection ...>`  
&emsp;&emsp;`...`  
&emsp;`</AnotherSection>`  
`</A_ROOT_SECTION>`

The first line is universal, telling the text viewer this file is xml encoded. Then here comes an **element**.
An xml element is a sealed text structure starting with `<value` and ending with `</value>` or `/>`. 
Elements in an xml file usually have multiple nesting relationship. Here as the indentation suggests, we have
a root element with **value** "A_ROOT_SECTION". It has two child elements "SectionName", "AnotherSection".
Elements have **attribute**, namely the key-value pairs in its definition line. Here in the example the 
element "SectionName" has two attributes: "name"-"abc" and "title"-"a section title". Only one root element
is allowed in standard xml format. Attributes cannot be repeated.

Besides the elements, xml file can also contain **character** and **comment**. Comment is the text starting 
with `<!--` and ending with `-->`. Character is the naked text. Here in the example, "Hello World!" is 
character, and "This is a comment" is comment. We don't use those two things in rml.

The symbol `<`, `>`, `&`, `"`, `'` may cause ambiguity in the main text of xml encoded file. Its better to
use escape string `&lt;`, `&gt;`, `&amp;`, `&quot;`, `&apos;` respectively.

For some historic reason, in REST, and in the following document, the xml element is also called **section** 
or **key structure**, the element value is also called **declare**, the element attribute is also called 
**field value**, the first line of xml element is also called **key definition**.


#### sections with classes

The root section in an rml file should be `<TRestManager`. Inside it we define several `<TRestXXX` sections
and `<addTask` setions. For each `<TRestXXX` section we instantiate the corresponding REST class and 
give the content of this section for its setup. For example in plotAnaSpectrum.rml we first define a
TRestRun section for file input/output. Then we add a TRestAnalysisPlot section and define several 
components to be plotted. Behind it we have a `<addTask` section. This tells REST what to do after
initialization of the previous classes, i.e. do the plot.

#### definition of parameter

Parameter is the most frequently used configuration. They are usually written as an xml section 
containing name and value attribute. Then these parameter sections will setup their parent sections.
For example:  
`<TRestRun name="Test_Run">`  
&emsp;`<parameter name="runNumber" value="-1"/>`  
`</TRestRun>`  
This sets parameter "runNumber" to "-1" for TRestRun target. Note that the previously defined 
parameter will overwrite the later one with same name.

Alternativally, the xml attributes at same section level can also be the parameter. In the previous 
example, if we write:  
`<TRestRun name="Test_Run" runNumber="-1">`  
`</TRestRun>`  
This is equivalent to the former definition.

#### variable and myParameter

The xml sections valued **variable** and **myParameter** are for the keyword replacement. They are defined 
with a line like: 

`<variable name="PITCH" value="3" overwrite="false" />`

xml sections with same or higher hierarchy than this definition section will know this variable. If this 
definition section is in the "globals" section, then all xml sections in the file can see it. To mark 
"variable" for REST to replace, we must use keyword `${}`. For "myParameter" we don't need to add any 
mark. For example, we add a line after the previous "variable" definition: 
`<myParameter name="pitch" value="${PITCH}" />`.
This marks out the keyword "PITCH" and it will be replaced by the word "3". Now we defined a "myParameter" with 
name "pitch" and value "3". Then we add another line: 

`<addPixel id="0" origin="(pitch,pitch/4+pitch)" size="(20,20)" rotation="45" />`

In this line all the apperrance of "pitch" will be replced by the word "3". The expression will be executed.
Finally what the program see will be:

`<addPixel id="0" origin="(3,3.75)" size="(20,20)" rotation="45" />`

REST works together with system environmental variable. The `${}` marked variable reference will also 
be replaced by system env, if the variable definition section is not found. If we add an `overwrite="false"` 
attribute to the variable definition section, then the system env will be used first if exist. By marking 
the variable reference with `$ENV{}`, REST will search only the system env to replace it. 

#### include definition

It is possible to link to other rml files in any section. The included file must also be xml encoded.
REST will open the file and searches for the section with the same declaration(or type attribute) and name 
attribute as the current section. If found, the external section will be expanded into the current section, 
inducing more attributes and parameters.(no overwriten). Variables in that file will be imported together.

There are two ways to make a include definition. The key word is the attribute "**file**". We recommend to 
use "auto insert include", it is like:

`<addProcess type="TRestRawSignalAnalysisProcess" name="sAna" value="ON" file="processes.rml"/>`  

Then REST will automatically find the corresponding section, according to the specified "type" and "name". 
Here "type" can alternatively be the element declaration. At least one of the two definitions should be 
given. In "processes.rml", we both only search in its a root element. If found, REST will insert its child 
sections and attributes into the local xml element.

Another include definition is "raw include". REST will parse all the lines in the file as xml element 
and insert them inside the local section. We use like:

`<addProcess type="TRestRawSignalAnalysisProcess" name="sAna" value="ON">`  
&emsp;`<include file="processes.rml" />`  
`</addProcess>`  

#### for loop expansion

The definition of **for** loop is implemented in RML in order to allow extense
definitions, where many elements may need to be added to an existing array in
our metadata structure. The use of for loops allows to introduce more
versatil and extense definitions. Its implementation was fundamentally triggered 
by its use in the construction of complex, multi-channel generic readouts by
TRestReadout.

The for loop definition is as follows, where *pitch* and *nChannels* are previously 
defined myParameters, and *nCh* and *nPix* are the *for* loop iteration variables.

`<for variable="nCh" from="0" to="nChannels-2" step="1" >`  
&emsp;`<readoutChannel id="${nCh}" >`  
&emsp;&emsp;`<for variable="nPix" from="0" to="nChannels-1" step="1" >`  
&emsp;&emsp;&emsp;`<addPixel id="${nPix}" origin="((1+${nCh})*pitch,pitch/4+${nPix}*pitch)" size="(pixelSize,pixelSize)" rotation="45" />`  
&emsp;&emsp;`</for>`  
&emsp;&emsp;`<addPixel id="nChannels" origin="(${nCh}*pitch,pitch/4+(nChannels-1)*pitch+pitch/2)" size="(pitch+pitch/2,pitch/2)" rotation="0" />`  
&emsp;`</readoutChannel>`  
`</for>`  

REST will recongize the fields "variable", "from", "to", "step" in the header of the for loop definition. 
The variable "nCh", definded at the header of the for loop definition, will be updated in each loop and be used
to replace values of the loop content. During the loop, REST will add the new content element at the 
front of the for loop element(add a new sibling). After the loop, REST will delete the for loop element, leaving
purely the loop content.

#### if expression

We can add `<if` sections to make rml to respond differently under different system condition. A "condition" 
attribute must be added in the if section. REST will parse the condition formula. If matches, the child 
sections inside if section will be seen. For example:  
`<if condition="${HOME}==/home/nkx">`  
&emsp;`<addProcess type="TRestSignalZeroSuppresionProcess" name="zS" value="ON" file="processes.rml"/>`  
`</if>`  
Here ${HOME} is a variable and will be replaced first under variable logic. So REST is just comparing two 
strings, only if the home directly is "/home/nkx", the `<addProcess` section is valid. Sometimes we need 
to use more complex conditions. Then we can add another attribute "evaluate". For example:  
`<if evaluate="date +%Y-%m-%d" condition=">2019-08-21">`  
&emsp;`<addProcess type="TRestSignalToHitsProcess" name="signalToHits" value="ON" file="processes.rml" />`  
`</if>`  
Here REST will call for execution of shell command `date +%Y-%m-%d`, the output of which is used for
comparing.

#### an example

Previously we used saveMetadata.rml in the ./example directory to generate a readout file. We now open it and see what
it did. Its overall structure is like:

`...`
`<TRestManager ...>`  
&emsp;`<globals>...</globals>`  
&emsp;`<TRestRun>...</TRestRun>`  
&emsp;`<addTask .../>`  
`</TRestManager>` 

The root section is declared "TRestManager". It contains a scetion declared "globals", a scetion declared 
"TRestRun" and a section declared "addTask".

The "globals" section provids global setting for others, whose child sections will be expanded to others.
The "TRestRun" section has a corresponding class in REST. This class mainly deals with file IO and data 
transmission. So we are adding TRestRun class inside TRestManager class. Lets see what's in it:

`<TRestRun name="readoutrun">`  
&emsp;`<TRestGas ... file="XXX" />`  
&emsp;`<TRestGas ... file="XXX" />`  
&emsp;`<TRestReadout ... file="XXX" />`  
&emsp;`<TRestReadout ...>`   
&emsp;&emsp;`(some parameters)`  
&emsp;&emsp;`<readoutModule ... file="XXX"/>`  
&emsp;&emsp;`<readoutPlane ...>`  
&emsp;&emsp;&emsp;`<addReadoutModule .../>`  
&emsp;&emsp;&emsp;`<addReadoutModule .../>`  
&emsp;&emsp;&emsp;`<addReadoutModule .../>`  
&emsp;&emsp;&emsp;`...`  
&emsp;&emsp;`</readoutPlane ...>`  
&emsp;`</TRestReadout>`  
`</TRestRun>`  

Its comprehensive that we are adding two TRestGas classes and two TRestReadout classes inside TRestRun class. 
The first three objects are using include definitions since they have a "file" attribute. In the last 
TRestReadout section, we first define a readout module template from external include file. Then we add the
pieces onto a readout plane. All together they form an one-plane readout system. This TRestReadout class is
also saved to TRestRun class.

Previous work is done within the initialization of these classes. Now these classes are ready and we need 
to tell REST what to do. Finally we have: 

`<addTask command="readoutrun->FormOutputFile()" value="ON"/>`  

, where we invoke the method FormOutputFile() from TRestRun object. Here "readoutrun" is the name
(not class name) of the previously defind TRestRun object. With the execution of this actual command, 
REST saves data to the output file.

[**prev**](2-installing-rest.md)
[**contents**](0-contents.md)
[**next**](4-the-rest-framework.md)