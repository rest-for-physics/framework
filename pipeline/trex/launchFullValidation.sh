restManager --c 01_raw.rml --f *.aqs
restManager --c 02_signal.rml --f RawData.root
restManager --c 03_hits.rml --f Signals.root
restRoot -b -q ../MakeBasicTree.C'("Hits.root")'
rm RawData.root
rm Signals.root
root -b -q ../ValidateTrees.C'("validation.root")'
