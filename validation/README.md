# PR #567 compact implementation experiment

Baseline: framework PR #567 at `6077128fe9edd582d6fa3dcfac5ff07d45152df7`.
Worktree: `/home/basti/src/rest_framework_pr567_reimplementation`.
Branch: `experiment/pr567-reimplementation`. Nothing has been pushed or merged.
Implementation commit: `c6bdebfd` (`Simplify writable ROOT I/O transactions and
retain schema checks`). The subsequent evidence commit is separate so the
implementation can be reviewed/cherry-picked without this experiment's tooling.
These local commits are unsigned because the reboot left the configured SSH
signing key unavailable; no persistent Git signing configuration was changed.

This is a smaller implementation of the same public interface and safety policy.
The public enum/RAII handle, legacy `TRestRun::OpenInputFile` string overload,
non-copyable run ownership, migrated callers, and previous tests are retained.
The experiment concentrates on the schema/update/transaction implementation in
`source/framework/tools/src/TRestTools.cxx`.

## What changed and why

1. `ReadSchemaSnapshot` can inventory and resolve one on-disk StreamerInfo list.
   It records the original name/version/checksum before ROOT's `BuildCheck`
   mutates the descriptors, collects registry numbers in a set, and follows
   ROOT's ownership transfer. This removes a second read of the same record and
   separate maximum-index/resolution state. Inventory-only validation still
   inspects the actual file rather than trusting ROOT's global cache.
2. Embedded schema-rule registration has a single per-rule path: recognize an
   existing normalized rule, register it for a loaded/emulated class, and verify
   that ROOT actually retained it. Historical user descriptors and rules remain
   mandatory; only the previous explicit STL/`__pair_base` normalization policy
   is exempt from exact identity comparison.
3. Private update/merge control flow uses `Require` and standard exceptions to
   unwind ROOT owners before removing an incomplete candidate. The public API
   still returns a boolean/invalid handle plus an error string. Filesystem
   exceptions also take this cleanup path. Both the candidate and installed
   result use the same schema/key/class/tree-count validator; installed-result
   validation exceptions trigger rollback.
4. Local identity is captured before schema preflight and compared after
   `ReOpen`. The previous UUID comparison read the same cached `TFile` UUID
   twice and did not independently establish disk identity, so it was removed.
5. ROOT local-path expansion is honored for filesystem checks. Transaction
   destinations and cleanup aliases are canonicalized, preserving a symlink
   destination and preventing input cleanup from deleting the installed output.

The implementation also incorporates all three confirmed findings from the
independent review at
`/home/basti/src/rest_framework_pr567_review/review/README.md`:

- A same-version incompatible user dictionary can make `BuildCheck` resolve to
  the wrong cached schema. The resolved descriptor is now compared with the
  original before writable transition or merge deserialization. ROOT's negative
  internal class version is compared as its serialized absolute version; this
  matters for `ROOT::TIOFeatures` and is not a general exemption for ROOT classes.
- Input cleanup through `dir/./target.root` could delete a successful output.
  Alias resolution now excludes the installed destination from removal.
- The baseline READ-first Update lost ROOT's create-if-absent behavior, breaking
  initial gain-map exports. Missing local files use CREATE, which refuses to
  overwrite a file that appears between the existence check and open. An
  existing file still goes through full read-only preflight.

## Size and review tradeoff

Physical lines relative to the baseline, including comments/blank lines:

| Scope | Added | Removed | Net |
| --- | ---: | ---: | ---: |
| Production (`TRestTools.cxx`) | 253 | 440 | **-187** |
| Permanent regression tests/CMake fixtures | 219 | 0 | +219 |
| Developer guide | 15 | 5 | +10 |

The two affected I/O implementation sections shrink from 877 to 688 physical
lines (about 22%); two new includes account for the difference from the net
production reduction. These counts exclude this experimental report, the replay
runner, benchmark, generated logs, builds, and all submodule checkout changes.
No previous tests or preservation checks were removed to obtain the saving.

The code is materially shorter, but it still implements the same multi-phase
transaction. It does not make ROOT schema handling intrinsically simple. The
main review choice is whether the centralized exception cleanup is clearer than
the baseline's repeated error/close/cleanup branches. The independent review
branch offers a smaller incremental change if maintainers prefer the old control
flow; that branch's correctness fixes plus optional cleanup net -9 production
lines and its validation is separate from this experiment.

## New permanent regressions

`source/framework/test/src/RootIO.cxx` adds checks for Update creation (including
real `TRestDataSetGainMap::Export`), output aliases during cleanup, rollback when
publishing a new destination, environment-variable path expansion, and updating
through a symlink. `RootIOConflict.cxx` runs separate old/current dictionary
executables defined under `source/framework/test/io/`, verifying byte-identical
failure for an incompatible same-version Update and merge.

The retained tests check loaded/unloaded schemas, automatic collection evolution,
renamed-member rules, worker-only historical metadata, ROOT implementation
normalization, actual AnalysisTree values, source retention, preflight and
publication failures, remote mutation policy, and ROOT file URLs.

## Validation evidence

The original run was interrupted by a machine crash on 2026-09-09. Persistent
worktrees/builds survived; host `/tmp` logs did not. The continuation inspected
surviving results before resuming, and writes evidence under this directory.

- Focused host build: ROOT 6.26/10, RelWithDebInfo, 26/26 non-network tests pass
  after adding the gain-map regression (`logs/focused-resume-tests.log`). The
  remote policy test is included in the full container run below.
