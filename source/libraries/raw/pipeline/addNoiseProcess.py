#!/usr/bin/python

import ROOT
from ROOT import TVector3

a = TVector3(1, 2, 3)
b = TVector3(4, 5, 6)

c = b - a


ROOT.gSystem.Load("libGdml.so")
ROOT.gSystem.Load("libRestCore.so")
ROOT.gSystem.Load("libRestEvents.so")
ROOT.gSystem.Load("libRestMetadata.so")
ROOT.gSystem.Load("libRestRaw.so")

babyField = ROOT.TRestAxionMagneticField("fields.rml", "babyIAXO")

if babyField.GetError():
    print babyField.GetErrorMessage()
    print "\nMagnetic field initialization failed! Exit code : 101"
    exit(101)

p1 = ROOT.TVector3(0,0,0)
p2 = ROOT.TVector3(0,0,-2500)
p3 = ROOT.TVector3(0,0,2500)
p4 = ROOT.TVector3(0,0,4500)

d1 = ROOT.TVector3(0,0,1)
d2 = ROOT.TVector3(1,1,1)
d3 = ROOT.TVector3(0.5,0.5,1)
d4 = ROOT.TVector3(1,1,0.5)

b1 = int( 1000 * babyField.GetTransversalComponent( p1, d1))
b2 = int( 1000 * babyField.GetTransversalComponent( p2, d1))
b3 = int( 1000 * babyField.GetTransversalComponent( p3, d1))
b4 = int( 1000 * babyField.GetTransversalComponent( p4, d1))

print "\nEvaluating field volume Bykovskiy_201906.dat centered at (0,0,0)",
if( b1 != 2007 or b2 != 1998 or b3 != 1998 or b4 != 1119 ):
    print "\nEvaluation of field failed! Exit code : 102"
    exit(102)
print "[\033[92m OK \x1b[0m]"

castField = ROOT.TRestAxionMagneticField("fields.rml", "CAST")

if castField.GetError():
    print castField.GetErrorMessage()
    print "\nMagnetic field initialization failed! Exit code : 201"
    exit(201)


b1 = int( 1000 * castField.GetTransversalComponent( p1, d1))
b2 = int( 1000 * castField.GetTransversalComponent( p2, d2))
b3 = int( 1000 * castField.GetTransversalComponent( p3, d3))
b4 = int( 1000 * castField.GetTransversalComponent( p4, d4))

print "\nEvaluating CAST magnet constant field defintion centered at (0,0,0)",
if( b1 != 8900 or b2 != 7266 or b3 != 8124 or b4 != 6633 ):
    print "\nEvaluation of field failed! Exit code : 202"
    exit(202)
print "[\033[92m OK \x1b[0m]"

boundaries = castField.GetFieldBoundaries(0, ROOT.TVector3(12,34,0), ROOT.TVector3(0,-1,0) )

yMax = int( 1000 * boundaries[0].Y() )
yMin = int( 1000 * boundaries[1].Y() )

print "\nEvaluating CAST cylindrical volume boundaries",
if( yMax != 17839 or yMin != -17839 ):
    print "\nEvaluation of field failed! Exit code : 203"
    exit(203)
print "[\033[92m OK \x1b[0m]"

print ""
print "All tests passed!"

exit(0)
