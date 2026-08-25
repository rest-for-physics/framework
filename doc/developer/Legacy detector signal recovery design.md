# Legacy detector signal recovery design

The recovery command is intentionally a worked solution for one known bad schema transition. It is
not infrastructure for arbitrary migrations. Its two stages are separated by a process boundary
because ROOT cannot safely load replica legacy classes and the current REST classes with the same
C++ names in one process.

## Architecture

1. `recoverLegacySignalData.C` runs under plain ROOT with small replicas of
   `TRestEvent`, `TRestDetectorSignal` v3, and `TRestDetectorSignalEvent`. It accepts only the
   exact event branch and signal class versions 1 through 3 and writes a private, flattened
   intermediate containing floats plus source UUID, file size, and counts.
2. `restRoot` then loads the modern REST libraries, verifies that the intermediate identifies the
   same source, and makes a byte-for-byte candidate copy beside the requested output.
3. The candidate is opened through `TRestRootFileHandle::Open(Update)`, so the safe writable-I/O
   rules in [Safe writable ROOT IO](Safe%20writable%20ROOT%20IO.md) apply. The incompatible
   top-level signal branch is disabled and `TTree::CloneTree(-1, "fast")` copies all other active
   branches as baskets. Only the modern signal branch is filled from the intermediate.
4. Validation compares the unaffected recursive branch inventory, unrelated key inventory, source
   identity and counts, and every reconstructed signal value. The candidate is atomically linked to
   a previously absent output name only after these checks pass.

The input and intermediate are local files. The private work directory is created on the output
filesystem so publication does not cross filesystems. There is no in-place mode, partial mode,
rollback protocol, arbitrary object copier, or user-visible intermediate format.

## ROOT 6.26 fast-clone boundary

Experiments with ROOT 6.26/10 established the useful boundary:

| Process state | Operation on the canonical file | Result |
| --- | --- | --- |
| Plain ROOT; REST event dictionaries absent | Disable the signal branch, then `CloneTree(-1, "fast")` | `TBranchElement::InitInfo` errors for other event classes followed by exit signal 11 |
| Modern REST libraries loaded | Same operation | 261 entries cloned; signal branch absent |
| Modern REST libraries loaded; fixture includes an unavailable unsplit event class | Same operation | succeeds and preserves that branch's compressed basket payloads exactly |

Thus plain ROOT is used only to deserialize the affected legacy branch. Fast cloning happens after
REST libraries are loaded. ROOT can raw-copy an unavailable branch in this setup, but it cannot
construct the complete canonical `TTree` in a process where nearly every event dictionary is
missing.

Other approaches were rejected:

- `TTree::CopyTree` runs an entry loop and therefore deserializes branches rather than preserving
  them opaquely.
- Reading keys with `TKey::ReadObj` deserializes arbitrary metadata.
- Manually rewriting `TKey`, `TBasket`, and `TTree` offsets would duplicate ROOT internals and be
  substantially larger and more fragile than the supported fast-clone path.
- Physically editing `TTree` branch and leaf arrays can work only if all related arrays remain
  internally consistent; it is unnecessary when top-level branch status is used.

For a future one-off migration, retain the same safety shape: exact schema recognition, isolated
legacy extraction, supported raw cloning for unaffected data, construction only of the intended new
branch, source/output separation, and focused value and structure validation. Do not generalize this
command by adding recovery modes; add a separately reviewed migration for a separately understood
transition.