- Final full CI build: **78/78 tests pass**, 449.79 seconds, recorded in
  `logs/ctest-ci-resume-final.log`.
  It enables all libraries, Geant4, Garfield, MPFR and tests, with the same flags
  as the official workflow. It runs CTest serially, as official CI does. The
  previous parallel run passed 76/78 and both order-sensitive restG4 cases passed
  on a targeted retry; the final serial run supersedes that evidence.
- Workflow replay: **105/105 recorded run/build steps pass**. Per-step commands,
  outputs and exit status are in `workflows/<repository>/{*.sh,*.log,results.json}`.
  This includes all modern/reference restG4 examples and standalone installation,
  all axion/detector/raw/geant4/connectors/track/wimp/legacy jobs, framework macro
  loading and PyROOT, metadata, PandaX-III, TREX-DM raw/signal/hits processing,
  both AnalysisPlot pipelines (including the 3.3 GB historical input download),
  event selection and the alpha-track response chain.
- The configured pre-commit hooks pass on all eight proposed implementation,
  test, CMake and developer-guide files (including pinned clang-format 15 and
  cmake-format). `git diff --check` also passes.

Replay uses the actual workflow run bodies and build flags. GitHub checkout,
cache and upload actions are represented by isolated local snapshots/installs;
they are not claims of an official GitHub Actions execution. Dependency downloads
are real. Initial transient HTTPS failures in axion optics and restG4 materials
passed on retry. The replay harness initially failed to expand digits in env
names and mixed restG4 reference jobs into the modern environment; those harness
bugs were corrected and affected jobs rerun in separate builds/containers.
Obsolete attempts remain in logs, not in the final matrix totals.

| Replay | Successful run/build steps |
| --- | ---: |
| Framework modern / reference | 17 / 8 |
| Axion | 15 |
| Detector | 6 |
| Raw | 12 |
| Geant4 | 4 |
| Connectors | 4 |
| Track | 4 |
| Wimp | 2 |
| Legacy | 2 |
| restG4 modern / reference | 22 / 9 |

The complete framework reference jobs use `install-reference`; standalone
reference restG4 uses `validation/install/restG4-reference`. Modern equivalents
are separate. Do not combine these installations: their Geant4 reference values
and material definitions differ.

## Dependencies and reproducibility

The existing Docker images were reused without replacing their tags:

| Container | ROOT | Geant4 | Garfield |
| --- | --- | --- | --- |
| `rest-pr567-reimplementation` | 6.26/06 | 11.0.3 | `eb1b5403` |
| `rest-pr567-reimplementation-reference` | 6.26/00 | 10.4.3 | `4ac1e62f` |

Their images are `ghcr.io/lobis/root-geant4-garfield:rest-for-physics` and
`:rest-for-physics-reference-jun2022`. The worktree is mounted at `/work`; the
old persistent dependency cache is mounted read-only at `/cache`. Builds use at
most four workers. Host focused tests use `/home/basti/src/root/root/install`.
The image IDs are respectively
`sha256:82ef4989edb6fb7d8b235c043f385efb805826a9e641fee599799c7ce26ba331`
and `sha256:33ecb446ef9f9e3895d3ecae2285b8707c83d375b887670714a13cfca3330817`.

Validation intentionally checked out the then-current sublibrary branches, as
the official reusable workflows do. These gitlinks are **not** proposed changes
and must not be accidentally staged with the implementation:

| Submodule | Validation commit |
| --- | --- |
| axion | `3f61a62d8374b415d8ecb5eb1cb594b3f74c2bba` |
| connectors | `17a92ecf2f0f134dffcaa7427f482a8ce26a50dc` |
| detector | `ddf7fb6aac6b35de9690615ee56a7d01ecf01d35` |
| geant4 | `1d5326ce02d7078bd2f00cb9834ff25e748fe4ba` |
| legacy | `caa4e2540f87b33071b1ae8e21f277aa69e60861` |
| raw | `90536dc1ddb99f6ac16f6751bcd45ebc7e1d67a0` |
| track | `ca774d25e89291e3b1d9be2d284007b5aab77cdd` |
| wimp | `8dc47c07e167aad0f52ad8b27a4d3e8dd44fea13` |
| restG4 | `c37d3b0e427b662e3fe44e7434a1c26c80a85727` |

`replay_workflows.py` runs inside the appropriate container. For example:

```sh
python3 /work/validation/replay_workflows.py axion
python3 /work/validation/replay_workflows.py restG4 --reference
```

`--jobs` selects workflow jobs; `--skip-build` reuses an existing matching
installation. Successful/retried steps replace their own result in the index.

## Illustrative benchmark and limits

`RootIOBenchmark.cxx` creates a 100,000-entry double tree, then performs 100
Update open/closes and 30 small-worker transactional merges. The preserved
baseline library and rebuilt compact library were loaded in separate processes
on the same host (logs `benchmark-{baseline,reimplementation}-resume.log`).

| Implementation | 100 updates | 30 merges |
| --- | ---: | ---: |
| Baseline | 637.6 ms | 518.4 ms |
| Compact | 646.5 ms | 497.8 ms |

This is a single noisy comparison during other validation work, not a claim of
meaningful speed improvement. It supports only that the compact implementation
has similar cost for this small case. This task's measurable gain is code size
and centralized cleanup, not throughput.

Remaining limits are the existing design limits: no interprocess writer lock,
no power-loss durability promise, no remote writable destination, ROOT transport
availability controls remote reads, and cleanup failure can return false after a
valid output has already been installed. No Windows runtime tests were run.
The tests cannot exhaust all possible third-party ROOT dictionaries or schema
rules; same-member-name explicit conversion rules remain subject to ROOT's own
schema-evolution behavior.
