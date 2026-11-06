#!/bin/bash
# =============================================================================
#  REST-for-Physics -- Interactive Installation Script
#  https://github.com/rest-for-physics/framework
#
#  Supports three modes:
#    1. Server install  -- ROOT/Geant4/Garfield already available (e.g. titan)
#    2. Full install    -- Builds ROOT 6.38.06, Geant4 11.0.3, Garfield++, REST
#                         from source (e.g. WSL / fresh Ubuntu)
#    3. CVMFS install   -- Loads a CERN LCG view from /cvmfs for ROOT/Geant4/
#                         Garfield and builds only REST (e.g. NAF-IAXO / RHEL9)
#
#  Usage:  bash install_rest.sh
# =============================================================================

# Self-fix Windows line endings before anything else
grep -qP '\r' "$0" 2>/dev/null && sed -i 's/\r//' "$0" && exec bash "$0" "$@"

set -euo pipefail

# -- Colours ------------------------------------------------------------------
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; BOLD='\033[1m'; RESET='\033[0m'

info()    { echo -e "${CYAN}[INFO]${RESET}  $*"; }
success() { echo -e "${GREEN}[OK]${RESET}    $*"; }
warn()    { echo -e "${YELLOW}[WARN]${RESET}  $*"; }
error()   { echo -e "${RED}[ERROR]${RESET} $*"; exit 1; }
ask()     { echo -e "${BOLD}$*${RESET}"; }
sep()     { echo "-------------------------------------------------------------"; }

# Download a source tarball and unpack it, safely across re-runs: a truncated
# download (dropped connection) is detected and fetched again, together with
# whatever was unpacked from it, instead of building from incomplete sources.
fetch_and_unpack() {  # fetch_and_unpack <url> <tarball> <source-dir>
    local url="$1" tarball="$2" dir="$3"
    if [[ -f "$tarball" ]] && ! gzip -t "$tarball" 2>/dev/null; then
        warn "$tarball is incomplete (interrupted download?), downloading it again."
        rm -rf "$tarball" "$dir"
    fi
    if [[ ! -f "$tarball" ]]; then
        wget -O "$tarball.part" "$url"
        gzip -t "$tarball.part" 2>/dev/null ||
            error "The download of $tarball is incomplete. Check the connection and re-run."
        mv "$tarball.part" "$tarball"
    fi
    if [[ ! -d "$dir" ]]; then
        tar xzf "$tarball" || { rm -rf "$dir"; error "Could not unpack $tarball."; }
    else
        info "$dir already exists, skipping extraction."
    fi
}

# -- Banner -------------------------------------------------------------------
echo -e "${CYAN}"
echo "  ____  _____ ____ _____      ___                 ____  _               _"
echo " |  _ \| ____/ ___|_   _|    / _| ___  _ __      |  _ \| |__  _   _ ___(_) ___ ___"
echo " | |_) |  _| \___ \ | |     | |_ / _ \| '__|     | |_) | '_ \| | | / __| |/ __/ __|"
echo " |  _ <| |___ ___) || |     |  _| (_) | |        |  __/| | | | |_| \__ \ | (__\__ \\"
echo " |_| \_\_____|____/ |_|     |_|  \___/|_|        |_|   |_| |_|\__, |___/_|\___|___/"
echo "                                                                |___/"
echo -e "${RESET}"
echo "  REST-for-Physics installation script"
echo "  https://github.com/rest-for-physics/framework"
echo ""

# -- GCC compatibility flags (fixes GCC 14 -Werror issues) --------------------
GCC_COMPAT_FLAGS="-Wno-error=ignored-attributes -Wno-error=overloaded-virtual -Wno-error=maybe-uninitialized"

# -- Dependency versions built in mode 2 --------------------------------------
# Why these versions, and their history: see "Version choices" in README.md.
# Keep installROOT.sh / installGeant4.sh / installGarfield.sh in sync.
ROOT_VERSION="6.38.06"     # same series as LCG_109 (ROOT 6.38.00), see README
GEANT4_VERSION="11.0.3"
# Garfield++ is pinned to the exact commit shipped in LCG_109, which REST was
# tested against on NAF-IAXO (there is no Garfield release tag for it).
# To build a different Garfield, e.g. the previous default if the new one
# causes trouble:  GARFIELD_REF=<commit-or-tag> bash install_rest.sh
GARFIELD_REF_DEFAULT="78fe1bd39eb2a1458bf731462f1c9d3f9d0b8aab"
GARFIELD_REF_PREVIOUS="a993c5eb015e23aa4bccdd1bdda027314afc2153"
GARFIELD_REF="${GARFIELD_REF:-$GARFIELD_REF_DEFAULT}"

# -- Per-mode variables (kept defined for set -u safety) ----------------------
CMAKE_EXTRA_FLAGS=""      # extra cmake flags (e.g. Garfield Heed dir in mode 3)
GARFIELD_HOME=""          # set by mode 3 (CVMFS shim)
GARFIELD_SETUP=""         # set by mode 2 (path to built Garfield setupGarfield.sh)
GARFIELD_BUILT="no"       # mode 2: did this run build Garfield? (for build hints)

