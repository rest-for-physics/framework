#!/bin/bash
WP=$PWD
set -e
if [ $(root-config --version | grep "6.") ];then
	echo ROOT version is $(root-config --version)


if [ $GARFIELD_HOME ]; then
    echo "Found defined garfield env: $GARFIELD_HOME"
    echo "GARFIELD_HOME should not be defined anymore."
    echo "We now use GARFIELD_INSTALL"
    echo "Please execute \"unset GARFIELD_HOME\"."
    echo "And try again!"
    exit 0
fi

if [ $GARFIELD_INSTALL ]; then
    echo Found defined garfield env: $GARFIELD_INSTALL
    echo "It seems that a previous intallation of garfield already exists"
    echo "If you want to overwrite the installation, just unset GARFIELD_INSTALL"
    echo "Please execute \"unset GARFIELD_INSTALL\"."
    echo "The re-launch this script"
    exit 0
else
    echo Creating garfield env: GARFIELD_INSTALL=~/apps/garfield6
    export GARFIELD_INSTALL=~/apps/garfield6
fi

rm -rf $GARFIELD_INSTALL/build

echo checkout the code...
git clone https://gitlab.cern.ch/garfield/garfieldpp.git $GARFIELD_INSTALL
cd $GARFIELD_INSTALL
git checkout tags/4.0
git checkout -b v4.0

echo building...
mkdir -p build
cd build

cmake -DWITH_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=$GARFIELD_INSTALL ../
make -j2 install

# I add || true so that the script continues in case the pcm is not inside
# rootdict/. As it is the case in some Garfield versions, in that case it
# will be found already in lib/.
#cp rootdict/*.pcm $GARFIELD_HOME/lib || true

cd $WP

echo ""
echo "----------------------------------------------------------------------"
echo ""
echo "Garfield has been installed at $GARFIELD_INSTALL"
echo ""
echo " -- You will need to load Garfield just before compiling REST         "
echo " "
echo " Executing: source ~/apps/garfield6/share/Garfield/setupGarfield.sh     "
echo " "
echo " "
echo " -- When you launch cmake during the REST compilation remember to enable Garfield"
echo " "
echo " Executing: cmake -DREST_GARFIELD=ON .."
echo " "
echo "----------------------------------------------------------------------"
echo ""

else
	echo ROOT6 required, but not found!
	echo ROOT version is: $(root-config --version)
fi
