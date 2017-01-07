#!/bin/bash
set -e
WP=$PWD

t=$HOME/apps/root_v5.34.32
mkdir -p $t

cd $t

f=root_v5.34.32.source.tar.gz

if [ -f $f ]; then
    echo use cache build/$f
else
    wget https://root.cern.ch/download/$f
fi
tar -xzf $f

rm -rf source
mv root source

mkdir -p $HOME/apps/root_v5.34.32/build
cd $HOME/apps/root_v5.34.32/build

cmake -Dgdml=ON -DCMAKE_CXX_COMPILER=g++-4.8 -DCMAKE_C_COMPILER=gcc-4.8 -DCMAKE_INSTALL_PREFIX=$HOME/apps/root_v5.34.32/install  ../source
make -j4
make install

cd $WP

echo ""
echo "You should add the following line to your .bashrc file"
echo ""
echo "source $HOME/apps/root_v5.34.32/install/bin/thisroot.sh"
echo ""

