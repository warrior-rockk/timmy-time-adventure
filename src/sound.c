/********************************************************************
* Sound System Code
*
* 20/02/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include <stdint.h>

#include "sound.h"
#include "utils.h"
//#include "game.h"

//#include "SPSONGS.H"
//#include "pcspeaker.h"

#define TRACE_FLAG  "[SOUND]"

//sound mode configured
static uint8_t soundMode;
//num sfx voices reserved
static uint8_t sfxVoices;
//voice sfx dynamic array
static tSfx *sfx;

//inits sound system
int sound_init()
{
    switch(soundMode)
    {
        case E_SOUND_SB_MODE:
            MY_TRACE_FLAG("Sound Blaster sound system init\n");
            return install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, 0);            
        break;
        case E_SOUND_SPEAKER_MODE:            
            MY_TRACE_FLAG("PC Speaker not implemented\n");
            return -1;
            /*MY_TRACE_FLAG("PC Speaker sound system init\n");
            return pc_speaker_init(10);*/
        break;
        case E_SOUND_OFF_MODE:
            MY_TRACE_FLAG("No sound configured init\n");
            return 0;
        break;
        default:
            return -1;
        break;
    }
}

//sets sound system mode
void sound_set_mode(enum E_SOUND_MODES _soundMode)
{
    soundMode = _soundMode;
}

//gets sound system mode
enum E_SOUND_MODES sound_get_mode()
{
    return soundMode;
}

//function to play music
void play_music(MIDI *midiFile, int loop)
{
    //stop actual music
    stop_music();

    if (soundMode == E_SOUND_SB_MODE)
    {
        //load index music index
        TRACE("Load music data file object\n");
        
        //play midi
        TRACE("Play midi music\n");
        play_midi(midiFile, loop);
    }
    else if(soundMode == E_SOUND_SPEAKER_MODE)
    {
        ;/*if (songs_notes[musicId] && songs_durations[musicId])
        {
            //play pcspeaker
            TRACE("Play pc speaker music\n");
            pc_speaker_play_song(songs_notes[musicId], songs_durations[musicId], loop); 
        }*/
    }
}

//function to stop midi music
void stop_music()
{
    //stop actual music
    MY_TRACE_FLAG("Stopping midi\n");
    if (soundMode == E_SOUND_SB_MODE)
        stop_midi();
    else if (soundMode == E_SOUND_SPEAKER_MODE)
        ;//pc_speaker_stop_song();
}

//function to pause music
void pause_music()
{
    if (soundMode == E_SOUND_SB_MODE)
        midi_pause();
    else if (soundMode == E_SOUND_SPEAKER_MODE)
        ;//pc_speaker_pause_song();
}

//function to resume music
void resume_music()
{
    if (soundMode == E_SOUND_SB_MODE)
        midi_resume();
    else if (soundMode == E_SOUND_SPEAKER_MODE)
        ;//pc_speaker_resume_song();
}

//returns music position
long music_get_pos()
{
    if (soundMode == E_SOUND_SB_MODE)
        return midi_pos;
    //else if (soundMode == E_SOUND_SPEAKER_MODE)
        //return pc_speaker_song_pos;
    else
        return 0;
}

//seeks music to position
void music_seek(int position)
{
    if (soundMode == E_SOUND_SB_MODE)
        midi_seek(position);
    else if (soundMode == E_SOUND_SPEAKER_MODE)
        ;//pc_speaker_seek_song(position);
}

//function to init sfx sound system
void sfx_init(uint8_t numVoices)
{
    //allocate sfx array
    sfx  = (tSfx *)malloc(numVoices * sizeof(tSfx));
    sfxVoices = numVoices;

    //SAMPLE *testSfx = load_wav("res/player/jump.wav");

    //init all sfx voices
    for (int i = 0; i < sfxVoices; i++)
    {
        //TODO: it's necessary?? Yes. If you want always use a specific voice on sfx_play, you need to pre allocate all voices
        /*
        //get soundcard voice (reallocate if exists)
        if (!voice_check(i))
        {
            int voice = allocate_voice(testSfx);
            MY_TRACE_FLAG("SFX voice %i allocated to soundcard voice %i\n", i, voice);
        }
        else
            reallocate_voice(i, testSfx);
        */

        //sfx[i].sampleId = sd_take;

        //init channel flags
        sfx[i].playing     = false;
        sfx[i].paused      = false;
        sfx[i].pause       = false;
        sfx[i].stop        = false;
        sfx[i].finished    = false;
        sfx[i].position    = -1;
    }

    MY_TRACE_FLAG("SFX system initialized\n");
}

//function to destroy sfx system (free resources)
void sfx_destroy()
{
    MY_TRACE_FLAG("Destroy SFX system\n");
    
    //free all sfx voices
    for (int i = 0; i < sfxVoices; i++)
    {
        //get soundcard voice (reallocate if exists)
        if (!voice_check(i))
            deallocate_voice(i);
    }

    //free sfx array
    free(sfx);
    sfx = NULL;

    MY_TRACE_FLAG("SFX system destroyed\n");
}

