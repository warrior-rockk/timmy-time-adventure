/********************************************************************
* Resources system code
*
* 23/03/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include "resources.h"
#include "utils.h"

#define TRACE_FLAG  "[RESOURCES]"

//loads a datafile object from datafile index checking the type
static DATAFILE* load_dat_indexed(DATAFILE_INDEX *dataFileIndex, int item, int type)
{
    DATAFILE *dataFileItem = load_datafile_object_indexed(dataFileIndex, item);
    
    if (!dataFileItem)
        abort_on_error("Can't load item %i from datafile%s\n", item, dataFileIndex->filename); 
    else if (dataFileItem->type != type)
        abort_on_error("Item %i from datafile index %s type incorrect\n", item, dataFileIndex->filename);
    
    MY_TRACE_FLAG("Loaded item %i from datafile%s\n", item, dataFileIndex->filename);
    return dataFileItem;
}

DATAFILE_INDEX* create_dat_index(const char *filename)
{
    DATAFILE_INDEX *dataFileIndex = create_datafile_index(filename);
    if (!dataFileIndex)
        abort_on_error("Error loading datafile %s\n", filename);
    
    MY_TRACE_FLAG("Create datafile index for %s\n", filename);
    return dataFileIndex;
}

BITMAP* load_dat_bmp_indexed(DATAFILE_INDEX *dataFileIndex, int item)
{
    return load_dat_indexed(dataFileIndex, item, DAT_BITMAP)->dat;
}

SAMPLE* load_dat_wav_indexed(DATAFILE_INDEX *dataFileIndex, int item)
{
    return load_dat_indexed(dataFileIndex, item, DAT_SAMPLE)->dat;
}

MIDI* load_dat_midi_indexed(DATAFILE_INDEX *dataFileIndex, int item)
{
    return load_dat_indexed(dataFileIndex, item, DAT_MIDI)->dat;
}

RGB* load_dat_pal_indexed(DATAFILE_INDEX *dataFileIndex, int item)
{
    return load_dat_indexed(dataFileIndex, item, DAT_PALETTE)->dat;
}

void destroy_dat_index(DATAFILE_INDEX *dataFileIndex)
{
    MY_TRACE_FLAG("Destroyed datafile index for %s\n", dataFileIndex->filename);
    if (dataFileIndex)
        destroy_datafile_index(dataFileIndex);
}