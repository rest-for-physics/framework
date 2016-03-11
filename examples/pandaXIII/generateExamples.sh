#!/bin/bash

## File : generateExamples.rml
## Description : Used to generate configuration files from a prototyped configuration template
## Date : 11-Mar-2016
## Authors : H. Liang/ J. Galan

# Isotopes 
Bi214='REST_ISOTOPE=Bi214;REST_FULLCHAIN=off'
Tl208='REST_ISOTOPE=Tl208;REST_FULLCHAIN=off'
Xe137='REST_ISOTOPE=Xe137;REST_FULLCHAIN=off'
Co60='REST_ISOTOPE=Co60;REST_FULLCHAIN=off'
XeNLDBD='REST_GENERATOR=Xe136bb0n'
Xe2NDBD='REST_GENERATOR=Xe136bb2n'
Th232='REST_ISOTOPE=Th232;REST_FULLCHAIN=off'
Ra228='REST_ISOTOPE=Ra228;REST_FULLCHAIN=on'
U238='REST_ISOTOPE=U238;REST_FULLCHAIN=off'
Th234='REST_ISOTOPE=Th234;REST_FULLCHAIN=on'



GEO='REST_GEOMETRY_TYPE=traditional;REST_GEOMETRY_SETUP=pandaXIII_Setup_WT12m.gdml'

 ######################       GAS        ###########################
PARAMS='REST_EMIN=0;REST_EMAX=10;REST_MAXSTEPSIZE=100'
NEVENTS=100000

mkdir gas -p
 ./../replaceRMLVars.py templates/isotopeFromVolume.rml gas/Xe137.rml \
 "[REST_VOLUME=gas;$Xe137;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml gas/Co60.rml \
 "[REST_VOLUME=gas;$Co60;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml gas/Bi214.rml \
 "[REST_VOLUME=gas;$Bi214;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/decay0FromVolume.rml gas/Xe136bb0n.rml \
 "[REST_VOLUME=gas;$XeNLDBD;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/decay0FromVolume.rml gas/Xe136bb2n.rml \
 "[REST_VOLUME=gas;$Xe2NDBD;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"



 ######################       VESSEL        ###########################

PARAMS='REST_EMIN=0;REST_EMAX=50;REST_MAXSTEPSIZE=100'
NEVENTS=100000

mkdir vessel -p
 ./../replaceRMLVars.py templates/isotopeFromVolume.rml vessel/Co60.rml \
 "[REST_VOLUME=vessel;$Co60;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml vessel/Th232.rml \
 "[REST_VOLUME=vessel;$Th232;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml vessel/Ra228.rml \
 "[REST_VOLUME=vessel;$Ra228;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml vessel/U238.rml \
 "[REST_VOLUME=vessel;$U238;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml vessel/Th234.rml \
 "[REST_VOLUME=vessel;$Th234;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml vessel/Bi214.rml \
 "[REST_VOLUME=vessel;$Bi214;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml vessel/Tl208.rml \
 "[REST_VOLUME=vessel;$Tl208;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"


 ######################       MICROMEGAS        ###########################

PARAMS='REST_EMIN=0;REST_EMAX=50;REST_MAXSTEPSIZE=100'
NEVENTS=100000

VOLUME='micromegas1'

mkdir $VOLUME -p

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Co60.rml \
 "[REST_VOLUME=$VOLUME;$Co60;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Th232.rml \
 "[REST_VOLUME=$VOLUME;$Th232;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Ra228.rml \
 "[REST_VOLUME=$VOLUME;$Ra228;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/U238.rml \
 "[REST_VOLUME=$VOLUME;$U238;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Th234.rml \
 "[REST_VOLUME=$VOLUME;$Th234;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Bi214.rml \
 "[REST_VOLUME=$VOLUME;$Bi214;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Tl208.rml \
 "[REST_VOLUME=$VOLUME;$Tl208;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"


 ######################       RINGS        ###########################

PARAMS='REST_EMIN=0;REST_EMAX=50;REST_MAXSTEPSIZE=100'
NEVENTS=100000

VOLUME='rings'

mkdir $VOLUME -p

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Co60.rml \
 "[REST_VOLUME=$VOLUME;$Co60;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Th232.rml \
 "[REST_VOLUME=$VOLUME;$Th232;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Ra228.rml \
 "[REST_VOLUME=$VOLUME;$Ra228;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/U238.rml \
 "[REST_VOLUME=$VOLUME;$U238;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Th234.rml \
 "[REST_VOLUME=$VOLUME;$Th234;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Bi214.rml \
 "[REST_VOLUME=$VOLUME;$Bi214;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Tl208.rml \
 "[REST_VOLUME=$VOLUME;$Tl208;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ######################       SUPPORTS        ###########################

PARAMS='REST_EMIN=0;REST_EMAX=50;REST_MAXSTEPSIZE=100'
NEVENTS=100000

VOLUME='supports'

mkdir $VOLUME -p

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Co60.rml \
 "[REST_VOLUME=$VOLUME;$Co60;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Th232.rml \
 "[REST_VOLUME=$VOLUME;$Th232;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Ra228.rml \
 "[REST_VOLUME=$VOLUME;$Ra228;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/U238.rml \
 "[REST_VOLUME=$VOLUME;$U238;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Th234.rml \
 "[REST_VOLUME=$VOLUME;$Th234;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Bi214.rml \
 "[REST_VOLUME=$VOLUME;$Bi214;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Tl208.rml \
 "[REST_VOLUME=$VOLUME;$Tl208;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"


 ######################       ELECTRONICS        ###########################

PARAMS='REST_EMIN=0;REST_EMAX=50;REST_MAXSTEPSIZE=100'
NEVENTS=100000

VOLUME='electronics'

mkdir $VOLUME -p

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Co60.rml \
 "[REST_VOLUME=$VOLUME;$Co60;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Th232.rml \
 "[REST_VOLUME=$VOLUME;$Th232;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Ra228.rml \
 "[REST_VOLUME=$VOLUME;$Ra228;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/U238.rml \
 "[REST_VOLUME=$VOLUME;$U238;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Th234.rml \
 "[REST_VOLUME=$VOLUME;$Th234;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Bi214.rml \
 "[REST_VOLUME=$VOLUME;$Bi214;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVolume.rml $VOLUME/Tl208.rml \
 "[REST_VOLUME=$VOLUME;$Tl208;$GEO;$PARAMS;REST_NEVENTS=$NEVENTS]"


 ######################       RESISTIVE FIELD CAGE      ###########################

GEO='REST_GEOMETRY_TYPE=resistive;REST_GEOMETRY_SETUP=pandaXIII_Setup.gdml'
VCYL='REST_LENGTH=2;REST_RADIUS=730]'

NEVENTS=100000
PARAMS='REST_EMIN=0;REST_EMAX=10;REST_MAXSTEPSIZE=100'

VOLUME='resistive'

mkdir $VOLUME -p

 ./../replaceRMLVars.py templates/isotopeFromVCylinder.rml $VOLUME/Bi214.rml \
 "[$Bi214;$GEO;$VCYL;$PARAMS;REST_NEVENTS=$NEVENTS]"

 ./../replaceRMLVars.py templates/isotopeFromVCylinder.rml $VOLUME/Tl208.rml \
 "[$Tl208;$GEO;$VCYL;$PARAMS;REST_NEVENTS=$NEVENTS]"

