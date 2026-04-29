# Bugs
- [ ] Possible bug when realloc objects or enemies with different local data?

# Engine
- [ ] Review debug keys on different translation units
- [ ] Count down when time's up (sound)
- [ ] Unload game resources? unload interface system?
- [ ] Reassign update object/enemy pointer on object/enemy create?
- [ ] Debug teleport to next checkpoint? or mouse change position?
- [ ] Item coins?
- [ ] Bonus stage? at each score points? item bonus collectable?
- [ ] Sfx particles for spider web or player splash water
- [ ] Redefine controls
- [ ] Platform objects
- [ ] PC Speaker effects?
- [ ] Intro
- [ ] Title screen
- [ ] Menus and options
- [ ] Tutorial
- [ ] Loading screens? test on 86box
- [ ] Change background color and fade to do sunset?
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

# Player
- [ ] Animation on edge
- [X] Idle special animation when no control
- [X] Slide crouched
- [X] Better jump velocities/gravity
- [X] Jump dynamic force when pressed
- [X] Crouch change size

# Gfx
- [ ] Paint player jeans blue

# Improvements
- [ ] Simple collisions check on entity update?
- [ ] Less collision points to entities than player?s
- [ ] DeltaTime ¿double? Review deltaTime calculation
- [ ] Are setting properties to all tiles of map? separate tile map codes of tiles info to save RAM
- [ ] Reset ground flag on entity init?
- [ ] Save half entity sizes on structure?
- [ ] For tiles, do the same as entities. Create sub-bitmap from tileSheet on the fly
- [ ] Collision points on entity structure?