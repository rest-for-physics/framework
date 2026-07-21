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

| Package       | Version                          |
|---------------|----------------------------------|
| ROOT          | 6.26.10                          |
| Geant4        | 11.0.3 (with GDML + data)       |
| Garfield++    | pinned commit `a993c5eb`         |
| REST          | latest from `master`             |

System dependencies are installed via `apt` (requires `sudo`).
If `sudo` is not available the script offers to skip this step, assuming
packages are already present.

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
2. Downloads and builds ROOT, Geant4, Garfield++ (mode 2),
   or loads a CERN LCG view from `/cvmfs` (mode 3)
3. Asks you to choose which REST libraries to enable
4. Clones the REST framework and pulls submodules
   (also the `restG4` package when Geant4 integration is enabled)
5. Configures and builds REST with CMake
6. Tests the installation (`restRoot`)
7. Optionally adds the environment setup to `~/.bashrc`
   (and ensures login shells source it, e.g. for SSH)

## Options asked during installation

- **Install directory** -- where REST source and build will live (default: `~/rest`)
- **Libraries** -- all, a common subset, or pick individually
- **Garfield++ / Geant4 integration** -- enable or disable
- **Submodule strategy** -- `--latest` commits (default) or the `--clean`
  framework-recorded versions (pick `--clean` if a latest build fails to compile)
- **Parallel jobs** -- capped at 16 for shared-server politeness

## Individual scripts (legacy)

The older per-package scripts are still available for reference:

- `installROOT.sh` -- ROOT 6.28.02
- `installGeant4.sh` -- Geant4 11.0.3
- `installGarfield.sh` -- Garfield++
- `installRequiredSoftware.sh` -- system packages via `apt-get`

`install_rest.sh` supersedes all of them in a single guided workflow.

## Requirements

- **OS**: Ubuntu 22.04 / 24.04 (or WSL with either) for mode 2; any Linux with
  CERN CVMFS mounted for mode 3 (e.g. RHEL 9 / NAF). Other distros may work but
  the mode-2 `apt` package list would need adapting.
- **Disk**: ~5 GB for a full build (ROOT + Geant4 + Garfield + REST)
- **Tools**: `bash`, `git`, `python3`, `cmake`, `make`, `wget`
