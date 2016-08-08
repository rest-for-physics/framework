#!/bin/bash

THIS_PATH_RAW=$(dirname ${BASH_ARGV[0]})
THIS_PATH=$(cd $THIS_PATH_RAW; pwd)
export REST_PATH=$(cd $THIS_PATH_RAW/..; pwd)
REST_PATH_PARENT=$(cd $REST_PATH/..; pwd)
#echo $REST_PATH_PARENT
echo "use      REST: $REST_PATH"

# setting environment varible for REST
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$REST_PATH/lib"
# restG4
export PATH=$PATH:$REST_PATH/packages/restG4-build

# garfield++
if [ -x $REST_PATH_PARENT/external/garfield ]; then
    export GARFIELD_HOME=$REST_PATH_PARENT/external/garfield
    echo "use  garfield: $GARFIELD_HOME"
    export HEED_DATABASE=$GARFIELD_HOME/Heed/heed++/database
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$GARFIELD_HOME/lib"
fi

# geant4
if [ -x $REST_PATH_PARENT/external/geant4_10_02_b01/share/Geant4-10.2.0/geant4make/geant4make.sh ]; then
	source $REST_PATH_PARENT/external/geant4_10_02_b01/share/Geant4-10.2.0/geant4make/geant4make.sh
    echo "use    geant4: $REST_PATH_PARENT/external/geant4_10_02_b01"
fi

# root
if [ -x $REST_PATH_PARENT/external/root_v5.34.32/bin/thisroot.sh ]; then
	echo "use      root: $REST_PATH_PARENT/external/root_v5.34.32"
	source $REST_PATH_PARENT/external/root_v5.34.32/bin/thisroot.sh 
fi

export REST_DATAPATH=$REST_PATH_PARENT/data
echo "default data path   $REST_DATAPATH"

export REST_CONFIG=$REST_PATH/examples/pandaXIII
echo "default config path $REST_CONFIG"

export REST_RUN=0
echo "default run number  $REST_RUN"

export REST_NEVENTS=1000
echo "default run events  $REST_NEVENTS"

