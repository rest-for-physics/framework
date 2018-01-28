#!/bin/bash
set -e
WP=$PWD

t=$HOME/apps/root
mkdir -p $t

cd $t

f=root_v6.10.06.source.tar.gz

if [ -f $f ]; then
    echo use cache build/$f
else
    wget https://root.cern.ch/download/$f
fi
tar -xzf $f

rm -rf source
mv root source

mkdir -p $HOME/apps/root/build
cd $HOME/apps/root/build

cmake -Dgdml=ON -DCMAKE_CXX_COMPILER=g++-4.8 -DCMAKE_C_COMPILER=gcc-4.8 -DCMAKE_INSTALL_PREFIX=$HOME/apps/root/install  ../source
make -j4
make install

cd $WP

echo ""
echo "You should add the following line to your .bashrc file"
echo ""
echo "source \$HOME/apps/root/install/bin/thisroot.sh"
echo ""

