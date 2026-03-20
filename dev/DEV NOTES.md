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