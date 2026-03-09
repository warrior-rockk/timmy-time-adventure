/********************************************************************
* Input system
*
* 31/01/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include "input.h"
#include "utils.h"

bool _anyKeyPressed = false;

//definition of keys assigned for game keys
static tKey gameKeys[E_GAME_KEYS_NUM] =
{
    {KEY_SPACE, 0x00},
    {KEY_ESC, 0x00},
    {KEY_UP, 0x00},
    {KEY_DOWN, 0x00},
    {KEY_LEFT, 0x00},
    {KEY_RIGHT, 0x00},
    {KEY_Z, 0x00},
    {KEY_X, 0x00},
    {KEY_D, 0x00},
    {KEY_S, 0x00},
    {KEY_I, 0x00},
};

//general keys update
void input_keys_update()
{
    _anyKeyPressed = false;

    for (int i = 0; i < E_GAME_KEYS_NUM; i++)
    {
        CLEAR_BIT(gameKeys[i].keyFlags, K_FLAG_DOWN);
        CLEAR_BIT(gameKeys[i].keyFlags, K_FLAG_UP);

        if (key[gameKeys[i].keyId])
        {
            _anyKeyPressed = true;
            SET_BIT(gameKeys[i].keyFlags, K_FLAG_PRESS);
            
            //check key down
            if (!CHECK_BIT(gameKeys[i].keyFlags, K_FLAG_MEM_PRESS))
            {
                SET_BIT(gameKeys[i].keyFlags, K_FLAG_MEM_PRESS);
                SET_BIT(gameKeys[i].keyFlags, K_FLAG_DOWN); 
            }
        }
        else
        {
            //check key up
            if (CHECK_BIT(gameKeys[i].keyFlags, K_FLAG_MEM_PRESS))
            {
                SET_BIT(gameKeys[i].keyFlags, K_FLAG_UP); 
            }

            CLEAR_BIT(gameKeys[i].keyFlags, K_FLAG_MEM_PRESS);
            CLEAR_BIT(gameKeys[i].keyFlags, K_FLAG_PRESS); 
        }
    }
}

bool input_key_press(uint8_t keyId)
{
    return CHECK_BIT(gameKeys[keyId].keyFlags, K_FLAG_PRESS);
}

bool input_key_down(uint8_t keyId)
{
    return CHECK_BIT(gameKeys[keyId].keyFlags, K_FLAG_DOWN);
}

bool input_key_up(uint8_t keyId)
{
    return CHECK_BIT(gameKeys[keyId].keyFlags, K_FLAG_UP);
}

bool input_any_key_pressed()
{
    return _anyKeyPressed;
}