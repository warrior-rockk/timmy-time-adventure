/********************************************************************
* Animation System definitions
*
* 09/11/2025
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef H_ANIM
#define H_ANIM

//animation modes
#define ANIM_LOOP               0
#define ANIM_ONCE               1
#define ANIM_PING_PONG          2
#define ANIM_PING_PONG_ONCE     3

typedef struct
{
    int16_t frame;
    int16_t lastFrame;
    int16_t frameTime;
    uint8_t reverse : 1;
    uint8_t pingPongComplete : 1;
} tAnimation;

typedef struct 
{
    uint8_t frameId;
    uint16_t duration;
} tAnimFrame;


//function to play animation. Recives pointer to struct actor animation
//Returns true when animation finished (ONCE mode) or repeats (LOOP mode)
bool play_animation(tAnimation *animation, uint8_t startFrame, uint8_t endFrame, uint16_t speed, uint8_t mode);
//function to play animation with sequence of frames. 
//Recives pointer to struct actor animation
//Returns true when animation finished (ONCE mode) or repeats (LOOP mode)
bool play_animation_seq(tAnimation *animation, tAnimFrame *frames, uint8_t numFrames, uint8_t mode);
#endif  //H_ANIM
