
Minimal script to generate the readouts file:

```
TRestReadout *r1 = new TRestReadout("readouts.rml", "readout" );
TRestReadout *r2 = new TRestReadout("readouts.rml", "cathode" );

TFile *f = new TFile( "readouts.root", "RECREATE" );
r1->Write("readout");
r2->Write("cathode");

f->Close();
```


Minimal script to visualize the readout:


```
restRootMacros
REST_Detector_ViewReadout("readouts.root", "cathode")
```

![readout][readout.png]