# =============================================================================
#  STEP 0 -- Choose install mode
# =============================================================================
sep
ask "Choose installation mode:"
echo "  1) Server install  -- use pre-installed ROOT/Geant4/Garfield on this machine"
echo "     (for clusters like titan where a system REST environment exists)"
echo "  2) Full install    -- build ROOT, Geant4, Garfield++ and REST from source"
echo "     (for a fresh Ubuntu / WSL system)"
echo "  3) CVMFS install   -- load a CERN LCG view from /cvmfs for ROOT/Geant4/"
echo "     Garfield and build only REST (for HEP clusters like NAF-IAXO / RHEL9)"
echo ""
read -rp "Enter choice [1/2/3]: " INSTALL_MODE

case "$INSTALL_MODE" in
    1) info "Mode: Server install (using pre-built dependencies)" ;;
    2) info "Mode: Full install from source" ;;
    3) info "Mode: CVMFS install (CERN LCG view + local REST build)" ;;
    *) error "Invalid choice. Please run again and enter 1, 2 or 3." ;;
esac
echo ""

# =============================================================================
#  STEP 1 -- Choose install directory
# =============================================================================
sep
DEFAULT_DIR="$HOME/rest"
ask "Where do you want to install REST?"
read -rp "Install directory [default: $DEFAULT_DIR]: " REST_DIR
REST_DIR="${REST_DIR:-$DEFAULT_DIR}"
REST_DIR="${REST_DIR/#\~/$HOME}"   # expand ~ manually
info "REST will be installed in: $REST_DIR"
echo ""

