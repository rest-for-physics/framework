# Installation Scripts

Interactive installer for [REST-for-Physics](https://github.com/rest-for-physics/framework) and its dependencies.

## Quick start

```bash
bash install_rest.sh
```

The script is interactive and will guide you through each step.

## Install modes

### Mode 1 -- Server install

For machines where ROOT, Geant4 and Garfield++ are already available system-wide
(e.g. the `titan` cluster at Zaragoza).

You will be asked for the path to the system `thisREST.sh` that loads the
pre-installed environment. The script then clones, configures and builds only
REST itself.

### Mode 2 -- Full install from source

For a fresh Ubuntu (22.04/24.04) or WSL system with nothing pre-installed.
Builds everything from source:

| Package       | Version                                   |
|---------------|-------------------------------------------|
| ROOT          | 6.38.06                                   |
| Geant4        | 11.0.3 (with GDML + data)                 |
| Garfield++    | pinned commit `78fe1bd3` (as in LCG_109)  |
| REST          | latest from `master`                      |

See [Version choices](#version-choices) for why these versions.

System dependencies are installed via `apt` (requires `sudo`).
If `sudo` is not available the script offers to skip this step, assuming
packages are already present.

Garfield++ is built after REST is cloned, and only if Garfield integration is
enabled.

### Mode 3 -- CVMFS install

For HEP clusters where CERN's CVMFS is mounted (e.g. NAF-IAXO at DESY, RHEL 9).
Instead of building the dependencies, the script loads a CERN **LCG view** from
`/cvmfs` (providing ROOT, Geant4, GSL and Garfield) and builds only REST on top.
This needs no `sudo` and survives OS upgrades far better than a hand-built stack.

You will be asked for the LCG view `setup.sh` (default: a tested el9 view,
`LCG_109/x86_64-el9-gcc13-opt`). The script then sets up a small Garfield
compatibility shim, clones REST, and builds it -- including `restG4` if Geant4
integration is enabled.

On clusters where `$HOME` is on AFS (not writable in-session), the script sets
`REST_HOME` to a writable location so REST's per-user `.rest` directory works.

## What the script does

1. Installs system packages (mode 2, optional)
2. Downloads and builds ROOT and Geant4 (mode 2),
   or loads a CERN LCG view from `/cvmfs` (mode 3)
3. Asks you to choose which REST libraries to enable
4. Clones the REST framework and pulls submodules
   (also the `restG4` package when Geant4 integration is enabled)
5. Checks the cloned sources (see [Compatibility checks](#compatibility-checks)),
   then builds Garfield++ (mode 2, if enabled) or applies the CVMFS patches
   that are still needed (mode 3)
6. Configures and builds REST with CMake
7. Tests the installation (`restRoot`)
8. Optionally adds the environment setup to `~/.bashrc`
   (and ensures login shells source it, e.g. for SSH)

## Options asked during installation

- **Install directory** -- where REST source and build will live (default: `~/rest`)
- **Libraries** -- all, a common subset, or pick individually
- **Garfield++ / Geant4 integration** -- enable or disable
- **Submodule strategy** -- `--latest` commits (default) or the `--clean`
  framework-recorded versions, for the libraries and `restG4` alike
  (pick `--clean` if a latest build fails to compile)
- **Parallel jobs** -- used for every build (ROOT, Geant4, Garfield++, REST).
  The default is one per CPU but at most one per 1.5 GB of available memory,
  so that the ROOT build is not killed for lack of memory; capped at 16 for
  shared-server politeness

## Per-package scripts

`install_rest.sh` installs everything in one guided run. If you only need one
dependency (for example ROOT on a laptop, or Geant4 on a machine that already
has ROOT), use the per-package scripts instead. They install the same versions
as `install_rest.sh`:

- `installRequiredSoftware.sh` -- system packages via `apt-get` (same list as mode 2)
- `installROOT.sh` -- ROOT 6.38.06
- `installGeant4.sh` -- Geant4 11.0.3
- `installGarfield.sh` -- Garfield++ commit `78fe1bd3` (override with `GARFIELD_REF=...`)

When changing a version, change it in `install_rest.sh` and in the matching
per-package script, and add a row to the history below.

## Version choices

The mode 2 versions follow the CERN LCG release used in mode 3, because that is
a combination REST has been built and run with (on NAF-IAXO, June 2026, with
LCG_109: ROOT 6.38.00, Geant4 11.4.0, Garfield++ `78fe1bd3`).

- **ROOT 6.38.06**: the latest patch release of the 6.38 series used by
  LCG_109 (patch releases only contain bug fixes). The previous version,
  6.26.10, was simply the version on a machine where REST was known to work;
  there was no technical requirement for it. ROOT 6.38 needs Python >= 3.9, so
  Ubuntu's own Python is used and the deadsnakes PPA (Python 3.10, needed only
  because ROOT 6.26 does not support Python 3.12) is no longer installed.
- **Geant4 11.0.3**: unchanged. LCG_109 ships 11.4.0; moving mode 2 to it would
  make mode 2 match the tested LCG combination exactly.
- **Garfield++ `78fe1bd3`** (February 2026, 10 commits after tag `2025.12`):
  the exact commit shipped in LCG_109. Garfield++ has no release tag for it,
  so it is pinned by commit. The previous pin, `a993c5eb` (April 2024), was
  likewise a known-good commit, not a workaround for a specific bug: it already
  used the modern `Component` API.

To build a different Garfield++ in mode 2, set `GARFIELD_REF`:

```bash
GARFIELD_REF=a993c5eb015e23aa4bccdd1bdda027314afc2153 bash install_rest.sh
```

If the REST build fails after building the default Garfield++, the installer
prints this command.

### History

| Date    | Change | Reason |
|---------|--------|--------|
| 2021–2023 | `installROOT.sh`: ROOT 6.24.02 (2021) → 6.26.10 (Jan 2023) → 6.28.02 (May 2023) | Successive upgrades; no specific requirement recorded |
| 2023-01 | `installGarfield.sh`: Garfield++ tag `4.0` | "Latest release" at the time |
| 2026-04 | `install_rest.sh` added: ROOT 6.26.10, Geant4 11.0.3, Garfield++ `a993c5eb` | Versions of a machine where REST was known to work |
| 2026-06 | Mode 3 (CVMFS) added, with patches for modern Garfield and C++20 Geant4 | Needed with LCG_109 and the detectorlib / restG4 of that time |
| 2026-11 | `install_rest.sh`: restG4 follows the `--latest` / `--clean` choice ([#570](https://github.com/rest-for-physics/framework/pull/570)) | It was always pulled at its 2023 recorded commit, which does not compile against the latest geant4lib |
| 2026-11 | ROOT 6.38.06 and Garfield++ `78fe1bd3` in all scripts; mode 3 patches applied only when needed; `installRequiredSoftware.sh` package list replaced | Align mode 2 and the per-package scripts with the tested LCG_109 combination; the old package list referred to packages that no longer exist on Ubuntu 22.04/24.04 (e.g. `qt4-dev-tools`, `python-dev`) |
| 2026-11 | `install_rest.sh`: the parallel-jobs question is asked before any build and applies to ROOT, Geant4 and Garfield++ too; memory-aware default | ROOT, Geant4 and Garfield++ were built with one job per CPU: on a 12-CPU WSL with 7.6 GB, the ROOT build was killed for lack of memory |
| 2026-11 | `libjpeg-dev`, `libgif-dev`, `libtiff-dev` added to the package list | Without them ROOT 6.38 builds its bundled libjpeg, which fails to link into `libASImage` (`recompile with -fPIC`); found in a mode 2 test on a fresh Ubuntu 24.04 |
| 2026-11 | Geant4 built with `GEANT4_USE_SYSTEM_EXPAT=OFF` | With CMake >= 3.27 (Ubuntu 24.04), Geant4 11.0 records an empty `EXPAT_LIBRARY` in its installed config, so every later `find_package(Geant4)` (Garfield++, restG4) fails; found in the same test |
| 2026-11 | Downloaded ROOT/Geant4 tarballs are checked (`gzip -t`) and fetched again if incomplete | An interrupted ROOT download left a truncated tarball and a half-unpacked source tree, which a re-run would have reused |

## Compatibility checks

The REST libraries and `restG4` the installer clones can be the latest ones
(`--latest`) or the commits recorded in the framework (`--clean`). In November
2026 the recorded commits of detectorlib and restG4 date from late 2023, so
they lack fixes that newer dependencies need. Instead of comparing
version numbers, the installer looks for the code each fix introduced:

| Check (in the cloned sources) | If present | If missing |
|---|---|---|
| detectorlib has the `Component.hh` branch (since March 2022) | Mode 2 builds Garfield++ | Mode 2 stops with an explanation: no Garfield++ the installer provides can work with it |
| detectorlib selects the Garfield API by itself ([detectorlib#131](https://github.com/rest-for-physics/detectorlib/pull/131), July 2026) | Nothing to do | Mode 3 patches the detector headers from `ComponentBase` to `Component` |
| restG4 accepts a C++17-or-newer Geant4 ([restG4#150](https://github.com/rest-for-physics/restG4/pull/150), July 2026) | Nothing to do | Mode 3 relaxes restG4's C++ standard check |

Once the commits recorded in the framework include detectorlib#131 and
restG4#150, the two mode 3 patches are never applied and can be deleted.

## Requirements

- **OS**: Ubuntu 22.04 / 24.04 (or WSL with either) for mode 2; any Linux with
  CERN CVMFS mounted for mode 3 (e.g. RHEL 9 / NAF). Other distros may work but
  the mode-2 `apt` package list would need adapting.
- **Disk**: ~5 GB for a full build (ROOT + Geant4 + Garfield + REST)
- **Tools**: `bash`, `git`, `python3`, `cmake`, `make`, `wget`
