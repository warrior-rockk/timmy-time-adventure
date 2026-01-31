/********************************************************************
* Input system
*
* 31/01/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include "allegro.h"

#include "input.h"
#include "utils.h"

//definition of keys assigned for game keys
static tKey gameKeys[GAME_KEYS_NUM] =
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
    {KEY_M, 0x00},
};

//general keys update
void input_keys_update()
{
    for (int i = 0; i < GAME_KEYS_NUM; i++)
    {
        CLEAR_BIT(gameKeys[i].keyFlags, K_FLAG_PRESSED);

        if (key[gameKeys[i].keyId])
        {
            SET_BIT(gameKeys[i].keyFlags, K_FLAG_PRESS);
            if (!CHECK_BIT(gameKeys[i].keyFlags, K_FLAG_MEM_PRESS))
            {
                SET_BIT(gameKeys[i].keyFlags, K_FLAG_MEM_PRESS);
                SET_BIT(gameKeys[i].keyFlags, K_FLAG_PRESSED); 
            }
        }
        else
        {
            CLEAR_BIT(gameKeys[i].keyFlags, K_FLAG_MEM_PRESS);
            CLEAR_BIT(gameKeys[i].keyFlags, K_FLAG_PRESS); 
        }
    }
}

bool input_key_press(uint8_t keyId)
{
    return CHECK_BIT(gameKeys[keyId].keyFlags, K_FLAG_PRESS);
}

bool input_key_pressed(uint8_t keyId)
{
    return CHECK_BIT(gameKeys[keyId].keyFlags, K_FLAG_PRESSED);
}