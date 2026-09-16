# Updating ROOT files from macros

**Review your existing macros as well as new ones.** If a macro adds histograms, writes a readout, changes
metadata, or otherwise modifies an existing ROOT file, use `TRestRootFileHandle` instead of a direct `TFile`
UPDATE open. This applies to personal macros outside the REST repositories too.

ROOT files can contain historical class descriptions (`StreamerInfo`) and schema-evolution rules needed to
read their data. A direct UPDATE open can allow ROOT to rewrite this metadata without REST's preservation
checks, even if your macro only adds a histogram. Not every UPDATE loses information, but successful execution
alone does not prove historical data remains readable.

This is a pre-existing risk, not a new incompatibility introduced by the handle. Updating REST protects code
that uses the checked interface; it does **not** automatically redirect direct ROOT calls in your macros.
Macros that only read files do not need this migration for schema preservation.

## What to look for

Look for `TFile::Open(..., "UPDATE")`, stack/heap `TFile` constructors using UPDATE (including lowercase
`"update"`), and calls to `ReOpen("UPDATE")`. Check Python/PyROOT macros and helper functions too. For example:

```sh
rg -n -i 'update|reopen' --glob '*.{C,cxx,cpp,h,py}' path/to/your/macros
```

This is only a starting point: inspect mode variables and wrapper functions manually. A search or CI check is
not proof that every writable open is safe, and repository CI cannot inspect private macros on your machine.

## Replace the open, preserve the write, check the close

The old pattern bypasses REST's preflight:

```cpp
TFile* file = TFile::Open(filename, "UPDATE");
// Write histograms, metadata, etc.
file->Close();
delete file;
```

Use a REST build that provides `TRestRootFileHandle`, with REST loaded in your macro environment. This complete
example adds a small metadata note; use the same structure around your own histogram or metadata writes:

```cpp
#include <TFile.h>
#include <TNamed.h>
#include <iostream>

#include "TRestTools.h"

bool AddAnalysisNote(const char* filename) {
    auto file = TRestRootFileHandle::Open(filename, TRestRootFileMode::Update);
    if (!file) {
        std::cerr << file.Error() << '\n';
        return false;
    }

    file->cd();
    TNamed note("analysisNote", "Updated with checked REST ROOT I/O");
    const bool written = note.Write() > 0;
    const bool closed = file.Close();
    if (!written) std::cerr << "Could not write analysisNote\n";
    if (!closed) std::cerr << file.Error() << '\n';
    return written && closed;
}
```

The handle owns the file and closes it automatically on destruction, but writers must explicitly check
`Close()` to report errors. Pass `file.Get()` to APIs expecting a `TFile*`; that pointer is borrowed. Do not
delete it or use it, or file-owned objects, after the handle closes. Do not retain the old `delete file` line.

Like ROOT UPDATE, `Update` creates a missing local file. If your macro requires an existing input, keep that
existence check. `Recreate` intentionally replaces existing contents: never substitute it for an UPDATE that
failed. Writable destinations must be local; remote reads remain subject to ROOT's available transports.

## If the update is refused

Report the error and stop; do not fall back to direct ROOT UPDATE or change the mode to RECREATE. Consult the
error before deciding whether a compatible dictionary, corrected schema rule, or separate legacy recovery is
needed. The handle preserves usable schema information already present; it does not reconstruct missing
StreamerInfo or repair incompatible evolution rules.

Keep a backup before modifying valuable data. Checked UPDATE is not a rollback transaction for your entire
macro: failed writes may leave partial changes, and it does not provide concurrent-writer locking or power-loss
durability. To work on a remote file, explicitly create a local copy and update that copy.

For existing code that must retain ownership of a READ-mode `TFile`, see `PrepareBorrowedUpdate` in the
[developer guide](../developer/Safe%20writable%20ROOT%20IO.md). The same guide covers transactional merging and
the intentionally non-copyable `TRestRun` interface.
