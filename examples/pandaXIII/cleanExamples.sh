#!/bin/bash

declare -a dirs=( "micromegas1" "vessel" "electronics" "supports" "rings" "gas" "resistive" )

for dir in "${dirs[@]}"
do
    if [[ -d $dir ]]; then
        echo "Removing $dir"
        rm -rf $dir
    fi
done