# AFS home is small and token-based; a REST build belongs on scratch (e.g. DUST).
if [[ "$REST_DIR" == /afs/* ]]; then
    warn "$REST_DIR is on AFS, which is small and not suited to a REST build."
    warn "On DESY/NAF, prefer a DUST area, e.g. /data/dust/user/<user>/rest."
    read -rp "Use this AFS path anyway? [y/N]: " AFS_CONFIRM
    [[ "$AFS_CONFIRM" =~ ^[Yy]$ ]] || error "Aborted. Re-run and choose a scratch/DUST path."
fi

if [[ -d "$REST_DIR" ]]; then
    warn "Directory $REST_DIR already exists."
    read -rp "Continue and use it anyway? [y/N]: " CONFIRM
    [[ "$CONFIRM" =~ ^[Yy]$ ]] || error "Aborted."
fi

# -- Parallel build jobs (used for every build: ROOT, Geant4, Garfield, REST) -
sep
ask "How many parallel jobs for make? (be careful on shared servers)"
# Default: one job per CPU, but no more than one per 1.5 GB of available
# memory. Compiling ROOT's bundled LLVM needs ~1 GB per job; with too many
# jobs the compiler is killed ("fatal error: Killed signal terminated program
# cc1plus"), e.g. 12 jobs on a laptop/WSL with 8 GB.
NCPU=$(nproc 2>/dev/null || echo 4)
MEM_JOBS=$(awk '/MemAvailable/ {print int($2 / 1536 / 1024)}' /proc/meminfo 2>/dev/null)
if [[ "${MEM_JOBS:-0}" -ge 1 && "$NCPU" -gt "$MEM_JOBS" ]]; then
    NCPU="$MEM_JOBS"
fi
if [[ "$NCPU" -gt 16 ]]; then
    NCPU=8
fi
read -rp "Jobs [default: $NCPU, max: 16]: " NJOBS
NJOBS="${NJOBS:-$NCPU}"
if ! [[ "$NJOBS" =~ ^[0-9]+$ ]]; then
    warn "Invalid number '$NJOBS', defaulting to $NCPU."
    NJOBS="$NCPU"
fi
if [[ "$NJOBS" -gt 16 ]]; then
    warn "Capping parallel jobs at 16 to be polite on shared servers."
    NJOBS=16
fi
info "Building with $NJOBS parallel jobs."
echo ""

# =============================================================================
#  MODE 2 -- Full install: check/install system dependencies
# =============================================================================
if [[ "$INSTALL_MODE" == "2" ]]; then

    sep
    # Check for sudo access before attempting anything
    if ! sudo -v 2>/dev/null; then
        error "Mode 2 requires sudo access to install system packages.\n       On shared servers (e.g. titan), use mode 1 instead."
    fi
    warn "This package list was designed for Ubuntu 22/24 with ROOT $ROOT_VERSION,"
    warn "Geant4 $GEANT4_VERSION and Garfield++. It has not been fully tested on a"
    warn "fresh system yet. If something fails, a package may be missing or"
    warn "named differently on your Ubuntu version."
    read -rp "Press Enter to continue or Ctrl+C to abort: "
    echo ""
    info "Installing system dependencies (requires sudo)..."
    sudo apt update && sudo apt install -y \
        build-essential cmake git wget curl \
        gcc-11 g++-11 gfortran \
        python3 python3-dev python3-full python3-pip python3-numpy \
        libssl-dev libxerces-c-dev libxml2-dev \
        nlohmann-json3-dev \
        libxxhash-dev libzstd-dev liblzma-dev liblz4-dev \
        libfreetype6-dev \
        libglew-dev libgl2ps-dev \
        libx11-dev libxpm-dev libxft-dev libxext-dev \
        libxmu-dev libxi-dev \
        libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev \
        qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools \
        libfftw3-dev libtinyxml2-dev libgsl-dev libglpk-dev \
        libcfitsio-dev libcurl4-openssl-dev \
        libjpeg-dev libgif-dev libtiff-dev
    # libjpeg/libgif/libtiff: without them ROOT 6.38 builds its bundled libjpeg,
    # which fails to link into libASImage ("recompile with -fPIC").
    # The system Python is enough for ROOT 6.38 (needs >= 3.9; Ubuntu 22.04
    # ships 3.10, 24.04 ships 3.12). The deadsnakes PPA with Python 3.10 was
    # only needed for ROOT 6.26, which does not support Python 3.12.
    success "System dependencies installed."
    echo ""

    SOFTWARE_DIR="$REST_DIR/../software"
    SOFTWARE_DIR="$(realpath -m "$SOFTWARE_DIR")"
    mkdir -p "$SOFTWARE_DIR"
    info "Dependencies will be built in: $SOFTWARE_DIR"

    # -- ROOT ----------------------------------------------------------------
    sep
    info "Downloading and building ROOT $ROOT_VERSION (this takes a while)..."
    cd "$SOFTWARE_DIR"
    fetch_and_unpack "https://root.cern/download/root_v$ROOT_VERSION.source.tar.gz" \
        "root_v$ROOT_VERSION.source.tar.gz" "root-$ROOT_VERSION"
    cd root-$ROOT_VERSION
    mkdir -p root_build root_install
    cd root_build
    cmake .. \
        -DCMAKE_INSTALL_PREFIX=../root_install \
        -DPython3_EXECUTABLE="$(command -v python3)" \
        -DCMAKE_C_COMPILER=gcc-11 \
        -DCMAKE_CXX_COMPILER=g++-11 \
        -DCMAKE_CXX_STANDARD=17 \
        -Dnetxng=OFF -Dxrootd=OFF
    make -j"$NJOBS" install
    ROOT_THISROOT="$SOFTWARE_DIR/root-$ROOT_VERSION/root_install/bin/thisroot.sh"
    set +u
    source "$ROOT_THISROOT"
    set -u
    success "ROOT $ROOT_VERSION installed."
    echo ""

    # -- Geant4 --------------------------------------------------------------
    sep
    info "Downloading and building Geant4 $GEANT4_VERSION..."
    cd "$SOFTWARE_DIR"
    fetch_and_unpack "https://gitlab.cern.ch/geant4/geant4/-/archive/v$GEANT4_VERSION/geant4-v$GEANT4_VERSION.tar.gz" \
        "geant4-v$GEANT4_VERSION.tar.gz" "geant4-v$GEANT4_VERSION"
    cd geant4-v$GEANT4_VERSION
    mkdir -p build install
    cd build
    # GEANT4_USE_SYSTEM_EXPAT=OFF: with CMake >= 3.27 (Ubuntu 24.04), Geant4
    # 11.0 records an empty EXPAT_LIBRARY in its installed Geant4Config, and
    # every later find_package(Geant4) (Garfield++, restG4) then fails with
    # "geant4_set_and_check_package_variable Macro invoked with incorrect
    # arguments". Geant4's bundled expat avoids that.
    cmake .. \
        -DCMAKE_INSTALL_PREFIX=../install \
        -DGEANT4_USE_SYSTEM_EXPAT=OFF \
        -DGEANT4_INSTALL_DATA=ON \
        -DGEANT4_USE_OPENGL_X11=ON \
        -DGEANT4_BUILD_MULTITHREADED=ON \
        -DCMAKE_C_COMPILER=gcc-11 \
        -DCMAKE_CXX_COMPILER=g++-11 \
        -DGEANT4_USE_GDML=ON
    make -j"$NJOBS" install
    # Fix ownership only if files ended up root-owned (e.g. from a previous sudo run)
    if [[ ! -O "$SOFTWARE_DIR/geant4-v$GEANT4_VERSION/install" ]]; then
        sudo chown -R "$(whoami):$(whoami)" "$SOFTWARE_DIR/geant4-v$GEANT4_VERSION/install"
    fi
    GEANT4_SH="$SOFTWARE_DIR/geant4-v$GEANT4_VERSION/install/bin/geant4.sh"
    set +u
    source "$GEANT4_SH"
    set -u
    success "Geant4 $GEANT4_VERSION installed."
    echo ""

    # Garfield++ is built later (after STEP 5), once the REST sources are
    # cloned: the installer checks them first, and skips Garfield entirely if
    # Garfield integration is not requested.

fi  # end MODE 2 dependencies

# =============================================================================
#  MODE 3 -- CVMFS install: load a CERN LCG view + shim Garfield
# =============================================================================
if [[ "$INSTALL_MODE" == "3" ]]; then

    sep
    info "Mode 3 uses CERN's CVMFS software (an LCG 'view') for ROOT, Geant4,"
    info "GSL and Garfield. Only REST itself is built locally. This survives OS"
    info "upgrades far better than a hand-built stack."
    echo ""
    DEFAULT_VIEW="/cvmfs/sft.cern.ch/lcg/views/LCG_109/x86_64-el9-gcc13-opt/setup.sh"
    ask "Path to the LCG view setup.sh:"
    echo "  Default is tested on NAF-IAXO (RHEL9): $DEFAULT_VIEW"
    read -rp "LCG view setup.sh [default: $DEFAULT_VIEW]: " LCG_VIEW_SETUP
    LCG_VIEW_SETUP="${LCG_VIEW_SETUP:-$DEFAULT_VIEW}"
    LCG_VIEW_SETUP="${LCG_VIEW_SETUP/#\~/$HOME}"
    [[ -f "$LCG_VIEW_SETUP" ]] || error "Not found: $LCG_VIEW_SETUP\n       Check that /cvmfs is mounted and the path is correct."

    info "Sourcing LCG view: $LCG_VIEW_SETUP"
    set +u
    source "$LCG_VIEW_SETUP"
    set -u
    info "ROOT $(root-config --version 2>/dev/null), Geant4 $(geant4-config --version 2>/dev/null)"
    success "CVMFS environment loaded."
    echo ""

    # -- Garfield shim -------------------------------------------------------
    # CVMFS Garfield ships no CMake config and uses lib64, so REST falls back to
    # its 'old' GARFIELD_HOME detection path. We present a shim directory with
    # the layout REST expects (lib + include), and later feed FindGarfieldOld a
    # valid Heed include dir (it hunts for the removed HeedChamber.hh).
    VIEW_DIR="$(dirname "$LCG_VIEW_SETUP")"
    PLATFORM="$(basename "$VIEW_DIR")"
    LCG_REL="$(basename "$(dirname "$VIEW_DIR")")"
    GV="$(ls -d /cvmfs/sft.cern.ch/lcg/releases/"$LCG_REL"/Garfield++/*/"$PLATFORM" 2>/dev/null | head -1)"

    if [[ -n "$GV" && -d "$GV" ]]; then
        GLIB="lib"; [[ -d "$GV/lib64" ]] && GLIB="lib64"
        # The shim dir lives inside REST_DIR, which must be empty for the
        # 'git clone' in STEP 5 -- so we only compute its path here and create
        # the actual symlinks after the clone (see STEP 5).
        GARFIELD_HOME="$REST_DIR/garfield_home"
        export GARFIELD_HOME
        unset GARFIELD_INSTALL || true   # force REST's old (GARFIELD_HOME) path
        CMAKE_EXTRA_FLAGS="-DGarfield_INCLUDE_Heed_DIRS=$GV/include/Garfield"
        info "Garfield found: $GV ($GLIB) -- shim created after clone."
    else
        GARFIELD_HOME=""
        warn "Garfield not found in $LCG_REL for platform $PLATFORM."
        warn "If you need Garfield, choose a different LCG view; otherwise answer"
        warn "'n' to the Garfield question below to build REST without it."
    fi
    echo ""

fi  # end MODE 3 setup

# =============================================================================
#  STEP 2 -- Source the environment (1: system, 2: freshly built, 3: CVMFS)
# =============================================================================
sep

if [[ "$INSTALL_MODE" == "1" ]]; then
    ask "Enter the path to the system thisREST.sh to load ROOT/Geant4/Garfield:"
    echo "  Sultan: /programas/rest/latest/thisREST.sh"
    echo "  Titan:  /programas/titan/rest/latest/thisREST.sh"
    case "$(hostname)" in
        *sultan*) DEFAULT_THIS_REST="/programas/rest/latest/thisREST.sh" ;;
        *titan*)  DEFAULT_THIS_REST="/programas/titan/rest/latest/thisREST.sh" ;;
        *)        DEFAULT_THIS_REST="" ;;
    esac
    if [[ -n "$DEFAULT_THIS_REST" ]]; then
        read -rp "Path [default: $DEFAULT_THIS_REST]: " SYSTEM_THIS_REST
        SYSTEM_THIS_REST="${SYSTEM_THIS_REST:-$DEFAULT_THIS_REST}"
    else
        read -rp "Path: " SYSTEM_THIS_REST
    fi
    SYSTEM_THIS_REST="${SYSTEM_THIS_REST/#\~/$HOME}"
    if [[ ! -f "$SYSTEM_THIS_REST" ]]; then
        error "File not found: $SYSTEM_THIS_REST"
    fi
    info "Sourcing system environment: $SYSTEM_THIS_REST"
    # shellcheck disable=SC1090
    set +u
    source "$SYSTEM_THIS_REST"
    set -u
    success "System environment loaded."
    ENV_LINES="source $SYSTEM_THIS_REST   # loads system ROOT / Geant4 / Garfield"
