# nes_emulator
A basic nes emulator. Not by any means accurate or even fast for that matter. The purpose of this was purely to learn a bit more about how computers work. I wanted to learn the inner workings of a CPU so I started this as a 6502 emulator, and got this to the point where I could run test roms. From there I started working on the PPU code, but quickly realised I was out of my depth. Most of the PPU code comes from OneLoneCoder who has an awesome youtube series on his nes emulator. 

The emulator has only one mapper (emulating the extra circruity found on the cartridge). Therefore it is only compatible with a select few games. I've managed to get Donkey Kong and Super Mario Bros working on this but both games have some glitches which can be worked out. There is no sound at the moment. 

The state of the code isn't great, it could be tidied up a lot, made more efficient. But that wasn't the purpose of this project so maybe one day this will get a refactor. Maybe later if sound and more mappers get added.

# Prerequisites

- C++ 17, to be compiled with G++
- SDL Version 2.0.14 
- SDL_TTF Version 2.0.15
- For Windows, the 32 bit version of SDL.dll and all dll files found in SDLTTf bin too (SDL2_ttf.dll, zlib1.dll, libfreetype-6.dll, etc). Place this into the same directory as nes_emulator.exe. Alternatively, you can place this into 32 bit DLL into your /System32 folder and the 64 bit DLL into the Syswow64 folder however I have noticed issues when doing this. If the application first encounters the 64 bit version in it's path, it can crash on startup. Therefore try to avoid doing this for the time being.  

# Compile instructions

For Linux, install SDL2 and SDL2_TTF as per the vendor instructions for your Linux distro.
For Windows 10, our makefile relies on using a symbolic links to link the project to the SDL library. Once you have downloaded SDL library for Mingw, create these with the following commands: 

>  ln -s /Path/To/SDL2/Lib/Folder lib/SDL2

>  ln -s /Path/To/SDL2/Include/Folder include/SDL2

My installation in Linux defaulted to putting the libs and includes from SDL2_TTF into the same folder as SDL2 libs and includes, so let's do the same in Windows. To install additional SDL libraries, simply download the SDK files and copy these files into your SDL2 lib and include folders. 

Once that's set up, simply use the makefile to compile to your chosen platform. 
> make

# Testing

Catch2 unit tests are available for this project. To compile the tests
> make tests

Then run the tests binary in the tests folder. 

# Usage

Simply execute the nes_emulator binary in the output folder. Make sure that you have the SDL.dll files available in the same folder, or within a path directory. 

# Acknowledgements

Thanks to the team of dedicated developers who maintain SDL and the various add on libraries,

Thanks to OLC for the awesome tutorials on how to build the PPU

Also thanks to Style64 for their amazing Commodore 64 style font. Found here: https://style64.org/c64-truetype

