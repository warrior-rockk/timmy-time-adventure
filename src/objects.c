/********************************************************************
* Object entities (test)
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#include "allegro.h"
#include "objects.h"

void object_gem_update(tEntity *this)
{
    switch (this->state)
    {
        case GEM_IDLE_STATE:
            this->state = GEM_MOVE_RIGHT_STATE;
        break;
        case GEM_MOVE_RIGHT_STATE:
            if (this->pos.x > 130)
                this->state = GEM_MOVE_LEFT_STATE;
            else   
                this->fixVel.x = ftofix(0.4);
        break;
        case GEM_MOVE_LEFT_STATE:
        if (this->pos.x < 70)
                this->state = GEM_MOVE_RIGHT_STATE;
            else   
                this->fixVel.x = ftofix(-0.4);
        break;
        default:
            this->state = GEM_IDLE_STATE;
    }

    //apply velocity
    this->fixPos.x += fixmul(this->fixVel.x, ftofix(deltaTime));
    this->fixPos.y += fixmul(this->fixVel.y, ftofix(deltaTime));

    //update position
    this->pos.x = fixtoi(this->fixPos.x);
    this->pos.y = fixtoi(this->fixPos.y);
}