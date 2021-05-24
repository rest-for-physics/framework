#!/usr/bin/python3

import ROOT

ROOT.gSystem.Load("libRestFramework.so")
ROOT.gSystem.Load("libRestRaw.so")

rawEv = ROOT.TRestRawSignalEvent()

sgnl1 = ROOT.TRestRawSignal()
sgnl1.SetID(4688)
# id = sgnl1.GetID();
# print ("\nsignl1 ID: %i" % id)
sgnl2 = ROOT.TRestRawSignal()
sgnl2.SetID(4705)
sgnl3 = ROOT.TRestRawSignal()
sgnl3.SetID(4676)
sgnl4 = ROOT.TRestRawSignal()
sgnl4.SetID(4)

for x in range(0,512):
    sgnl1.AddPoint(0)
    sgnl2.AddPoint(0)
    sgnl3.AddPoint(0)
    sgnl4.AddPoint(0)

rawEv.AddSignal(sgnl1)
rawEv.AddSignal(sgnl2)
rawEv.AddSignal(sgnl3)
rawEv.AddSignal(sgnl4)

# addVetoProcess = ROOT.TRestRawVetoAnalysisProcess("veto.rml")
addVetoProcess = ROOT.TRestRawVetoAnalysisProcess("processes.rml")

outEv = addVetoProcess.ProcessEvent( rawEv )

print ("\nChecking if observable was created and veto signal was removed")
for i in [1,2,3]:
# for i in [4688.4705,4676]:
    if outEv.GetSignalIndex(i) != -1:
        print ("\nVeto failed: veto signal still present!")
        exit(202)
if outEv.GetSignalIndex(4) == -1:
    print("\nVeto failed: non veto signal removed!")
    exit(202)


print ("\nChecking if veto observables were created")

rootfile = ROOT.TFile.Open("R01208_output.root","READ")
tree = rootfile.Get("AnalysisTree")


if tree.ObservableExists("veto_PeakTime_top") == False:
    print ("\nVeto failed: observable not created!")
    exit(202)

if tree.ObservableExists("veto_MaxPeakAmplitude_back") == False:
    print ("\nVeto failed: observable not created!")
    exit(202)
        
print ("[\033[92m OK \x1b[0m]")

exit(0)





# outEv.GetSignal(0).CalculateBaseLine(50,450)
# baseLineSigma = outEv.GetSignal(0).GetBaseLineSigma()

# print ("\nEvaluating baseline sigma at the output event")
# if baseLineSigma < 15 or baseLineSigma > 25:
#     print ("\nEvaluation failed! Sigma is outside (15,25) range!" )
#     exit(202)
# print ("[\033[92m OK \x1b[0m]")

# exit(0)
