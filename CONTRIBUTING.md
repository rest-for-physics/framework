# Contributing to REST

## 1. Contributing changes to the repository with Git

The Git system is an efficient way to track changes to the code in the repository *if used smartly*.
The history of the code repository will be digested into **commits**. Being a commit a minimum change to
the code of the repository. Usually involving **no more than 2-3 files**. 

The code can be independently developed into **branches** where we add *commits*. The main branch or 
**master branch** is the branch where we should finally merge the definitive changes of any parallel **development branch**.

You may refer to the [Git website](https://git-scm.com/book/en/v2/Getting-Started-Git-Basics) for details on basic git usage. See also basic [CERN Gitlab tutorials](https://gitlab.cern.ch/help/gitlab-basics/README.md).

### Using branches in REST repository

A branch can be contributed by several users at the same time. The REST repository will contain at 
least **one main `development` branch** that can be contributed by anyone with developer access (while 
any user can create his own branch for personal testing and/or future merging to the *development branch*).

You can place yourself in the development branch by updating your local repository index `git fetch` and using `git checkout`

```
git fetch
git checkout development
```

Use `git status` at any time in the command line to get information of the **branch name** you are working on,
and the files you have modified.

The *development* branch is intended for obvious bug fixes or marked as minor issue. It is recommended that new contributions and functionalities added to REST have been previously discussed in [the forum](https://ezpc10.unizar.es/).

Major contributions or experimental updates should be placed in independent branches. User or dedicated branches that we name here`experimental` branches. 
While the *development* branch will be merged to master in the short term, experimental branches might not be finally merged to master, and are in principle considered temporary branches that might be removed at any time once they got stalled.
Therefore, small changes to the code, i.e. bug fixes, new class methods, new processes, etc, can be directly pushed to the `development branch`.
While major changes taking place in longer development periods, i.e. days or weeks, should be contributed in an independent branch for future merge to the **main development branch**.

For details on branches usage refer to the [branches section](https://git-scm.com/book/en/v2/Git-Branching-Branches-in-a-Nutshell) at the Git documentation.

### Documenting new methods or classes added to the repository

TODO : Explain doxygen formatting, tutorials, where official doc is located. ETC.

### Pipeline validation tests

TODO : Explain how pipeline validation tests should be implemented

### REST forum. Communication of updates to the code and discussion on development topics.

TODO : describe how changes should be discussed at REST forum. Bug report, report on changes, encourage discussion, user support, etc. Not defined yet.

### Adding a new commit to the development branch

It is critical to prepare the **commit** following a few basic rules in order to allow future code review and track
historical changes that may produce a future conflict identified much later on in time.

The changes introduced to any single file added to the **commit** should be minimal, i.e. avoiding to upload a 
file with temporary debugging code or comments used during the code development and testing process.

Only the files involved in a particular change, delimited by the commit message, should be included in the **commit**.

In order to check the differences introduced in the **local copy** of a particular file we can use `git diff`,

```
git diff sourceFile.cxx
```

This will allow us to identify the changes we will upload or `push` to the repository later on. When we are
certain that the changes in a file should be included in a `commit` we can add the file to be committed

```
git add sourceFile.cxx
```

Hint: Use `git status` frequently!

We should proceed following this scheme (checking for differences for each file to be committed before 
adding it to the `commit`). When we have added all the files in a commit we can prepare the commit.

```
git commit -m "My contribution to REST"
```

It is important to know that, up to now, these changes took place only on your **local machine**, and in order
to `push` (or upload) these changes to the repository we must do `git push`.

```
git push
```

Note: This action may fail in case your local branch is not synchronized with the **remote branch**, or in 
other words, the repository contains new commits that you have not pulled (downloaded) to your **local copy**.

In that case you will need to update your local copy before pushing any changes using `git pull`.


The first steps with git may be sometimes confusing for beginners. Specially if you enter in conflict
with others code. We encourage you to request help in the *REST forum* in case you run into troubles.

### The commit message format

One of the *most critical* parts of a commit in the REST repository is the *commit message*. The GitLab
web interface will allow to list and navigate through the commit history and changes introduced in
each file.

The interface allows to search commits by using the contents of the commit message. Therefore, a proper
commit message will allow us to search and identify promptly where given changes were introduced.

The commit message **should include** at least the name of the main class, the name of the binary, script
 or package being modified. Having a commit message format will help in the future to find all changes 
 related to a given REST class or file. Some examples of **good commit messages** are the following ones

```
git commit -m "TRestReadout::GetPixelPosition method modified. Bug fixed related to pixel rotation."

git commit -m "restG4. PrimaryGenerator new type of spatial generator added."

git commit -m "TRestFieldMap class added for the first time."

```

Please, make your **commit message as short and significant** as possible. The commit message is not intended
to explain others detailed changes you introduced, but to give an idea. The changes introduced will be accessible
in the contents of the commit that can be searched in the web interface. Therefore, more detailed comments
can be introduced in the *documentation* of the class. Differences will be highlighted in the commit by
the GitLab web interface when we access the commit contents.

A **bad commit message** will pretend to provide the use of the new feature introduced, for example.

```
git commit -m "I added a new method that provides the energies of gamma transfer in a biasing volume. The new method should receive 3 parameters as input ... and it will return a value in the specified energy range."
```

The above-mentioned commit message contains the following mistakes:

- It **does not contain the name** of the *class*, neither the *method*.
- It **is not concise**. The explanations should be included inside the code together the documentation of the method.
- It **is not short**`.

## 2. REST Versioning

Since version `2.2.1`, REST is adopting **automatic schema evolution** feature of ROOT.
Therefore, the impact of changes in REST classes, i.e. adding, removing or modifying class members, 
should have been minimized. That is, any future REST version *should be able to read 
older versions* without major issues or warnings. Therefore, any new/future generation 
of data `before v2.2.1` is **not recommended** for compatibility reasons.

----

The REST versioning system will allow to **stamp the data generated** with REST and it will 
allow to **identify new features or major changes** to the code.

The stamped version number in the file might serve as a solution to reproduce or 
recover previous results which may show discrepancies with future versions. The version 
number shall be provided together with published or internal results. Moreover, if we own 
the data file, we will always be able to recover the version used to generate those results.

A change in REST version serves to mark down an important step or a timeline in the evolution
of the code. The version **might be increased** in the following scenarios:

1. When new features are added.
2. When changes or modifications affect the behaviour of the framework.
3. To fix a REST version release to produce data in a experiment physics run.
4. New processes, metadata or event data types that introduce new functionalities to REST.
5. Important changes on REST core libraries that introduce new features.

A version number increase **will be mandatory** when the modification of existing processes or
REST core libraries change the behaviour and may lead to different results:
- leading to different results by modifying, upgrading or debugging of existing processes or REST classes,
- modifying the structure of ROOT output file,
- changes to metadata structures that REST users should be aware of.

### Git tagging system and its relation to REST versioning system

REST identifies each version to a Git tag. The process to increase a new REST version goes through creating a new `TRestVersion.h` header, commit it and associate the commit to a new tag that is named following the version number.

In order to generate a new REST version just go to `scripts/` directory, execute

```
./generateVersionHeader.py TRestVersion.h
```

and follow, step by step, the detailed instructions given on screen to push the new version to the development branch where you are, then request a merge to master.

The contents of the generated file will look something like this
```c++
#ifndef REST_Version
#define REST_Version
/* Version information automatically generated by installer. *//*
 * These macros can be used in the following way:
 * 
 * #if REST_VERSION_CODE >= REST_VERSION(2,23,4)
 *     #include <newheader.h>
 * #else
 *     #include <oldheader.h>
 * #endif
 *
 */
#define REST_RELEASE "2.2.1"
#define REST_RELEASE_DATE "2018-11-01"
#define REST_RELEASE_TIME "07:40:41 +0800"
#define REST_GIT_COMMIT "a71c196f"
#define REST_VERSION_CODE 131585
#define REST_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#define REST_SCHEMA_EVOLUTION "OFF"
#endif
```

Once the new version has been uploaded to the repository we must add the main changes to the [Tags section of the Gitlab repository](https://lfna.unizar.es/rest-development/REST_v2/tags/).

### Version control strategy

REST uses the git tagging system to control its version. The tag value is directly used to extract the REST version number. 
It must be written using the following format 2.X.Y, where X represents a major change or transition in 
the code, and Y represents a minor change/correction/update of the code. Usually we increase the value 
of Y when updating the version. Both X and Y ranges from 0 to 255. Usually X will be updated when a major change, or 
backwards compatibility takes place.

The script `generateVersionHeader.py` found under scripts directory will generate a `TRestVersion.h` header. This header
contains information about version, version number, commit id, date, etc.. (see example of header above). If satisfied
with the result generated inside `TRestVersion.h` you may update the REST version by overwritting the `TRestVersion.h`
found under `source/restcore/inc/`.

Therefore the steps to increase the version of REST in the development branch.

1. Execute the script "REST_v2/scripts/generateVersionHeader.py"
2. Follow instructions shown in screen.
3. Enter to the Gitlab website and document the changes on the **tags** section.
4. Merge the development branch to master.

Tags may be created (leading to an update of REST version) at any time. We reserve that for the main development
branch, that will be at some point merged to master, and the *only one* that will be merged to master.

### Using the version number

Any `TRestMetadata` class contains a member named `fVersion` that will be initialized using `TRestVersion.h` 
and that will be written to disk together with other metadata information. This member can be accessed by 
inherited classes by using `GetVersion()`, `GetVersionCode()` and `SetVersion()`.
 
`fVersion` is retrieved together with the metadata structure from a previously written ROOT file. Then the result of GetVersion()
might be different from the version of current REST build. We can compare them and act differently according to the result.
 
There are two important parameters defined in `TRestVersion.h`: `REST_RELEASE` and `REST_VERSION_CODE`.

* `REST_RELEASE` is a string that will be stored in any `TRestMetadata::fVersion` class member when it is written
to disk, and it can be recovered in future using `TRestMetadata::GetVersion()`. 
* `REST_VERSION_CODE` is a 
code generated using `REST_VERSION( 2, X, Y)` where X and Y are the major and minor version numbers. 
This code can be used to determine if a REST version is more recent or older than the installed REST
version. The code of any metadata structure can be retrieved calling `TRestMetadata::GetVersionCode()`.

These two parameters, `REST_RELEASE` and `REST_VERSION` will allow us to compare the installed version 
to the version stored in a `TRestMetadata` structures as follows.

```c++
//in restRoot or in some source codes

TRestSpecificMetadataClass *md = (TRestSpecificMetadataClass *) file->Get("mdName");

if( md->GetVersionCode() > REST_VERSION( 2, 2, 1 ) )
    cout << "This metadata structure was generated with a version newer than 2.2.1!" << endl;

if( md->GetVersionCode() < REST_VERSION_CODE )
    cout << "This metadata structure was generated with a version older than current version!" << endl;

if( md->GetVersion() == REST_RELEASE )
    cout << "The REST version used to generate this metadata structure is the same as the installed REST version!" << endl;
```

This programming enables the REST users to take special actions that may need to be done with a particular 
version or after a particular version.

## 3. Programming style

All Contributors should try to follow a single coding style in order to maintain cohesion of the code and make the code as readable as possible.

### Clang-Format

In order to achieve this cohesion we chose the tool [clang-format](https://clang.llvm.org/docs/ClangFormat.html) which is the industry standard tool to format C++ code. It only requires a single `.clang-format` file which is included on the root of the repository.

This file contains all the information about our coding style of choice which is based on the [Google C++ style](https://google.github.io/styleguide/cppguide.html). This file will track the latest changes in style directives and contributors are encouraged to submit improvements to this file with the goal of increasing readability and cohesion.

clang-format can be launched from the terminal and also is supported by almost any IDE or editor (such as [Vim](https://clang.llvm.org/docs/ClangFormat.html#vim-integration)). There is also an optional script (`/scripts/reformat-clang.sh`) which can be launched to reformat the whole repository. The contributor is encouraged to either enable clang-format support in their editor of choice or to run this script before a significant push.


### Control the amount of output message in a process

When writing a process, the hierarchy of five verbose level should be clarified:

* silent    : output nothing.
* essential : + print information (parameter value, status, warnings, etc) **before process starts**.
* info      : + print important but occasional **information** during the process. Occasional means only for some events.
* debug     : + print values during the process, usually no more than 10 lines. e.g. Number of input signals;
Number of hits added; total energy of output event, etc. These messages shall be helpful during **debug**.
* extreme   : + print detailed values, usually within **sub-event level**, e.g. position & energy of each hits added;
peak, width, rms for each signals added, sampling points for the first signal added.

There are some other notes:

* One can directly use `info << "some message" << endl;` for convenience and better appearance. The efficiency 
is a little lower than `if(GetVerboseLevel() >= REST_Info) cout << "some message" << endl;`  
* Process name should be attached when printing info message. e.g. 
`info << this->GetName() << " : Signals added : " << N <<endl;`  
* Don't add separators like `cout << "--------------" << endl;` for debug or extreme message. 
REST will add them for you.  
* Don't add getchar() for debug or extreme message. REST will also pause for you. In debug level, it will
pause after each event. In extreme level, it will pause after each process.  
* In both two verbose levels multi threading is disabled.  

### Style of PrintMetadata()

The default style for metadata information, that it usually printed by the `PrintMetadata` method, it is controlled through the `metadata << ... << endl;` directive. Furthermore, when we implement the `PrintMetadata` method of an inherited class we should call the pure `TRestMetadata::PrintMetadata` method in order to create a proper header.

```c++
void TRestReadout::PrintMetadata(Int_t DetailLevel) {
    if (DetailLevel >= 0) {
        TRestMetadata::PrintMetadata();

        metadata << "Number of readout planes : " << fNReadoutPlanes << endl;
        metadata << "Decoding was defined : ";
```
