# Timmy Time Adventure

A retro platform game inspired by the gameplay of 8-bit console games and featuring the graphics of 16-bit games.

## Synopsis

Timmy is a little inventor, and his latest project is a time machine.

But things don't go as planned when he turns it on, and now Timmy is lost in different eras of history. He has to recover the four missing pieces of his machine to repair it and return to his own time.

Help Timmy succeed by traversing four levels set in different historical periods, overcoming numerous enemies and various traps.

## Requirements

Running the game:

- MS/DOS or compatible
- A DOS Protected Mode Interface, e.g. [CWSDPMI](http://sandmann.dotster.com/cwsdpmi/)
- 486DX2 66MHz (or better) with VGA
- At least 8MB of RAM
- Sound Blaster (or no sound)

## Development:

- GNU Make
- DJGPP C (used this cross-compile binaries [repo](https://github.com/andrewwutw/build-djgpp/releases)
- Allegro 4.2.3.1 (latest with DOS support)
- Vscode
- DOSBOX-X
- Tiled
- Aseprite
- Reaper

## Build:

You can build the game with `make release` (or compile from vscode) and run it in DOSBOX-X with `TIMMY.EXE`.

You need to edit the makefile `OS_INC_DIR`, `OS_GCC`, `OS_DOSBOX` vars with your actual paths.

If you use C/C++ vscode extension you may edit `c_cpp_properties.json` to point your compiler and library paths.

## Author

This game was made by Warrior (Warcom Soft.).