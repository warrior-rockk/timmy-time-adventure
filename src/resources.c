/********************************************************************
* Resources system code
*
* 23/03/2026
* Warcom Soft. - warrior.rockk@gmail.com
********************************************************************/
#include "resources.h"
#include "utils.h"

DATAFILE_INDEX* create_dat_index(const char *filename)
{
    DATAFILE_INDEX *dataFileIndex = create_datafile_index(filename);
    if (!dataFileIndex)
        abort_on_error("Error loading datafile %s\n", filename);
    
    return dataFileIndex;
}
