#!/bin/bash
set -e
WP=$PWD

t=$WP/external/root_v5.34.32
mkdir $t -p


f=root_v5.34.32.source.tar.gz
mkdir build -p
cd build

if [ -f $f ]; then
    echo use cache build/$f
else
    wget https://root.cern.ch/download/$f
fi
tar -xf $f

mkdir -p root_v5.34.32-build
cd root_v5.34.32-build
    cmake -Dgdml=ON -DCMAKE_INSTALL_PREFIX=$t  ../root
    make -j4
    make install
cd $WP

