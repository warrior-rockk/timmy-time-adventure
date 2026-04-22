/********************************************************************
* Language code
*
* 20/04/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include <stdio.h>
//#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "lang.h"

#define MAX_LINES 100

static uint8_t numLangs;                    //number of initialized languages
static uint16_t numTexts;                   //number of initialized texts
static uint8_t currentLang;                 //current language
static char ***texts;                       //dynamic array of texts[LANG][TXT_NUM]

void lang_init(uint8_t _numLangs, uint16_t _numTexts)
{
    //memory reservation for each language    
    texts = (char ***)malloc(_numLangs * sizeof(char **)); 
    
    //for each language, text num memory reservation
    for (int i = 0; i < _numLangs; i++) {        
        texts[i] = (char **)malloc(_numTexts * sizeof(char *));       
    }

    //store data
    numLangs = _numLangs;
    numTexts = _numTexts;
}

void lang_set(uint8_t langIndex)
{
    currentLang = langIndex;
}

void lang_load_mem(char *langBuffer, int langIndex) {
    char line[256];
    int row = 0;
    int i = 0, j = 0;
    
    //Run the memory buffer
    while (langBuffer[i] != '\0' && row < 100) {
        
        if (langBuffer[i] == '\n' || langBuffer[i] == '\r') {
            
            line[j] = '\0';
            if (j > 0) { // avoid empty lines
                texts[langIndex][row] = strdup(line); 
                row++;
            }
            j = 0;
            // Jump possible Windows \r\n
            if (langBuffer[i] == '\r' && langBuffer[i+1] == '\n') i++;
        } else {
            line[j++] = langBuffer[i];            
        }
        i++;
    }
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
    for (int i = 0; i < numLangs; i++) {
        for (int f = 0; f < MAX_LINES; f++) {
            if (texts[i][f] != NULL) {
                free(texts[i][f]);
            }
        }
    }
}