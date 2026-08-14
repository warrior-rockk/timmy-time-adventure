# Bugs
- [ ] Bug returning to tile no timeout reset?
- [ ] Lost vynsc (testing custom vsync check and remove 03 optimization):
    - [X] custom vsync
    - [X] remove 03 optimization
    - [X] test on 86box and dosbox-x
    - [X] test on another level not medieval (happens at same time: 15 min aprox)
- [ ] Record input bug with deltaTimes and different cycles
- [ ] 104 lives!? when??
- [ ] Slow down fps coming from fade out/in. Made custom function asyncronous?
- [ ] There's a problem with the dynamic entity creation: when realloc the dynamic arrays, if the ram position it's not contiguious, may be the array moved to another position and the actual this or local pointer on enemy/object it's not longer pointing the actual entity. May be needs to memorize the creation of entity and creates and next frame to prevent each entity to control this? At the moment, access all local variables of entity before entity creation to prevent
- [X] Variable deltaTime causes collision system fails (player falls on solid tiles) (limit fall vY and deltaTime)
- [X] falling on fire destroy blocks puzzle fails and loses lives (limit fall vY and deltaTine)

# Performance
- [ ] Draw map: more background color tiles? (jurassic cavern)
- [X] Draw map: only draw_sprite front tiles. Use blit for backLayer
- [X] All entities, except player, minimal collision points (ensure can check the same collision points of all set) 
- [X] Jump on 486 deltatime not reach high sometimes (increment max player jump)
- [X] Egyptian stairs jump from mummy doesn't work the stop scroll left
- [X] Don't draw (visible=false) persistent objects offscreen

# Level design
- [ ] Jurassic: spider web, make improve position of piranhas?

# Game design
- [ ] fps timer interrupt and counter only on debug mode?
- [ ] Animation of player run of level time?
- [ ] Confirm exit to title
- [ ] Title screen
- [ ] Too fast enemies! (west for example) TO FAST ALL! ADJUST FOR LOWER DIFFICULTY
- [ ] Respawn extra live item for easy gameplay?
- [ ] volume up all midi tracks
- [ ] Secret areas?
- [ ] Cheat codes?
- [ ] Item coins?
- [ ] Bonus stage? at each score points? item bonus collectable?
- [ ] Sfx particles for spider web or player splash water

# Engine
- [ ] Sound when extra live by score
- [ ] Use fixed for scroll main movements and velocities
- [ ] midi fade off volume ramp
- [ ] freeze entities during dead animation? on crush?
- [ ] adjust scroll threshold to prevent scroll movement when crouch (scroll target on players feet?)
- [ ] solve balancing on edge direction. Balancing on objects?
- [ ] Refactor remove entities (mark as deleted and reuse position with new)
- [ ] Dead enemies with dynamite explosion?
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
- [ ] Paint player jeans blue
- [ ] Cleaunup enemy animations

# Improvements
- [ ] check collision slopes only on player?
- [ ] Maybe it's not necessary the structure type on local metadata on objects and enemies because i don't do any specified free on each type. (maybe global add and remove function?)
- [ ] Refactor collision functions and unified get collision maps
- [ ] Check solid_object entities collision check! Any way to not check all the entities level?
- [ ] Entity_draw: Not create sub_bitmap on each entity instead store a current frame bitmap on entity and change when last_frame != frame
- [ ] Simple collisions check on entity update?
- [ ] DeltaTime ¿double? Review deltaTime calculation
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