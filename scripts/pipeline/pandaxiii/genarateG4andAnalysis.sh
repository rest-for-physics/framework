restG4 Xe136bb2n.rml
restRoot -b -q ValidateG4.C'("Xe136bb2n_n2E06.root")'
restManager --c AllProcesses.rml --i Xe136bb2n_n2E06.root --o Xe136bb2n_Process.root --e 1000 --j 1
restRoot -b -q ValidateTrack.C'("Xe136bb2n_Process.root")'
restManager --c plots.rml --i Xe136bb2n_Process.root
restRoot -b -q ValidatePlot.C'("trackParameter.png")'