elif [[ "$INSTALL_MODE" == "3" ]]; then
    # Environment already loaded in the MODE 3 setup block above.
    info "Using CVMFS environment from: $LCG_VIEW_SETUP"
    if [[ -n "$GARFIELD_HOME" ]]; then
        ENV_LINES="source $LCG_VIEW_SETUP   # CERN CVMFS LCG view (ROOT/Geant4/Garfield)
export GARFIELD_HOME=$GARFIELD_HOME   # CVMFS Garfield shim (lib64 -> lib)"
    else
        ENV_LINES="source $LCG_VIEW_SETUP   # CERN CVMFS LCG view (ROOT/Geant4/Garfield)"
    fi
else
    # Mode 2: ROOT/Geant4 freshly built above (Garfield is added after STEP 5)
    set +u
    source "$ROOT_THISROOT"
    source "$GEANT4_SH"
    set -u
    success "Build environment loaded."
    ENV_LINES="source $ROOT_THISROOT
source $GEANT4_SH"
fi
echo ""

# =============================================================================
#  STEP 3 -- Choose libraries to enable
# =============================================================================
sep
ask "Which REST libraries do you want to enable?"
echo "  1) All libraries  (REST_ALL_LIBS=ON  -- recommended, includes everything)"
echo "  2) Common subset  (detector + raw + track + Garfield + Geant4)"
echo "  3) Custom         (you will be asked for each library)"
echo ""
read -rp "Enter choice [1/2/3, default: 1]: " LIB_CHOICE
LIB_CHOICE="${LIB_CHOICE:-1}"

