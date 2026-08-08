/********************************************************************
* Map System
*
* 11/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "map.h"

#define TRACE_FLAG  "[MAP]"

//map header info
struct mapHeader{
    uint16_t tile_width;
    uint16_t tile_height;
    uint16_t map_width;
    uint16_t map_height;
    uint16_t backgroundColor;
    uint16_t levelTime;
    uint16_t tileCount;
    uint16_t tileColumns;
    uint16_t numTilesWithProperty;
    uint16_t numTilesWithAnimation;
} mapHeader;

//type of map object entity data
typedef struct {
    uint8_t class;
    uint8_t type;
    uint16_t x;
    uint16_t y;
    uint8_t dir;
    int16_t spare;
} tMapEntity;

tTile *map;
tTile *tilesWithProperty;
tTileAnimation *tileAnimation;
tMapEntity *mapEntities;
BITMAP **tiles;
BITMAP *mapTileSheet;
uint8_t *mapIds;
tVector screenLimit;
bool tilesOnFrontLayer;

void map_load(char *mapFile, BITMAP *tileset, tVector screenSize)
{
    //load map file
    FILE *file = fopen(mapFile, "rb");
    if (!file) {
        abort_on_error("Error opening map file %s\n", mapFile);
    }

    //read map file header    
    if (fread(&mapHeader, sizeof(mapHeader), 1, file) != 1) {
        fclose(file);
        abort_on_error("Error reading map header\n");
    }
    
    //check square tile size
    if (mapHeader.tile_height != mapHeader.tile_width)
        abort_on_error("Tile size must be square\n");

    //set screen limit for draw map (adds 1 tile x and y to screen limit for maps larger than screen)
    screenLimit.x = mapHeader.map_width > (screenSize.x / mapHeader.tile_width) ? (screenSize.x / mapHeader.tile_width) + 1  : screenSize.x / mapHeader.tile_width;
    screenLimit.y =  mapHeader.map_height > (screenSize.y / mapHeader.tile_height) ? (screenSize.y / mapHeader.tile_height) + 1 : screenSize.y / mapHeader.tile_height;
    
    MY_TRACE_FLAG("Loading map: %s\n", mapFile);
    MY_TRACE_FLAG("\tTile dimensions: %u x %u px\n", mapHeader.tile_width, mapHeader.tile_height);
    MY_TRACE_FLAG("\tMap dimensions: %u x %u tiles\n", mapHeader.map_width, mapHeader.map_height);
    MY_TRACE_FLAG("\tBackground color: %u\n", mapHeader.backgroundColor);
    MY_TRACE_FLAG("\tLevel time: %u\n", mapHeader.levelTime);
    MY_TRACE_FLAG("\tTile count: %u\n", mapHeader.tileCount);
    MY_TRACE_FLAG("\tTiles with property: %u\n", mapHeader.numTilesWithProperty);
    MY_TRACE_FLAG("\tTiles with animations: %u\n", mapHeader.numTilesWithAnimation);

    //Calculate number of tiles and reservate memory
    uint16_t total_tiles = mapHeader.map_width * mapHeader.map_height;
    MY_TRACE_FLAG("\tTotal tiles on map: %i\n", total_tiles);

    mapIds  = (uint8_t *)malloc(total_tiles * sizeof(uint8_t));
    map     = (tTile *)malloc(total_tiles * sizeof(tTile));
    
    if (mapIds == NULL || map == NULL) {
        fclose(file);
        abort_on_error("Can't allocate memory for %u tiles\n", total_tiles);
    }

    //read the full tile array id
    size_t read_count = fread(mapIds, sizeof(uint8_t), total_tiles, file);
    if (read_count != total_tiles) {
        abort_on_error("Expected read %u tiles, but readed %zu\n", total_tiles, read_count);
    }
    
    //allocate memory for temporal array of tile with property
    tilesWithProperty = (tTile *)malloc(mapHeader.numTilesWithProperty * sizeof(tTile)); 
    if (tilesWithProperty == NULL) {
        fclose(file);
        abort_on_error("Can't allocate memory for %u tiles with property\n", mapHeader.numTilesWithProperty);
    }
    
    //read temporal array of tiles with property
    for (uint8_t i = 0; i < mapHeader.numTilesWithProperty; i++)
    {
        fread(&tilesWithProperty[i].tileId,         sizeof(uint8_t),    1, file);
        fread(&tilesWithProperty[i].tileProperty,   sizeof(uint16_t),    1, file);
        //MY_TRACE_FLAG("\tReaded tile id: %i with property: %i\n", tilesWithProperty[i].tileId, tilesWithProperty[i].tileProperty);
    }
    
    //fill the tile properties of full map
    for (uint16_t i = 0; i < total_tiles; i++)
    {
        //assign id tile
        map[i].tileId = mapIds[i];
        map[i].tileProperty = 0;    //default property value

        //iterate the tiles with properties
        for (uint16_t j = 0; j < mapHeader.numTilesWithProperty; j++)
        {
            //search tile id 
            if (tilesWithProperty[j].tileId == map[i].tileId)
            {
                //if exists, asign property
                map[i].tileProperty = tilesWithProperty[j].tileProperty;
                break;    
            }
        }
    }
    
    //allocate memory for array of tile with animation
    tileAnimation = (tTileAnimation *)malloc(mapHeader.numTilesWithAnimation * sizeof(tTileAnimation)); 
    if (tileAnimation == NULL) {
        fclose(file);
        abort_on_error("Can't allocate memory for %u tiles with animation\n", mapHeader.numTilesWithAnimation);
    }
    
    //read array of tiles with animation
    for (uint8_t i = 0; i < mapHeader.numTilesWithAnimation; i++)
    {
        //read tile animation data
        fread(&tileAnimation[i].tileId,      sizeof(uint8_t),    1, file);
        fread(&tileAnimation[i].numFrames,   sizeof(uint8_t),    1, file);
        MY_TRACE_FLAG("Tile animation id: %i has tileId: %i with %i frames\n", i, tileAnimation[i].tileId, tileAnimation[i].numFrames);

        //inits frame time to ensure tile animation syncronization
        tileAnimation[i].anim.frameTime = 0;

        //if tile has animation, save animation property to tile to check later
        for (uint16_t j = 0; j < total_tiles; j++)
        {            
            //search tile id 
            if (map[j].tileId == tileAnimation[i].tileId)
            {
                //if exists, assign animation property
                SET_FLAG(map[j].tileProperty, E_TILE_PROP_ANIMATION);
                //assign animation id
                map[j].tileAnimationId = i;      
                MY_TRACE_FLAG("\tTile id: %i has tileAnimationId: %i\n", j, i);          
            }            
        }

        //allocate memory for tile frames of animation
        tileAnimation[i].frames = (tAnimFrame *)malloc(tileAnimation[i].numFrames * sizeof(tAnimFrame)); 
        if (tileAnimation[i].frames == NULL) {
            fclose(file);
            abort_on_error("Can't allocate memory for %u frames of animation\n", tileAnimation[i].numFrames);
        }

        //read tile animation frames
        for (uint8_t frame = 0; frame < tileAnimation[i].numFrames; frame++)
        {
            fread(&tileAnimation[i].frames[frame].frameId,      sizeof(uint8_t),     1, file);
            fread(&tileAnimation[i].frames[frame].duration,    sizeof(uint16_t),    1, file);
            
            //adjust time to game clock
            tileAnimation[i].frames[frame].duration /= GAME_CLOCK_TICK;

            MY_TRACE_FLAG("\t\tTile animation id: %i and frame %i has frameId: %i with duration: %i\n", i, frame, tileAnimation[i].frames[frame].frameId, tileAnimation[i].frames[frame].duration);
        }
    }

    //read map entities
    uint16_t numMapEntities;
    fread(&numMapEntities, sizeof(uint16_t), 1, file);
    MY_TRACE_FLAG("\tNum entities on map: %i\n", numMapEntities);
    
    //allocate memory for objects
    mapEntities = (tMapEntity *)malloc(numMapEntities * sizeof(tMapEntity));
    if (mapEntities == NULL) {
        fclose(file);
        abort_on_error("Can allocate memory for %u entities on map\n", numMapEntities);
    }

    //read data by field to avoid padding problems
    for (uint16_t i = 0; i < numMapEntities; i++)
    {
        fread(&mapEntities[i].class,   sizeof(uint8_t),    1, file);
        fread(&mapEntities[i].type,    sizeof(uint8_t),    1, file);
        fread(&mapEntities[i].x,       sizeof(uint16_t),   1, file);
        fread(&mapEntities[i].y,       sizeof(uint16_t),   1, file);
        fread(&mapEntities[i].dir,     sizeof(uint8_t),    1, file);
        fread(&mapEntities[i].spare,   sizeof(int16_t),    1, file);

        #if DEBUG_TRACE_CREATE_ENTITIES
            MY_TRACE_FLAG("\tEntity Class: %u Type: %u X: %i Y: %i Dir: %u Spare: %i\n", mapEntities[i].class, mapEntities[i].type, mapEntities[i].x, mapEntities[i].y, mapEntities[i].dir, mapEntities[i].spare);
        #endif
        
        //create entity
        entity_create(mapEntities[i].class, mapEntities[i].type, (tVector){mapEntities[i].x, mapEntities[i].y}, mapEntities[i].dir, mapEntities[i].spare);        
    }

    //clean resources    
    fclose(file);    
    free(mapIds);    
    free(mapEntities);    
    free(tilesWithProperty);
    mapIds = NULL;
    mapEntities = NULL;
    tilesWithProperty = NULL;

    //allocate tiles bitmaps    
    tiles = (BITMAP **)malloc(mapHeader.tileCount * sizeof(BITMAP));
    
    //create tiles from tilesheet image
    for (uint8_t i = 0; i < mapHeader.tileCount; i++)
    {
        tiles[i] = create_sub_bitmap(tileset, (i % mapHeader.tileColumns) * mapHeader.tile_height, (int)(i / mapHeader.tileColumns) * mapHeader.tile_width, mapHeader.tile_width, mapHeader.tile_height);
    }        
}

void map_unload()
{    
    //destroy all tile sub-bitmaps
    for (uint8_t i = 0; i < mapHeader.tileCount; i++)
    {
        destroy_bitmap(tiles[i]);    
    }
    destroy_bitmap(mapTileSheet);
    
    //free map data
    free(map);    
    free(tiles);
    free(tileAnimation);
    map = NULL;
    tiles = NULL;

    //initialize map data
    memset(&mapHeader, 0, sizeof(mapHeader));
    
    MY_TRACE_FLAG("Map unloaded\n");
}

void map_draw(BITMAP *buffer, bool frontLayer)
{
    tTile *tile;
    tVector scrollPos = scroll_get_position();

    int16_t sx = scrollPos.x % mapHeader.tile_width;      //tile pos x on scroll
    int16_t sy = scrollPos.y % mapHeader.tile_height;     //tile pos y on scroll
    int16_t tx = scrollPos.x / mapHeader.tile_width;      //tile num x on scroll
    int16_t ty = scrollPos.y / mapHeader.tile_height;     //tile num y on scroll  
    
    if (!frontLayer)
    {
        clear_to_color(buffer, mapHeader.backgroundColor);
        tilesOnFrontLayer = false;

        //update tile animations
        for (uint16_t i = 0; i < mapHeader.numTilesWithAnimation; i++)
        {            
            play_animation_seq(&tileAnimation[i].anim, tileAnimation[i].frames, tileAnimation[i].numFrames, ANIM_LOOP);            
        }
    }        

    //draw map if not front layer or tiles on front layer
    if (!frontLayer || tilesOnFrontLayer)
    {
        for (int y = 0; y < screenLimit.y; y++)
        {
            for (int x = 0; x < screenLimit.x; x++)        
            {
                //check if tile exists on map array
                if ((((y + ty) * mapHeader.map_width) + x + tx) < (mapHeader.map_width * mapHeader.map_height))
                    //get map tile on current position
                    tile = &map[((y + ty) * mapHeader.map_width) + x + tx];
                else
                    break;
                
                //if has tile id
                if (tile->tileId != 0)
                {  
                    //no front layer tile not draw on front layer
                    if (!CHECK_FLAG(tile->tileProperty, E_TILE_PROP_FRONT_LAYER) && frontLayer)
                        ;
                    //if tile is front layer, but are in back layer, set flag one front layer tile at least
                    else if (CHECK_FLAG(tile->tileProperty, E_TILE_PROP_FRONT_LAYER) && !frontLayer)
                        tilesOnFrontLayer = true;
                    else 
                    {
                        if (buffer != NULL)
                        {
                            //check if tile animation 
                            if (CHECK_FLAG(tile->tileProperty, E_TILE_PROP_ANIMATION))
                                //draw tile animation frame                            
                                //draw_sprite(buffer, tiles[tileAnimation[tile->tileAnimationId].frames[tileAnimation[tile->tileAnimationId].anim.frame].frameId], (x * mapHeader.tile_width) - sx, (y * mapHeader.tile_height) - sy);                                                
                                blit(tiles[tileAnimation[tile->tileAnimationId].frames[tileAnimation[tile->tileAnimationId].anim.frame].frameId], buffer, 0, 0, (x * mapHeader.tile_width) - sx, (y * mapHeader.tile_height) - sy, 16, 16);
                            else
                            {
                                //draw tile id (for performance reasons, only draw sprite with transparency on front layer tiles)
                                if (frontLayer)
                                    draw_sprite(buffer, tiles[tile->tileId - 1], (x * mapHeader.tile_width) - sx, (y * mapHeader.tile_height) - sy);                        
                                else
                                    blit(tiles[tile->tileId - 1], buffer, 0, 0, (x * mapHeader.tile_width) - sx, (y * mapHeader.tile_height) - sy, 16, 16);
                            }
                        }
                        else
                        {
                            abort_on_error("map: buffer pointer null\n");
                        }
                    }
                }
            }    
        }
    }
}

uint16_t map_tile_exists(tVector checkPosition)
{
    return (checkPosition.x / mapHeader.tile_width) < mapHeader.map_width && (checkPosition.y / mapHeader.tile_height) < mapHeader.map_height && checkPosition.x >= 0 && checkPosition.y >= 0;
}

uint16_t map_get_tile_property(tVector checkPosition)
{
    if (!map_tile_exists(checkPosition))
        return E_TILE_PROP_NO_SOLID;
    else if (map[((checkPosition.y / mapHeader.tile_height) * mapHeader.map_width) + (checkPosition.x / mapHeader.tile_width)].tileId == 0)
        return E_TILE_PROP_NO_SOLID;
    else
    {
        return  map[((checkPosition.y / mapHeader.tile_height) * mapHeader.map_width) + (checkPosition.x / mapHeader.tile_width)].tileProperty;
    }
}

tVector map_get_dimensions()
{
    return (tVector){mapHeader.map_width * mapHeader.tile_width, mapHeader.map_height * mapHeader.tile_height};
}

int16_t map_snap_x_to_tile(tEntity *entity)
{
    return (entity->pos.x + (entity->size.x >> 1)) + (mapHeader.tile_width >> 1) - ((entity->pos.x + (entity->size.x >> 1)) % mapHeader.tile_width) - (entity->size.x >> 1);
}

int16_t map_snap_y_to_tile(tEntity *entity)
{
    return (entity->pos.y + (entity->size.y >> 1)) + (mapHeader.tile_height >> 1) - ((entity->pos.y + (entity->size.y >> 1)) % mapHeader.tile_height) - (entity->size.y >> 1);
}

tVector map_get_tile_position(tVector absolutePosition)
{
    return (tVector){absolutePosition.x / mapHeader.tile_height, absolutePosition.y / mapHeader.tile_height};
}

uint16_t map_get_level_time()
{
    return mapHeader.levelTime;
}

void map_change_background_color(uint8_t color)
{
    mapHeader.backgroundColor = color;
}

uint8_t map_get_tile_size()
{
    return mapHeader.tile_height;
}

void map_change_tile(tVector tilePosition, uint8_t tileId, uint16_t tileProperty)
{
    map[(tilePosition.y * mapHeader.map_width) + tilePosition.x].tileId = tileId;
    map[(tilePosition.y * mapHeader.map_width) + tilePosition.x].tileProperty = tileProperty;
}

void map_set_tile_animation(tVector tilePosition, uint8_t tileId, uint8_t tileAnimId)
{
    map[(tilePosition.y * mapHeader.map_width) + tilePosition.x].tileAnimationId = tileAnimId;
}