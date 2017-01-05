#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

# This script can be used to compile all the tags and install under a given directory REST_Install_Path
# All the existing tags will be installed under REST_Install_Path/tagName
# It will also update REST_Install_Path/master with the latest update of master branch
#
# J. Galan - Javier.Galan.Lacarra@cern.ch
# 23 - Dec - 2016

import os,sys, time, commands
import subprocess, StringIO

#===================================================
#================ Must be changed ==================
#===================================================
# Modify this PATH with your REST_v2 build directory
REST_Build_Path = "/home/jgalan/git/REST_v2/build"

# Modify this PATH with the final destination of REST installation
REST_Install_Path = "/programas/REST/"
#===================================================

os.chdir( REST_Build_Path )

# Updating master
os.system ('/usr/bin/git checkout master' )

p = subprocess.Popen(['/usr/bin/git', 'pull', '--tags'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

out, err = p.communicate()

if err == "" and "up-to-date" in out:
	print "Master is already updated"
else:
	os.system( "cmake -DINSTALL_PREFIX=" + REST_Install_Path + "master" + " ../" )
	os.system ('/usr/bin/make clean' )
	os.system ('/usr/bin/make -j4' )
	os.system ('/usr/bin/make install' )


# Installing new tags

p = subprocess.Popen(['/usr/bin/git', 'tag'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

out, err = p.communicate()

buf = StringIO.StringIO( out )

gitTags = []
length = 1
while length > 0:
	tag = buf.readline().rstrip()
	length = len( tag )
	if length > 0:
		gitTags.append( tag )

p = subprocess.Popen(['ls', REST_Install_Path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

out, err = p.communicate()

buf = StringIO.StringIO( out )

installedTags = []

length = 1
while length > 0:
	tag = buf.readline().rstrip()
	length = len( tag )
	if length > 0 and tag.find("master"):
		installedTags.append( tag )

notInstalledTags = []

for x in gitTags:
	alreadyInstalled = 0
	for y in installedTags:
		if x == y:
			alreadyInstalled = 1

 	if x.find("v2.0.0") == 0:
		alreadyInstalled = 1
		
	if not alreadyInstalled:
		notInstalledTags.append( x )

for tag in notInstalledTags:
	os.system( "git checkout -b " + tag + " " + tag )
	os.system( "cmake -DINSTALL_PREFIX=" + REST_Install_Path + tag + " ../" )
	os.system ('/usr/bin/make clean' )
	os.system ('/usr/bin/make -j4' )
	os.system ('/usr/bin/make install' )
	os.system ('git checkout master' )

