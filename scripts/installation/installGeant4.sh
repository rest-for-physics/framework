#!/bin/bash

if [ $(geant4-config --version | grep "11.0.3") ];then

echo Geant4 of version 11.0.3 has already been installed
echo prefix:
echo $(geant4-config --prefix)
echo cflags:
echo $(geant4-config --cflags)

else

set -e
WP=$PWD

G4_ROOT=geant4-v11.0.3
G4_FILE=$G4_ROOT.tar.gz

echo installing: $G4_ROOT

mkdir -p $HOME/apps/
cd $HOME/apps/

if [ -f $G4_FILE ]; then
    echo "use build/$G4_FILE"
else
    wget http://cern.ch/geant4-data/releases/$G4_FILE
fi

tar xvf $G4_FILE

mkdir -p $G4_ROOT-build
cd $G4_ROOT-build
cmake ../$G4_ROOT -DGEANT4_USE_GDML=ON -DGEANT4_USE_QT=OFF -DCMAKE_INSTALL_PREFIX=$HOME/apps/$G4_ROOT-install -DGEANT4_INSTALL_DATA=ON -DGEANT4_INSTALL_DATA_TIMEOUT=7200 -DCMAKE_CXX_STANDARD=17
make -j30
make install
cd $WP
echo ""
echo "---------------------------------------------------------"
echo ""
echo "The following line has been added to your .bashrc to setup the Geant4 environment"
echo ""
echo "source \$HOME/apps/$G4_ROOT-install/bin/geant4.sh"
echo " " >> ~/.bashrc
echo "#Added by REST installGeant4.sh script to setup geant4 environment" >> ~/.bashrc
echo "source \$HOME/apps/$G4_ROOT-install/bin/geant4.sh" >> ~/.bashrc
echo " " >> ~/.bashrc
echo ""
echo "---------------------------------------------------------"

fi
