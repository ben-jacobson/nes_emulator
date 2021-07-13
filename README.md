# nes_emulator
This will eventually be worked into a nes emulator, in due time. For the mean time, this is just a simple 6502 emulator, designed to output the virtual machines memory and allow you to step through code, displaying the output to screen.

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

My installation in Linux defaulted to putting the libs and includes from SDL2_TTF into the same folder as SDL2, so let's do the same in Windows. Simply download SDL_TTF files and copy these files into your SDL2 lib and include folders.

Once that's set up, simply use the makefile to compile to your chosen platform. 
> make

# Testing

Catch2 unit tests are available for this project. To compile the tests
> make tests

Then run the tests binary in the tests folder. 

# Usage

Simply execute the nes_emulator binary in the output folder. Make sure that you have the SDL.dll files available in the same folder, or within a path directory. 