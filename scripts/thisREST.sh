#!/bin/bash

REST_DIR=$(cd "$(dirname "$0")"; pwd)
if [ ! -s $REST_DIR/lib/libRestCore.so ]; then
    REST_DIR=$(cd "$(dirname "$BASH_SOURCE")"; pwd)
fi

if [ ! -s "$REST_DIR/lib/libRestCore.so" ]; then
    echo "Can not find REST path."
	exit 1
fi

echo "REST path set to $REST_DIR"

alias restRoot="root -l $REST_DIR/LoadRESTScripts.C"

export REST_PATH=$REST_DIR
export PATH=$PATH:$REST_DIR/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$REST_DIR/lib
export LIBRARY_PATH=$LIBRARY_PATH:$REST_DIR/lib
