#!/bin/bash

REST_DIR=$(cd "$(dirname "$0")"; pwd)
if [ ! -d $REST_DIR/lib/libRestCore.so ]; then
    REST_DIR=$(cd "$(dirname "$BASH_SOURCE")"; pwd)
fi

if [ ! -d $REST_DIR/lib/libRestCore.so ]; then
    echo "Can not find REST path."
    exit 1
fi
export PATH=$PATH:$REST_DIR/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$REST_DIR/lib
export LIBRARY_PATH=$LIBRARY_PATH:$REST_DIR/lib
