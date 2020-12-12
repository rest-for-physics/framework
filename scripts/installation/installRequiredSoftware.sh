#!/bin/bash
sudo apt-get update

sudo apt-get -y install vim zip unzip

sudo apt-get -y install cmake g++ gfortran
#sudo apt-get -y install g++-4.8 gcc-4.8 gfortran-4.8
sudo apt-get -y install libgl1-mesa-dev libglu1-mesa-dev
sudo apt-get -y install libfftw3-dev

# version control system
sudo apt-get -y install subversion git

# we install our version of root
#sudo apt-get install root-system

# garfield

# latex
sudo apt-get -y install texlive-latex-base

# root
sudo apt-get -y install libxpm-dev libxft-dev 

# geant4
sudo apt-get -y install expat libexpat1-dev zlib1g-dev libxerces-c3.2 libxerces-c-dev libicu-dev qt4-dev-tools qt4-doc qt4-qtconfig

#root 6.20 recommended packages
sudo apt-get install -y libtbb2 dpkg-dev cmake g++ gcc binutils libx11-dev libxpm-dev libxft-dev libxext-dev python libssl-dev gfortran libpcre3-dev xlibmesa-glu-dev libglew1.5-dev libftgl-dev libmysqlclient-dev libfftw3-dev libcfitsio-dev graphviz-dev libavahi-compat-libdnssd-dev libldap2-dev python-dev libxml2-dev libkrb5-dev libgsl0-dev wget git

