# XcomPerkEdit
Simple GUI app made with Qt that allows you to reassign soldier perks in Long War modification of a XCOM: Enemy Within game.
This is my first somewhat seriours project, so feedback on any matter will be greatly appreciated!
Thanks to [tracktwo](github.com/tracktwo) for creating [xcomsave](https://github.com/tracktwo/xcomsave) library.
<div align="center">
<img src="https://github.com/user-attachments/assets/f5049b07-1b06-46f6-b92d-64f0d70b3e2c">
<img src="https://github.com/user-attachments/assets/5505099d-0b1a-45da-a926-324ddda8dced">
<img src="https://github.com/user-attachments/assets/0b53f544-bd31-4ac0-85d1-2797772b901b">
</div>

## Usage
 - Launch the app.
 - Update the path if you are not using default one.
 - Press "Load Path..." in the top right to select save directory.
 - Wait for the directory to finish loading, and choose a save to load it.
 - Select soldier to edit from the list on right side.
 - Reassign the perks.
 - Press "Save File" to update the save file.
## Features
- For now only vanilla [Long War](https://www.nexusmods.com/xcom/mods/88) modification of the XCOM: Enemy Within is supported.
- App has same limitations as [xcomsave](https://github.com/tracktwo/xcomsave) library, most importantly only geoscape saves are supported.
- You can only edit soldiers that are alive, and have already assigned rank above specialist.
- Only perks from already assigned ranks are editable.
- You can edit few soldiers at once, press "Save File" to write changes to save file.
- There is a backup directory with 10 most recent saves.
## Building from Source
- Qt binaries are required.
- C++17 is required.
- App was build using Qt6, but Qt5 should work too.
- use `gh repo clone Bodzioo44/XcomPerkEdit -- --recurse-submodules` to also include submodules.
### Linux
Requires GCC and GNU Make. Tested on Ubuntu 22.04.
1. Building XcomPerkEdit:
   - Download Qt binaries `sudo apt install qt6-base-dev` , or build from source (only qt-base module is needed)
   - Run CMake:
   - `cmake -S . -B build`
   - `cd build`
   - `cmake --build . `
### Windows
Requires preferably MSVC and CMake. It's possible to build using Qt Creator, but you would have to configure it to build xcomsave too. Tested on Windows 10 with Visual Studio 2022.
1. Building XComPerkEdit:
  - Download Qt binaries, or build from source (only qt-base module is needed)
  - Add Qt to CMake path: `setx CMAKE_PREFIX_PATH "C:\Qt\Qt-6.10.1`
  - Run CMake with the approperiate flag in the XcomPerkEdit directory: 
  - `cmake -S . -B build -G "Visual Studio 18 2026" -A x64`
  - `cd build`
  - `cmake --build . --config Release`
  - Use windeployqt.exe on XcomPerkEdit.exe to add required dll's inside Release folder.


