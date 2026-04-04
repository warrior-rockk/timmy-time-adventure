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

#define INPUT_LOG_MAX_EVENTS        1000    //max record frames to store
#define INPUT_CHECK_CONTROL_NUM     6		//num of controls to record
#define INPUT_END_RECORD_CODE       128     //end record code

//key state flags
enum E_KEY_STATE
{
    E_K_ST_MEM_PRESS,    
    E_K_ST_PRESS,        
    E_K_ST_DOWN,         
    E_K_ST_UP, 
    E_KEY_ST_NUM,          
};

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
    uint8_t keyId;      //id of the key
    uint8_t keyFlags;   //key flag state
} tKey;

//input logger event
typedef struct {
    uint16_t frameTime;     //Frame time stamp
	uint8_t controlCode;	//Input code
	uint8_t controlEvent;   //Input event 
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

//starts to record input log file
void input_log_record();
//stops record or play input log
void input_log_stop();
//starts to play input log file
void input_log_play();
//checks if recording input log file
bool input_log_recording();
//check if playing input log file
bool input_log_playing();
//checks if play input log file is finished
bool input_log_play_finished();
#endif