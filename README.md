# Timmy Time Adventure

A retro platform game inspired by the gameplay of 8-bit console games and featuring the graphics of 16-bit games.

## Synopsis

Timmy is a little inventor, and his latest project is a time machine.

But things don't go as planned when he turns it on, and now Timmy is lost in different eras of history. He has to recover the four missing pieces of his machine to repair it and return to his own time.

Help Timmy succeed by traversing four levels set in different historical periods, overcoming numerous enemies and various traps.

![alt text](https://img.itch.zone/aW1hZ2UvNDk2NDkyMS8yOTcxNDAyOS5wbmc=/347x500/a6W73Q.png)
![alt text](https://img.itch.zone/aW1hZ2UvNDk2NDkyMS8yOTcxNDAyOC5wbmc=/347x500/lG5FhR.png)
![https://img.itch.zone/aW1hZ2UvNDk2NDkyMS8yOTcxNDAzMS5wbmc=/347x500/OzIqrK.png](https://img.itch.zone/aW1hZ2UvNDk2NDkyMS8yOTcxNDAzMS5wbmc=/347x500/OzIqrK.png)
![alt text](https://img.itch.zone/aW1hZ2UvNDk2NDkyMS8yOTcxNDAzMC5wbmc=/347x500/uj5JOH.png)


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
- Python
- DOSBOX-X
- Tiled
- Aseprite
- Reaper

## Build:

You can build the game with `make release` (or compile from vscode) and run it in DOSBOX-X with `TIMMY.EXE`.

You need to edit the makefile `OS_INC_DIR`, `OS_GCC`, `OS_DOSBOX` vars with your actual paths.

If you use C/C++ vscode extension you may edit `c_cpp_properties.json` to point your compiler and library paths.

## Tooling:

* You need python (at least 3.10.11) for the tools needed to convert the tmx Tiled map to binary files.
* For resource dat files used by the game, has been used the Allegro dat replacement developed by [Jordi](https://github.com/jsmolina) [repo](https://github.com/jsmolina/allegro-dat-replacement)

## Author

This game was made by Warrior (Warcom Soft.).