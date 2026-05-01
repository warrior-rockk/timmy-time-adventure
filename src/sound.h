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

#define SFX_FREQ_RND_PERCENT        20 //(+/- % freq variation on sfx_play_rnd)

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
void music_play(MIDI *midiFile, int loop);
//stops game music
void music_stop();
//pauses game music
void music_pause();
//resume game music
void music_resume();
//get music position
long music_get_pos();
//seeks music to position
void music_seek(int position);
//sets music volume
void music_set_volume(uint8_t volume);
//sets music tempo
void music_set_tempo(uint8_t tempo);

//inits sfx system. Uses any sample file to preallocate voices
void sfx_init(SAMPLE *initSample, uint8_t numVoices);
//destroys sfx system
void sfx_destroy();
//updates sfx system
void sfx_update();
//plays a sample on sound card voice
void sfx_play(SAMPLE* sampleFile, uint8_t voice);
//plays a sample on sound card voice randomize the frequency
void sfx_play_rnd(SAMPLE* sampleFile, uint8_t voice);
//stops sfx voice
void sfx_stop(uint8_t voice);
//pause sfx voice
void sfx_pause(uint8_t voice);
//resumes sfx voice
void sfx_resume(uint8_t voice);
//get voice data
tSfx sfx_get_voice_data(uint8_t voice);
//set voice data
void sfx_set_voice_data(uint8_t voice, tSfx voiceData);
//check if sfx is playing
bool sfx_voice_is_playing(uint8_t voice);
//check if sfx are finished
bool sfx_voice_finished(uint8_t voice);
//clear sfx finished flag
void sfx_voice_clear_finished(uint8_t voice);
//sets sfx volume
void sfx_set_volume(uint8_t volume);

//int sfx_get_voice_sample_id(uint8_t voice);

//reallocate sfx voice
void sfx_voice_reallocate(SAMPLE* sampleFile, uint8_t voice);
//set position, on millis, of sfx
void sfx_voice_set_position(uint8_t voice, int position);
#endif