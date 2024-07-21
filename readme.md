XcomPerkEdit

simple Qt app that allows you to edit soldier perks in Long War modification of a XCOM: Enemy Within game.
It has the same limitations as xcomsave library, so check out that repo for more info.
So far it supports Soldiers that are alive, have assigned rank, and have rank above specialist.
Thanks to tracktwo for creating xcomsave library!

Building from Source:

linux:
mkdir build
cd build
qmake -o Makefile ../XcomPerkEdit.pro

XcomPerkEdit executable has to be in a subfolder of the main folder for proper assets loading.

I'm probably doing something wrong, but whenever I compile xcomsave source files inside my .pro file, It seems to work faster than including pre-compiled xcomsave library.