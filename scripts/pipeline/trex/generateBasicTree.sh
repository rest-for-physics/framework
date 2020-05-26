restManager --c 01_raw.rml --f data.aqs
restManager --c 02_signal.rml --f RawData.root
restManager --c 03_hits.rml --f Signals.root
restRoot -b -q MakeBasicTree.C
rm RawData.root
rm Signals.root
rm Hits.root
