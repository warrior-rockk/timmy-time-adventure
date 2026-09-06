# Bugs
- [ ] Record input bug with deltaTimes and different cycles
- [ ] There's a problem with the dynamic entity creation: when realloc the dynamic arrays, if the ram position it's not contiguious, may be the array moved to another position and the actual this or local pointer on enemy/object it's not longer pointing the actual entity. May be needs to memorize the creation of entity and creates and next frame to prevent each entity to control this? At the moment, access all local variables of entity before entity creation to prevent (temporal solution may be, store the actual id, create a instance, and reget the instance with the address updated with the stored id...)

# Performance
- [X] entity_draw: don't get scroll position if not img?
- [X] Draw map: more background color tiles? (jurassic cavern)
- [X] Draw map: only draw_sprite front tiles. Use blit for backLayer
- [X] All entities, except player, minimal collision points (ensure can check the same collision points of all set) 
- [X] Jump on 486 deltatime not reach high sometimes (increment max player jump)
- [X] Egyptian stairs jump from mummy doesn't work the stop scroll left
- [X] Don't draw (visible=false) persistent objects offscreen

# Level design
- [ ] On cavern, fade level palette and take light to iluminate?

# Game design
- [ ] Sound with delay when start game?
- [ ] Intro music stops on beat?
- [ ] Credits to opengamegart?
- [ ] White frame on credits image scene?
- [ ] Blink blue circle on level complete?
- [ ] Sfx particles for player splash water

# Engine
- [ ] Translate launch loading texts
- [ ] It's necessary to load all game sfx on init?
- [ ] fps timer interrupt and counter only on debug mode?
- [ ] reset interrupt counters to not overflow?
- [ ] Use fixed for scroll main movements and velocities
- [ ] midi fade off volume ramp
- [ ] freeze entities during dead animation? on crush?
- [ ] adjust scroll threshold to prevent scroll movement when crouch (scroll target on players feet?)
- [ ] solve balancing on edge direction. Balancing on objects?
- [ ] Refactor remove entities (mark as deleted and reuse position with new)
- [ ] Number counter dynamite?
- [ ] Review debug keys on different translation units
- [ ] PC Speaker effects?
- [X] Platform objects
- [X] Continue menu with player animation crying?
- [X] Animation: implement PING_PONG_ONCE
- [X] Count down when time's up (sound)
- [X] Menus and options
- [X] Redefine controls
- [X] Camera shake
- [X] Destroy dialog (not system)
- [X] Translation text
- [X] Level checkpoints
- [X] Collision to border of screen
- [X] Entity destroy function pointer to unload entity resources
- [X] Type of tiles: solid, hurt...
- [X] Implement Y Scroll following player
- [X] Controls
- [X] Enemies
- [X] Improve front layer marking flag on no front layer if are tiles with front property
- [X] Tile animations
- [X] Pause system
- [X] Ladder tiles
- [X] Frame counter added to trace to check the game cycle of msg
- [X] Input: replace key_press and key_pressed for key_down, key_up, key_press
- [X] Items: extra live, life, score...
- [X] Palette for each level with player colors on top?
- [X] Collisions with the stop scroll x
- [X] Refactor stop scroll system. Scroll system internal scroll instead return scroll object?
- [X] With stop scroll y, don't check collision on tiles out scroll (fall on pit)
- [X] Playback system
- [X] Maps tmx with templates and generate output file on save
- [X] Egypt trap spike very slow (adjusted tickCounter to accumulate ticks and module check in range)

# Sound
- [ ] Use sample id for not reallocate when id is allocated? Unique id number by voice group
- [ ] Possible spare channel for simultaneous sfx played?


# Player
- [X] Animation on edge
- [X] Idle special animation when no control
- [X] Slide crouched
- [X] Better jump velocities/gravity
- [X] Jump dynamic force when pressed
- [X] Crouch change size

# Gfx
- [ ] Cleanup enemy animations

# Improvements
- [ ] Item coins?
- [ ] Bonus stage? at each score points? item bonus collectable?
- [ ] check collision slopes only on player?
- [ ] Maybe it's not necessary the structure type on local metadata on objects and enemies because i don't do any specified free on each type. (maybe global add and remove function?)
- [ ] Refactor collision functions and unified get collision maps
- [ ] Check solid_object entities collision check! Any way to not check all the entities level?
- [ ] Entity_draw: Not create sub_bitmap on each entity instead store a current frame bitmap on entity and change when last_frame != frame
- [ ] Simple collisions check on entity update?
- [ ] Are setting properties to all tiles of map? separate tile map codes of tiles info to save RAM
- [ ] Reset ground flag on entity init?
- [ ] Save half entity sizes on structure?
- [ ] For tiles, do the same as entities. Create sub-bitmap from tileSheet on the fly
- [ ] Collision points on entity structure?

# Next engine gen
- [ ] Flip tiles
- [ ] 2 Layer tiles
- [ ] Tiles with same tile id but different properties
- [ ] Parallax
- [ ] Parse sprite dimensions and animations from aseprite json
- [ ] Set size from Tiled export
- [ ] Background color fade to do sunset?
- [ ] Change life icon on hurt/die
- [ ] Option on make to specify level (and use this on Tiled command to test directly opened level)
- [ ] Function get screenCoordinates(worldCoordinates) and get worldCoordinates to toogle between world and screen positions
- [ ] Not use array of entities. Instead, array of pointers to entities. This mitigates the bug of the reallocation of the entire entity list pointer address when grows
- [ ] easy mode: auto attack / jumps shorts / more items? / slow enemy velocity?
- [ ] swing on waters