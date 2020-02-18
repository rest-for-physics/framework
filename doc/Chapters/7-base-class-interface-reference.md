## Base Class Interface Reference

The users are welcomed to develop new classes based on REST. In this section, we will talk about the 
interface of classes when one is going to write his own derived classes. We will first cover some 
points of inheriting the class TObject, which is the base class of many REST base classes. Then we will
talk about the three main base classes in REST: TRestEvent, TRestMetadata and TRestEventProcess. Both
of them are abstract class.

### TObject

Classes inherited from TObject can be saved in both TFile and TTree. If one wants to save his data
in REST, it is recommended to write a data class inherited from TObject and save the class directly. 
Note that the name of the header file must be the same as the class defined in it. Otherwise CINT and TClass 
won't work for the class. For example, in file TRestHits.h, we write:

`class TRestHits : public TObject{`  
`public:`  
`//some class members`  
`//some methods`  
`}`  

The source code will first get compiled with CINT. This compiler will generate an additional .cxx file 
implementing the Streamer() method. This method, provided by TObject, will help to save the class members.
Both public, protected and private class members can be saved.

Sometimes it is not necessary to save all the class members in a class. Some class members may be too large,
or contains temporary data. In this case we can turn them off by adding some annotations after their 
declaration in the header file. For example:

`class TRestHits : public TObject{`  
`public:`  
`int a; //!`  
`int b;`  
`}`  

Then the class member "a" will not be saved when the class TRestHits gets saved in TFile/TTree. In TTree we
are unable to see the "a" branch. In TFile when we retrieve the class and get the class object, the class
member "a" will be its default value.

If the class member is a pointer, we cannot save it directly. We need to either add "//->" annotation in 
the same line(not tested), or create and link an instance of the pointer, then save the instance. There 
are two ways to do it, as shown in the code below. 

`1)`  
`TH1D* h; //->`  

`2)`  	
`TH1D* h; //!`  
`TH1D _h;`  
`//in some method before saving`  
`_h=*h;`  

If one directly saves the pointer(adding no comments for CINT), he will get a memory leak problem when 
reading the saved file later on. This problem is fatal when the class is in a tree, and he is trying to 
loop all the entries to, e.g. draw something.. 

### TRestEvent

#### class member
This is an abstract class inherited from TObject. We have some additional class members defined in it.

Type | Name | Description
-------------|------------|-------------
Int_t      |   fRunOrigin;	       | Run ID number of the event
Int_t      |   fSubRunOrigin;      | Sub-run ID number of the event
Int_t      |   fEventID;           | Event identificative number. (Default: 0)
Int_t      |   fSubEventID;        | Sub-Event identificative number. (Default: 0)
TString    |   fSubEventTag;       | A short length label to identify the sub-Event. (Default: "")
TTimeStamp |   fEventTime;         | Absolute event time. (Default: 0)
Bool_t     |   fOk;                | Flag to be used by processes to define an event status. (Default: true)

All of these class members are in hidden level "protected", which means we can directly use them in the 
inherited classes, while cannot access to them in the other classes. In other classes, we need to call
getter and setter methods of TRestEvent. e.g. GetID(), GetSubEventTag(), SetTime(), etc.

These seven class members contains basic and universal infomation of an event. In the derived class, the user
needs to add more class members to store event data. For example, in class TRestRawSignalEvent, we 
define a vector of TRestSignal object and store all the readout wave forms in it.  

