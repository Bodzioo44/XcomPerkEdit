# XcomPerkEdit

Simple Qt app that allows you to edit soldier perks in Long War modification of a XCOM: Enemy Within game.
It has the same limitations as [xcomsave](https://github.com/tracktwo/xcomsave) library, so check out that repo for more info.
So far it supports Soldiers that are alive, have assigned rank, and have rank above specialist.
Thanks to [tracktwo](github.com/tracktwo) for creating [xcomsave](https://github.com/tracktwo/xcomsave) library!

## Building from Source
- Qt binaries are required.
- XcomPerkEdit executable has to be in a subfolder of the main folder for proper assets loading.
- I'm probably doing something wrong, but app seems to work faster whenever I link xcomsave source files directly to executable, instead of linking pre-compiled xcomsave library.
### Linux
```
mkdir build
```
```
cd build
```
```
qmake ..
```
## Windows
Todo

