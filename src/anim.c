/********************************************************************
* Animation System Code
*
* 09/11/2025
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "anim.h"
#include "timer.h"

bool play_animation(tAnimation *animation, uint8_t startFrame, uint8_t endFrame, uint16_t speed, uint8_t mode)
{
    bool animFinished = false;

    //reset animation state
    if (animation->frame > endFrame || animation->frame < startFrame)
    {
        animation->frame = startFrame;
        animation->lastFrame = startFrame;
        animation->frameTime = 0;
        animation->reverse = false;
    }
                
    //reset frame time on frame change
    if (animation->lastFrame != animation->frame)
    {
        animation->frameTime = 0;
        animation->lastFrame = animation->frame;
    }

    //increment frame time
    if (get_clock_tick())
    {
        animation->frameTime += 1;
    }
    
    //if frame time reached
    if (animation->frameTime >= speed)
    {
        //if not last frame
        if (animation->frame < endFrame)
        {
            if (animation->frame == startFrame && mode == ANIM_PING_PONG)
                animation->reverse = false;

            //next frame
            if (!animation->reverse)
                animation->frame += 1;
            else
                animation->frame -= 1;
        }
        else if (mode == ANIM_LOOP)
        {
            //repeat animation
            animation->frame = startFrame;
            animFinished = true;
        }
        else if (mode == ANIM_PING_PONG)
        {
            //reverse animation
            animation->reverse = true;
            animation->frame -=1;
            animFinished = true;
        }
        else
           animFinished = true;
           
    }
    
    return animFinished;
}

bool play_animation_seq(tAnimation *animation, tAnimFrame *frames, uint8_t numFrames, uint8_t mode)
{
    bool animFinished = false;    

    //reset animation state
    if (animation->frame > numFrames - 1 || animation->frame < 0)
    {
        animation->frame = 0;
        animation->lastFrame = numFrames - 1;
        animation->frameTime = 0;
        animation->reverse = false;
    }
                
    //reset frame time on frame change
    if (animation->lastFrame != animation->frame)
    {
        animation->frameTime = 0;
        animation->lastFrame = animation->frame;
    }

    //increment frame time
    if (get_clock_tick())
    {
        animation->frameTime += 1;
    }
    
    //if frame time reached
    if (animation->frameTime >= frames[animation->frame].duration)
    {
        //if not last frame
        if (animation->frame < numFrames - 1)
        {
            if (animation->frame == 0 && mode == ANIM_PING_PONG)
                animation->reverse = false;

            //next frame
            if (!animation->reverse)
                animation->frame += 1;
            else
                animation->frame -= 1;
        }
        else if (mode == ANIM_LOOP)
        {
            //repeat animation
            animation->frame = 0;
            animFinished = true;
        }
        else if (mode == ANIM_PING_PONG)
        {
            //reverse animation
            animation->reverse = true;
            animation->frame -=1;
            animFinished = true;
        }
        else
           animFinished = true;           
    }
    
    return animFinished;
}