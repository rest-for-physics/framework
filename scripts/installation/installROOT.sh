#!/bin/bash
set -e

CURRENT_DIR=$PWD

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    machine=Cygwin;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac

ROOT_VERSION=6.20.00
ROOT_DIR=$HOME/apps/root-$ROOT_VERSION

mkdir -p ${ROOT_DIR}
cd ${ROOT_DIR}

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

mkdir -p ${ROOT_DIR}/build
cd ${ROOT_DIR}/build

cmake -Dgdml=ON -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/install  ../source
make -j4
make install

cd ${CURRENT_DIR}

echo ""
echo "The following lines were added to your .bashrc file"
echo ""
echo "#Added by REST installROOT.sh script to setup ROOT environment" >> ~/.bashrc
echo "source ${ROOT_DIR}/install/bin/thisroot.sh"
echo "source ${ROOT_DIR}/install/bin/thisroot.sh" >> ~/.bashrc
echo ""
