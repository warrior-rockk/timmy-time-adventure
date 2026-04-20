/********************************************************************
* Language code
*
* 20/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include "lang.h"

uint8_t currentLang = E_LANG_ENG;
char *texts[E_TXT_NUM][E_NUM_LANGS] = {
    {"PRESS KEY TO START", "PULSA TECLA PARA EMPEZAR"},      
};	//Tabla de idiomas de textos

void lang_init()
{
    
}

char *lang_get_txt(int16_t txtIndex)
{
    return texts[currentLang][txtIndex];
}