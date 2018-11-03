## Contributing changes to the repository with Git

The Git system is an efficient way to track changes to the code in the repository `if used smartly`.
The history of the code repository will be digested into `commits`. Being a commit a minimum change to
the code of the repository. Usually involving no more than 2-3 files. 

The code can be independently developed into `branches` where we add `commits`. The main branch or `master
branch` is the branch where we should finally merge the definitive changes of any `development branch`.

You may refer to the [Git website](https://git-scm.com/book/en/v2/Getting-Started-Git-Basics) for 
details on basic git usage.

### Using branches in REST repository

A branch can be contributed by several users at the same time. The `REST` repository will contain at 
least `one main development branch` that can be contributed by anyone with developper access (while 
any user can create his own branch for personal testing and/or future merging to the development branch). 
The name of this branch will be the REST version followed by `_dev`, i.e. `v2.2.1_dev`.

You can place yourself in the development branch by using `git checkout`

```
git checkout v2.2.1_dev
```

Use `git status` at any time in the command line to get information of the `branch name` you are working on,
and the files you have modified.


Small changes to the code, i.e. bug fixes, new class methods, new processes, etc, can be directly pushed
 to the `development branch`. While, major changes taking place in longer development periods, i.e. days or 
weeks, should be contributed in an independent branch for future merge to the `development branch`.

For details on branches usage refer to the [branches section](https://git-scm.com/book/en/v2/Git-Branching-Branches-in-a-Nutshell) 
at the Git documentation.

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
certain that the changes in a file should be included in a `commit` we can add the file to be commited

```
git add sourceFile.cxx
```

Hint: Use `git status` frequently!

We should proceed following this scheme (checking for differences for each file to be commited before 
adding it to the `commit`). When we have added all the files in a commit we can prepare the commit.

```
git commit -m "My contribution to REST"
```

It is important to know that, up to now, these changes took place only on your `local machine`, and in order
to push (or upload) these changes to the repository we must do `git push`.

```
git push
```

Note: This action may fail in case your local branch is not synchronized with the remote branch, or in 
other words, the repository contains new commits that you have not pulled (downloaded) to your local copy.

In that case you will need to update your local copy using `git pull`.


The first steps with git may be sometimes confusing for beginners. Specially if you enter in conflict
with others code. We encourage you to request help in the `REST forum` in case you run into troubles.

### The commit message format

One of the most critical parts of a commit in the REST repository is the commit message. The Gitlab
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

Please, make your `commit message as short and significative` as possible. The commit message is not intended
to explain others the changes you introduced, but to give an idea. The changes introduced should be visible
and accessible on the changes introduced in the `documentation` of the class.

A `bad commit message` will pretend to provide the use of the new feature introduced, for example.

```
git commit -m "I added a new method that provides the energies of gamma transfer in a biasing volume. The new method should receive 3 parameters as input ... and it will return a value in the specified energy range."
```

The following commit message contains the following errors

- It does not contain the name of the `class`, neither the `method`.
- It is `not concise`. The explanations should be included inside the code together the documentation of the method.
- It is `not short`.

## REST Versioning

The impact of changes in REST classes, i.e. adding, removing or modifying class members 
should be minimized by the use of `automatic schema evolution` feature of ROOT.

REST is compiled by default using `automatic schema evolution` since version `2.2.1`.
Therefore, any new/future generation of data before this version is `not recommended` for compatibility reasons.

Any data produced with version `2.2.1`, and later, will be generated using `automatic schema evolution`, and therefore,
any future REST version `should be able to read older versions` without major issues or warnings.

----

The REST versioning system will allow to stamp the data generated with REST in order to identify new features 
or major changes to the code.

A change in REST version `serves to markdown an important step` in the evolution of the code. 

REST version might serve as `a solution to reproduce or recover previous results` which may show discrepancies with future versions, 
i.e. a version number will serve as reference and it can be provided together with published 
or internal results that were produced with a given version.

----

REST version might be increased in at least three different scenarios.

1. When new features are added (optional).

2. When changes or modifications affect the behaviour of the framework.

3. To fix a REST version release to produce data in a experiment physics run.


A version number increase will be optional but `may be justified` by the following updates to the code.

- New processes, metadata or event data types that introduce new funtionalities to REST.

- Important changes on REST core libraries that introduce new features.


A version number increase `will be mandatory` when the modification of existing processes or REST core libraries that change the behaviour and may lead to different results.


- leading to different results by modifying, upgrading or debugging of existing processes or REST classes

- modifying the structure of ROOT outputfile

- changes to metadata structures that REST users should be aware of


### Generating a new version number

REST uses the git tagging system to generate the REST version number at compilation time.

The basics of tagging in Git are described at the following site [GitLab tagging](https://git-scm.com/book/en/v2/Git-Basics-Tagging)

The version number must be written using the following format 2.X.Y, where X represents a major change or transition in the code,
and Y represents a minor change/correction/update of the code.

In order to change the version of REST we need to tag the state of the code using `git tag` command. Usually by increasing the value of Y.

I.e. if we are in version 2.2.1 we will upgrade the REST version to 2.2.2, indicating a message providing a clear clue of the reason of the new tagging version.
 
`git tag -a v2.2.2 -m "A small step for REST but a big step for humanity"`

Additionaly, to publish the new version and make it visible to other users we need to use the `--tags` flag when pushing the code.

`git push --tags`

Any tag character that is not a number will be ignored in the construction of the REST release and the REST version code.

I.e. the tag "v2.2.3b" will become "2.2.3", and the REST version code will be 131586.

Then, at REST compilation time this tag will be retrieved and the `TRestVersion.h` header will be produced containning the following information.

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

It would be appropiate that if a new REST version has been generated using this system we `make a merge request to the master branch`.

### Using the version number

Any `TRestMetadata` class contains a member named `fVersion` that will be initialized using `TRestVersion.h` and that will be written 
to disk together with other metadata information.

The version of a metadata structure recovered from a ROOT file can be retrieved by using `GetVersion()` and  `GetVersionCode()` methods.
And it can be used to compare to the installed version of REST to identify if the version we are running is newer, older or the same as the one stored in disk.

```c++
TRestSpecificMetadataClass *md = (TRestSpecificMetadataClass *) file->Get("mdName");

if( md->GetVersionCode() > REST(2,2,1) )
    cout << "Yes. This metadata structure was generated with a version newer than 2.2.1!" << endl;

if( md->GetVersion() == REST_RELEASE )
    cout << "Yes. The REST version used to generate this metadata structure is the same as the installed REST version!" << endl;
```

This programming enables the REST users to take special actions need to be taken at a particular version or after a particular version.


