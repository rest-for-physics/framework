import os, sys

os.system(
    "restRoot -b -q StreamerOutput.C'(\"Hits_01928.root\")' | grep Process | grep TRest | grep version"
)

os.system(
    "restRoot -b -q StreamerOutput.C'(\"Hits_01928.root\")' | grep Process | grep TRest | grep version | wc -l > output.log 2>&1"
)

with open("output.log") as f:
    lines = f.readlines()

for line in lines:
    if line.find("9") == 0:
        print("The number of processes inside the event data chain is 9. Succeed!")
        sys.exit(0)
    else:
        print("The number of processes inside the event data chain is NOT 9! Fail!")
        print("The number of processes is: " + line + "\n")
        sys.exit(1)

sys.exit(0)
