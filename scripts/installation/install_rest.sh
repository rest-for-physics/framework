#!/bin/bash
# =============================================================================
#  REST-for-Physics -- Interactive Installation Script
#  https://github.com/rest-for-physics/framework
#
#  Supports two modes:
#    1. Server install  -- ROOT/Geant4/Garfield already available (e.g. titan)
#    2. Full install    -- Builds ROOT 6.26.10, Geant4 11.0.3, Garfield++, REST
#                         from source (e.g. WSL / fresh Ubuntu)
#
#  Usage:  bash install_rest.sh
# =============================================================================

# Self-fix Windows line endings before anything else
grep -q $'\r' "$0" 2>/dev/null && sed -i 's/\r//' "$0" && exec bash "$0" "$@"

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

# =============================================================================
#  STEP 0 -- Choose install mode
# =============================================================================
sep
ask "Choose installation mode:"
echo "  1) Server install  -- use pre-installed ROOT/Geant4/Garfield on this machine"
echo "     (for clusters like titan where a system REST environment exists)"
echo "  2) Full install    -- build ROOT, Geant4, Garfield++ and REST from source"
echo "     (for a fresh Ubuntu / WSL system)"
echo ""
read -rp "Enter choice [1/2]: " INSTALL_MODE

case "$INSTALL_MODE" in
    1) info "Mode: Server install (using pre-built dependencies)" ;;
    2) info "Mode: Full install from source" ;;
    *) error "Invalid choice. Please run again and enter 1 or 2." ;;
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

if [[ -d "$REST_DIR" ]]; then
    warn "Directory $REST_DIR already exists."
    read -rp "Continue and use it anyway? [y/N]: " CONFIRM
    [[ "$CONFIRM" =~ ^[Yy]$ ]] || error "Aborted."
fi

