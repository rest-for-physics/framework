## Contributing changes to the repository with Git (WP)

- Describe the relevance of using a Git repository system to track changes.

- Philoshophy of minimal commit changes.

- Define commit message format

- Define the use of branches

## REST Versioning

The impact of changes in REST classes, i.e. adding, removing or modifying class members 
should be minimized by the use of `automatic schema evolution` feature of ROOT.

REST is compiled by default using `automatic schema evolution` since version `2.2.1`.
Therefore, any new/future generation of data before this version is `not recommended` for compatibility reasons.

Any data produced with version `2.2.1`, and later, will be generated using `automatic schema evolution`, and therefore,
any future REST version `should be able to read older versions` without major issues or warnings.

----

REST versioning system will allow to stamp the data generated with REST in order to identify major changes in the code.

Therefore, allowing to identify when new features or important changes to the code were made.

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

XXX

