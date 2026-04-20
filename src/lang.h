/********************************************************************
* Language definitions
*
* 20/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_LANG_
#define _H_LANG_

#include <stdint.h>

enum E_LANGUAGES
{    
    E_LANG_ENG,
    E_LANG_ESP,
    E_NUM_LANGS,
};

enum E_GAME_TEXTS
{
    E_TXT_GAME_TITLE,
    E_TXT_NUM,
};

void lang_init();
char *lang_get_txt(int16_t txtIndex);
#endif