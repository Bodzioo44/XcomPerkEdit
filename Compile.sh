#!/bin/bash
#compile xcomsave
cd xcomsave
mkdir build
cd build
cmake ..
make
cd ..
cd ..
#compile xcomperkedit
mkdir build
cd build
qmake CONFIG+=release -o Makefile ../XcomPerkEdit.pro
make
