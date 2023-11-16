import os, sys

os.system("restRoot -b -q --m 1 > error.log 2>&1")

with open("error.log") as f:
    lines = f.readlines()

for line in lines:
    if line.find("warning") >= 0 or line.find("Warning") >= 0:
        print("Found warnings when launching restRoot!")
        print(lines)
        sys.exit(1)
    if line.find("error") >= 0 or line.find("Error") >= 0:
        print("Found error messages when launching restRoot!")
        print(lines)
        sys.exit(2)
    if line.find("undefined symbol") >= 0:
        print("Found undefined symbol messages when launching restRoot!")
        print(lines)
        sys.exit(3)

print("No warning found when loading macros")
print("Succeed")

sys.exit(0)
