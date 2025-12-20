# XcomPerkEdit
Simple GUI app made with Qt that allows you to reassign soldier perks in Long War modification of a XCOM: Enemy Within game.
This is my first somewhat seriours project, so feedback on any matter will be greatly appreciated!
Thanks to [tracktwo](github.com/tracktwo) for creating [xcomsave](https://github.com/tracktwo/xcomsave) library.
<div align="center">
<img src="https://github.com/user-attachments/assets/f5049b07-1b06-46f6-b92d-64f0d70b3e2c">
<img src="https://github.com/user-attachments/assets/5505099d-0b1a-45da-a926-324ddda8dced">
<img src="https://github.com/user-attachments/assets/da8c1ede-daac-49fa-ba65-6e82a8efef7e">
</div>

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
## Building from Source
- Qt binaries/CMake is required.
- use `git clone --recursive` to also include submodules.
- Checkout [README.md](https://github.com/tracktwo/xcomsave/blob/master/README.md) of xcomsave library for more info on how to compile it.
### Linux
Requires GCC and GNU Make. Tested on Ubuntu 22.04.
1. Run `./Compile.sh` script from project directory.
### Windows
You can either use Mingw (Qt Creator default) or MSVC to build the app, I used MSVC since xcomsave was already configured for that. Tested on Windows 10 with Visual Studio 2022.
1. Building XComPerkEdit:
  - Run CMake with the approperiate flag in the xcomsave build subdirectory: 
  - 'cmake -S . -B build -G "Visual Studio 18 2026" -A x64'
  - 'cd build'
  - 'cmake --build . --config Release'
  - Use windeployqt.exe on XcomPerkEdit.exe to add required dll's


