#!/bin/bash
WP=$PWD
set -e
if [ $(root-config --version | grep "6.") ];then
	echo ROOT version is $(root-config --version)


if [ $GARFIELD_HOME ]; then
    echo Find defined garifeld env: $GARFIELD_HOME
else
    echo Creating garfield env: GARFIELD_HOME=~/garfield6
    export GARFIELD_HOME=~/garfield6
    echo "export GARFIELD_HOME=~/garfield6" >> ~/.bashrc
fi



if [ $HEED_DATABASE ]; then
    echo Find defined garifeld env: $HEED_DATABASE
else
    echo Creating garfield env: HEED_DATABASE=$GARFIELD_HOME/Heed/heed++/database
    export HEED_DATABASE=$GARFIELD_HOME/Heed/heed++/database
    echo "export HEED_DATABASE=\$GARFIELD_HOME/Heed/heed++/database" >> ~/.bashrc
fi


if [[ $LD_LIBRARY_PATH =~ "$GARFIELD_HOME/lib" ]]; then
    echo Find garfield lib path $GARFIELD_HOME/lib in LD_LIBRARY_PATH
else
    echo Adding garifeld lib path into env
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GARFIELD_HOME/lib
    echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\$GARFIELD_HOME/lib" >> ~/.bashrc
fi


rm -rf $GARFIELD_HOME/build

echo checkout the code...
svn co http://svn.cern.ch/guest/garfield/tags/v1r0 $GARFIELD_HOME
cp garfield6/CMakeLists.txt $GARFIELD_HOME/CMakeLists.txt

cd $GARFIELD_HOME


echo building...
mkdir build -p
cd build

cmake -DCMAKE_INSTALL_PREFIX=$GARFIELD_HOME -DWITH_EXAMPLES=OFF ../
make -j2
mkdir ../lib -p

cp lib/* ../lib 
cp rootdict/*.pcm ../lib

cd $WP

echo ""
echo "----------------------------------------------------------------------"
echo ""
echo "DONE!"
echo ""
echo "----------------------------------------------------------------------"
echo ""




else
	echo ROOT6 required, but not found!
	echo ROOT version is: $(root-config --version)
fi


