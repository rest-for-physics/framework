#!/bin/bash

declare -a dirs=( "micromegas1" "vessel" "electronics" "supports" "rings" "gas" "resistive" "external" "waterTank" )

for dir in "${dirs[@]}"
do
    if [[ -d $dir ]]; then
        echo "Removing $dir"
        rm -rf $dir
    fi
done
