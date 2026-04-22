/********************************************************************
* Language definitions
*
* 20/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_LANG_
#define _H_LANG_

#include <stdint.h>

//inits language system
void lang_init(uint8_t _numLangs, uint16_t _numTexts);
//destroys language system
void lang_destroy();

//set language
void lang_set(uint8_t langIndex);
//get language
uint8_t lang_get();

//loads a language file
void lang_load(const char *langFileName, uint8_t langIndex);
//loads language from memory buffer
void lang_load_mem(char *langBuffer, int langIndex);

//gets text line from current lang
char *lang_get_txt(int16_t txtIndex);
#endif