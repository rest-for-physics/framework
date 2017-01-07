#!/bin/bash
WP=$PWD
set -e

export GARFIELD_HOME=$HOME/apps/garfield

mkdir -p $GARFIELD_HOME
cd $GARFIELD_HOME

echo checkout the code...
svn co http://svn.cern.ch/guest/garfield/trunk $GARFIELD_HOME

echo building...
mkdir build -p
cd build

cmake -DCMAKE_INSTALL_PREFIX=$GARFIELD_HOME ../
make -j4
mkdir ../lib -p

cp lib/* ../lib 

cd $WP

echo ""
echo "----------------------------------------------------------------------"
echo ""
echo "IMPORTANT : You should insert the following lines in your .bashrc file"
echo ""
echo "export GARFIELD_HOME=\$HOME/apps/garfield/"
echo "export HEED_DATABASE=\$GARFIELD_HOME/Heed/heed++/database"
echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\$GARFIELD_HOME/lib"
echo ""
echo "----------------------------------------------------------------------"
echo ""

