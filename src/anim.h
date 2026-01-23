/********************************************************************
* Animation System definitions
*
* 09/11/2025
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef H_ANIM
#define H_ANIM

//animation modes
#define ANIM_LOOP       0
#define ANIM_ONCE       1
#define ANIM_PING_PONG  2

typedef struct tAnimation
{
    int16_t frame;
    int16_t lastFrame;
    int16_t frameTime;
    bool reverse;
} tAnimation;

//declare functions
bool play_animation(tAnimation *animation, int startFrame, int endFrame, int speed, uint8_t mode);
#endif  //H_ANIM
