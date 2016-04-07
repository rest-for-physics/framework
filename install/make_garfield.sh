#!/bin/bash
WP=$PWD
set -e

source REST_v2/install/REST_ENV.sh
mkdir external/garfield -p
cd external/garfield
    echo checkout the code...
    svn co http://svn.cern.ch/guest/garfield/trunk $GARFIELD_HOME
    echo building...
    mkdir build -p
    cd build
        cmake ..
        make -j4
        mkdir ../lib -p
        cp lib/* ../lib 
cd $WP


