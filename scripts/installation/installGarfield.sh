#!/bin/bash
WP=$PWD
set -e
if [ $(root-config --version | grep "6.") ];then
	echo ROOT version is $(root-config --version)


if [ $GARFIELD_HOME ]; then
    echo Found defined garfield env: $GARFIELD_HOME
else
    echo Creating garfield env: GARFIELD_HOME=~/apps/garfield6
    export GARFIELD_HOME=~/apps/garfield6
    echo " ++++ GARFIELD_HOME has been added to your .bashrc"
    echo " " >> ~/.bashrc
    echo "#Added by REST installGarfield.sh script to setup garfield++ environment" >> ~/.bashrc
    echo "export GARFIELD_HOME=~/apps/garfield6" >> ~/.bashrc
fi



if [ $HEED_DATABASE ]; then
    echo Find defined garfield env: $HEED_DATABASE
else
    echo Creating garfield env: HEED_DATABASE=$GARFIELD_HOME/Heed/heed++/database
    export HEED_DATABASE=$GARFIELD_HOME/Heed/heed++/database
    echo " ++++ HEED_DATABASE has been added to your .bashrc"
    echo "export HEED_DATABASE=\$GARFIELD_HOME/Heed/heed++/database" >> ~/.bashrc
fi


if [[ $LD_LIBRARY_PATH =~ "$GARFIELD_HOME/lib" ]]; then
    echo Find garfield lib path $GARFIELD_HOME/lib in LD_LIBRARY_PATH
else
    echo Adding garfield lib path into env
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GARFIELD_HOME/lib
    echo " ++++ LD_LIBRARY_PATH has been added to your .bashrc"
    echo "export LD_LIBRARY_PATH=\$GARFIELD_HOME/lib:\$LD_LIBRARY_PATH" >> ~/.bashrc
fi
echo " " >> ~/.bashrc


rm -rf $GARFIELD_HOME/build

echo checkout the code...
#svn co http://svn.cern.ch/guest/garfield/tags/v1r0 $GARFIELD_HOME
git clone https://gitlab.cern.ch/garfield/garfieldpp.git $GARFIELD_HOME
cd $GARFIELD_HOME
git checkout d9a5bbf0


echo building...
mkdir build -p
cd build

cmake -DWITH_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=$GARFIELD_HOME ../
make -j2
mkdir ../lib -p

cp lib/* ../lib

# I add || true so that the script continues in case the pcm is not inside
# rootdict/. As it is the case in some Garfield versions, in that case it
# will be found already in lib/.
cp rootdict/*.pcm ../lib || true

cd $WP

echo ""
echo "----------------------------------------------------------------------"
echo ""
echo "Garfield has been installed at $GARFIELD_HOME"
echo ""
echo " -- Few lines were added to your .bashrc. Check them out! "
echo "----------------------------------------------------------------------"
echo ""

else
	echo ROOT6 required, but not found!
	echo ROOT version is: $(root-config --version)
fi
