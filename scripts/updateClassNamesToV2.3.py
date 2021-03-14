#!/usr/bin/python3.5
# -*- coding: iso-8859-15 -*-

# This script renames any class found from v2.2.X to v2.3.X
# J. Galan - javier.galan@unizar.es
# 19 - Dec - 2020

import os,sys
import fileinput
import glob

narg = len(sys.argv)
pathToUpdate = ""

print 

if( narg <= 2):
    print( "\n" )
    print( "Please provide a PATH and an EXTENSION filename)" )
    print( "Usage: python3.5 updateRMLsToV2.3.py RELATIVE_ROOT_PATH EXTENSION[rml,md,...]\n" )
    sys.exit(1)

pathToUpdate = sys.argv[1]
extension = sys.argv[2]

print ( "Path: " + pathToUpdate )
print ( "Extension: " + extension )
allRMLs = glob.glob(pathToUpdate+'/**/*'+extension, recursive = True)

f=open(os.getenv("REST_SOURCE") + "miscellaneous/renamed.classes","r")
lines=f.readlines()
result=[]
for x in lines:
        if( x.find("TRest") >= 0 and len(x.rstrip().split(' ')) == 2 ):
            result.append(x.rstrip().split(' '))
f.close()

for fileToSearch in allRMLs:
    print ( "Replacing in file : " + fileToSearch )
    f = open( fileToSearch, 'r')
    file_content = f.read() # Read whole file in the file_content string
    f.close()

    for restClass in result:
        textToSearch = restClass[0]
        textToReplace = restClass[1]
        # print ( "Replacing " + restClass[0] + " by " + restClass[1] )
        file_content = file_content.replace( textToSearch, textToReplace )

    f = open( fileToSearch, 'w')
    f.write( file_content )
    f.close()
