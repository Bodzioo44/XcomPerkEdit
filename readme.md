# XcomPerkEdit

Simple Qt app that allows you to edit soldier perks in Long War modification of a XCOM: Enemy Within game.
It has the same limitations as [xcomsave](https://github.com/tracktwo/xcomsave) library, so check out that repo for more info.
So far it supports Soldiers that are alive, and have already assigned rank above specialist.
Thanks to [tracktwo](github.com/tracktwo) for creating [xcomsave](https://github.com/tracktwo/xcomsave) library!

## Building from Source
- Qt binaries/CMake is required.
- use git clone --recursive to also include submodules.
- Checkout [README.md](https://github.com/tracktwo/xcomsave/blob/master/README.md) of xcomsave library on how to compile it.
  
### Linux
```
mkdir build
cd build
qmake -o Makefile ../XcomPerkEdit.pro
make
```
### Windows
You can either use Mingw (Qt Creator default) or MSVC to build the app, I used MSVC since xcomsave was already configured for that.
1. Compiling xcomsave:
  -Run CMake with the approperiate flag in the xcomsave build subdirectory: `cmake -G "Visual Studio 17 2022" ..`
    -Open generated solution in Visual Studio and compile it as Release.
2.Compiling XComPerkEdit:
  -.pro file is configured with MSVC flags.
    -You can use Qt Creator which will build the app for you.
      -Use qmake on .pro file to generate makefile, and use nmake/jom to compile it.


