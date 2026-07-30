# Safe writable ROOT I/O

REST ROOT files may contain several historical `TStreamerInfo` entries and embedded schema rules. Opening such
a file directly with `TFile::Open(..., "UPDATE")` bypasses REST's schema preflight and can let ROOT rewrite
schema metadata before REST has established that every historical class layout is usable. Framework code that
creates or mutates ROOT files must therefore use `TRestRootFileHandle`.

## Opening files

Use `TRestRootFileHandle::Open` for new code:

```cpp
#include "TRestTools.h"

auto file = TRestRootFileHandle::Open(filename, TRestRootFileMode::Update);
if (!file) {
    ReportError(file.Error());
    return false;
}

file->cd();
WriteObjects();

if (!file.Close()) {
    ReportError(file.Error());
    return false;
}
```

The available modes are `Read`, `Recreate`, and `Update`. `Recreate` intentionally replaces an existing file
and must not be used as a shortcut for `Update`. An update is initially opened read-only. REST inventories the
exact class-name, class-version, and checksum tuples stored in the file, collects the embedded schema rules,
prepares loaded or emulated ROOT classes, and preflights the historical entries. It then transitions the same
`TFile` to update mode and verifies the local file identity and ROOT UUID before marking the exact historical
class-index entries required for writing.

`PrepareBorrowedUpdate(TFile&, std::string*)` provides the same update preparation when legacy code already
owns a `TFile`. The supplied file must be valid, open in `READ` mode, and not writable:

```cpp
std::unique_ptr<TFile> file(TFile::Open(filename.c_str(), "READ"));
std::string error;
if (!file || !TRestRootFileHandle::PrepareBorrowedUpdate(*file, &error)) {
    ReportError(error);
    return false;
}
```

Prefer `TRestRootFileHandle` whenever ownership can be changed. A borrowed file remains the caller's
responsibility, including checking its close/write status. If preparation fails, propagate the error and do not
attempt to write through that file.

The preflight preserves the historical StreamerInfo entries and schema rules it found on disk; it does not make
an incompatible class change or an incorrect schema rule valid. Class authors must still increment class
versions as required, write correct evolution rules, and test representative old files both before and after a
writable open.

## Ownership and error handling

`TRestRootFileHandle` is move-only. Pass it by reference while it remains owned by a component, or transfer it
with `std::move`. Pointers returned by `Get()` and `operator->` are non-owning and must not be deleted or retained
beyond the handle's lifetime.

The destructor closes an open file, but cannot report failure. Code that writes must call `Close()` explicitly
and handle a false result using `Error()`. Close a live destination before move-assigning another handle to it,
because move assignment cannot return a close error for the previous file.

## Replacing or merging files

Use `TRestTools::MergeRootFilesTransactionally` instead of merging directly into the destination or manually
renaming a partially written file:

```cpp
std::string error;
const std::string existing = outputAlreadyExists ? output : "";
if (!TRestTools::MergeRootFilesTransactionally(output, newInputs, existing, true, &error)) {
    ReportError(error);
    return false;
}
```

When non-empty, `existingTarget` is included as the first merge input. Callers updating an existing output must
pass it explicitly; otherwise its existing contents are not part of the merge.

The helper inventories every input, rejects incompatible classes at the same key path, and constructs the
result in a temporary sibling of the local destination. Before replacement it validates the expected
StreamerInfo and schema rules, recursive key paths and classes, and summed `TTree` entries. It validates the
installed file again and attempts to restore the previous destination from a rollback backup on failure.
Local input files are removed only after successful replacement and validation when
`removeInputsOnSuccess` is true.

A false return can also mean that the merged output is valid but a backup or input could not be removed.
Always inspect the returned error before deciding how to recover. Replacement uses platform filesystem
operations on sibling paths, but this is not a promise of power-loss durability or atomic behavior on every
mounted filesystem. Rollback can itself fail; preserve and report the detailed error, including any retained
backup path.

## Local and remote paths

Remote ROOT files may be read and may be merge inputs if the installed ROOT transports can open them. Writable
opens and transactional merge destinations must resolve to local paths; remote URLs are rejected before
mutation. Local `file://` URLs are accepted. Use `TRestTools::IsRemoteRootPath` when a caller needs to validate
or explain this policy before opening a file.

Only local entries in `inputFiles` are candidates for removal after a successful merge. Remote inputs are not
deleted by the helper.

## `TRestRun` is intentionally non-copyable

`TRestRun` owns live input and output handles and also holds raw aliases to file-owned objects. The previous
implicit copy would have shallow-copied that state, making ownership and lifetime unsafe. Its copy constructor
and copy assignment operator are therefore deleted.

APIs should pass runs as `TRestRun&`, `const TRestRun&`, or pointers rather than by value. Use
`std::unique_ptr<TRestRun>` when ownership of a run object itself must be transferred, or construct a separate
`TRestRun` from the appropriate filename/configuration when an independent instance is required. `TRestRun`
does not currently expose move construction or move assignment, so do not rely on `std::move` to transfer the
object directly.
