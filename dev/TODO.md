# Bugs

- [ ] There's a problem with the dynamic entity creation: when realloc the dynamic arrays, if the ram position it's not contiguious, may be the array moved to another position and the actual this or local pointer on enemy/object it's not longer pointing the actual entity. May be needs to memorize the creation of entity and creates and next frame to prevent each entity to control this? At the moment, access all local variables of entity before entity creation to prevent
- [ ] Slow down fps coming from fade out/in
- [ ] Pick object already picked?? (prevented but review for better code?)
- [ ] Prevent object throwed push player to ground

# Engine

- [ ] function to divide lines of text?
- [ ] mark level as completed on map
- [ ] solve balancing on edge direction. Balancing on objects?
- [ ] Secret areas?
- [ ] Cheat codes?
- [ ] No need to check player on objects that hurt. Use a entity property OBJECT_HURT?
- [ ] Check possible problems to add player and solid object availability to check collision with triggers
- [ ] Refactor remove entities (mark as deleted and reuse position with new)
- [ ] Dead enemies with dynamite explosion?
- [ ] Remove from game_init the initialization that goes to game_update on state GAME_INIT
- [ ] Number counter dynamite?
- [ ] Level selection/acomplished
- [ ] Do the same system as enemies instance list add/remove on objects?
- [ ] Review debug keys on different translation units
- [ ] Unload game resources? unload interface system?
- [ ] Reassign update object/enemy pointer on object/enemy create?
- [ ] Debug teleport to next checkpoint? or mouse change position?
- [ ] Item coins?
- [ ] Bonus stage? at each score points? item bonus collectable?
- [ ] Sfx particles for spider web or player splash water
- [ ] PC Speaker effects?
- [ ] Intro
- [ ] Title screen
- [ ] Tutorial
- [ ] Loading screens? test on 86box
- [ ] Background color fade to do sunset?
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

# Sound
- [ ] Use sample id for not reallocate when id is allocated? Unique id number by voice group
- [ ] Possible spare channel for simultaneous sfx played?

# Player
- [ ] Change life icon on hurt/die
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
- [ ] All assets of levels named the same? (tiles, palette, map, music...)
- [ ] Refactor collision functions and unified get collision maps
- [ ] Check solid_object entities collision check! Any way to not check all the entities level?
- [ ] Entity_draw: Not create sub_bitmap on each entity instead store a current frame bitmap on entity and change when last_frame != frame
- [ ] Simple collisions check on entity update?
- [ ] Less collision points to entities than player?s
- [ ] DeltaTime ¿double? Review deltaTime calculation
- [ ] Are setting properties to all tiles of map? separate tile map codes of tiles info to save RAM
- [ ] Reset ground flag on entity init?
- [ ] Save half entity sizes on structure?
- [ ] For tiles, do the same as entities. Create sub-bitmap from tileSheet on the fly
- [ ] Collision points on entity structure?