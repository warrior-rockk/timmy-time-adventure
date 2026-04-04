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
    uint8_t stop           : 1;    	//stop input play/record flag
} inputLoggerStatus;

int controlPlayingFrame;						//numero de frame reproducido actual
static uint16_t controlFrameCounter = 0;		//contador frames grabaci�n
static uint16_t eventIndex = 0;					//indice de registro
tInputLogEvent inputLogEvent;
uint8_t controlLogger[E_GAME_KEYS_NUM][E_KEY_ST_NUM];	//Array de controles del controlLogger

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
void input_log_player_update();

//general keys update
void input_keys_update()
{
    _anyKeyPressed = false;

    for (int i = 0; i < E_GAME_KEYS_NUM; i++)
    {
        CLEAR_BIT(gameKeys[i].keyFlags, E_K_ST_DOWN);
        CLEAR_BIT(gameKeys[i].keyFlags, E_K_ST_UP);

        if (key[gameKeys[i].keyId])
        {
            _anyKeyPressed = true;
            SET_BIT(gameKeys[i].keyFlags, E_K_ST_PRESS);
            
            //check key down
            if (!CHECK_BIT(gameKeys[i].keyFlags, E_K_ST_MEM_PRESS))
            {
                SET_BIT(gameKeys[i].keyFlags, E_K_ST_MEM_PRESS);
                SET_BIT(gameKeys[i].keyFlags, E_K_ST_DOWN); 
            }
        }
        else
        {
            //check key up
            if (CHECK_BIT(gameKeys[i].keyFlags, E_K_ST_MEM_PRESS))
            {
                SET_BIT(gameKeys[i].keyFlags, E_K_ST_UP); 
            }

            CLEAR_BIT(gameKeys[i].keyFlags, E_K_ST_MEM_PRESS);
            CLEAR_BIT(gameKeys[i].keyFlags, E_K_ST_PRESS); 
        }
    }

	//input logger
	if (inputLoggerStatus.recording)
		input_log_record_update();
	if (inputLoggerStatus.playing)
		input_log_player_update();
}

bool input_key_press(uint8_t keyId)
{
    return CHECK_BIT(gameKeys[keyId].keyFlags, E_K_ST_PRESS) || (controlLogger[keyId][E_K_ST_PRESS] && inputLoggerStatus.playing);
}

bool input_key_down(uint8_t keyId)
{
    return CHECK_BIT(gameKeys[keyId].keyFlags, E_K_ST_DOWN)  || (controlLogger[keyId][E_K_ST_DOWN]  && inputLoggerStatus.playing);
}

bool input_key_up(uint8_t keyId)
{
    return CHECK_BIT(gameKeys[keyId].keyFlags, E_K_ST_UP)  || (controlLogger[keyId][E_K_ST_UP]  && inputLoggerStatus.playing);
}

bool input_any_key_pressed()
{
    return _anyKeyPressed;
}

void input_log_record()//const char *filename)
{
	//reset flags
	inputLoggerStatus.recording = true;
	inputLoggerStatus.finished 	= false;
	inputLoggerStatus.playing 	= false;
	controlFrameCounter         = 0;		//contador frames grabaci�n
	eventIndex                  = 0;		//indice de registro

	//clear record buffer
	for (uint16_t i = 0; i < INPUT_LOG_MAX_EVENTS; i++)
    {
		inputLogEvent.frameTime[i]      = 0;
		inputLogEvent.controlCode[i]    = 0;
	}

	MY_TRACE_FLAG("Input log recording started\n");
}

//record log input on each frame
void input_log_record_update()
{
	if (eventIndex < INPUT_LOG_MAX_EVENTS && !inputLoggerStatus.stop)
	{					
        //check the number of controls configured
        for (uint8_t i = 0; i < INPUT_CHECK_CONTROL_NUM; i++)
        {
            //check control pressed
            if (input_key_press(i))
            {				
                //register control code with frametimestamp
                inputLogEvent.frameTime[eventIndex]      = controlFrameCounter;
                inputLogEvent.controlCode[eventIndex]    = i;
                //register control event
                if (input_key_down(i))
                    inputLogEvent.controlEvent[eventIndex]  = E_K_ST_DOWN;
                else if (input_key_up(i))
                    inputLogEvent.controlEvent[eventIndex]  = E_K_ST_UP;
                else
                    inputLogEvent.controlEvent[eventIndex]  = E_K_ST_PRESS;
                
                //increment event index
                eventIndex++;
            }
        }
        //increment frame counter
        controlFrameCounter++;	
    } 
	else
	{
    	//mark end of log if not max event reached
		if (eventIndex < INPUT_LOG_MAX_EVENTS)
		{
			inputLogEvent.frameTime[eventIndex]      = controlFrameCounter;
			inputLogEvent.controlCode[eventIndex]    = INPUT_END_RECORD_CODE; 	
		}
		//reset flag
		inputLoggerStatus.recording = false;
        inputLoggerStatus.stop      = false;
		MY_TRACE_FLAG("Input log recording finished\n");
		
		//store the events to file
		char *_file = "record.rec";
		FILE *recordFile = fopen(_file, "wb");
		if (!recordFile) {
			MY_TRACE_FLAG("Error creating input log record file %s. Input log stored only on volatile memory\n", _file);			
		}
		else
		{
			//write log records to file
			for (uint16_t i = 0; i < INPUT_LOG_MAX_EVENTS; i++)
			{
				fwrite(&inputLogEvent.frameTime[i],     sizeof(uint16_t),   1, recordFile);
				fwrite(&inputLogEvent.controlCode[i],   sizeof(uint8_t),    1, recordFile);
				fwrite(&inputLogEvent.controlEvent[i],  sizeof(uint8_t),    1, recordFile);
			}
			//close file
			fclose(recordFile);
			MY_TRACE_FLAG("Input log record file %s successfully write\n", _file);
		}
	}
}

