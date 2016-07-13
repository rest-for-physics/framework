#!/bin/bash

REST_DIR=$(cd "$(dirname "$0")"; pwd)

export PATH=$PATH:$REST_DIR/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$REST_DIR/lib
export LIBRARY_PATH=$LIBRARY_PATH:$REST_DIR/lib