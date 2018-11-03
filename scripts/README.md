# REST Scripts

here contains several python scripts for different usage

### REST GUI installer

Type `python scriptsGUI.py` and just follow the wizard!  
You must run the python scripts in this dicectory!  

Required package: python2.7, python-tk. They can be installed by following:

**in centos:**  
`yum update python`  
`yum install tkinter`  

**in debain:**  
`apt-get update python`  
`apt-get install python-tk`  

>REST required package: ROOT6  
>Optional packages: geant4, garfield(compiled with ROOT6), postgresql  

### rml updating tool for V2.1

**Usage**: `./restv2_1ToXml.py rmlInFile rmlOutFile {restG4,processes,readouts,restManager,gases}`

>the last argument means the root section name, which must be manually specified
>python2.7 is required

**Programme**:
1. Remove the "section" keyword in front of "TRestXXX" tags.
2. Cancel the `<environment>` section and move its contents into the `<globals>` section.
3. Avoid same attribute multiple times in a tag.
4. Close self closing tags with "/>" instead of ">"
5. Remove '<' and '>' from attribute values.
6. Leave "<for" opening tag open.
7. Add a root node.

### cluster job launching tool

>Last update : 3-Nov-2018  

>Visit our user guide [here](https://pandax.physics.sjtu.edu.cn/docdb3/ShowDocument?docid=561)  