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

uint8_t currentLang = E_LANG_ESP;
char *texts[E_NUM_LANGS][E_TXT_NUM];

void lang_init()
{
    
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
    for (int i = 0; i < E_NUM_LANGS; i++) {
        for (int f = 0; f < MAX_LINES; f++) {
            if (texts[i][f] != NULL) {
                free(texts[i][f]);
            }
        }
    }
}