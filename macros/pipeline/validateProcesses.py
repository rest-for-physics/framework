#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

# This script generates the version header to be used in REST installation
# J. Galan - Javier.Galan.Lacarra@cern.ch
# 23 - Dec - 2019

#from __future__ import print_function

import os,sys,re,filecmp
import subprocess


def validateProcess( className ):
    print ""
    print "++++ Validating process : " + className
    with open(className, 'r') as file:
        data = file.read()

        data = data[data.find( "ProcessEvent" ):]

        data = getMethodDefinition( data )
        nullPos = data.rfind("RETURN NULL")
        if nullPos != -1:
            lastObsValue = data.rfind( "SETOBSERVABLEVALUE" )
            if lastObsValue > nullPos:
                print " - return NULL declared before last SetObservableValue!!"
                print " - FILE : " + className
                sys.exit(1)
    print "OK"
    return;

def getMethodDefinition( text ):
    initPos = text.find("{")

    counter = 1
    start = initPos+1
    while( counter > 0 ):
        pos1 = text.find("{",start)
        pos2 = text.find("}",start)

        endPosition = pos2+1

        if( pos1 != -1 and pos2 != -1 ):
            if( pos1 < pos2 ):
                counter = counter + 1
                start = pos1 + 1
            if( pos2 < pos1 ):
                counter = counter - 1
                start = pos2 + 1
        elif pos1 != -1:
                print "Big error!!"
        else:
                counter = counter - 1
                start = pos2+1

    return text[initPos:endPosition].upper()

files = []

# r=root, d=directories, f = files
for r, d, f in os.walk(sys.argv[1]):
        for file in f:
                    if 'Process.cxx' in file:
 #                                   files.append(os.path.join(r, file))
                                    validateProcess( os.path.join(r, file) )

#for f in files:
#    print(f)

#validateProcess(sys.argv[1]);

sys.exit(0)
