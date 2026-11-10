# Installation Scripts

Interactive installer for [REST-for-Physics](https://github.com/rest-for-physics/framework) and its dependencies.

## Quick start

```bash
bash install_rest.sh
```

The script is interactive and will guide you through each step. Press Enter at
the release prompt to accept the displayed stable version:

```text
REST release to install [default: v2.4.3]:
```

The default is fixed in this copy of the installer; it does not automatically
advance when another release is published.

## Installing a release

Accept the default, type a different published tag at the prompt, or skip the
release question by passing a tag explicitly:

```bash
bash install_rest.sh --release TAG
```

Replace `TAG` with the published framework release tag. The installer checks out
that exact tag and the public library and restG4 commits recorded in it. It does
not follow their latest branches or force-reset local changes. Optional data,
documentation, and private repositories are not downloaded.

Choose a fresh directory. A retry in an existing checkout is allowed only when
its framework commit matches the tag and its tracked files/submodules are clean;
build outputs are retained. Older releases requiring source patches for a modern
CVMFS environment stop with an explanation rather than modifying release code.

Development installation requires an explicit option:

```bash
bash install_rest.sh --development
```

This follows the framework development branch. Recorded library commits remain
the default, with latest development libraries offered as an explicit choice.
`--development` and `--release` cannot be combined. A missing or inaccessible
release tag stops the installation; it never falls back to development code.

For release maintainers: set `DEFAULT_RELEASE_TAG` in `install_rest.sh` to the
matching tag before publishing each release, update the prompt example above,
and validate the no-option installation. The current `v2.4.3` default is the
existing published stable release; the upcoming release's name is still pending.

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
| REST          | displayed stable tag; override with `--release TAG` or `--development` |

System dependencies are installed via `apt` (requires `sudo`). Use mode 1 when
dependencies are already available and you do not have sudo access.

This release preparation retains the dependency versions above. The separate
proposal to upgrade ROOT to 6.38.06 and Garfield is not included. A complete fresh
mode-2 installation of the final release candidate still needs validation;
Ubuntu 24.04 coverage is not established by the installer regression checks.

Parallelism is chosen before any compilation and applies to all four builds.
The default is limited by CPU count, available memory (about 1.5 GiB per job),
and a maximum of 16. This is a conservative estimate, not a memory guarantee;
choose fewer jobs if needed. Invalid job counts, including zero, use the default.

ROOT/Geant4 downloads are checked with `gzip -t` before use. Interrupted downloads
are retried; an existing source/build tree associated with a corrupt archive is
preserved under a `.incomplete.*` directory. New extractions use temporary
directories so interrupted unpacking is not mistaken for a complete source tree.
Geant4 uses its bundled Expat to avoid the reported Geant4 11.0.3 package-config
failure with newer CMake versions.

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

1. Asks for build parallelism, then installs system packages (mode 2)
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
- **Release** -- press Enter for the displayed stable tag, or type another tag.
  `--release TAG` skips this question; `--development` opts into development code.
- **Submodule strategy** -- release installs always use recorded commits.
  Development installs offer recorded commits (default) or latest commits.
- **Parallel jobs** -- asked before dependency builds; the default accounts for
  available memory and is capped at 16

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
