# Contributing to REST

## 1. Contributing changes to the repository with Git

The Git system is an efficient way to track changes to the code in the repository `if used smartly`.
The history of the code repository will be digested into `commits`. Being a commit a minimum change to
the code of the repository. Usually involving `no more than 2-3 files`. 

The code can be independently developed into `branches` where we add `commits`. The main branch or 
`master branch` is the branch where we should finally merge the definitive changes of any `development branch`.

You may refer to the [Git website](https://git-scm.com/book/en/v2/Getting-Started-Git-Basics) for details on basic git usage. See also basic [CERN Gitlab tutorials](https://gitlab.cern.ch/help/gitlab-basics/README.md).

### Using branches in REST repository

A branch can be contributed by several users at the same time. The `REST` repository will contain at 
least `one main development branch` that can be contributed by anyone with developer access (while 
any user can create his own branch for personal testing and/or future merging to the `development branch`). 
The name of this branch will be the REST version followed by `_dev`, e.g. `v2.2.1_dev`.

You can place yourself in the development branch by using `git checkout`

```
git checkout v2.2.1_dev
```

Use `git status` at any time in the command line to get information of the `branch name` you are working on,
and the files you have modified.


Small changes to the code, i.e. bug fixes, new class methods, new processes, etc, can be directly pushed
 to the `development branch`. While major changes taking place in longer development periods, i.e. days or 
weeks, should be contributed in an independent branch for future merge to the `development branch`.

For details on branches usage refer to the [branches section](https://git-scm.com/book/en/v2/Git-Branching-Branches-in-a-Nutshell) 
at the Git documentation.

### Documenting new methods or classes added to the repository

TODO : Explain doxygen formatting, tutorials, where official doc is located. ETC.

### REST forum. Communication of updates to the code and discussion on development topics.

TODO : describe how changes should be discussed at REST forum. Bug report, report on changes, encourage discussion, user support, etc.

### Adding a new commit to the development branch

It is critical to prepare the `commit` following a few basic rules in order to allow future code review and track
historical changes that may produce a future conflict identified much later on in time.

The changes introduced to any single file added to the `commit` should be minimal, i.e. avoiding to upload a 
file with temporary debugging code or comments used during the code development and testing process.

Only the files involved in a particular change should be included in the `commit`.

In order to check the differences introduced in the `local copy` of a particular file we can use `git diff`,

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

It is important to know that, up to now, these changes took place only on your `local machine`, and in order
to `push` (or upload) these changes to the repository we must do `git push`.

```
git push
```

Note: This action may fail in case your local branch is not synchronized with the `remote branch`, or in 
other words, the repository contains new commits that you have not pulled (downloaded) to your `local copy`.

In that case you will need to update your local copy using `git pull`.


The first steps with git may be sometimes confusing for beginners. Specially if you enter in conflict
with others code. We encourage you to request help in the `REST forum` in case you run into troubles.

### The commit message format

One of the `most critical` parts of a commit in the REST repository is the `commit message`. The GitLab
web interface will allow to list and navigate through the commit history and changes introduced in
each file.

The interface allows to search commits by using the contents of the commit message. Therefore, a proper
commit message will allow us to search and identify promptly where given changes were introduced.

The commit message `should include` at least the name of the main class, the name of the binary, script
 or package being modified.

Having a commit message format will help in the future to find all changes related to a given REST class
or file. Some examples of `good commit messages` are the following ones

```
git commit -m "TRestReadout::GetPixelPosition method modified. Bug fixed related to pixel rotation."

git commit -m "restG4. PrimaryGenerator new type of spatial generator added."

git commit -m "TRestFieldMap class added for the first time."

```

Please, make your `commit message as short and significant` as possible. The commit message is not intended
to explain others the changes you introduced, but to give an idea. The changes introduced will be accessible
in the contents of the commit that can be searched in the web interface. Therefore, more detailed comments
can be introduced in the `documentation` of the class, and it will be highlighted in the commit differences by
the GitLab web interface when we access the commit contents.

A `bad commit message` will pretend to provide the use of the new feature introduced, for example.

```
git commit -m "I added a new method that provides the energies of gamma transfer in a biasing volume. The new method should receive 3 parameters as input ... and it will return a value in the specified energy range."
```

The above-mentioned commit message contains the following mistakes:

- It does not contain the name of the `class`, neither the `method`.
- It is `not concise`. The explanations should be included inside the code together the documentation of the method.
- It is `not short`.

## 2. REST Versioning

`!!!NOTE!!!`  
Since version `2.2.1`, REST is adopting `automatic schema evolution` feature of ROOT.
Therefore, the impact of changes in REST classes, i.e. adding, removing or modifying class members, 
should have been minimized. That is, any future REST version `should be able to read 
older versions` without major issues or warnings. Therefore, any new/future generation 
of data before v2.2.1 is `not recommended` for compatibility reasons.

----

The REST versioning system will allow to **stamp the data generated** with REST and it will 
allow to **identify new features or major changes** to the code.

The stamped version number in the file might serve as a solution to reproduce or 
recover previous results which may show discrepancies with future versions. The version 
number shall be provided together with published or internal results. Moreover, if we own 
the data file, we will always be able to recover the version used to generate those results.

A change in REST version serves to mark down an important step or a timeline in the evolution
of the code. The version `might be increased` in at the following scenarios:
1. When new features are added.
2. When changes or modifications affect the behaviour of the framework.
3. To fix a REST version release to produce data in a experiment physics run.
4. New processes, metadata or event data types that introduce new functionalities to REST.
5. Important changes on REST core libraries that introduce new features.

A version number increase `will be mandatory` when the modification of existing processes or
REST core libraries change the behaviour and may lead to different results:
- leading to different results by modifying, upgrading or debugging of existing processes or REST classes,
- modifying the structure of ROOT output file,
- changes to metadata structures that REST users should be aware of.

### Git tagging system and its relation to REST versioning system

The basics of tagging in Git are described at the following site: [Git Tagging](https://git-scm.com/book/en/v2/Git-Basics-Tagging)

In order to change the version of REST we need to tag the state of the code using `git tag` command:

`git tag -a v2.2.2 -m "A small step for REST but a big step for humanity"`

Additionally, to publish the new version and make it visible to other users we need to use the `--tags` flag 
when pushing the code:

`git push --tags`

At REST compilation time this tag will be retrieved and the `TRestVersion.h` header will be produced 
containing the following information:

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
#define REST_GIT_BRANCH "v2.2.1"
#define REST_GIT_TAG "v2.2.1"
#define REST_VERSION_CODE 131585
#define REST_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#define REST_SCHEMA_EVOLUTION "OFF"
#endif
```

### Version control strategy

REST uses the git tagging system to control its version. The tag value is directly our version number. 
It must be written using the following format 2.X.Y, where X represents a major change or transition in 
the code, and Y represents a minor change/correction/update of the code. Usually we increase the value 
of Y when updating the version. Both X and Y ranges from 0 to 255.

A header `TRestVersion.h` is generated at compilation time, calling git commands from cmd line. This header
contains various information about version, version number, commit id, branch name, etc.. (see example of header above). We also keep a default 
TRestVersion.h in master branch for those who are unfamiliar with git. They may directly download zip from 
git website, and then unzip and call `cmake && make install` in REST directory. TRestVersion.h will be updated
together with version update.

We only create tags for commits in the master branch. This branch is also the default branch on the website or 
during `git clone`. We keep master branch being updated weekly or monthly, in each update we will assign a new 
tag. So if the user only download/clone the master branch, he will always get the **tagged commit**, which is 
exactly a definite version.

All the development work shall be within individual branches. The development branches should be named after 
the version from which they are checked out. e.g. `v2.2.1_dev` or `v2.2.3_trackAnalysisNew`. Whenever the 
developer verifies himself that the modification is working, he can `make a merge request to the master branch`.
We will test those changes also. If we decide to accept the merge request, and if the changes are important, 
we will push the merge immediately. Otherwise we shall wait several other merges before pushing.

After the merge-to-master is pushed to gitlab, we will:

1. increase the version/tag to e.g. v2.2.2,
2. update TRestVersion.h in master branch,
3. remove the development branch, and create a new one,
4. create a release note for the new version
5. send a mail to rest-dev@cern.ch mail list to inform the update.

### Using the version number

Any `TRestMetadata` class contains a member named `fVersion` that will be initialized using `TRestVersion.h` 
and that will be written to disk together with other metadata information. This member can be accessed by 
inherited classes by using `GetVersion()`, `GetVersionCode()` and `SetVersion()`.
 
fVersion is retrieved together with the metadata structure from a ROOT file. Then the result of GetVersion()
might be different from the version of current REST build. We can compare them and act differently according to the result.
 
There are two important parameters defined in `TRestVersion.h`: `REST_RELEASE` and `REST_VERSION_CODE`.

* `REST_RELEASE` is a string that will be stored in any `TRestMetadata::fVersion` class member when it is written
to disk, and it can be recovered in future using `TRestMetadata::GetVersion()`. 
* `REST_VERSION_CODE` is a 
code generated using `REST_VERSION( 2, X, Y)` where X and Y are the major and minor version numbers. This code can be used to determine if a REST version is more recent or older than the installed REST
version. The code of any metadata structure can be retrieved calling `TRestMetadata::GetVersionCode()`.

These two parameters, `REST_RELEASE` and `REST_VERSION` will allow us always to compare the installed version 
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

The contributors may first have a quick look read of common [C++ coding styles](http://geosoft.no/development/cppstyle.html) 
before starting coding in the repository. Even if you have long experience writing you will reinforce your coding style 
and detect few things you might be doing wrong when writ-ting readable code. We should try to keep a fixed style.
Points 84 and 71 (4 spaces indentation) are very important!

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

We need to make `Printing` better with the help of REST string output tool. The style of PrintMetadata()
shall be:

* centered in screen
* bordered with "||" (at side) and "=======" (at top/bottom)
* white colored

