/********************************************************************
* Input system
*
* 31/01/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include <stdio.h>

#include "input.h"
#include "utils.h"

#define TRACE_FLAG  "[INPUT]"

bool _anyKeyPressed = false;

//input logger status flags
static struct inputLoggerStatus
{
    uint8_t recording      : 1;		//record input flag
    uint8_t playing        : 1;		//playing input flag
    uint8_t finished       : 1;		//finished input play flag
    uint8_t stopPlay       : 1;    	//stop input play flag
} inputLoggerStatus;

int controlPlayingFrame;					//numero de frame reproducido actual
static uint16_t controlFrameCounter = 0;		//contador frames grabaci�n
static uint16_t eventIndex = 0;						//indice de registro
tInputLogEvent inputLogEvent;
uint8_t controlLogger[6][3];					//Array de controles del controlLogger
char *recordFilename;

//definition of keys assigned for game keys
static tKey gameKeys[E_GAME_KEYS_NUM] =
{
    {KEY_UP, 0x00},
    {KEY_DOWN, 0x00},
    {KEY_LEFT, 0x00},
    {KEY_RIGHT, 0x00},
    {KEY_Z, 0x00},
    {KEY_X, 0x00},
    {KEY_SPACE, 0x00},
    {KEY_ESC, 0x00},
    {KEY_D, 0x00},
    {KEY_S, 0x00},
    {KEY_I, 0x00},
};
void input_log_record_update();

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

	//input logger
	if (inputLoggerStatus.recording)
		input_log_record_update();
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

void input_log_record()//const char *filename)
{
	//reset flags
	inputLoggerStatus.recording = true;
	inputLoggerStatus.finished = false;

	controlFrameCounter = 0;		//contador frames grabaci�n
	eventIndex = 0;						//indice de registro

	//clear record buffer
	for (uint16_t i = 0; i < cControlLoggerMaxFrames; i++)
    {
		inputLogEvent.frameTime[i]      = 0;
		inputLogEvent.controlCode[i]    = 0;
	}

	MY_TRACE_FLAG("Input log recording started\n");
}

//Funcion que registra los controles para grabar partida
void input_log_record_update()
{
	//loop grabacion
	if (eventIndex < cControlLoggerMaxFrames) //TODO: control + S
	//until(eventIndex == cControlLoggerMaxFrames || wgeKey(_control,E_PRESSED) && wgeKey(_s,E_DOWN));
    {
		//comprobamos si el player esta vivo
		//if (get_status(idPlayer) <> STATUS_ALIVE)
		//	log("Esperando a player para grabacion",DEBUG_ENGINE);
	    //else
						
			//comprobamos todos los controles disponibles
			for (uint8_t i = 0; i <= cControlCheckNumber; i++)
            {
				//si se ha presionado un control
                if (input_key_press(i))
                {				
					//registramos el control con el frametimestamp
					inputLogEvent.frameTime[eventIndex]      = controlFrameCounter;
					inputLogEvent.controlCode[eventIndex]    = i;
					//registramos el tipo de evento
					if (input_key_down(i))
						inputLogEvent.controlEvent[eventIndex]  	= K_FLAG_DOWN;
					else if (input_key_up(i))
						inputLogEvent.controlEvent[eventIndex]   = K_FLAG_UP;
					else
						inputLogEvent.controlEvent[eventIndex]  = K_FLAG_PRESS;
					
					//incrementamos el indice
					eventIndex ++;
					if (eventIndex == cControlLoggerMaxFrames)
						break;
					
					//TODO: log("Grabado control "+controlStrings[i]+" con evento "+inputLogEvent.controlEvent[eventIndex-1]+" en frame: "+controlFrameCounter+" e indice: "+eventIndex,DEBUG_CONTROLS);
				}
            }
			
			controlFrameCounter++;
		
		//}	
    } 
	else
	{
    	//marcamos fin de grabacion si no lleg� al maximo
		if (eventIndex < cControlLoggerMaxFrames)
		{
			inputLogEvent.frameTime[eventIndex]      = controlFrameCounter;
			inputLogEvent.controlCode[eventIndex]    = cendRecordCode; 	
		}
		
		inputLoggerStatus.recording = false;
		
		MY_TRACE_FLAG("Input log recording finished\n");
		
		//guardamos la grabacion a archivo
		char *_file = "record.rec";
		FILE *recordFile = fopen(_file, "wb");
		if (!recordFile) {
			MY_TRACE_FLAG("Error creating input log record file %s. Input log stores on volatile memory\n", _file);			
		}
		else
		{
			//escribimos los registros grabados
			for (uint16_t i = 0; i < cControlLoggerMaxFrames; i++)
			{
				fwrite(&inputLogEvent.frameTime[i],     sizeof(uint16_t),   1, recordFile);
				fwrite(&inputLogEvent.controlCode[i],   sizeof(uint8_t),    1, recordFile);
				fwrite(&inputLogEvent.controlEvent[i],  sizeof(uint8_t),    1, recordFile);
			}
			//cerramos el archivo
			fclose(recordFile);
			MY_TRACE_FLAG("Input log record file %s succefully write\n", _file);			
		}
	}
}

//funcion que reproduce los controles grabados
void input_log_player(const char *_file)
{
	uint16_t eventIndex;					//indice del registro
	
	//iniciamos flags
	inputLoggerStatus.finished = false;
	inputLoggerStatus.stopPlay	  = false;
	
	//abrimos la reproduccion de archivo
    FILE *playerFile = fopen(_file, "rb");
    if (!playerFile) {
        abort_on_error("Error reading input log record file %s\n", _file);
        //TODO: log("Grabacion se guarda en memoria",DEBUG_ENGINE);
    }
    else
    {
		//leemos los registros grabados
		for (uint16_t i = 0; i < cControlLoggerMaxFrames; i++)
        {
			fread(&inputLogEvent.frameTime[i],     sizeof(uint16_t),   1, playerFile);
            fread(&inputLogEvent.controlCode[i],   sizeof(uint8_t),    1, playerFile);
            fread(&inputLogEvent.controlEvent[i],  sizeof(uint8_t),    1, playerFile);
        }
		//cerramos el archivo
		fclose(playerFile);
		//TODO: log("Archivo "+_file+" le�do con �xito",DEBUG_ENGINE);
	}
	
	//TODO: log("Reproduccion iniciada",DEBUG_ENGINE);
	
	do
    {
		//comprobamos si el player esta vivo
		//if (get_status(idPlayer) <> STATUS_ALIVE)
		//	log("Esperando a player para reproduccion",DEBUG_ENGINE);
		//	inputLoggerStatus.playing = false;
		//else
			inputLoggerStatus.playing = true;
			
			//recorremos el array de teclas a comprobar
			for (uint8_t i = 0; i < cControlCheckNumber; i++)
            {
				//limpiamos los eventos del control actual
				controlLogger[i][K_FLAG_PRESS] = false;
				controlLogger[i][K_FLAG_DOWN]	= false;
				controlLogger[i][K_FLAG_UP] 		= false;
				//si el timestamp actual coincide con el registro y el control activo es el actual
				if ( inputLogEvent.frameTime[eventIndex] == controlPlayingFrame && 
					 inputLogEvent.controlCode[eventIndex]  == i )
                {
					//seteamos el control y su evento en el controlLogger
					controlLogger[inputLogEvent.controlCode[eventIndex]][inputLogEvent.controlEvent[eventIndex]] = true;
					//si el evento es E_DOWN, �mplicitamente es E_PRESSED tambi�n
					if (inputLogEvent.controlEvent[eventIndex] == K_FLAG_DOWN)
						controlLogger[inputLogEvent.controlCode[eventIndex]][K_FLAG_PRESS] = true;
					
					//incrementamos indice
					eventIndex++;
					if (eventIndex == cControlLoggerMaxFrames)
						break;
					
					//TODO: log("Reproducido control "+controlStrings[i]+" con evento:"+inputLogEvent.controlEvent[eventIndex-1]+" en frame: "+controlPlayingFrame+" e indice: "+eventIndex,DEBUG_CONTROLS);
				}
            }
			
			controlPlayingFrame ++;

		//}
	
    } while(eventIndex < cControlLoggerMaxFrames && inputLogEvent.controlCode[eventIndex]  == cendRecordCode && !inputLoggerStatus.stopPlay); //TODO: control + s
	//se comprueba con key porque wgeKey esta deshabilitado en reproduccion
	//until (eventIndex == cControlLoggerMaxFrames || inputLogEvent.controlCode[eventIndex]  == cendRecordCode || key(_control) && key(_s) || inputLoggerStatus.stopPlaying ); 
	
	//limpiamos el buffer de reproduccion
	for (uint8_t i = 0; i < cControlCheckNumber; i++)
    {
		controlLogger[i][K_FLAG_PRESS] = false;
		controlLogger[i][K_FLAG_DOWN]	= false;
		controlLogger[i][K_FLAG_DOWN] 		= false;
	}
	
	//reiniciamos flags
	inputLoggerStatus.playing = false;
	inputLoggerStatus.finished = true;
	inputLoggerStatus.stopPlay = false;
	controlPlayingFrame = 0;
	
	//TODO: log("Reproduccion detenida",DEBUG_ENGINE);
}