#### virtual methods
Initialize() is a pure virtual method in TRestEvent. So in derived classes the user **must** implement it. 
This method is used to reset data in the class. In the base class TRestEvent, this method resets values of:
fEventID, fSubEventID, fSubEventTag, fEventTime and fOk (Note that run origin of the event won't be reset). 
When one is going to implement this method in derived class, he can first call TRestEvent::Initialize() 
and set these five universal class members, before reseting other defined class members. The followings are 
some example codes in TRestRawSignalEvent.

`void TRestRawSignalEvent::Initialize(){`  
&emsp;`TRestEvent::Initialize();`  
&emsp;`fSignal.clear();`  
&emsp;`fPad = NULL;`  
&emsp;`gr = NULL;`  
&emsp;`mg = NULL;`  
&emsp;`fMinValue = 1E10;`  
&emsp;`fMaxValue = -1E10;`  
&emsp;`fMinTime = 1E10;`  
&emsp;`fMaxTime = -1E10;`  
`}`  

PrintEvent() is a virtual method used for printing data of the event. By default it prints only five universal
class members of the event: fEventID, fSubEventID, fSubEventTag, fEventTime and fOk. To print more details,
one needs to implement it in the derived class. For example, in TRestRawSignalEvent, we make a print of the
value of waveforms of each signal.

DrawEvent() returns a TPad object containing the plot of the event. It is called in TRestGenericEventViewer.
By default the method returns a blank pad. To enable the functionality of drawing, one needs to implement this. 

#### useful methods and tools

The virtual method CloneTo() defines how the data in an event is cloned to another existing event. This method 
by default calls ROOT streamer to do cloning. It is already functional for all the derived classes. However,
one can still override this default behavior in the derived class. This may improve the efficiency in some cases.
Also,when we hide some class members againist ROOT streamer, by overriding this method, we can still copy them 
if we want them.

PrintAddress() is used for debugging. It prints the address of this event together with the address of its class
members.

SetEventInfo() copies only the universal information from the input event. It is used when we want to manually 
transfer an event and not to use CloneTo();

### TRestMetadata

This is an abstract class inherited from TNamed (TNamed is inherited from TObject). To write a TRestMetadata inherited
class, there is a pure virtual method InitFromConfigFile() which everyone needs to implement. This method defines
how this class loads data from rml config file. As there is too many methods in the class, we just explain
roughly its usage here. For detailed class reference, check the 
[doxygen website](https://p3.doxygen.pandax.sjtu.edu.cn/classTRestMetadata.html).

#### startup of TRestMetadata

Derived class can have the same start up strategy defined in TRestMetadata. The main starter method 
InitFromConfigFile() needs to be called manually after or in the constructor. For example:

// in constructor  
`TRestReadout::TRestReadout( const char *cfgFileName, string name) : TRestMetadata (cfgFileName){`  
&emsp;`cout << "Loading readout. This might take few seconds" << endl;`  
&emsp;`Initialize();`  
&emsp;`LoadConfigFromFile( fConfigFileName, name );`  
`}`  

`int main(){`  
&emsp;`TRestReadout*readout=new TRestReadout("readout.rml","strippedReadout");`  
&emsp;`...`  
`}`  

or  
// after constructor  
`int main(){`  
&emsp;`TRestReadout*readout=new TRestReadout();`  
&emsp;`readout->LoadConfigFromFile("readout.rml");`  
&emsp;`...`  
`}`  

In method LoadConfigFromFile(), we first open the given rml file and find the corresponding xml section in it.
The xml section, wrapped by tinyxml class "TiXmlElement", is saved in the TRestMetadata class. Then the method
LoadSectionMetadata() is called, which sets name, title and verboseLevel for the class. It also makes some 
parpration with config xml section (expand for loop, replave env, etc.). Then the method InitFromConfigFile() 
is called. This method is pure virtual and must be implemented in the derived class.

InitFromConfigFile() is implemented to give the startup logic. Specially, when we are doing sequential 
startup, it is good to write some methods which loops up all the child elements in config xml section.

For example, in most of the process classes, we implement the method directly to load config file, by 
calling method GetParameter():

`void TRestRawSignalTo2DHitsProcess::InitFromConfigFile(){`  
&emsp;`fSelection = GetParameter("selection", "0");`  
&emsp;`fBaseLineRange = StringTo2DVector(GetParameter("baseLineRange", "(5,55)"));`  
&emsp;`...`  
`}`  

In TRestManager, we implement with sequential startup:

//TRestManager.h  
`void InitFromConfigFile() {`  
&emsp;`if (fElement != NULL){`  
&emsp;&emsp;`TiXmlElement*e = fElement->FirstChildElement();`  
&emsp;&emsp;`while (e != NULL){`  
&emsp;&emsp;&emsp;`ReadConfig(...)`  
&emsp;&emsp;&emsp;`e = e->NextSiblingElement();`  
`}}}`  
`Int_t ReadConfig(string keydeclare, TiXmlElement* e);`  

//TRestManager.cxx  
`Int_t TRestManager::ReadConfig(string keydeclare, TiXmlElement* e){`  
&emsp;`if (Count(keydeclare, "TRest") > 0){...}`  
&emsp;`else if (keydeclare == "addTask") {...}`  
&emsp;`return -1`  
`}`

![alt](Image/Metadata_Stratup.png)

#### other virtual methods

The startup method LoadSectionMetadata() and Initialize() can be overriden by child classes.

We also provide interface of printing in the method PrintMetadata(). REST macros like REST_PrintMetadata 
call this method and print the information on screen. The user needs to implement it in some times.

#### useful methods and tools

REST provides rml parsing methods, reflection methods and string output tools in TRestMetadata, as 
shown in the following. Inline methods from the class TRestStringHelper are also available. We will 
not talk about them here. Check in our 
[doxygen website](https://p3.doxygen.pandax.sjtu.edu.cn/classTRestStringHelper.html).

##### protected class members

Type | Name | Description
-------------|------------|-------------
std::string                           | fConfigFileName | Full name of the rml config file
std::string                           | fSectionName    | Section name given in the constructor of the derived metadata class
REST_Verbose_Level                    | fVerboseLevel   | Verbose level used to print debug info. Won't save in file.
TRestManager*                         | fHostmgr        | All metadata classes can be initialized and managed by TRestManager. We keep a pointer of TRestManager in all the classes. Won't save in file.
Bool_t                                | fStore          | This variable is used to determine if the metadata structure should be stored in the ROOT file. Won't save in file.
TiXmlElement*                         | fElement        | Saving the sectional element together with global element. Won't save in file.
TiXmlElement*                         | fElementGlobal  | Saving the global element, to be passed to the resident class, if necessary. Won't save in file.
vector&lt;TiXmlElement*>              | fElementEnv     | Saving a list of environmental variables. Won't save in file.
TRestLeveledOutput&lt;REST_Silent>    | fout            | use this to display message when verbose level is >= REST_Silent. The message is in middle with color bold blue.
TRestLeveledOutput&lt;REST_Essential> | essential       | use this to display message when verbose level is >= REST_Essential. The message is in middle with color bold green.
TRestLeveledOutput&lt;REST_Info>      | info            | use this to display message when verbose level is >= REST_Info. The message is in middle with color bold green.
TRestLeveledOutput&lt;REST_Silent>    | error           | use this to display error messages. The message is left orientated with color bold red.
TRestLeveledOutput&lt;REST_Essential> | warning         | use this to display warning messages. The message is left orientated with color bold yellow.
TRestLeveledOutput&lt;REST_Debug>     | debug           | use this to display debug messages. The message is left orientated with color white.
TRestLeveledOutput&lt;REST_Extreme>   | extreme         | use this to display extreme debug messages. The message is left orientated with color white.

String output tools are as data member of TRestMetadata. The usage is very simple, just like cout:

`fout << "hellow world" << endl;`  
`debug << "Setting input event..." << endl;`

Add the lines like this in your class' method functions. During running, when verbose level of this class is 
set to be higher than(or equal to) REST_Debug, the second message will automatically be shown. Otherwise 
there will only be the first message on screen. 

TRestStringOutput allows the user to set output color, border, length and orientation. Check its usage
[here](https://p3.doxygen.pandax.sjtu.edu.cn/classTRestStringOutput.html).

To set colors, use the method TRestStringOutput::setcolor(). For example: 

`fout.setcolor(COLOR_RED)`

Here COLOR_RED is one of the pre-defined colors in REST, in the following list.

Name of Color | actual value(type: const char*)
-------------|------------
COLOR_RESET   |  "\033[0m"
COLOR_BLACK   |  "\033[30m"      
COLOR_RED     |  "\033[31m"      
COLOR_GREEN   |  "\033[32m"      
COLOR_YELLOW  |  "\033[33m"      
COLOR_BLUE    |  "\033[34m"      
COLOR_MAGENTA |  "\033[35m"      
COLOR_CYAN    |  "\033[36m"      
COLOR_WHITE   |  "\033[37m"      
COLOR_BOLDBLACK   |  "\033[1m\033[30m"      
COLOR_BOLDRED     |  "\033[1m\033[31m"      
COLOR_BOLDGREEN   |  "\033[1m\033[32m"      
COLOR_BOLDYELLOW  |  "\033[1m\033[33m"      
COLOR_BOLDBLUE    |  "\033[1m\033[34m"      
COLOR_BOLDMAGENTA |  "\033[1m\033[35m"      
COLOR_BOLDCYAN    |  "\033[1m\033[36m"      
COLOR_BOLDWHITE   |  "\033[1m\033[37m"      
COLOR_BACKGROUNDBLACK   |  "\033[1m\033[40m"      
COLOR_BACKGROUNDRED     |  "\033[1m\033[41m"      
COLOR_BACKGROUNDGREEN   |  "\033[1m\033[42m"      
COLOR_BACKGROUNDYELLOW  |  "\033[1m\033[43m"      
COLOR_BACKGROUNDBLUE    |  "\033[1m\033[44m"      
COLOR_BACKGROUNDMAGENTA |  "\033[1m\033[45m"      
COLOR_BACKGROUNDCYAN    |  "\033[1m\033[46m"      
COLOR_BACKGROUNDWHITE   |  "\033[1m\033[47m"      


##### rml methods

Here we list roughly the main rml methods in TRestMetadata. To look for more details, check the 
[doxygen website](https://p3.doxygen.pandax.sjtu.edu.cn/classTRestMetadata.html).

Return Type | Name | Number of Overloads | Description
-------------|------------|-------------|-----------
std::string   | GetParameter                  | 3 | Basic "GetParameter" functionality. Search in system env, section attribute value and "parameter" child section in sequence.
std::string   | GetFieldValue                 | 2 | Basic "GetParameter" functionality. Search only in section attribute value.
double        | GetDblParameterWithUnits      | 3 | Basic "GetParameter" functionality. Search unit definition near the parameter, and convert the value to double automatically.
TVector2      | Get2DVectorParameterWithUnits | 3 | Basic "GetParameter" functionality. Search unit definition near the parameter, and convert the value to TVector2 automatically.
TVector3      | Get3DVectorParameterWithUnits | 3 | Basic "GetParameter" functionality. Search unit definition near the parameter, and convert the value to TVector3 automatically.
TiXmlElement* | GetRootElementFromFile        | 1 | Tinyxml interface. Open an xml file and returns its root element.
TiXmlElement* | GetElement                    | 3 | Tinyxml interface. Finds the xml element with certain decalration, wraps it as class TiXmlElement.
TiXmlElement* | GetElementWithName            | 2 | Tinyxml interface. Finds the xml element with certain decalration and name attribute, wraps it as class TiXmlElement.
TiXmlElement* | StringToElement               | 1 | Old xml parser interface. Convert std::string to an xml element.
std::string   | ElementToString               | 1 | Old xml parser interface. Convert an xml element to string.
std::string   | GetKEYStructure               | 5 | Old xml parser interface. Get directly a string of xml section from either another string or TiXmlElement
std::string   | GetKEYDefinition              | 4 | Old xml parser interface. Get directly a string of xml section, exclude its child sections.

In most cases we just write a simple class with few class members to set from rml config file. We can simply use
GetParameter() to do the setting.

##### reflection methods

Reflection means a mapping from string type class/classmember name to the actual address of the 
class/classmember. For example some times we need to write a config file to set values for a hundred of 
class members. The actual lines in the config file may be just several, but we need to parpare a hundred
of lines like:   
`par1=GetParameter("par1","")==""?0:StringToInteger(GetParameter("par1"));`  
in the code. Of course we don't want to write like this. In this case we need reflection. 

Another example is that when we are writing a base class, we usually want to make it easier to inherit. 
We want to directly implement the InitFromConfigFile() method for all of the derived class. How can we 
set the class members' value even when we don't know them? We also need reflection. Note that if the 
class member has no ROOT streamer(annotation //! is added after member definition), the reflection
won't work for it. 

The reflection methods are in the table below:

Return Type | Name        | Input Type | Description
------------|-------------|------------|---------
TStreamerElement* | GetDataMemberWithName      | string                      | Get ROOT streamer class of the target class member with class member name
TStreamerElement* | GetDataMemberWithID        | int                         | Get ROOT streamer class of the target class member with class member's streamer id(not real id)
int               | GetNumberOfDataMember      | ~                           | Get total number of avaliable class members. Initialize the reflection functionality.
double            | GetDblDataMemberVal        | TStreamerElement*           | Get the value of class member assume its type is double.
int               | GetIntDataMemberVal        | TStreamerElement*           | Get the value of class member assume its type is int.
char*             | GetDataMemberRef           | TStreamerElement*           | Get address of the given class member in type of char*
string            | GetDataMemberValString     | TStreamerElement*           | Get the value of class member assume its type is string.
void              | SetDataMemberVal           | TStreamerElement#, char#  | Set the value of class member with a pointer. Assume the pointer is of same type of the class member.
void              | SetDataMemberVal           | TStreamerElement*, string   | Set the value of class member with a number string. Convert the string to certain value type(int, double, float).
void              | SetDataMemberValFromConfig | TStreamerElement*           | Autometically set the value of corresponding class member with the given xml section.

(#: same as *)

Now to setup value of a hundred class members from the rml file, we can write a loop:

`int n = GetNumberOfDataMember();`  
`for (int i = 1; i < n; i++) {`  
&emsp;`TStreamerElement* e = GetDataMemberWithID(i);`   
&emsp;`SetDataMemberValFromConfig(e);`  
`}`  


### TRestEventProcess

#### implementing

The most frequent case of developing REST is to write or modifiy an event process. TRestEventProcess is 
inherited from TRestMetadata, adding extra interfaces and tools to it. We need to implement/redefine 
additional methods/variables in TRestEventProcess. The followings are a list of them. 
The ones with a "!" mark ahead must be implemented/redefined by the user.

! TRestEvent* fInputEvent  

! TRestEvent* fOutputEvent  

virtual void InitProcess()  

virtual void BeginOfEventProcess()  

! virtual TRestEvent *ProcessEvent( TRestEvent *evInput ) = 0  

virtual void EndOfEventProcess()  

virtual void EndProcess()  

The variables "fInputEvent" and "fOutputEvent" defines the address of the process's input and output event.
It is recommended to set them in the process's constructor. During launch, REST will check the 
input-output event type of each process to make sure the process chain is vailed. The following shows an 
example to write a constructor which sets input and output events. 

`class TRestRawSignalAnalysisProcess :public TRestEventProcess {`  
&emsp;`private:`  
&emsp;`//we define specific pointer of TRestEvent in class`  
&emsp;`TRestRawSignalEvent *fSignalEvent;//!`  
&emsp;`...`  
`}`  

`TRestRawSignalAnalysisProcess::TRestRawSignalAnalysisProcess(){`  
&emsp;`fSignalEvent = new TRestRawSignalEvent();//we instantiate specific TRestEvent object`  
&emsp;`fOutputEvent = fSignalEvent;//and then sets fOutputEvent and fInputEvent to this pointer.`  
&emsp;`fInputEvent = fSignalEvent;`  
&emsp;`...`  
`}`  

After the input-output check, REST will call the method InitProcess() of each loaded process. Some values 
should be cleared/reset here in case the process needs to run again. 

The three methods: BeginOfEventProcess(), ProcessEvent() and EndOfEventProcess() are called in sequence
for one event. The most important one is the pure virtual method ProcessEvent(). It receives an input 
event and gives an output event, both in type TRestEvent. We need to force transform the pointer type
inside the method. Note that although ProcessEvent() seems to be independent on fInputEvent and 
fOutputEvent, we still need to set fOutputEvent/fInputEvent inside the method. This is because sometimes
after process we still need to access to them. To set fOutputEvent/fInputEvent, we can directly copy 
the pointer(shallow copy) at the beginning of the method. If we want a deep copy, we can use 
TRestEvent::CloneTo(). REST also allows the event cut. Simply return a NULL pointer inside the
method, and this event will be skipped. The following is an example:

`TRestEvent* TRestRawSignalAnalysisProcess::ProcessEvent( TRestEvent *evInput ){`  
&emsp;`fSignalEvent = (TRestRawSignalEvent *)evInput;`  
&emsp;`//hallow copy`  
&emsp;`fOutputEvent = fSignalEvent;`  
&emsp;`//deep copy`  
&emsp;`//fSignalEvent->CloneTo(fOutputEvent);`  
&emsp;`fInputEvent = fSignalEvent;`  
&emsp;`//a cut which selects event that have 10 or more signals`    
&emsp;`if(fSignalEvent->GetNumberOfSignals()<10)return NULL;`
&emsp;`//some analysis for fSignalEvent`  
&emsp;`...`  
&emsp;`return fSignalEvent;`  
`}`  


The method EndProcess() is a method to be called after all the events are finished. Some additional jobs 
can be done here. For example, we can show a message about the process status in this method. Or, we can
call saving for a TH1D object to the output file.

#### using observables to save the result

There are two ways to save analysis result of a process: creating observables and saving internal 
variables. They both have advantages and shortcomings.

To save observables, we can call the method TRestAnalysisTree::SetObservableValue() inside
TRestEventProcess::ProcessEvent(). TRestEventProcess objects share pointer to a public analysis 
tree, we can directly call it like following:

//inside function ProcessEvent()(TRestRawSignalAnalysisProcess.cxx)  
`Double_t baseLineSigma = fSignalEvent->GetBaseLineSigmaAverage( fBaseLineRange.X(), fBaseLineRange.Y() );`  
`fAnalysisTree->SetObservableValue( this, "baseLineSigmaMean", baseLineSigma );`  

We also need to add a line in rml file to tell REST to save the observable. The name should be same 
as the string argument in SetObservableValue() method:

`<addProcess type="TRestRawSignalAnalysisProcess" name="sAna" value="ON">`  
&emsp;`...`  
&emsp;`<observable name="baseLineSigmaMean" value="ON" />`  
`</addProcess>`  

It is recommended to use observables to store important analysis result. They are saved in single branches,
so the reading could be faster. The observables can only be double type.

Another way is to define a class member inside the process class, then change its value during
the ProcessEvent() function. When this function returns a non-null pointer, REST will auto save 
this internal variable. The following is an example:

//inside class definition(TRestRawSignalAnalysisProcess.h)  
`Double_t baseLineSigmaMean;`  

//inside function ProcessEvent()(TRestRawSignalAnalysisProcess.cxx)  
`baseLineSigmaMean= fSignalEvent->GetBaseLineSigmaAverage( fBaseLineRange.X(), fBaseLineRange.Y() );`  

We can find the internal variable "baseLineSigmaMean" in process branch in AnalysisTree inside output file.
This kind of definition could be more complex. Changing class member may cause previous version unreadable. 
The user also needs to reset the values of the class members if the method will return before value 
assignment. The good point is that it supports various data type, not only double type. We can add vector,
map, or even self-defined structures in the process branch. For example, in TRestRawSignalAnalysisProcess, 
we want to save baseline sigma for each signal, not an average value. So we use a vector&lt;double> member
in class definition. 

#### single thread process

Processes can be set to single thread only by a bool value "fSingleThreadOnly". This option is for some 
special processes like external process and viewer process. The external processes read external data file,
and the viewer process displays event figure. They cannot be executed in parallel. Set its value to true in 
the constructor. 

REST will only activate one thread if a single thread process exists in the process chain.
(note that external process is not in the process chain.)

#### other class members

TRestEventProcess keeps the following pointers that may be useful:

Type | Name | Description
-------------|------------|-------------
TRestAnalysisTree*            | fAnalysisTree      | The pointer to analysis tree, we usually call fAnalysisTree->SetObservableValue()
TRestRun*                     | fRunInfo           | The pointer to TRestRun, which can provide the process with some metadata objects and run information in some cases.
vector&lt;TRestEventProcess*> | fFriendlyProcesses | A list to all the friendly processes in the process chain. We may need to use the analysis result of other processes in some cases.
TCanvas*                      | fCanvas            | A TCanvas object for drawing.

We also implemented a series of methods GetXXXMetadata(). For example to get the data of readout definition,
we can simply use  
`fReadout = (TRestReadout*)GetReadoutMetadata();`  
in the class's method funciton. The alternative call is:  
`fReadout = (TRestReadout*)fRunInfo->GetMetadata("TRestReadout")`


[**prev**](6-some-metadata-classes.md)
[**contents**](0-contents.md)
[**next**](8-start-your-own-analysis-with-rest.md)