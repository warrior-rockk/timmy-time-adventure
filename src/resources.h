/********************************************************************
* Resources system definitions
*
* 23/03/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#ifndef _H_RESOURCES_
#define _H_RESOURCES_

#include "allegro.h"

//creates datafile index to access the objects of datafile by his index
DATAFILE_INDEX* create_dat_index(const char *filename);
//destroys datafile index
void destroy_dat_index(DATAFILE_INDEX *dataFileIndex);
//loads bmp object from datafile by his object index
BITMAP* load_dat_bmp_indexed(DATAFILE_INDEX *dataFileIndex, int item);
//loads wav object from datafile by his object index
SAMPLE* load_dat_wav_indexed(DATAFILE_INDEX *dataFileIndex, int item);
//loads midi object from datafile by his object index
MIDI* load_dat_midi_indexed(DATAFILE_INDEX *dataFileIndex, int item);
//loads palette object from datafile by his object index
RGB* load_dat_pal_indexed(DATAFILE_INDEX *dataFileIndex, int item);
//checks if the entity resource is assigned or loads the bmp resource of the resource index
void load_entity_bmp_resources(BITMAP **entityResource, DATAFILE_INDEX *resourceIndex, uint8_t resourceItemId);
//checks if the entity resource is assigned or loads the wav resource of the resource index
void load_entity_wav_resources(SAMPLE **entityResource, DATAFILE_INDEX *resourceIndex, uint8_t resourceItemId);
#endif