# monocle.krk

[Monocle](https://github.com/UncraftedName/Monocle) bindings for [vkuroko](https://github.com/evanlin96069/vkuroko)

## Install
1. Download `vkuroko.dll` from [vkuroko](https://github.com/evanlin96069/vkuroko). Place it in `Source_Unpack\portal`
1. Create folder `Source_Unpack\portal\kuroko`.
1. Download `monocle.krk` and `monocle.dll`. Place them in the folder.
1. Open the game and run commands:
	- `plugin_load vkuroko`
	- `vkrk_run monocle`
1. Use `monocle_scan` to create the overlay portal image.

You can edit `monocle.krk` if needed. Use `vkrk_reset; vkrk_run monocle` to reload the script.

## Building

I'm using CMake with VS2022. When opening the project with Visual Studio, the CMakeSettings.json file should automatically set up 32-bit configurations.
