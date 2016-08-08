#!/bin/bash

REST_DIR=$(cd "$(dirname "$0")"; pwd)
if [ ! -s $REST_DIR/lib/libRestCore.so ]; then
    REST_DIR=$(cd "$(dirname "$BASH_SOURCE")"; pwd)
fi

if [ ! -s "$REST_DIR/lib/libRestCore.so" ]; then
    echo "Can not find REST path."
	exit 1
fi

alias restRoot="root -l $REST_DIR/LoadRESTScripts.C"

export REST_PATH=$REST_DIR
export PATH=$PATH:$REST_DIR/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$REST_DIR/lib
export LIBRARY_PATH=$LIBRARY_PATH:$REST_DIR/lib

echo "                                                    "
echo "  **************************************************"
echo "              W E L C O M E   to  R E S T           "
echo "                  Environmental set-up              "
echo "                                                    "
echo "        Version : untagged     5th August 2016      "
echo "                                                    "
echo "        REST_PATH = $REST_PATH                      "
echo "                                                    "
echo "        Use restRoot command to launch ROOT with    "
echo "        REST libraries and scripts loaded           "
echo "                                                    "
echo "     REST Support mail-list : rest-support@cern.ch  "
echo "     REST Development mail-list : rest-dev@cern.ch  "
echo "                                                    "
echo "         You can subscribe at egroups.cern.ch       "
echo "                                                    "
echo "         If you have no CERN account you can        "
echo "         get an account linked to your usual        "
echo "             mail address at this site:             "
echo "                                                    "
echo "      https://account.cern.ch/account/Externals/    "
echo "  **************************************************"
echo "                                                    "