# =============================================================================
#  MODE 2 -- Full install: check/install system dependencies
# =============================================================================
if [[ "$INSTALL_MODE" == "2" ]]; then

    sep
    # Check for sudo access before attempting anything
    SKIP_APT=false
    if ! sudo -v 2>/dev/null; then
        warn "Cannot obtain sudo access."
        warn "If system packages are already installed, you can skip this step."
        read -rp "Skip apt package installation? [y/N]: " SKIP_APT_ANS
        if [[ "$SKIP_APT_ANS" =~ ^[Yy]$ ]]; then
            SKIP_APT=true
            info "Skipping apt package installation."
        else
            error "Mode 2 requires sudo access to install system packages.\n       On shared servers (e.g. titan), use mode 1 instead."
        fi
    fi

    if [[ "$SKIP_APT" == false ]]; then
        warn "This package list was designed for Ubuntu 22/24 with ROOT 6.26.10,"
        warn "Geant4 11.0.3 and Garfield++. It has not been fully tested on a"
        warn "fresh system yet. If something fails, a package may be missing or"
        warn "named differently on your Ubuntu version."
        read -rp "Press Enter to continue or Ctrl+C to abort: "
        echo ""
        info "Installing system dependencies (requires sudo)..."
        sudo apt update && sudo apt install -y \
            build-essential cmake git wget curl \
            gcc-11 g++-11 gfortran \
            python3 python3-full python3-pip python3-numpy \
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
            software-properties-common

        # Python 3.10 for ROOT 6.26 compatibility
        info "Adding deadsnakes PPA and installing Python 3.10..."
        sudo add-apt-repository ppa:deadsnakes/ppa -y
        sudo apt update
        sudo apt install -y python3.10 python3.10-dev python3.10-venv
        success "System dependencies installed."
    fi
    echo ""

    SOFTWARE_DIR="$REST_DIR/../software"
    SOFTWARE_DIR="$(realpath -m "$SOFTWARE_DIR")"
    mkdir -p "$SOFTWARE_DIR"
    info "Dependencies will be built in: $SOFTWARE_DIR"

    # -- ROOT 6.26.10 --------------------------------------------------------
    sep
    info "Downloading and building ROOT 6.26.10 (this takes a while)..."
    cd "$SOFTWARE_DIR"
    if [[ ! -f root_v6.26.10.source.tar.gz ]]; then
        wget https://root.cern/download/root_v6.26.10.source.tar.gz
    fi
    if [[ ! -d root-6.26.10 ]]; then
        tar xzf root_v6.26.10.source.tar.gz
    else
        info "ROOT source directory already exists, skipping extraction."
    fi
    cd root-6.26.10
    mkdir -p root_build root_install
    cd root_build
    cmake .. \
        -DCMAKE_INSTALL_PREFIX=../root_install \
        -DPython3_EXECUTABLE=/usr/bin/python3.10 \
        -DCMAKE_C_COMPILER=gcc-11 \
        -DCMAKE_CXX_COMPILER=g++-11 \
        -DCMAKE_CXX_STANDARD=17 \
        -Dnetxng=OFF -Dxrootd=OFF
    make -j"$(nproc)" install
    ROOT_THISROOT="$SOFTWARE_DIR/root-6.26.10/root_install/bin/thisroot.sh"
    set +u
    source "$ROOT_THISROOT"
    set -u
    success "ROOT 6.26.10 installed."
    echo ""

    # -- Geant4 11.0.3 -------------------------------------------------------
    sep
    info "Downloading and building Geant4 11.0.3..."
    cd "$SOFTWARE_DIR"
    if [[ ! -f geant4-v11.0.3.tar.gz ]]; then
        wget https://gitlab.cern.ch/geant4/geant4/-/archive/v11.0.3/geant4-v11.0.3.tar.gz
    fi
    if [[ ! -d geant4-v11.0.3 ]]; then
        tar xzf geant4-v11.0.3.tar.gz
    else
        info "Geant4 source directory already exists, skipping extraction."
    fi
    cd geant4-v11.0.3
    mkdir -p build install
    cd build
    cmake .. \
        -DCMAKE_INSTALL_PREFIX=../install \
        -DGEANT4_INSTALL_DATA=ON \
        -DGEANT4_USE_OPENGL_X11=ON \
        -DGEANT4_BUILD_MULTITHREADED=ON \
        -DCMAKE_C_COMPILER=gcc-11 \
        -DCMAKE_CXX_COMPILER=g++-11 \
        -DGEANT4_USE_GDML=ON
    make -j"$(nproc)" install
    # Fix ownership if files ended up root-owned (e.g. from a previous sudo run)
    if [[ -O "$SOFTWARE_DIR/geant4-v11.0.3/install" ]]; then
        : # Already owned by current user, nothing to do
    else
        sudo chown -R "$(whoami):$(whoami)" "$SOFTWARE_DIR/geant4-v11.0.3/install"
    fi
    GEANT4_SH="$SOFTWARE_DIR/geant4-v11.0.3/install/bin/geant4.sh"
    set +u
    source "$GEANT4_SH"
    set -u
    success "Geant4 11.0.3 installed."
    echo ""

    # -- Garfield++ ----------------------------------------------------------
    sep
    info "Cloning and building Garfield++ (pinned commit for REST compatibility)..."
    cd "$SOFTWARE_DIR"
    if [[ ! -d garfield ]]; then
        git clone https://gitlab.cern.ch/garfield/garfieldpp.git garfield
    fi
    cd garfield
    git checkout a993c5eb015e23aa4bccdd1bdda027314afc2153
    mkdir -p build
    cd build
    cmake .. \
        -DCMAKE_INSTALL_PREFIX=../install \
        -DCMAKE_C_COMPILER=gcc-11 \
        -DCMAKE_CXX_COMPILER=g++-11
    make -j"$(nproc)" install
    # Fix legacy HEED include paths
    cp -r "$SOFTWARE_DIR/garfield/Heed" "$SOFTWARE_DIR/garfield/install/"
    GARFIELD_SETUP="$SOFTWARE_DIR/garfield/install/share/Garfield/setupGarfield.sh"
    success "Garfield++ installed."
    echo ""

fi  # end MODE 2 dependencies

# =============================================================================
#  STEP 2 -- Source the environment (mode 1: system; mode 2: freshly built)
# =============================================================================
sep

