# Super Mario Eclipse

Source code for compiling the Super Mario Eclipse mods using Clang and Kuribo

## Pre-compile

 - Build Clang using the modified source located at https://github.com/DotKuribo/llvm-project
 - Supply built Clang files into `src/compiler`
 - Supply the NTSC-U DOL file for SMS
 
## Compile

 - Following the arguments given by `build.py`, supply individual source files or a folder (source included), and optionally provide a destination and/or linker map (NTSC-U provided)
