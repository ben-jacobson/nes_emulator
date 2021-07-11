# nes_emulator
This will eventually be worked into a nes emulator, in due time.

For the mean time, this is just a simple 6502 emulator, designed to output the virtual machines memory and allow you to step through code, displaying the output to screen.

# Prerequisites

- C++ 17, to be compiled with G++
- SDL Version 2.0.14 with symlinks established within the project directory, see below. Make sure to use the 32 bit / i686 version as we are compiling with Mingw32 

# Compile instructions

For Linux, smply install SDL2 as per the vendor instructions. 
For Windows 10, it's best to use a symbolic links to link the project to the SDL library. Once you have downloaded SDL library for Mingw, create these with the following commands: 

>  ln -s /Path/To/SDL2/Lib/Folder lib/SDL2
>  ln -s /Path/To/SDL2/Include/Folder include/SDL2

Once that's set up, simply use the makefile to compile to your chosen platform. 
> make

# Testing

Unit tests are available for this project, using Catch2. To compile the tests
> make tests

Then run the tests binary in the tests folder. 

# Usage

Simply execute the nes_emulator binary in the output folder


