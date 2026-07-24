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
If remove vsync, works well (sightly slow) on 486 and if adjust with deltaTime, it's near to perfect but too fast on faster cpus

Not difference using fixed point, floats or double

## Blit back tiles instead draw_sprite
Reducing time with blit not front tiles and deltaTime adjustement makes game playable. Continue optimizing the code to reduce ms processing

## No debug traces
Boost performance on release version

## Minimal collision points to simple entities
Only the player needs all collision points. Testing this:
- 20,5ms  min collision points
- 24,4ms  all collision points
- Entities that uses a fixed collision point (need to change to a point enabled on minimal):
    - spider 
    - eagle 
    - firedrop

    - object_fall
    - spike_trap_2

# Historical improveement

- Blit tiles:               33-34fps (egypt) (24/7/26)
- Min collision points:     26-29fps before / 32fps after (egypt spike trap with blocks) (NOT DONE YET!)