//function to update sfx sound system
void sfx_update()
{
    for (int i = 0; i < sfxVoices; i++)
    {
        //handles sound pause
        if (sfx[i].pause)
        {
            if (sfx[i].playing)
            {
                //do the stop/pause
                voice_stop(i);
                //set flag
                sfx[i].paused = true;
            }
            else
                //clear flag
                sfx[i].pause = false;
        }
    
        //handles sound resume
        if (!sfx[i].pause && sfx[i].paused)
        {
            //resume sound if was started
            if (sfx[i].position >= 0)
                voice_start(i);
            //clear flag
            sfx[i].paused = false;
        }
    
        //handles sound stop
        if (sfx[i].stop)
        {
            if (sfx[i].playing)
                //do sound stop
                voice_stop(i);
            //clear flag
            sfx[i].stop = false;
            //set flag
            sfx[i].finished = true;
        }
    
        //handles clear sound playing flag
        if (sfx[i].playing && !sfx[i].paused)
        {
            //stores sound position
            switch (soundMode)
            {
                case E_SOUND_SB_MODE:
                    sfx[i].position = voice_get_position(i);
                break;
                case E_SOUND_SPEAKER_MODE:
                    //sfx[i].position = (int)pc_speaker_song_pos;
                break;
            }
            
            //clear flag when sound finished
            if (sfx[i].position == -1)
            {
                sfx[i].playing = false;
                sfx[i].finished = true;
            }
        }
    }
}

//function to play a sound
void sfx_play(SAMPLE* sampleFile, uint8_t voice, bool rndFreq)
{
    ASSERT(voice < sfxVoices);
    
    //TODO: for what??
    //sfx[voice].sampleId = soundId;
    
    switch (soundMode)
    {
        case E_SOUND_SB_MODE:            
            //reallocate the sample on select voice of selected channel
            if (!voice_check(voice))
            {                
                voice = allocate_voice(sampleFile);             
            }
            else
            {                
                reallocate_voice(voice, sampleFile);                
            }

            //TODO: other function to randomize frequency?
            //randomize frequency
            /*
            if (rndFreq)
            {
                //get a random percent variation from twice of SFX_FREQ_RND_PERCENT (half for negative, half for positive)
                int freqVariation = (rand() % (SFX_FREQ_RND_PERCENT * 2));

                //get sample original frequency
                int sampleFreq = voice_get_frequency(voice);
                TRACE("Original freq: %iHz | ", sampleFreq);

                //calculate new frequency
                fixed newFreq;
                //if variation is below half
                if (freqVariation < SFX_FREQ_RND_PERCENT)
                {
                    //sub the percentage variation to original freq
                    newFreq = itofix(sampleFreq) - fixmul(itofix(sampleFreq),(fixdiv(itofix(freqVariation),itofix(100))));
                    TRACE("Variation: -%i%% | ", freqVariation);
                }
                else
                {
                    //add the percentage variation to original freq
                    newFreq = fixmul(itofix(sampleFreq), fixdiv(itofix(freqVariation - SFX_FREQ_RND_PERCENT), itofix(100.0))) + itofix(sampleFreq);
                    TRACE("Variation: +%i%% | ", (freqVariation - SFX_FREQ_RND_PERCENT));
                }

                //set the new frequency
                voice_set_frequency(voice, fixtoi(newFreq));
                TRACE("New freq: %iHz\n", fixtoi(newFreq));
                
            }*/
            
            //start sample allocated on voice channel
            voice_start(voice);
        break;
        case E_SOUND_SPEAKER_MODE:
            //pc_speaker_play_sfx(_sfx_notes, _sfx_durations);
        break;
    }

    //set flag
    sfx[voice].playing = true;
    sfx[voice].finished = false;
}

void sfx_stop(uint8_t voice)
{
    sfx[voice].stop = true;
}

void sfx_pause(uint8_t voice)
{
    sfx[voice].pause = true;
}

void sfx_resume(uint8_t voice)
{
    sfx[voice].pause = false;
}

tSfx sfx_get_voice_data(uint8_t voice)
{
    return sfx[voice];
}

void sfx_set_voice_data(uint8_t voice, tSfx voiceData)
{
    sfx[voice] = voiceData;
}

bool sfx_voice_is_playing(uint8_t voice)
{
    return sfx[voice].playing;
}

bool sfx_voice_finished(uint8_t voice)
{
    return sfx[voice].finished;
}

int sfx_get_voice_sample_id(uint8_t voice)
{
    return sfx[voice].sampleId;
}

void sfx_voice_clear_finished(uint8_t voice)
{
    sfx[voice].finished = false;    
}

void sfx_voice_reallocate(SAMPLE* sampleFile, uint8_t voice)
{
    reallocate_voice(voice, sampleFile);
}

void sfx_voice_set_position(uint8_t voice, int position)
{
    voice_set_position(voice, position);
}
