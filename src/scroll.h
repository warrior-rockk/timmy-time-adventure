/********************************************************************
* Scroll System
*
* 07/05/2025
* Warcom Soft.  - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_SCROLL_
#define _H_SCROLL_

#include "utils.h"

//scroll definitions
#define SCROLL_OFFSET_X             0   //Offset X target position to follow camera
#define SCROLL_OFFSET_Y             14  //Offset Y target position to follow camera
#define SCROLL_CAMERA_X_OFFSET      0   //Offset X camera position
#define SCROLL_CAMERA_Y_OFFSET      16  //Offset Y camera position
#define SCROLL_BY_WINDOW_RANGE      16  //Range to scroll by window
#define SCROLL_BY_WINDOW_VEL_Y      12  //Scroll velocity Y moving by window

#define SCROLL_IN_REGION_OFFSET_X   32  //Offset X to scroll region check
#define SCROLL_IN_REGION_OFFSET_Y   8  //Offset Y to scroll region check

#define SCROLL_SHAKE_VELOCITY       2
#define SCROLL_SHAKE_DURATION       20
#define SCROLL_AUTOSCROLL_VEL       0.6

#define SCROLL_SHAKE_ENABLE         1       

//scroll modes
enum E_SCROLL_MODE
{
    E_SCROLL_MODE_NONE,             //no scroll
    E_SCROLL_MODE_ALL_MOVE,         //continuous follow camera
    E_SCROLL_MODE_BY_WINDOW,        //continuous follow camera adjusting to scroll window screen
    E_SCROLL_MODE_BY_WINDOW_Y_ONLY, //continuous follow camera adjusting only Y to scroll window screen
    E_SCROLL_MODE_AUTOSCROLL_X,     //scroll moves automatically on x coordinate
};

//scroll movements on by Window mode
enum E_SCROLL_BY_WINDOW_MOVE
{
    E_SCROLL_MOVE_NONE,
    E_SCROLL_MOVE_LEFT,
    E_SCROLL_MOVE_RIGHT,
    E_SCROLL_MOVE_UP,
    E_SCROLL_MOVE_DOWN,
};

//stop scroll options
enum E_STOP_SCROLL_OPTIONS
{
    E_STOP_SCROLL_LEFT      = 0x01,
    E_STOP_SCROLL_RIGHT     = 0x02,
    E_STOP_SCROLL_DOWN      = 0x04,
    E_STOP_SCROLL_UP        = 0x08, 
};

//scroll object
typedef struct
{    
    uint8_t mode;                   //scroll mode
    uint8_t moving;                 //scroll moving flags
    tVector pos;                    //scroll actual position
    tVector target;                 //scroll target position
    tVector window;                 //scroll window size
    tVector limit;                  //scroll limit position
    t4dir stopScroll;               //stop scroll direction positions
    bool stopScrollLeftNoSolid;     //flag for stop scroll not solid (for collisions)
    bool stopScrollRightNoSolid;    //flag for stop scroll not solid (for collisions)
    tFixVector fixPos;              //scroll actual position fixed point
    tFixVector fixVel;              //scroll velocity fixed point
    bool cameraShake;               //flag to shake camera
    tVector shakeValue;             //scroll offset shake value
    uint16_t shakeTimer;            //timer of duration of shake
} tScroll;

//creates scroll with the window dimensions, limits and mode
void scroll_create(tVector window, tVector limit, uint8_t mode);
//inits scroll object
void scroll_init(tVector initPos);
//updates scroll object. Receives the camera target follow position
void scroll_update(tVector cameraTarget);
//check if rectangle is on scroll region
bool scroll_rect_on_region(tRectangle rect);
//check if position is on scroll region
bool scroll_position_on_region(tVector checkPosition);
//returns scroll position
tVector scroll_get_position();
//returns scroll fxied position
tFixVector scroll_get_fix_position();
//set stop scroll value for a stop scroll direction (left, right, down or up) and no solid flag
void scroll_stop_scroll(uint8_t dir, int16_t value, bool noSolid);
//get stop scroll values for a stop scroll direction (left, right, down or up)
int16_t scroll_get_stop_scroll(uint8_t dir);
//gets not solid stop scroll flag for direction
bool scroll_get_stop_scroll_nosolid(uint8_t dir);
//function to shake screen camera
void scroll_shake_camera();
//sets the scroll mode
void scroll_set_scroll_mode(uint8_t mode);
//gets scroll mode
uint8_t scroll_get_scroll_mode();
#endif