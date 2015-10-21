#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

# This script generates the plots from the spectras
# J. Galan - Javier.Galan.Lacarra@cern.ch
# 18 - May - 2007

import os,sys, time, commands

calrun = 0
bckrun = 0
sleep = 0
repeat = 1

narg = len(sys.argv)
queue = ""
inputfile = ""
taskName = ""

for x in range(narg-1):
    if ( sys.argv[x+1] == "--input" ):
	inputfile = sys.argv[x+2]
    if ( sys.argv[x+1] == "--queue" ):
	queue = sys.argv[x+2]
    if ( sys.argv[x+1] == "--task" ):
	taskName = sys.argv[x+2]
    if ( sys.argv[x+1] == "--sleep" ):
	sleep = int(sys.argv[x+2])
    if ( sys.argv[x+1] == "--repeat" ):
	repeat = int(sys.argv[x+2])

if queue == "":
	print "Please specify a queue name : --queue QUEUE [1nh, 2nh, 1nd, 2nd, 1nw, 2nw]" 
	sys.exit(0)

if taskName == "":
	print "Please specify a task name : --task NAME"
	sys.exit(0)

if inputfile == "":
	print "Please specify an input file with commands to execute in lxbatch --input FILE"
	sys.exit(0)

f = open(inputfile,'r')
lines = []
for line in f.readlines():
    if line[0] != "#":
	lines.append( line )
f.close()


cont = 0
for command in lines:
	
    rpt = repeat
    while ( rpt > 0 ):
	f = open("/tmp/suehrjksui", "w")
	f.write("cd " + os.getenv("REST_PATH") + "/restG4-build" "\n")
	f.write( "export LD_LIBRARY_PATH=\"/afs/cern.ch/sw/lcg/contrib/gcc/4.8.4/x86_64-slc6-gcc48-opt/lib64:$LD_LIBRARY_PATH\"\n")
	f.write( command.rstrip() + " " + str(rpt-1) + "\n" )
	f.close()
	cont = cont + 1
	bsubcommand = "bsub -q " + queue + " -J " + taskName + "_" + str(cont) + " < /tmp/suehrjksui"
	print "Launching : " + command
	time.sleep(sleep)
	print commands.getstatusoutput( bsubcommand )
    	rpt = rpt - 1


