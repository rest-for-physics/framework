# Recovering legacy detector signal files

REST files written with `TRestDetectorSignal` schema versions 1 through 3 store signal times and
charges as `Float_t`. Modern detectorlib uses `Double_t` (schema version 4). Files from the legacy
period that do not contain usable ROOT `StreamerInfo` cannot be opened safely with the modern
class. They can be converted once with:

```console
restRoot --recover-legacy-signals legacy.root
```

`TRestRun` refuses to open an affected file before reading its metadata or events and prints this
command. It does not offer partial access because accidentally reading the incompatible signal
branch can cause excessive allocation or a crash. Legacy files that contain the required signal
`StreamerInfo` continue through ROOT's normal schema evolution without requiring recovery.

The default output is a new sibling named `legacy_Fixed.root`. Select another new local path with:

```console
restRoot --recover-legacy-signals legacy.root --output recovered.root
```

The command never modifies the input and never replaces an existing output. It first extracts only
the affected branch in an isolated plain-ROOT process. It then starts from a byte-for-byte copy of
the input, fast-clones every unaffected `EventTree` branch as ROOT baskets, and rebuilds only
`TRestDetectorSignalEventBranch` with the modern class. Unrelated top-level objects are left in
place without being deserialized. The output is published only after its structure and all recovered
signal values have been checked.

This is deliberately a recovery for the known `vector<Float_t>` to `vector<Double_t>` transition,
not a generic schema migration tool. Inputs with another signal schema, malformed signal contents,
ROOT URLs, in-place output, and existing output files are rejected. A failed command leaves the
original untouched and produces no output file.

The recovered file contains a `REST_LegacySignalRecovery` provenance key with the source UUID,
legacy signal schema version, and recovered entry, signal, and point counts.
