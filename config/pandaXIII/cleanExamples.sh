#!/bin/bash

declare -a dirs=( "micromegas1" "vessel" "electronics" "supporters" "rings" "gas" "resistive" "external" "waterTank" "external" )

for dir in "${dirs[@]}"
do
    if [[ -d $dir ]]; then
        echo "Removing $dir"
        rm -rf $dir
    fi
done
