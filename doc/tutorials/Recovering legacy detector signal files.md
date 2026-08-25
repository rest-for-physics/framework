# Recovering legacy detector signal files

REST files written with detectorlib versions before the `TRestDetectorSignal`
schema update stored signal time and charge arrays as `vector<float>`. Current
detectorlib uses `vector<double>`. Some old files do not contain enough ROOT
streamer information to convert that change safely; directly selecting the
legacy detector-signal branch can otherwise cause a very large allocation,
crash, or out-of-memory failure.

The recovery command supports the known legacy signal schema versions 1, 2,
and 3:

```sh
restRoot --recover-legacy-signals input.root
```

The default result is `input_Fixed.root`. The input is not modified. Choose a
different new output path with:

```sh
restRoot --recover-legacy-signals input.root --output recovered.root
```

Existing outputs are never overwritten. Input and output must be local
filesystem paths. If the supplied input is a symbolic link, the default output
is created beside that link while the canonical target is used as the
authenticated source. In-place recovery through a symbolic link is refused.

## In-place recovery

In-place replacement is deliberately explicit:

```sh
restRoot --recover-legacy-signals input.root --in-place
```

The rebuilt candidate is fully closed, reopened, and checked before the input
is changed. The original is then retained as `input.root.bak`, and its basic
filesystem permissions are applied to the replacement. Recovery refuses to
start if that backup path already exists.

Use the default sibling output first when practical. In-place replacement
cannot preserve every platform-specific attribute, such as all ACLs, extended
attributes, or ownership changes that the current user cannot apply.

## Why the command uses two processes

Recovery has two intentionally isolated stages:

1. A plain, build-matched ROOT process reads the old `vector<float>` layout
   with replica classes and writes a private intermediate file. ROOT startup
   files are disabled so they cannot load REST classes into this process.
2. A fresh REST process authenticates that intermediate against the exact
   source identity and rebuilds the branch with the current
   `vector<double>` classes.

The command never sends filenames through a shell or a ROOT expression. Paths
are passed in the child environment, so spaces, quotes, and shell metacharacters
are treated as filename characters. It uses the ROOT binary and REST
installation that match the invoked installed `restRoot`, even if `PATH`,
`ROOTSYS`, or `REST_PATH` point elsewhere.

Interrupt, termination, and hangup signals are forwarded to the complete
active child process group before the command reports the resulting status.

The one-command workflow is strict: unreadable metadata, event branches, or
additional top-level trees make it fail instead of silently producing an
incomplete result. Every highest-cycle top-level tree is copied. Every
candidate is read back and its provenance, tree names, classes, entry counts,
branch inventories, signal schema, signal counts, and point counts are
checked.

Recovery provenance prevents accidental source/intermediate mix-ups. It is an
integrity guard for the workflow, not a cryptographic signature against a
maliciously modified file.

## Failures and cleanup

A nonzero exit status means recovery did not complete. Candidate creation and
validation do not touch the original. Sibling-output installation uses an
atomic no-overwrite operation, so a destination created by another process is
not replaced.

After validation, in-place installation moves the original to `.bak` and then
moves the candidate into place. If the second move fails, recovery attempts to
restore the original automatically. If that rollback also fails, the error
reports the exact original-at-backup and candidate paths; no file is silently
discarded.

The command reports its private work directory after a stage failure and
retains it for diagnosis. It removes only its own work directory after
success.

Check that:

- there is enough free space beside the requested output;
- the output directory is writable;
- no requested output or `.bak` path already exists;
- the file contains a known legacy `TRestDetectorSignalEventBranch`; and
- REST was installed after building the framework and detectorlib versions
  being used.

Cross-process file locking and power-loss transaction guarantees are outside
the current workflow. Do not modify the source or destination concurrently.
The one-command orchestrator is currently available on POSIX systems; the
underlying manual macros remain available on other supported platforms.

For detailed command help, run:

```sh
restRoot --recover-legacy-signals --help
```
