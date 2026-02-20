/********************************************************************
* Sound System Definitions
*
* 20/02/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/

#ifndef _H_SOUND_
#define _H_SOUND_

#include <stdbool.h>
#include "allegro.h"

//sound modes
enum E_SOUND_MODES{
    E_SOUND_SB_MODE,
    E_SOUND_SPEAKER_MODE,
    E_SOUND_OFF_MODE,
};

//sfx samples structure
typedef struct 
{
    int position;
    int sampleId;
    bool playing;
    bool paused;
    bool finished;
    bool stop;
    bool pause;
} tSfx;

//inits sound system
int sound_init();
//sets sound mode
void sound_set_mode(enum E_SOUND_MODES _soundMode);
//get sound mode
enum E_SOUND_MODES sound_get_mode();

//plays game music
void play_music(MIDI *midiFile, int loop);
//stops game music
void stop_music();
//pauses game music
void pause_music();
//resume game music
void resume_music();
//get music position
long music_get_pos();
//seeks music to position
void music_seek(int position);

void sfx_init(SAMPLE *initSample, uint8_t numVoices);
void sfx_destroy();
void sfx_update();
void sfx_play(SAMPLE* sampleFile, uint8_t voice, bool rndFreq);
void sfx_stop(uint8_t voice);
void sfx_pause(uint8_t voice);
void sfx_resume(uint8_t voice);
tSfx sfx_get_voice_data(uint8_t voice);
void sfx_set_voice_data(uint8_t voice, tSfx voiceData);
bool sfx_voice_is_playing(uint8_t voice);
bool sfx_voice_finished(uint8_t voice);
void sfx_voice_clear_finished(uint8_t voice);
int sfx_get_voice_sample_id(uint8_t voice);

void sfx_voice_reallocate(SAMPLE* sampleFile, uint8_t voice);
void sfx_voice_set_position(uint8_t voice, int position);
#endif