case "$LIB_CHOICE" in
    1)
        CMAKE_LIB_FLAGS="-DREST_ALL_LIBS=ON"
        ;;
    2)
        CMAKE_LIB_FLAGS="-DRESTLIB_DETECTOR=ON -DRESTLIB_RAW=ON -DRESTLIB_TRACK=ON"
        ;;
    3)
        CMAKE_LIB_FLAGS=""
        for LIB in AXION CONNECTORS DETECTOR GEANT4 LEGACY RAW TRACK WIMP; do
            read -rp "  Enable RESTLIB_${LIB}? [y/N]: " LANS
            [[ "$LANS" =~ ^[Yy]$ ]] && CMAKE_LIB_FLAGS="$CMAKE_LIB_FLAGS -DRESTLIB_${LIB}=ON"
        done
        ;;
    *)
        warn "Invalid choice, defaulting to all libraries."
        CMAKE_LIB_FLAGS="-DREST_ALL_LIBS=ON"
        ;;
esac

# Always ask about Garfield and Geant4 integrations
echo ""
read -rp "Enable Garfield++ integration? (REST_GARFIELD) [Y/n]: " GARF_ANS
[[ ! "$GARF_ANS" =~ ^[Nn]$ ]] && CMAKE_LIB_FLAGS="$CMAKE_LIB_FLAGS -DREST_GARFIELD=ON"

read -rp "Enable Geant4 integration / restG4? (REST_G4) [Y/n]: " G4_ANS
[[ ! "$G4_ANS" =~ ^[Nn]$ ]] && CMAKE_LIB_FLAGS="$CMAKE_LIB_FLAGS -DREST_G4=ON"

info "CMake library flags: $CMAKE_LIB_FLAGS"
echo ""

# =============================================================================
#  STEP 4 -- Choose submodule update strategy
# =============================================================================
sep
ask "How do you want to pull REST submodules?"
echo "  1) (default)  --latest -- pull the latest commit from each submodule"
echo "                branch (newest features; may occasionally be momentarily"
echo "                incompatible across submodules)"
echo "  2) --clean    use the versions recorded in the framework repository"
echo "                (reproducible / guaranteed compatible -- try this if"
echo "                option 1 fails to build)"
echo ""
read -rp "Enter choice [1/2, default: 1]: " SUB_CHOICE
SUB_CHOICE="${SUB_CHOICE:-1}"

# =============================================================================
#  STEP 5 -- Clone / update REST framework
# =============================================================================
sep
if [[ -d "$REST_DIR/.git" ]]; then
    info "REST repository already exists at $REST_DIR, updating..."
    cd "$REST_DIR"
    git pull
else
    info "Cloning REST framework into $REST_DIR..."
    git clone https://github.com/rest-for-physics/framework.git "$REST_DIR"
    cd "$REST_DIR"
fi

if ! command -v python3 &>/dev/null; then
    error "python3 not found in PATH. Please install Python 3 and retry."
fi

info "Pulling submodules..."
# --onlylibs skips private university repos (iaxo, detector-template, etc.)
# that would hang waiting for SSH access most users don't have.
if [[ "$SUB_CHOICE" == "1" ]]; then
    python3 pull-submodules.py --latest --onlylibs
else
    # --clean asks "Are you sure?" -- answer automatically so it doesn't
    # consume the script's stdin.
    echo y | python3 pull-submodules.py --clean --onlylibs
fi

# restG4 lives in packages/, not libraries/, so --onlylibs skips it.
# Pull it explicitly if Geant4 integration / restG4 was requested.
if [[ "$CMAKE_LIB_FLAGS" == *"REST_G4=ON"* ]]; then
    info "Pulling restG4 package..."
    if [[ "$SUB_CHOICE" == "1" ]]; then
        python3 pull-submodules.py --latest --only:restG4
    else
        echo y | python3 pull-submodules.py --clean --only:restG4
    fi
