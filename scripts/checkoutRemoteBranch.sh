#!/bin/bash

echo "Fetching repository"
git fetch

var=$(git ls-remote --heads origin ${1})

if [[ -z $var ]]; then
   echo "Checking out master"
   git checkout master
   git pull
else
  echo "Checking out ${1}"
  git checkout ${1}
  git pull
fi

