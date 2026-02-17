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
#define G_KEY_PAUSE     0
#define G_KEY_EXIT      1
#define G_KEY_UP        2
#define G_KEY_DOWN      3
#define G_KEY_LEFT      4
#define G_KEY_RIGHT     5
#define G_KEY_JUMP      6
#define G_KEY_ACTION    7
#define G_KEY_D         8
#define G_KEY_S         9
#define GAME_KEYS_NUM   10

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
//return true if any key pressed
bool input_any_key_pressed();
#endif