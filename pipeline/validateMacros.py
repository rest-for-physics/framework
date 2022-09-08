#!/usr/bin/python3

import os,sys

with open('error.log') as f:
    lines = f.readlines()

for x in lines:
    if( x.find("warning") >= 0 ):
        sys.exit(1)
print("No warning found when loading macros")
print("Succeed")
sys.exit(0)
