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
//init sample to allocate voices
SAMPLE *initSfx;

union uMidiTempo {
    uint32_t value;
    uint8_t bytes[sizeof(uint32_t)];
};

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

void sound_set_mode(enum E_SOUND_MODES _soundMode)
{
    soundMode = _soundMode;
}

enum E_SOUND_MODES sound_get_mode()
{
    return soundMode;
}

void music_play(MIDI *midiFile, bool loop)
{
    //stop actual music
    music_stop();

    if (soundMode == E_SOUND_SB_MODE)
    {
        //load index music index
        MY_TRACE_FLAG("Load music data file object\n");
        
        //play midi
        MY_TRACE_FLAG("Play midi music\n");
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

void music_stop()
{
    //stop actual music
    MY_TRACE_FLAG("Stopping midi\n");
    if (soundMode == E_SOUND_SB_MODE)
        stop_midi();
    else if (soundMode == E_SOUND_SPEAKER_MODE)
        ;//pc_speaker_stop_song();
}

void music_pause()
{
    if (soundMode == E_SOUND_SB_MODE)
        midi_pause();
    else if (soundMode == E_SOUND_SPEAKER_MODE)
        ;//pc_speaker_pause_song();
}

void music_resume()
{
    if (soundMode == E_SOUND_SB_MODE)
        midi_resume();
    else if (soundMode == E_SOUND_SPEAKER_MODE)
        ;//pc_speaker_resume_song();
}

long music_get_pos()
{
    if (soundMode == E_SOUND_SB_MODE)
        return midi_pos;
    //else if (soundMode == E_SOUND_SPEAKER_MODE)
        //return pc_speaker_song_pos;
    else
        return -1;
}

void music_seek(int position)
{
    if (soundMode == E_SOUND_SB_MODE)
        midi_seek(position);
    else if (soundMode == E_SOUND_SPEAKER_MODE)
        ;//pc_speaker_seek_song(position);
}

void music_set_volume(uint8_t volume)
{
    set_hardware_volume(-1, volume);
    set_volume(-1, volume);
}

void music_set_tempo(uint8_t tempo)
{
    if (soundMode != E_SOUND_OFF_MODE)
    {
        union uMidiTempo midiTempo;
        
        //calculate uSeconds based on tempo
        midiTempo.value = (60000 / tempo) * 1000;
        
        //compose midi msg
        uint8_t msg[6];
        msg[0] = 0xFF;  //custom midi msg
        msg[1] = 0x51;  //set tempo command
        msg[2] = 0x03;  //tempo num bytes
        //tempo value from union
        msg[3] = midiTempo.bytes[2];
        msg[4] = midiTempo.bytes[1];
        msg[5] = midiTempo.bytes[0];
        
        //send midi msg
        midi_out(msg, 6);
    }
}

void sfx_init(SAMPLE *initSample, uint8_t numVoices)
{
    //allocate sfx array
    sfx  = (tSfx *)malloc(numVoices * sizeof(tSfx));
    sfxVoices = numVoices;

    //assign the init sample for destroy later
    initSfx = initSample;

    //init all sfx voices with sample sfx (If you want always use a specific voice on sfx_play, you need to pre allocate all voices)
    for (int i = 0; i < sfxVoices; i++)
    {
        //get soundcard voice (reallocate if exists)
        if (!voice_check(i))
        {
            int voice = allocate_voice(initSfx);
            MY_TRACE_FLAG("SFX voice %i allocated to soundcard voice %i\n", i, voice);
        }
        else
            reallocate_voice(i, initSfx);
        

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
    
    //free init sample
    destroy_sample(initSfx);

    MY_TRACE_FLAG("SFX system destroyed\n");
}

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

void sfx_play(SAMPLE* sampleFile, uint8_t voice)
{
    ASSERT(voice < sfxVoices);
    
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

            //start sample allocated on voice channel
            voice_start(voice);
            MY_TRACE_FLAG("SFX voice %i played\n", voice);
        break;
        case E_SOUND_SPEAKER_MODE:
            //pc_speaker_play_sfx(_sfx_notes, _sfx_durations);
        break;
    }

    //set flag
    sfx[voice].playing = true;
    sfx[voice].finished = false;
}

void sfx_play_rnd(SAMPLE* sampleFile, uint8_t voice)
{
    ASSERT(voice < sfxVoices);
    
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

            //randomize frequency
            
            //get a random percent variation from twice of SFX_FREQ_RND_PERCENT (half for negative, half for positive)
            int freqVariation = (rand() % (SFX_FREQ_RND_PERCENT * 2));

            //get sample original frequency
            int sampleFreq = voice_get_frequency(voice);
            MY_TRACE_FLAG("Original freq: %iHz | ", sampleFreq);

            //calculate new frequency
            fixed newFreq;
            //if variation is below half
            if (freqVariation < SFX_FREQ_RND_PERCENT)
            {
                //sub the percentage variation to original freq
                newFreq = itofix(sampleFreq) - fixmul(itofix(sampleFreq),(fixdiv(itofix(freqVariation),itofix(100))));
                MY_TRACE_FLAG("Variation: -%i%% | ", freqVariation);
            }
            else
            {
                //add the percentage variation to original freq
                newFreq = fixmul(itofix(sampleFreq), fixdiv(itofix(freqVariation - SFX_FREQ_RND_PERCENT), itofix(100.0))) + itofix(sampleFreq);
                MY_TRACE_FLAG("Variation: +%i%% | ", (freqVariation - SFX_FREQ_RND_PERCENT));
            }

            //set the new frequency
            voice_set_frequency(voice, fixtoi(newFreq));
            MY_TRACE_FLAG("New freq: %iHz\n", fixtoi(newFreq));
                
            //start sample allocated on voice channel
            voice_start(voice);
            MY_TRACE_FLAG("SFX voice %i played\n", voice);
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

/*int sfx_get_voice_sample_id(uint8_t voice)
{
    return sfx[voice].sampleId;
}*/

void sfx_voice_clear_finished(uint8_t voice)
{
    sfx[voice].finished = false;    
}

void sfx_voice_reallocate(SAMPLE* sampleFile, uint8_t voice)
{
    reallocate_voice(voice, sampleFile);
}

void sfx_voice_deallocate(uint8_t voice)
{
    deallocate_voice(voice);
}

void sfx_voice_set_position(uint8_t voice, int position)
{
    voice_set_position(voice, position);
}

void sfx_set_volume(uint8_t volume)
{
    set_hardware_volume(volume, -1);
    set_volume(volume, -1);
}