fi
success "REST framework and submodules ready."
echo ""

# -- Compatibility checks on the cloned REST sources -------------------------
# Depending on the submodule choice, the libraries and restG4 are either the
# latest ones or the commits recorded in the framework (--clean), which can be
# years older. Instead of comparing version numbers,
# look for the code each relevant fix introduced. See "Compatibility checks"
# in README.md.
DET_DIR="$REST_DIR/source/libraries/detector"
RESTG4_DIR="$REST_DIR/source/packages/restG4"

# detectorlib supports the Garfield API of Garfield >= 4.0 (Component.hh).
# Present since March 2022.
detector_has_modern_garfield_api() {
    grep -qF '"Component.hh"' "$DET_DIR/inc/TRestDetectorGeometry.h" 2>/dev/null
}
# detectorlib#131 (July 2026): when Garfield is found through GARFIELD_HOME,
# use the legacy ComponentBase API only if Garfield actually ships it.
detector_selects_garfield_api() {
    grep -qF 'Garfield_INCLUDE_DIRS}/ComponentBase.hh' "$DET_DIR/CMakeLists.txt" 2>/dev/null
}
# restG4#150 (July 2026): accept a Geant4 built with C++17 or newer.
restg4_has_strict_cxx17_check() {
    grep -qF 'MATCHES "17")' "$RESTG4_DIR/CMakeLists.txt" 2>/dev/null
}

# -- Mode 2: build Garfield++ (only if Garfield integration was requested) ---
if [[ "$INSTALL_MODE" == "2" && "$CMAKE_LIB_FLAGS" == *"REST_GARFIELD=ON"* ]]; then
    sep
    if ! detector_has_modern_garfield_api; then
        error "The detector library in this REST checkout predates support for the\n       Garfield++ API used since Garfield 4.0 (2021), so it cannot be built with\n       any Garfield this installer provides. Re-run choosing '--latest'\n       submodules, or answer 'n' to the Garfield integration question."
    fi
    if [[ "$GARFIELD_REF" == "$GARFIELD_REF_DEFAULT" ]]; then
        info "Cloning and building Garfield++ (commit ${GARFIELD_REF:0:8}, as in LCG_109)..."
    else
        warn "Building Garfield++ at GARFIELD_REF=$GARFIELD_REF (overrides the default)."
    fi
    cd "$SOFTWARE_DIR"
    if [[ ! -d garfield ]]; then
        git clone https://gitlab.cern.ch/garfield/garfieldpp.git garfield
    fi
    cd garfield
    git checkout "$GARFIELD_REF"
    mkdir -p build
    cd build
    cmake .. \
        -DCMAKE_INSTALL_PREFIX=../install \
        -DCMAKE_C_COMPILER=gcc-11 \
        -DCMAKE_CXX_COMPILER=g++-11
    make -j"$NJOBS" install
    # Fix legacy HEED include paths
    cp -r "$SOFTWARE_DIR/garfield/Heed" "$SOFTWARE_DIR/garfield/install/"
    GARFIELD_SETUP="$SOFTWARE_DIR/garfield/install/share/Garfield/setupGarfield.sh"
    set +u
    source "$GARFIELD_SETUP"
    set -u
    ENV_LINES="$ENV_LINES
source $GARFIELD_SETUP"
    GARFIELD_BUILT="yes"
    success "Garfield++ installed."
    echo ""
fi

# -- Mode 3: create the Garfield shim now that REST_DIR exists ---------------
# Deferred from the MODE 3 setup block: REST_DIR had to be empty for the clone.
# Presents the CVMFS Garfield (include + lib64) in the lib/include layout REST
# expects via GARFIELD_HOME.
if [[ "$INSTALL_MODE" == "3" && -n "$GARFIELD_HOME" ]]; then
    mkdir -p "$GARFIELD_HOME"
    ln -sfn "$GV/include" "$GARFIELD_HOME/include"
    ln -sfn "$GV/$GLIB"   "$GARFIELD_HOME/lib"
    # Capital-I 'Include' (-> the lowercase include): lets cling resolve the
    # Garfield dictionary's autoload header paths (Include/Garfield/*.hh) and so
    # silences ~50 harmless "Missing FileEntry" warnings at restRoot startup.
    # (Used together with ROOT_INCLUDE_PATH below.)
    ln -sfn include "$GARFIELD_HOME/Include"
    success "Garfield shim ready: $GARFIELD_HOME -> $GV ($GLIB)"
fi

