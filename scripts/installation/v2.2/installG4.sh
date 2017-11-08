#!/bin/bash

if [ $(geant4-config --version | grep "10.2.2") ];then

echo Geant4 of version 10.2.2 has already been installed
echo prefix:
echo $(geant4-config --prefix)
echo cflags:
echo $(geant4-config --cflags)

else

set -e
WP=$PWD

G4_FILE=geant4_10_02_p02
echo installing: $G4_FILE

mkdir -p $HOME/apps/
cd $HOME/apps/

if [ -f $G4_FILE.zip ]; then
    echo "use build/$G4_FILE.zip"
else
    wget http://geant4.cern.ch/support/source/$G4_FILE.zip
fi

unzip -q -u $G4_FILE.zip

mkdir $G4_FILE-build -p
cd $G4_FILE-build
cmake ../$G4_FILE -DGEANT4_USE_GDML=ON  -DGEANT4_USE_QT=ON -DCMAKE_INSTALL_PREFIX=$HOME/apps/$G4_FILE-install -DGEANT4_INSTALL_DATA=ON -DGEANT4_INSTALL_DATA_TIMEOUT=7200
make -j4
make install
cd $WP
echo ""
echo "---------------------------------------------------------"
echo ""
echo "You should add the following line to your .bashrc to setup the Geant4 environment"
echo ""
echo "source \$HOME/apps/geant4_10_02_p02-install/bin/geant4.sh"
echo ""
echo "---------------------------------------------------------"



fi