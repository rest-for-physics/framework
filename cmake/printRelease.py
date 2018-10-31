#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

# This script generates the version header to be used in REST installation
# J. Galan - Javier.Galan.Lacarra@cern.ch
# 1 - Nov - 2018

import os,re

tag = os.popen( 'git describe --tags $(git rev-list --tags --max-count=1)' ).read().rstrip("\n")

first =  tag.find(".")
last = tag.rfind(".")

a = int( re.sub("[^0-9]", "", tag[0:first] ) )
b = int( re.sub("[^0-9]", "", tag[first+1:last] ) )
c = int( re.sub("[^0-9]", "", tag[last+1:] ) )

restRelease = str(a) + "." + str(b) + "." + str(c)

print restRelease