# -- Mode 3: modern-Garfield patch, only for detectorlib older than #131 -----
# CVMFS/LCG ships a modern Garfield, which renamed ComponentBase -> Component
# (header and class). Before detectorlib#131, the detector library always
# selected its USE_Garfield_OLD branch when Garfield is found through
# GARFIELD_HOME (as here: CVMFS Garfield has no CMake config), and that branch
# references the removed names. For such checkouts, patch the old-branch names
# to the modern ones. Checkouts that include #131 need nothing.
# This block becomes dead code once the detectorlib commit recorded in the
# framework includes #131; it can then be deleted.
if [[ "$INSTALL_MODE" == "3" && -n "$GARFIELD_HOME" ]]; then
    if detector_selects_garfield_api; then
        info "detectorlib selects the Garfield API itself (detectorlib#131): no patch needed."
    elif [[ -f "$DET_DIR/inc/TRestDetectorGeometry.h" ]]; then
        sed -i \
            -e 's|#include "ComponentBase.hh"|#include "Component.hh"|' \
            -e 's|typedef Garfield::ComponentBase Component;|typedef Garfield::Component Component;|' \
            "$DET_DIR/inc/TRestDetectorGeometry.h" \
            "$DET_DIR/inc/TRestDetectorGarfieldDriftProcess.h"
        info "detectorlib predates detectorlib#131: applied the modern-Garfield"
        info "(ComponentBase->Component) patch to its headers."
    fi
fi

# -- Mode 3: restG4 C++ check, only for restG4 older than #150 ---------------
# The LCG/CVMFS Geant4 11.4 (and ROOT 6.38) are built with C++20. Before
# restG4#150, restG4's CMake accepted only a C++17 Geant4. REST itself builds
# fine with C++20, so for such checkouts relax the check to also accept 20/23.
# Latest restG4 includes #150; this applies with --clean until the restG4 commit
# recorded in the framework includes #150, and the block can then be deleted.
if [[ "$INSTALL_MODE" == "3" && -f "$RESTG4_DIR/CMakeLists.txt" ]]; then
    if restg4_has_strict_cxx17_check; then
        sed -i 's#MATCHES "17")#MATCHES "17|20|23")#' "$RESTG4_DIR/CMakeLists.txt"
        info "restG4 predates restG4#150: relaxed its C++ standard check to allow"
        info "C++20 (CVMFS Geant4)."
    else
        info "restG4 accepts C++17 or newer (restG4#150): no patch needed."
    fi
fi

# =============================================================================
#  STEP 6 -- Configure and build
# =============================================================================
sep
BUILD_DIR="$REST_DIR/build"
INSTALL_DIR="$REST_DIR/install"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Clean stale CMakeCache if present
if [[ -f CMakeCache.txt ]]; then
    warn "Existing build directory found. Cleaning CMakeCache..."
    rm -f CMakeCache.txt
fi

info "Running cmake..."

# shellcheck disable=SC2086
cmake "$REST_DIR" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="$GCC_COMPAT_FLAGS" \
    $CMAKE_LIB_FLAGS $CMAKE_EXTRA_FLAGS

echo ""
info "Building REST with $NJOBS parallel jobs..."
info "This can take 15-40 minutes on a first build."
echo ""

if ! make -j"$NJOBS" install; then
    if [[ "$GARFIELD_BUILT" == "yes" && "$GARFIELD_REF" == "$GARFIELD_REF_DEFAULT" ]]; then
        warn "If the errors above mention Garfield, try re-running the installer"
        warn "with the previous default Garfield commit (tested with ROOT 6.26):"
        warn "  GARFIELD_REF=$GARFIELD_REF_PREVIOUS bash install_rest.sh"
    fi
    error "REST build failed. See the compiler output above."
fi
success "REST compiled and installed in $INSTALL_DIR"
echo ""

# =============================================================================
#  STEP 7 -- Test
# =============================================================================
sep
info "Testing installation..."
set +u
source "$INSTALL_DIR/thisREST.sh"
set -u
if command -v restRoot &>/dev/null; then
    success "restRoot is available -- installation successful!"
else
    warn "restRoot not found in PATH. Check the output above for errors."
fi
echo ""

# =============================================================================
#  STEP 8 -- Optional: add to .bashrc
# =============================================================================
sep
# Flush leftover keystrokes only when running interactively
[[ -t 0 ]] && { read -r -t 0.1 -n 10000 discard 2>/dev/null || true; }
ask "How do you want to set up your ~/.bashrc?"
echo "  1) Full setup    -- system environment + your local REST"
echo "                      (recommended for new users)"
echo "  2) System only   -- only add the system thisREST.sh"
echo "                      (you source your local REST manually when needed)"
echo "  3) No            -- do not touch ~/.bashrc"
echo ""
read -rp "Enter choice [1/2/3, default: 1]: " BASHRC_CHOICE
BASHRC_CHOICE="${BASHRC_CHOICE:-1}"

case "$BASHRC_CHOICE" in
    1|2)
        # ENV_LINES (set in STEP 2) loads the dependency environment for the
        # chosen mode. Full setup also sources the local REST; system-only
        # leaves it commented for the user to enable manually.
        if [[ "$BASHRC_CHOICE" == "1" ]]; then
            LOCAL_LINE="source $INSTALL_DIR/thisREST.sh   # your local REST install"
            # On NAF $HOME is AFS (not writable in-session); REST stores its
            # .rest dir under $REST_HOME, which thisREST.sh sets to $HOME -- so
            # override it AFTER sourcing, to a writable path.
            if [[ "$INSTALL_MODE" == "3" ]]; then
                LOCAL_LINE="$LOCAL_LINE
