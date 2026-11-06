#!/bin/bash
# System packages needed to build ROOT, Geant4, Garfield++ and REST on
# Ubuntu 22.04 / 24.04 (or WSL with either).
# Keep this list in sync with the mode 2 package list in install_rest.sh.
set -e

sudo apt-get update
sudo apt-get -y install \
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
