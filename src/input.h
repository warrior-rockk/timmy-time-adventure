/********************************************************************
* Input definitions
*
* 31/01/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_INPUT_
#define _H_INPUT_

#include "allegro.h"
#include <stdbool.h>

//key state flags
#define K_FLAG_MEM_PRESS    0x00
#define K_FLAG_PRESS        0x01
#define K_FLAG_PRESSED      0x02

//game keys
enum E_GAME_KEYS
{
    E_G_KEY_PAUSE,
    E_G_KEY_EXIT,
    E_G_KEY_UP,
    E_G_KEY_DOWN,
    E_G_KEY_LEFT,
    E_G_KEY_RIGHT,
    E_G_KEY_JUMP,
    E_G_KEY_ACTION,
    E_G_KEY_D,
    E_G_KEY_S,
    E_G_KEY_I,
    E_GAME_KEYS_NUM,
};

//key struct
typedef struct {
    uint16_t keyId;
    uint8_t keyFlags;
} tKey;

//updates keys state
void input_keys_update();
//return true if key is pressed
bool input_key_press(uint8_t keyId);
//return true if key pressed on raising edge
bool input_key_pressed(uint8_t keyId);
//return true if any key pressed of the game keys
bool input_any_key_pressed();
#endif