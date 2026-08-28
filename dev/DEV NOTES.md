# Maps
* Map screen resolution: 256px x 160px (16 tiles width @ 10 tiles height)
* Auto scroll zone: 32px (2 tiles)

# Player
* Max tile jump width: 4 tiles
* Max tile jump height: 2 tiles

# Resources

* Game resources:       res/game/*.wav *mid *.bmp       -> GAME.DAT
* Player resources:     res/player/*.wav *mid *.bmp     -> PLAYER.DAT
* Object resources:     res/objects/*.wav *mid *.bmp    -> OBJECT.DAT
* Enemies resources:    res/enemies/*.wav *mid *.bmp    -> ENEMY.DAT
* Jurassic level:       res/levels/jurassic/.bmp *.mid  -> LVLJU.DAT
				        dev/maps/jurassic.tmx 		    -> LVLJU.BIN
# Build structure				 
	BUILD\DEBUG\BIN\
				GAME.EXE
				---.DAT
				---.BIN

# Menu structure
## Main Menu
	|--PLAY: go to play
	|--OPTIONS
		|--LANG: ENGLISH / ESPAÑOL
		|--CONTROLS--
					|--UP:
					|--DOWN:
					|--LEFT:
					|--RIGHT:
					|--ACTION:
					+--JUMP:
		|--SFX VOLUME: 0...100
		+--MUSIC VOLUME: 0...100
	+--EXIT: exit game
## In-game menu
	|--CONTINUE: go to play
	|--OPTIONS
		|--LANG: ENGLISH / ESPAÑOL
		|--CONTROLS--
					|--UP:
					|--DOWN:
					|--LEFT:
					|--RIGHT:
					|--ACTION:
					+--JUMP:
		|--SFX VOLUME: 0...100
		+--MUSIC VOLUME: 0...100
	|--EXIT TO TITLE: exit to tile
	+--EXIT TO DOS: exit game

## Test gameplay
* Automatically atack the enemies when jump or get more visual feedback when press action. It's not very visible and confuse (OK)
* Too difficult the perodayctile jump (OK)
* Less time to pick a stone (OK)
* Too much friction and inertia (OK = 0.92 to 0.88)
* Bug on time when repeat game overs (OK)
* reduce collision hitbox on hurt tiles (spikes, lava..) (OK = hit only with center down point)

* jurassic level: first spikes first gap when falls too small to jump (OK)
* too difficult first jump with piranha. Maybe a little more power on lateral jump ¿horizontal acceleration?
* hurt on wagon not back impulse?
* west: first ceil spikes on mine difficult jump (OK)
* bug: when fall to spikes, no second hurt (OK: must be solid and half tile)
* too much inertia!
* velocyraptor stops on front on you without attacking
* respawn lives to easy game?

## Puzzles
* Use two persistent solid objects to move trought spikes like floor is lava
* Egypt: some kind of puzzle with buttons and eyes symbol closes... (bring eye, bettle, symbol object and throw on each piece to unlock area)
* Egypt: more traps

## Debug objdump
`..\..\..\..\..\cross-compile\djgpp\bin\i586-pc-msdosdjgpp-objdump.exe -S -l -C game.exe > code.txt`

## Credits

* Programming: Warrior
* Graphics: Warrior
* Music: Warrior
* Sound: Warrior
* Level Design: Warrior
* Betatesting: Lolocop / Marc (onepopcorn) / Pedro (RandomShaper)

## End stadistics

* Play Time: 3h 30min 12seg
* Score: 12312
* HighScore: 12312
* Lives used: 23
* Lives remain: 12
* Continues used: 10