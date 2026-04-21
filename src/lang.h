/********************************************************************
* Language definitions
*
* 20/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_LANG_
#define _H_LANG_

#include "utils.h"

enum E_LANGUAGES
{    
    E_LANG_ENG,
    E_LANG_ESP,
    E_NUM_LANGS,
};

enum E_GAME_TEXTS
{
    E_TXT_GAME_TITLE,
    E_TXT_PRESS_TO_START,
    E_TXT_MENU_PLAY,
    E_TXT_MENU_SOUND,
    E_TXT_MENU_SOUND_OPTIONS,
    E_TXT_MENU_VOLUME,
    E_TXT_MENU_EXIT,
    E_TXT_NUM,
};

//inits language system
void lang_init();
//destroys language system
void lang_destroy();

//loads a language file
void lang_load(const char *langFileName, uint8_t langIndex);
//loads language from memory buffer
void lang_load_mem(char *langBuffer, int langIndex);

//gets text line from current lang
char *lang_get_txt(int16_t txtIndex);
#endif