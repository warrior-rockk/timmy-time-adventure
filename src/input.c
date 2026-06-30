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

bool _anyKeyPressed = false;    //flag for any key pressed
static uint8_t numGameKeys;     //number of configured game keys
static tKey *gameKeys;          //definition of keys assigned for game keys

//input logger status
static struct inputLoggerStatus
{
    uint16_t frameCounter;          //frame counter to record or play
    uint16_t eventIndex;		    //event index
    char *filename;                 //record log filename
    uint8_t recording      : 1;		//record input flag
    uint8_t playing        : 1;		//playing input flag
    uint8_t finished       : 1;		//finished input play flag
    uint8_t stop           : 1;    	//stop input play/record flag
} inputLoggerStatus;

tInputLogEvent inputLogEvent[INPUT_LOG_MAX_EVENTS]; //input log event
bool **controlLogger;                               //input play logger array

//prototypes
void input_log_record_update();
void input_log_player_update();

void input_keys_init(uint8_t _numGameKeys)
{
    //set number of game keys
    numGameKeys = _numGameKeys;
    
    //alloc memory for game keys array
    gameKeys = (tKey *)malloc(numGameKeys * sizeof(tKey));    
    
    //alloc memory for controlLogger array
    controlLogger = (bool **)malloc(numGameKeys * sizeof(bool *));
    for (uint8_t i = 0; i < numGameKeys; i++)
    {
        controlLogger[i] = (bool *)malloc(E_KEY_ST_NUM * sizeof(bool));
    }    
}

void input_keys_destroy()
{
    if (gameKeys)
        free(gameKeys);
    
    for (uint8_t i = 0; i < numGameKeys; i++)
        free(controlLogger[i]);
    
    free(controlLogger);
}

void input_key_redefine(uint8_t keyId, uint8_t keyCode)
{
    ASSERT(keyId < numGameKeys);

    gameKeys[keyId].keyCode = keyCode;
    gameKeys[keyId].keyFlags = 0x00;

    MY_TRACE_FLAG("Redefine key id %i with code %i\n", keyId, keyCode);
}

uint8_t input_get_defined_key(uint8_t keyId)
{
    return gameKeys[keyId].keyCode;
}

