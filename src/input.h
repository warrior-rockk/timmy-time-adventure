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

#define INPUT_LOG_MAX_EVENTS        50000   //max record frames to store (max uint16_t: 65535)
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

//key struct
typedef struct {
    uint8_t keyCode;      //id of the key
    uint8_t keyFlags;   //key flag state
} tKey;

//input logger event
typedef struct {
    uint16_t frameTime;     //Frame time stamp
	uint8_t controlCode;	//Input code
	uint8_t controlEvent;   //Input event 
} tInputLogEvent;

//inits input system
void input_keys_init(uint8_t _numGameKeys);
//destroys input system
void input_keys_destroy();
//redefine control
void input_key_redefine(uint8_t keyId, uint8_t keyCode);
//get defined control id
uint8_t input_get_defined_key(uint8_t keyId);
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
void input_log_record(char *filename);
//stops record or play input log
void input_log_stop();
//starts to play input log file
void input_log_play(char *_file);
//checks if recording input log file
bool input_log_recording();
//check if playing input log file
bool input_log_playing();
//checks if play input log file is finished
bool input_log_play_finished();
//return play frame position of input log
uint16_t input_log_get_frame();
#endif