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
#define K_FLAG_MEM_PRESS    0
#define K_FLAG_PRESS        1
#define K_FLAG_DOWN         2
#define K_FLAG_UP           3

#define cControlLoggerMaxFrames     1000
#define cControlCheckNumber     	6		//Numero de controles a comprobar
#define cendRecordCode     		    128    //Valor no asociado a control que indica fin de grabacion

//game keys
enum E_GAME_KEYS
{
    E_G_KEY_UP,
    E_G_KEY_DOWN,
    E_G_KEY_LEFT,
    E_G_KEY_RIGHT,
    E_G_KEY_JUMP,
    E_G_KEY_ACTION,
    E_G_KEY_PAUSE,
    E_G_KEY_EXIT,
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

//input logger event
typedef struct {
    uint16_t frameTime[cControlLoggerMaxFrames];	//Frame time stamp
	uint8_t controlCode[cControlLoggerMaxFrames];	//Input code
	uint8_t controlEvent[cControlLoggerMaxFrames];  //Input event 
} tInputLogEvent;

//updates keys state
void input_keys_update();
//return true if key is pressed
bool input_key_press(uint8_t keyId);
//return true if key pressed down (on raising edge)
bool input_key_down(uint8_t keyId);
//return true if key pressed up (on raising edge)
bool input_key_up(uint8_t keyId);
//return true if any key pressed of the game keys
bool input_any_key_pressed();
#endif