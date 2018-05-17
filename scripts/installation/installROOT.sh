#!/bin/bash
set -e
WP=$PWD

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    machine=Cygwin;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac

ROOT_VERSION=6.10.06

t=$HOME/apps/root-$ROOT_VERSION
mkdir -p $t

cd $t

f=root_v$ROOT_VERSION.source.tar.gz

if [ -f $f ]; then
    echo use cache build/$f
else
	if [ $machine == "Mac" ]; then
	    curl https://root.cern.ch/download/$f -o $f
	else
	    wget https://root.cern.ch/download/$f
	fi
fi
tar -xvzf $f

rm -rf source
mv root-$ROOT_VERSION source

mkdir -p $HOME/apps/root-$ROOT_VERSION/build
cd $HOME/apps/root-$ROOT_VERSION/build

cmake -Dgdml=ON -DCMAKE_INSTALL_PREFIX=$HOME/apps/root-$ROOT_VERSION/install  ../source
make -j4
make install

cd $WP

echo ""
echo "The following line was added to your .bashrc file"
echo ""
echo "#Added by REST installROOT.sh script to setup ROOT environment" >> ~/.bashrc
echo "source \$HOME/apps/root-$ROOT_VERSION/install/bin/thisroot.sh"
echo "source \$HOME/apps/root-$ROOT_VERSION/install/bin/thisroot.sh" >> ~/.bashrc
echo ""

