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
- Min collision points:     26-29fps after / 32fps before (egypt spike trap with blocks)

# Testing improvee platform persistent frame drop
- Egypt platforms without modifications: 32~34fps (entities_update 12.2ms)
- Platform object only checks 2 collision points (left and rigth or up and down) 34~35fps (entities_update 7.8ms)
- Disable slope detection: (60fps!! entities_update 4.9ms /5.4 with if and not #if) <- Only slope detection on player or enabled by level?:
    * Jurassic: slopes / no platforms
    * Egypt: no slopes / platforms
    * Medieval: no slopes / platforms
    * West: slopes / no platforms

# Vsync bug

BUG NOT APPEARS:
(TESTED) window 1: no timer no sound (no level timeout): 30 MINUTES AND BUG DOESN'T APPEAR!

BUG APPEARS
(TESTED) window 2: timer and no sound (BUG APPPEARS AT 15 min)
(TESTED) window 3: all enabled and wait on level select to test timeout (15 minutes on level select and bug appears on start of level)
(TESTED) new window 3: no timers but sound (I think midi it's not working) BUG APPEARS
(TESTED) window 4: timer and sound: implemented control+g to restore video mode (DOESN'T WORK. BUG PERSIST)
(TESTED) new window 4: timer and sound but not installed fps interrupt (BUG APPEARS)
(TESTED) allegro install_timer and midi sound but not installed any custom interrupt (BUG APPEARS!!)



Betatester text:

* Falta la música del intro
* Música del título es provisional
* Falta la música de la selección de nivel
* Falta la música del final y alguna animación mas
