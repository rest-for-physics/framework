import os,sys

os.system("restRoot -b -q -m 1 > error.log 2>&1")

with open('error.log') as f:
    lines = f.readlines()

for x in lines:
    if( x.find("warning:") >= 0 ):
        sys.exit(1)
    if( x.find("error:") >= 0 ):
        sys.exit(1)
print("No warning found when loading macros")
print("Succeed")
sys.exit(0)
