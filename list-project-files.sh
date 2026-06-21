#!/bin/bash

# list all the files from a subproject
# @param 1 : sub project

if [ $# -lt 1 ]; then
  echo "Usage: open-project.sh <sub_folder>"
  echo
  echo "e.g.: open-project.sh cmnlib-io"
  exit -1
fi

echo "Listing files from '$1'..."

find ./$1 -type f | grep -v "/.git/" | grep -v ".git\$"| grep -v "/build/"
