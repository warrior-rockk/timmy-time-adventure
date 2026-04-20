/********************************************************************
* Language code
*
* 20/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lang.h"

#define MAX_LINES 100

uint8_t currentLang = E_LANG_ENG;
char *texts[E_NUM_LANGS][E_TXT_NUM];

void lang_init()
{
    
}

void lang_load(const char *langFileName, uint8_t langIndex) {
    FILE *langFile = fopen(langFileName, "r");
    if (!langFile) return;

    char tempBuffer[1024];
    int i = 0;

    while (fgets(tempBuffer, sizeof(tempBuffer), langFile) && i < MAX_LINES) {
        //remove line jump
        tempBuffer[strcspn(tempBuffer, "\n")] = 0;
        //memory reservation and copy the string (strdup makes malloc and strcpy)
        texts[langIndex][i] = strdup(tempBuffer);
        //next line
        i++;
    }
    fclose(langFile);
}

char *lang_get_txt(int16_t txtIndex)
{
    return texts[currentLang][txtIndex];
}

void lang_destroy() {
    for (int i = 0; i < E_NUM_LANGS; i++) {
        for (int f = 0; f < MAX_LINES; f++) {
            if (texts[i][f] != NULL) {
                free(texts[i][f]);
            }
        }
    }
}