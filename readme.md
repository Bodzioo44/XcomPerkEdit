# XcomPerkEdit
Simple GUI app made with Qt that allows you to reassign soldier perks in Long War modification of a XCOM: Enemy Within game.
Thanks to [tracktwo](github.com/tracktwo) for creating [xcomsave](https://github.com/tracktwo/xcomsave) library!
## Usage
 - Launch the app.
 - Update the path if you are not using default one.
 - Press "Load Path..." in the top right to select save directory.
 - Wait for saves to load, and click any save to load it.
 - Select soldier to edit from the list on right side.
 - Reassign the perks.
 - Press "Save File" to update the save file.
## Features
- For now only [Long War](https://www.nexusmods.com/xcom/mods/88) modification of the XCOM: Enemy Within is supported.
- App has same limitations as [xcomsave](https://github.com/tracktwo/xcomsave) library, most importantly only geoscape saves are supported.
- You can only edit soldiers that are alive, and have already assigned rank above specialist.
- Only perks from already assigned ranks are editable.
- You can edit few soldiers at once, press "Save File" to write changes to save file.
- There is a backup directory with 10 most recent saves.
## Disclaimer
I'm a beginner dev, this is my first somewhat seriours project. I've never managed to corrupt any save files, but I encourage you to make your own backups anyway. Any feedback on any matter will be greatly appreciated!
## Building from Source
- Qt binaries/CMake is required.
- use `git clone --recursive` to also include submodules.
- Checkout [README.md](https://github.com/tracktwo/xcomsave/blob/master/README.md) of xcomsave library for more info on how to compile it.
### Linux
```
mkdir build
cd build
qmake -o Makefile ../XcomPerkEdit.pro
make
```
### Windows
You can either use Mingw (Qt Creator default) or MSVC to build the app, I used MSVC since xcomsave was already configured for that. Tested on Windows 10 with Visual Studio 2022.
1. Building xcomsave:
  - Run CMake with the approperiate flag in the xcomsave build subdirectory: `cmake -G "Visual Studio 17 2022" ..`
  - Open generated solution in Visual Studio and compile it as Release.
2. Building XComPerkEdit:
  - Use qmake on .pro file in the XComPerkEdit build subdirectory.
  - Use nmake/jom (or any other MSVC build tool) to compile it.