void input_keys_update()
{
    _anyKeyPressed = false;

    for (int i = 0; i < numGameKeys; i++)
    {
        CLEAR_BIT(gameKeys[i].keyFlags, E_K_ST_DOWN);
        CLEAR_BIT(gameKeys[i].keyFlags, E_K_ST_UP);

        if (key[gameKeys[i].keyCode])
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
    return (CHECK_BIT(gameKeys[keyId].keyFlags, E_K_ST_PRESS) && !inputLoggerStatus.playing) || (controlLogger[keyId][E_K_ST_PRESS] && inputLoggerStatus.playing);
}

bool input_key_down(uint8_t keyId)
{
    return (CHECK_BIT(gameKeys[keyId].keyFlags, E_K_ST_DOWN) && !inputLoggerStatus.playing) || (controlLogger[keyId][E_K_ST_DOWN]  && inputLoggerStatus.playing);
}

bool input_key_up(uint8_t keyId)
{
    return (CHECK_BIT(gameKeys[keyId].keyFlags, E_K_ST_UP) && !inputLoggerStatus.playing)  || (controlLogger[keyId][E_K_ST_UP]  && inputLoggerStatus.playing);
}

bool input_any_key_pressed()
{
    return _anyKeyPressed;
}

void input_log_record(char *_file)//const char *filename)
{
	//reset flags
	inputLoggerStatus.recording     = true;
	inputLoggerStatus.finished 	    = false;
	inputLoggerStatus.playing 	    = false;
    inputLoggerStatus.stop   	    = false;
	inputLoggerStatus.frameCounter  = 0;		
	inputLoggerStatus.eventIndex    = 0;
    inputLoggerStatus.filename      = _file;

	//clear record buffer
	for (uint16_t i = 0; i < INPUT_LOG_MAX_EVENTS; i++)
    {
		inputLogEvent[i].frameTime      = 0;
		inputLogEvent[i].controlCode    = 0;
        inputLogEvent[i].controlEvent   = 0;
	}

	MY_TRACE_FLAG("Input log recording started\n");
}

//record log input on each frame
void input_log_record_update()
{
	if (inputLoggerStatus.eventIndex < INPUT_LOG_MAX_EVENTS && !inputLoggerStatus.stop)
	{					
        //check the number of controls configured
        for (uint8_t i = 0; i < numGameKeys; i++)
        {
            //check control pressed
            if (input_key_press(i))
            {				
                //register control code with frametimestamp
                inputLogEvent[inputLoggerStatus.eventIndex].frameTime      = inputLoggerStatus.frameCounter;
                inputLogEvent[inputLoggerStatus.eventIndex].controlCode    = i;
                //register control event
                if (input_key_down(i))
                    inputLogEvent[inputLoggerStatus.eventIndex].controlEvent  = E_K_ST_DOWN;
                else if (input_key_up(i))
                    inputLogEvent[inputLoggerStatus.eventIndex].controlEvent  = E_K_ST_UP;
                else
                    inputLogEvent[inputLoggerStatus.eventIndex].controlEvent  = E_K_ST_PRESS;
                
                //increment event index
                inputLoggerStatus.eventIndex++;
            }
        }
        //increment frame counter
        inputLoggerStatus.frameCounter++;	
    } 
	else
	{
    	//mark end of log if not max event reached
		if (inputLoggerStatus.eventIndex < INPUT_LOG_MAX_EVENTS)
		{
			inputLogEvent[inputLoggerStatus.eventIndex].frameTime      = inputLoggerStatus.frameCounter;
			inputLogEvent[inputLoggerStatus.eventIndex].controlCode    = INPUT_END_RECORD_CODE; 	
            inputLogEvent[inputLoggerStatus.eventIndex].controlEvent   = 0x00; 	

            inputLoggerStatus.eventIndex++;
		}
		//reset flag
		inputLoggerStatus.recording = false;
        inputLoggerStatus.stop      = false;
		MY_TRACE_FLAG("Input log recording finished\n");
		
		//store the events to file
		FILE *recordFile = fopen(inputLoggerStatus.filename, "wb");
		if (!recordFile) {
			MY_TRACE_FLAG("Error creating input log record file %s. Input log stored only on volatile memory\n", inputLoggerStatus.filename);			
		}
		else
		{
			//write log records to file
			for (uint16_t i = 0; i < inputLoggerStatus.eventIndex; i++)
			{
				fwrite(&inputLogEvent[i].frameTime,     sizeof(uint16_t),   1, recordFile);
				fwrite(&inputLogEvent[i].controlCode,   sizeof(uint8_t),    1, recordFile);
				fwrite(&inputLogEvent[i].controlEvent,  sizeof(uint8_t),    1, recordFile);
			}
			//close file
			fclose(recordFile);
			MY_TRACE_FLAG("Input log record file %s successfully write. Recorded %i events\n", inputLoggerStatus.filename, inputLoggerStatus.eventIndex);
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

void input_log_play(char *_file)
{
	//reset flags
	inputLoggerStatus.recording     = false;
	inputLoggerStatus.finished 	    = false;
	inputLoggerStatus.playing 	    = false;
    inputLoggerStatus.stop   	    = false;
	inputLoggerStatus.frameCounter  = 0;		
	inputLoggerStatus.eventIndex    = 0;
    inputLoggerStatus.filename      = _file; 

	//open input log file
	FILE *playerFile = fopen(inputLoggerStatus.filename, "rb");
    if (!playerFile) {
        MY_TRACE_FLAG("Error reading input log record file %s. Input log reads on volatile memory\n", inputLoggerStatus.filename);	
    }
    else
    {
		//read input log records
        uint16_t i = 0;
        while(fread(&inputLogEvent[i],     sizeof(tInputLogEvent),   1, playerFile))
		{
			i++;
        }
		//close file
		fclose(playerFile);
		MY_TRACE_FLAG("Input logger file %s successfully readed. Readed %i events\n", inputLoggerStatus.filename, i);
		inputLoggerStatus.playing = true;
		MY_TRACE_FLAG("Input logger playing\n");
	}
}

//plays the records of a input log file
void input_log_player_update()//const char *_file)
{
	if (inputLoggerStatus.eventIndex < INPUT_LOG_MAX_EVENTS && inputLogEvent[inputLoggerStatus.eventIndex].controlCode != INPUT_END_RECORD_CODE && !inputLoggerStatus.stop)
    {		
        //check the configured number of controls
        for (uint8_t i = 0; i < numGameKeys; i++)
        {
            //clear events of current control
            controlLogger[i][E_K_ST_PRESS] 	= false;
            controlLogger[i][E_K_ST_DOWN]	= false;
            controlLogger[i][E_K_ST_UP] 	= false;
            //if frametimestamp and control equals with record
            if ( inputLogEvent[inputLoggerStatus.eventIndex].frameTime == inputLoggerStatus.frameCounter && inputLogEvent[inputLoggerStatus.eventIndex].controlCode  == i )
            {
                //sets the control and his event
                controlLogger[inputLogEvent[inputLoggerStatus.eventIndex].controlCode][inputLogEvent[inputLoggerStatus.eventIndex].controlEvent] = true;
                //if ST_DOWN event, is ST_PRESS too
                if (inputLogEvent[inputLoggerStatus.eventIndex].controlEvent == E_K_ST_DOWN)
                    controlLogger[inputLogEvent[inputLoggerStatus.eventIndex].controlCode][E_K_ST_PRESS] = true;
                    
                //increment event index
                inputLoggerStatus.eventIndex++;					
            }
        }
        //increment playing frame
        inputLoggerStatus.frameCounter ++;
    }	
	else
	{
		//clears play control buffer
		for (uint8_t i = 0; i < numGameKeys; i++)
		{
			controlLogger[i][E_K_ST_PRESS]  = false;
			controlLogger[i][E_K_ST_DOWN]   = false;
			controlLogger[i][E_K_ST_DOWN] 	= false;
		}
		
		//reset flags
		inputLoggerStatus.playing 	    = false;
		inputLoggerStatus.finished 	    = true;
		inputLoggerStatus.stop		    = false;
		inputLoggerStatus.frameCounter  = 0;
		
		MY_TRACE_FLAG("Input logger player stopped. Last event index %i\n", inputLoggerStatus.eventIndex);
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

uint16_t input_log_get_frame()
{
    return inputLoggerStatus.frameCounter;
}