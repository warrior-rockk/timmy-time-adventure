# Bugs
- [ ] Possible bug when realloc objects or enemies with different local data?

# Engine
- [ ] Simple collisions on entity update?
- [ ] Items: extra live, life, score...
- [ ] Level checkpoints
- [ ] Less collision points to entities than player?
- [ ] Are setting properties to all tiles of map? separate tile map codes of tiles info to save RAM
- [ ] Sfx particles for spider web or player splash water
- [ ] Fps drop down (486@66) when all screen full of tiles (with the second layer feature)
- [ ] Load resources function to check errors
- [ ] Redefine controls
- [ ] Playback system
- [ ] DeltaTime ¿double? Review deltaTime calculation
- [ ] Reset ground flag on entity init?
- [ ] Save half entity sizes on structure?
- [ ] Platform objects
- [ ] All entities on one layer and map_load creates all
- [ ] For tiles, do the same as entities. Create sub-bitmap from tileSheet on the fly
- [ ] Collision points on entity structure?
- [ ] PC Speaker effects?
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

# Sound
- [ ] Use sample id for not reallocate when id is allocated? Unique id number by voice group

# Player
- [ ] Animation on edge
- [X] Idle special animation when no control
- [X] Slide crouched
- [X] Better jump velocities/gravity
- [X] Jump dynamic force when pressed
- [X] Crouch change size