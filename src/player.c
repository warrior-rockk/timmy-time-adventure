/********************************************************************
* Player system
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#include <stdbool.h>
#include <math.h>
#include "allegro.h"
#include "player.h"
#include "input.h"
#include "utils.h"
#include "game.h"
#include "entity.h"
#include "collisions.h"

static fixed accel_x;
static fixed accel_x_air;
static fixed friction;
static fixed air_friction;
static fixed gravity;
static fixed accel_y;
static fixed max_vel_x;
static fixed max_vel_y;
static fixed cMinVelToIdle;

static fixed localFriction;
static fixed localAccelX;

static uint8_t playerCrouched = false;
static uint8_t playerMoving = false;
static uint8_t playerAttack = false;

static void player_update_controls(tEntity *player);
static void player_update_collisions(tEntity *player);
static void player_update_state(tEntity *player);
static void player_update_animations(tEntity *player);

void player_init(tEntity *player)
{
    //initialize player vars
    player->state = ST_PLAYER_IDLE;
    playerCrouched = false;
    playerMoving = false;
    player->ground = false;

    accel_x       = ftofix(0.20); //0.06 without friction on move
    accel_x_air   = ftofix(0.40); 
    friction      = ftofix(0.92); //more friction, more sloppy (0.94-0.96 is like ice) 0.86 testing
    air_friction  = ftofix(0.6);  //less than floor friction
    gravity       = ftofix(0.2);
    accel_y       = ftofix(4.0);  //jump acceleration
    max_vel_x     = ftofix(1.0);
    max_vel_y     = ftofix(6);
    cMinVelToIdle = ftofix(0.1);
}

void player_update(tEntity *player)
{
    //update friction
    localFriction = player->ground ? friction: air_friction;
    localAccelX = player->ground ? accel_x : accel_x_air;

    //update controls
    player_update_controls(player);
    
    //update collisions    
    player_update_collisions(player);       

    
    //update velocities and position on main entity_update?
    /*
    //apply velocities
    player->fixPos.x += fixmul(player->fixVel.x, ftofix(deltaTime));        
    if (player->ground)
        player->fixVel.y = 0;  
    else  
    {
        //gravity 
	    player->fixVel.y += player->fixVel.y >= max_vel_y ? 0 : fixmul(gravity, ftofix(deltaTime));
        player->fixPos.y += fixmul(player->fixVel.y, ftofix(deltaTime));
    }
    
    
    //update position
    player->pos.x = fixtoi(player->fixPos.x);
    player->pos.y = fixtoi(player->fixPos.y);
    */
   
    //update state
    player_update_state(player);

    //update animations
    player_update_animations(player);
}

static void player_update_controls(tEntity *player)
{
    //update controls
    if (input_key_press(G_KEY_RIGHT) && player->fixVel.x < max_vel_x && !playerCrouched)
    {
        player->fixVel.x += fixmul(fixmul(localAccelX, (itofix(1) - friction)), ftofix(deltaTime));
        //player->fixVel.x+= fixmul(accel_x, ftofix(deltaTime));
        player->dir = E_ENT_DIR_RIGHT;
        playerMoving = true;
    }

    if (input_key_press(G_KEY_LEFT) && player->fixVel.x > -max_vel_x && !playerCrouched)
    {
        player->fixVel.x -= fixmul(fixmul(localAccelX, (itofix(1) - friction)), ftofix(deltaTime));
        //player->fixVel.x -= fixmul(accel_x, ftofix(deltaTime));
        player->dir = E_ENT_DIR_LEFT;
        playerMoving = true;
    }

    if (input_key_pressed(G_KEY_JUMP) && player->ground)
    {
        player->fixVel.y = -accel_y;
        player->ground = false;
    }

    if (input_key_pressed(G_KEY_ACTION) && !player->ground)
    {
        playerAttack = true;
    }

    playerCrouched = input_key_press(G_KEY_DOWN) && player->ground;

    //update vels
    if ((!input_key_press(G_KEY_RIGHT) && !input_key_press(G_KEY_LEFT)) || playerCrouched)
    {
        //this the equivalent formula for vX *= friction with deltaTime
        player->fixVel.x = fixmul(player->fixVel.x, ftofix(pow(fixtof(localFriction), (deltaTime * fixtof(localFriction))))); 
        if (abs(player->fixVel.x) > cMinVelToIdle)
            playerMoving = true;
        else    
            playerMoving = false;
    }
}

