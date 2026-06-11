#!/bin/bash


mkdir -p ../build
pushd ../build >> /dev/null
gcc ../source/gtk_rbyedit.c -g -o debug $(pkg-config --cflags --libs gtk4)
popd >> /dev/null