export REST_HOME=$REST_DIR   # REST .rest dir on writable disk (AFS \$HOME is not writable)"
                if [[ -n "$GARFIELD_HOME" ]]; then
                    LOCAL_LINE="$LOCAL_LINE
export ROOT_INCLUDE_PATH=$GARFIELD_HOME:\$ROOT_INCLUDE_PATH   # silence Garfield cling autoload warnings"
                fi
            fi
            CHECK_LINE="$INSTALL_DIR/thisREST.sh"
        else
            LOCAL_LINE="# source $INSTALL_DIR/thisREST.sh   # uncomment to use your local REST build"
            CHECK_LINE="REST-for-Physics"
            info "Your local REST is installed at: $INSTALL_DIR/thisREST.sh"
            info "Source it manually any time you want to switch to your local build."
        fi

        BASHRC_BLOCK="
# -- REST-for-Physics ---------------------------------------------------------
$ENV_LINES
$LOCAL_LINE
# -----------------------------------------------------------------------------"

        # The REST block goes in ~/.bashrc (canonical for interactive shells).
        RC_FILE="$HOME/.bashrc"
        if grep -qF "$CHECK_LINE" "$RC_FILE" 2>/dev/null; then
            warn "$RC_FILE already contains a REST entry -- skipping."
        else
            echo "$BASHRC_BLOCK" >> "$RC_FILE"
            success "Added REST setup to $RC_FILE."
        fi

        # Login shells (e.g. SSH on NAF) read a profile file, NOT ~/.bashrc, so
        # make sure the login profile sources ~/.bashrc -- otherwise REST would
        # not load on login. (Ubuntu's default ~/.profile already does this.)
        LOGIN_FILE=""
        for f in "$HOME/.bash_profile" "$HOME/.bash_login" "$HOME/.profile"; do
            [[ -f "$f" ]] && { LOGIN_FILE="$f"; break; }
        done
        [[ -z "$LOGIN_FILE" ]] && LOGIN_FILE="$HOME/.bash_profile"  # none exists: create standard one
        if grep -q '\.bashrc' "$LOGIN_FILE" 2>/dev/null; then
            info "$LOGIN_FILE already sources ~/.bashrc."
        else
            printf '\n# Source ~/.bashrc for login shells (added by REST installer)\n[ -f ~/.bashrc ] && . ~/.bashrc\n' >> "$LOGIN_FILE"
            success "Updated $LOGIN_FILE to source ~/.bashrc (needed for login shells, e.g. SSH)."
        fi

        # ROOT browser fix
        if [[ ! -f "$HOME/.rootrc" ]]; then
            echo "Browser.Name: TRootBrowser" > "$HOME/.rootrc"
            info "Created ~/.rootrc with TRootBrowser fix."
        fi
        ;;
    3)
        info "~/.bashrc left unchanged."
        info "To activate REST manually:"
        if [[ "$INSTALL_MODE" == "1" ]]; then
            info "  source $SYSTEM_THIS_REST"
        fi
        info "  source $INSTALL_DIR/thisREST.sh"
        ;;
    *)
        warn "Invalid choice, ~/.bashrc left unchanged."
        ;;
esac
echo ""

# =============================================================================
#  Done!
# =============================================================================
echo -e "${GREEN}${BOLD}"
echo "  +------------------------------------------------------------+"
echo "  |      REST-for-Physics installation complete!               |"
echo "  +------------------------------------------------------------+"
echo -e "${RESET}"
echo "  To activate REST (if not already in your ~/.bashrc):"
if [[ "$INSTALL_MODE" == "1" ]]; then
    echo "    source $SYSTEM_THIS_REST   # system ROOT/Geant4/Garfield"
elif [[ "$INSTALL_MODE" == "3" ]]; then
    echo "    source $LCG_VIEW_SETUP   # CVMFS ROOT/Geant4/Garfield"
    [[ -n "$GARFIELD_HOME" ]] && echo "    export GARFIELD_HOME=$GARFIELD_HOME"
fi
echo "    source $INSTALL_DIR/thisREST.sh   # your local REST build"
[[ "$INSTALL_MODE" == "3" ]] && echo "    export REST_HOME=$REST_DIR   # REST .rest dir (AFS \$HOME is not writable)"
echo ""
echo "  Quick test:    restRoot"
echo "  Docs:          https://rest-for-physics.github.io"
echo "  Forum:         https://rest-forum.unizar.es"
echo ""
echo -e "${BOLD}\033[0;35m"
echo "  +------------------------------------------------------+"
echo "  |                                                      |"
echo "  |      Congratulations! You have survived the          |"
echo "  |                 installation. :)                     |"
echo "  |                                                      |"
echo "  |      You are now ready to toil and suffer in         |"
echo "  |               more interesting ways.                 |"
echo "  |                                                      |"
echo "  |          Welcome to REST-for-Physics!                |"
echo "  |                                                      |"
echo "  +------------------------------------------------------+"
echo -e "${RESET}"
echo ""