static void player_update_state(tEntity *player)
{
    //update state
    player->prevState = player->state;    
    if (!player->ground == true)
    {
        if (playerAttack)
            player->state = ST_PLAYER_ATTACK;
        else
            player->state = ST_PLAYER_JUMP;
    }
    else if (playerAttack)
    {
        player->state = ST_PLAYER_LAND;
    }    
    else if (playerCrouched)
        player->state = ST_PLAYER_CROUCHED;
    else if (abs(player->fixVel.x) > cMinVelToIdle || playerMoving)
    {
        player->state = ST_PLAYER_RUN;
    }
    else
    {
        player->state = ST_PLAYER_IDLE;
    }
}

static void player_update_animations(tEntity *player)
{
    //update animations
    switch (player->state)
    {
        case ST_PLAYER_IDLE:
            play_animation(&player->anim, ANIM_PLY_BREATH);            
        break;
        case ST_PLAYER_RUN:
            play_animation(&player->anim, ANIM_PLY_RUN);
        break;
        case ST_PLAYER_JUMP:
            if (player->fixVel.y < 0)
                if (playerMoving)
                    play_animation(&player->anim, ANIM_PLY_JUMP_RUN_UP);
                else
                    play_animation(&player->anim, ANIM_PLY_JUMP_UP);
            else
                if (playerMoving)
                    play_animation(&player->anim, ANIM_PLY_JUMP_RUN_DOWN);
                else
                    play_animation(&player->anim, ANIM_PLY_JUMP_DOWN);
        break;
        case ST_PLAYER_CROUCHED:
            play_animation(&player->anim, ANIM_PLY_CROUCH);
        break;
        case ST_PLAYER_ATTACK:
            play_animation(&player->anim, ANIM_PLY_ATTACK);
        break;
        case ST_PLAYER_LAND:
            if (play_animation(&player->anim, ANIM_PLY_LAND))
                playerAttack = false;
        break;
    }
}

static void player_update_collisions(tEntity *player)
{
    uint8_t colDir = 0;
    player->ground = false;
    
    //check all the entity collision points    
    for (uint8_t i = 0; i < NUM_COL_POINTS; i++)
    {                
        //check collision tile for collision point
        colDir = collision_check_tile(player, i);        
        //apply collision direction
        collision_apply_dir(player, colDir);        
    }

    //check entities collisions
    uint8_t numEntities = entities_get_num();
    tEntity *checkEntity;
    for (uint8_t i = 0; i < numEntities; i++)
    {
        checkEntity = entity_get(i);
        if (checkEntity->id != player->id && !checkEntity->dead)
        {
            switch (checkEntity->entClass)
            {
                case E_ENT_CLASS_OBJECT:
                    colDir = collision_check_entity(player, checkEntity, E_CHECK_PROCESS_VERTICALAXIS);
                    collision_apply_dir(player, colDir);
            
                    colDir = collision_check_entity(player, checkEntity, E_CHECK_PROCESS_HORIZONTALAXIS);
                    collision_apply_dir(player, colDir);            
                break;
                case E_ENT_CLASS_ENEMY:
                    colDir = collision_check_entity(player, checkEntity, E_CHECK_PROCESS_INFOONLY);     

                    //testing kill a enemy
                    if (colDir == E_COLLISION_DOWN && playerAttack && checkEntity->signal != E_ENT_SIGNAL_HURT)
                    {
                        checkEntity->signal = E_ENT_SIGNAL_HURT;               
                        player->fixVel.y = itofix(-4);
                    }
                break;
            }            
        }
    }
}