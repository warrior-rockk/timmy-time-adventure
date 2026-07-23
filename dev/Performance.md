486@66mhz:

fps platform persistent (20 visible entities):		34~35
fps platforms no persistent (7 visible entities): 	35


jurassic 56 entities: 70fps
jurassic 56 entities all background tiles: 35fps

medieval: 138 ent 							(34-35fps)
medieval: 138 ent without map_draw 			(60~70fps) <-
medieval: 138 ent without ent_draw 			(33~35fps)
medieval: 138 ent without ent_update 		(60~70fps) <-
medieval: 138 ent without map_draw front	(32~35fps) 
medieval: 138 ent map blit instead drawSpr	(47~50fps) <-relevant but drops to 33-35 when play

egypt: 173 ent


why items persistent?

860 862

# Profile

## max cpu
- gameUpdate  = 1ms
- gameDraw	= 13,18ms

- total = 14,2258ms = 70fps

## 486@66
- gameUpdate:               15,57ms
    - gameUpdateLevel:          5,64ms
        - entities_update:          5,64ms     
    - gameDrawLevel:            9,92ms
        - mapDraw(back)             8,87ms  / 3,6ms if more background
            -drawSprite             0,055ms x tile / blit without transparency: 0,03ms
        - mapDraw(front)            0,01ms
        - entitiesDraw              1,05ms

- gameDraw:	                13,12ms
    -blit-world-to-buffer       0,6ms
    -blit-buffer-to-screen      0,9ms
    -vsync                      11,1ms

- total:                    28,18ms = 35fps

# Analisis
Vsync ensures to sync with monitor trace and limits fps on high cpus but drops down fps on slower cpus because takes 11,1ms on 486@66.
If remove vsync, works well (sightly slow) on 486 and if adjust with deltaTime, it's near to perfect