void input_log_stop()
{
	inputLoggerStatus.stop = true;

	if (inputLoggerStatus.recording)
		MY_TRACE_FLAG("Stopping input recording...\n");
	if (inputLoggerStatus.playing)
		MY_TRACE_FLAG("Stopping input playing...\n");
}

void input_log_play()
{
	//reset flags
	inputLoggerStatus.finished 	  = false;
	inputLoggerStatus.stop		  = false;	
	inputLoggerStatus.recording   = false;
	eventIndex                    = 0;

	//open input log file
	char *_file = "record.rec";
    FILE *playerFile = fopen(_file, "rb");
    if (!playerFile) {
        MY_TRACE_FLAG("Error reading input log record file %s. Input log reads on volatile memory\n", _file);	
    }
    else
    {
		//read input log records
		for (uint16_t i = 0; i < INPUT_LOG_MAX_EVENTS; i++)
        {
			fread(&inputLogEvent.frameTime[i],     sizeof(uint16_t),   1, playerFile);
            fread(&inputLogEvent.controlCode[i],   sizeof(uint8_t),    1, playerFile);
            fread(&inputLogEvent.controlEvent[i],  sizeof(uint8_t),    1, playerFile);
        }
		//close file
		fclose(playerFile);
		MY_TRACE_FLAG("Input logger file %s successfully readed\n", _file);
		inputLoggerStatus.playing = true;
		MY_TRACE_FLAG("Input logger playing\n");
	}
}

//plays the records of a input log file
void input_log_player_update()//const char *_file)
{
	if (eventIndex < INPUT_LOG_MAX_EVENTS && inputLogEvent.controlCode[eventIndex] != INPUT_END_RECORD_CODE && !inputLoggerStatus.stop)
    {		
        //check the configured number of controls
        for (uint8_t i = 0; i < INPUT_CHECK_CONTROL_NUM; i++)
        {
            //clear events of current control
            controlLogger[i][E_K_ST_PRESS] 	= false;
            controlLogger[i][E_K_ST_DOWN]	= false;
            controlLogger[i][E_K_ST_UP] 	= false;
            //if frametimestamp and control equals with record
            if ( inputLogEvent.frameTime[eventIndex] == controlPlayingFrame && inputLogEvent.controlCode[eventIndex]  == i )
            {
                //sets the control and his event
                controlLogger[inputLogEvent.controlCode[eventIndex]][inputLogEvent.controlEvent[eventIndex]] = true;
                //if ST_DOWN event, is ST_PRESS too
                if (inputLogEvent.controlEvent[eventIndex] == E_K_ST_DOWN)
                    controlLogger[inputLogEvent.controlCode[eventIndex]][E_K_ST_PRESS] = true;
                    
                //increment event index
                eventIndex++;					
            }
        }
        //increment playing frame
        controlPlayingFrame ++;
    }	
	else
	{
		//clears play control buffer
		for (uint8_t i = 0; i < INPUT_CHECK_CONTROL_NUM; i++)
		{
			controlLogger[i][E_K_ST_PRESS]  = false;
			controlLogger[i][E_K_ST_DOWN]   = false;
			controlLogger[i][E_K_ST_DOWN] 	= false;
		}
		
		//reset flags
		inputLoggerStatus.playing 	= false;
		inputLoggerStatus.finished 	= true;
		inputLoggerStatus.stop		= false;
		controlPlayingFrame         = 0;
		
		MY_TRACE_FLAG("Input logger player stopped\n");
	}
}

bool input_log_recording()
{
	return inputLoggerStatus.recording;
}
bool input_log_playing()
{
	return inputLoggerStatus.playing;
}

bool input_log_play_finished()
{
    return inputLoggerStatus.finished;
}