if [[ "$INSTALL_MODE" == "1" ]]; then
    ask "Enter the path to the system thisREST.sh to load ROOT/Geant4/Garfield:"
    echo "  Example: /programas/titan/rest/latest/thisREST.sh"
    read -rp "Path: " SYSTEM_THIS_REST
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
else
    # Already sourced above during build
    set +u
    source "$ROOT_THISROOT"
    source "$GEANT4_SH"
    source "$GARFIELD_SETUP"
    set -u
    success "Build environment loaded."
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
echo "  1) --latest   Pull the latest commit from each submodule branch"
echo "               (may occasionally be incompatible but stays up to date)"
echo "  2) (default)  Use the version recorded in the framework repository"
echo "               (safer, guaranteed compatible)"
echo ""
read -rp "Enter choice [1/2, default: 2]: " SUB_CHOICE
SUB_CHOICE="${SUB_CHOICE:-2}"

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
    if [[ -d "$REST_DIR" && "$(ls -A "$REST_DIR" 2>/dev/null)" ]]; then
        # Directory exists and is non-empty; clone to temp then move contents
        TMPCLONE="$(mktemp -d)"
        git clone https://github.com/rest-for-physics/framework.git "$TMPCLONE/framework"
        shopt -s dotglob
        mv "$TMPCLONE/framework"/* "$REST_DIR/"
        shopt -u dotglob
        rm -rf "$TMPCLONE"
    else
        git clone https://github.com/rest-for-physics/framework.git "$REST_DIR"
    fi
    cd "$REST_DIR"
fi

if ! command -v python3 &>/dev/null; then
    error "python3 not found in PATH. Please install Python 3 and retry."
fi

info "Pulling submodules..."
# --onlylibs skips private university repos (iaxo, detector-template, etc.)
# that would hang waiting for SSH access most users don't have.
# If REST_G4 is enabled, also pull the restG4 package explicitly.
if [[ "$SUB_CHOICE" == "1" ]]; then
    python3 "$REST_DIR/pull-submodules.py" --latest --onlylibs
else
    # --clean asks "Are you sure?" — answer automatically to avoid consuming
    # the main script's stdin when running non-interactively.
    echo y | python3 "$REST_DIR/pull-submodules.py" --clean --onlylibs
fi

# restG4 lives in packages/, not libraries/, so --onlylibs skips it.
# Pull it explicitly if Geant4 integration was requested.
if [[ "$CMAKE_LIB_FLAGS" == *"REST_G4=ON"* ]]; then
    info "Pulling restG4 package..."
    echo y | python3 "$REST_DIR/pull-submodules.py" --clean --only:restG4
fi
success "REST framework and submodules ready."
echo ""

# =============================================================================
#  STEP 6 -- Configure and build
# =============================================================================
sep
ask "How many parallel jobs for make? (be careful on shared servers)"
NCPU=$(nproc 2>/dev/null || echo 4)
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
    $CMAKE_LIB_FLAGS

echo ""
info "Building REST with $NJOBS parallel jobs..."
info "This can take 15-40 minutes on a first build."
echo ""

make -j"$NJOBS" install
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
# Flush any leftover input from the build output before asking
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
        if [[ "$BASHRC_CHOICE" == "1" ]]; then
            # Full setup: system env + local REST
            if [[ "$INSTALL_MODE" == "1" ]]; then
                BASHRC_BLOCK="
# -- REST-for-Physics ---------------------------------------------------------
source \"$SYSTEM_THIS_REST\"   # loads system ROOT / Geant4 / Garfield
source \"$INSTALL_DIR/thisREST.sh\"   # your local REST install
# -----------------------------------------------------------------------------"
            else
                BASHRC_BLOCK="
# -- REST-for-Physics ---------------------------------------------------------
source \"$ROOT_THISROOT\"
source \"$GEANT4_SH\"
source \"$GARFIELD_SETUP\"
source \"$INSTALL_DIR/thisREST.sh\"   # your local REST install
# -----------------------------------------------------------------------------"
            fi
            CHECK_LINE="$INSTALL_DIR/thisREST.sh"
        else
            # System only
            if [[ "$INSTALL_MODE" == "1" ]]; then
                BASHRC_BLOCK="
# -- REST-for-Physics ---------------------------------------------------------
source \"$SYSTEM_THIS_REST\"   # loads system ROOT / Geant4 / Garfield
# source \"$INSTALL_DIR/thisREST.sh\"   # uncomment to use your local REST build
# -----------------------------------------------------------------------------"
            else
                BASHRC_BLOCK="
# -- REST-for-Physics ---------------------------------------------------------
source \"$ROOT_THISROOT\"
source \"$GEANT4_SH\"
source \"$GARFIELD_SETUP\"
# source \"$INSTALL_DIR/thisREST.sh\"   # uncomment to use your local REST build
# -----------------------------------------------------------------------------"
            fi
            CHECK_LINE="REST-for-Physics"
            info "Your local REST is installed at: $INSTALL_DIR/thisREST.sh"
            info "Source it manually any time you want to switch to your local build."
        fi

        # Avoid duplicate entries
        if grep -qF "$CHECK_LINE" ~/.bashrc 2>/dev/null; then
            warn "~/.bashrc already contains a REST entry -- skipping."
        else
            echo "$BASHRC_BLOCK" >> ~/.bashrc
            success "Added to ~/.bashrc."
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
fi
echo "    source $INSTALL_DIR/thisREST.sh   # your local REST build"
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