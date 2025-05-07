/********************************************************************
* Game Globals
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#include "globals.h"

//clamp value between min and max
int clamp(int value, int min, int max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else
        return value;
}