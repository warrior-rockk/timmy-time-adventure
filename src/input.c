/********************************************************************
* Input system
*
* 31/01/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include <stdio.h>

#include "input.h"
#include "utils.h"

bool _anyKeyPressed = false;

bool controlLoggerRecording;				//flag de grabando controles	
bool controlLoggerPlaying;					//flag de reproduciendo controles
bool controlLoggerFinished;					//flag de reproduccion finalizada
int controlPlayingFrame;					//numero de frame reproducido actual
bool StopControlPlaying;					//flag para detener la reproduccion

tInputLogEvent inputLogEvent;
uint8_t controlLogger[6][3];					//Array de controles del controlLogger

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

//Funcion que registra los controles para grabar partida
void input_log_record(const char *_file)
{
	uint16_t controlFrameCounter;		//contador frames grabaci�n
	uint16_t index;					//indice de registro
	
	//reset flags
	controlLoggerRecording = true;
	controlLoggerFinished = false;
	
	//TODO: log("Grabacion iniciada",DEBUG_ENGINE);
	
	//clear record buffer
	for (uint16_t i = 0; i < cControlLoggerMaxFrames; i++)
    {
		inputLogEvent.frameTime[i]      = 0;
		inputLogEvent.controlCode[i]    = 0;
	}
	
	//loop grabacion
	do
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
					inputLogEvent.frameTime[index]      = controlFrameCounter;
					inputLogEvent.controlCode[index]    = i;
					//registramos el tipo de evento
					if (input_key_down(i))
						inputLogEvent.controlEvent[index]  	= K_FLAG_DOWN;
					else if (input_key_up(i))
						inputLogEvent.controlEvent[index]   = K_FLAG_UP;
					else
						inputLogEvent.controlEvent[index]  = K_FLAG_PRESS;
					
					//incrementamos el indice
					index ++;
					if (index == cControlLoggerMaxFrames)
						break;
					
					//TODO: log("Grabado control "+controlStrings[i]+" con evento "+inputLogEvent.controlEvent[index-1]+" en frame: "+controlFrameCounter+" e indice: "+index,DEBUG_CONTROLS);
				}
            }
			
			controlFrameCounter++;
		
		//}
		
	
    } 
    while(index < cControlLoggerMaxFrames); //TODO: control + S
	//until(index == cControlLoggerMaxFrames || wgeKey(_control,E_PRESSED) && wgeKey(_s,E_DOWN));
	
	//marcamos fin de grabacion si no lleg� al maximo
	if (index < cControlLoggerMaxFrames)
    {
		inputLogEvent.frameTime[index]      = controlFrameCounter;
		inputLogEvent.controlCode[index]    = cendRecordCode; 	
	}
	
	controlLoggerRecording = false;
	
	//TODO: log("Grabacion Finalizada",DEBUG_ENGINE);
	
	//guardamos la grabacion a archivo
    FILE *recordFile = fopen(_file, "wb");
    if (!recordFile) {
        abort_on_error("Error creating input log record file %s\n", _file);
        //TODO: log("Grabacion se guarda en memoria",DEBUG_ENGINE);
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
		//TODO: log("Archivo "+_file+" guardado con �xito",DEBUG_ENGINE);
    }
}

//funcion que reproduce los controles grabados
void input_log_player(const char *_file)
{
	uint16_t index;					//indice del registro
	
	//iniciamos flags
	controlLoggerFinished = false;
	StopControlPlaying	  = false;
	
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
		//	controlLoggerPlaying = false;
		//else
			controlLoggerPlaying = true;
			
			//recorremos el array de teclas a comprobar
			for (uint8_t i = 0; i < cControlCheckNumber; i++)
            {
				//limpiamos los eventos del control actual
				controlLogger[i][K_FLAG_PRESS] = false;
				controlLogger[i][K_FLAG_DOWN]	= false;
				controlLogger[i][K_FLAG_UP] 		= false;
				//si el timestamp actual coincide con el registro y el control activo es el actual
				if ( inputLogEvent.frameTime[index] == controlPlayingFrame && 
					 inputLogEvent.controlCode[index]  == i )
                {
					//seteamos el control y su evento en el controlLogger
					controlLogger[inputLogEvent.controlCode[index]][inputLogEvent.controlEvent[index]] = true;
					//si el evento es E_DOWN, �mplicitamente es E_PRESSED tambi�n
					if (inputLogEvent.controlEvent[index] == K_FLAG_DOWN)
						controlLogger[inputLogEvent.controlCode[index]][K_FLAG_PRESS] = true;
					
					//incrementamos indice
					index++;
					if (index == cControlLoggerMaxFrames)
						break;
					
					//TODO: log("Reproducido control "+controlStrings[i]+" con evento:"+inputLogEvent.controlEvent[index-1]+" en frame: "+controlPlayingFrame+" e indice: "+index,DEBUG_CONTROLS);
				}
            }
			
			controlPlayingFrame ++;

		//}
	
    } while(index < cControlLoggerMaxFrames && inputLogEvent.controlCode[index]  == cendRecordCode && !StopControlPlaying); //TODO: control + s
	//se comprueba con key porque wgeKey esta deshabilitado en reproduccion
	//until (index == cControlLoggerMaxFrames || inputLogEvent.controlCode[index]  == cendRecordCode || key(_control) && key(_s) || StopControlPlaying ); 
	
	//limpiamos el buffer de reproduccion
	for (uint8_t i = 0; i < cControlCheckNumber; i++)
    {
		controlLogger[i][K_FLAG_PRESS] = false;
		controlLogger[i][K_FLAG_DOWN]	= false;
		controlLogger[i][K_FLAG_DOWN] 		= false;
	}
	
	//reiniciamos flags
	controlLoggerPlaying = false;
	controlLoggerFinished = true;
	StopControlPlaying = false;
	controlPlayingFrame = 0;
	
	//TODO: log("Reproduccion detenida",DEBUG_ENGINE);
}