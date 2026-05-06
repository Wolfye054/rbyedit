#!/bin/bash

mkdir -p ../build
pushd ../build >> /dev/null
gcc ../source/rby-edit.c -o debug -g
popd >> /dev/null
