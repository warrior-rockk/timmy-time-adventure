/********************************************************************
* Map System
*
* 11/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "entity.h"

#define TRACE_FLAG  "[MAP]"

//map header info
struct mapHeader{
    uint16_t tile_width;
    uint16_t tile_height;
    uint16_t map_width;
    uint16_t map_height;
    uint16_t backgroundColor;
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
} tMapEntity;

tTile *map;
tTile *tilesWithProperty;
tTileAnimation *tileAnimation;
tMapEntity *mapObjects;
tMapEntity *mapEnemies;
BITMAP **tiles;
BITMAP *mapTileSheet;
uint8_t *mapIds;
tVector screenLimit;
bool tilesOnFrontLayer;

void map_load(char *mapFile, char *tileFile, tVector screenSize)
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
    
    //set screen limit for draw map
    screenLimit.x = mapHeader.map_width > (screenSize.x / mapHeader.tile_width) ? (screenSize.x / mapHeader.tile_width) + 1 : screenSize.x / mapHeader.tile_width;
    screenLimit.y =  mapHeader.map_height > (screenSize.y / mapHeader.tile_height) ? (screenSize.y / mapHeader.tile_height) + 1 : screenSize.y / mapHeader.tile_height;

    MY_TRACE_FLAG("Loading map: %s with tileFile: %s\n", mapFile, tileFile);
    MY_TRACE_FLAG("\tTile dimensions: %u x %u px\n", mapHeader.tile_width, mapHeader.tile_height);
    MY_TRACE_FLAG("\tMap dimensions: %u x %u tiles\n", mapHeader.map_width, mapHeader.map_height);
    MY_TRACE_FLAG("\tBackground color: %u\n", mapHeader.backgroundColor);
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
        fread(&tilesWithProperty[i].tileProperty,   sizeof(uint8_t),    1, file);
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

    //read map objects
    uint16_t numMapObjects;
    fread(&numMapObjects, sizeof(uint16_t), 1, file);
    MY_TRACE_FLAG("\tNum objects on map: %i\n", numMapObjects);
    
    //allocate memory for objects
    mapObjects = (tMapEntity *)malloc(numMapObjects * sizeof(tMapEntity));
    if (mapObjects == NULL) {
        fclose(file);
        abort_on_error("Can allocate memory for %u objects on map\n", numMapObjects);
    }

    //read data by field to avoid padding problems
    for (uint16_t i = 0; i < numMapObjects; i++)
    {
        fread(&mapObjects[i].class,   sizeof(uint8_t),    1, file);
        fread(&mapObjects[i].type,    sizeof(uint8_t),    1, file);
        fread(&mapObjects[i].x,       sizeof(uint16_t),   1, file);
        fread(&mapObjects[i].y,       sizeof(uint16_t),   1, file);
        fread(&mapObjects[i].dir,     sizeof(uint8_t),    1, file);

        MY_TRACE_FLAG("\t\tObject Class: %u Type: %u X: %i Y: %i Dir: %u\n", mapObjects[i].class, mapObjects[i].type, mapObjects[i].x, mapObjects[i].y, mapObjects[i].dir);

        //create entity
        entity_create(mapObjects[i].class, mapObjects[i].type, (tVector){mapObjects[i].x, mapObjects[i].y}, mapObjects[i].dir);
    }

    //read map enemies
    uint16_t numMapEnemies;
    fread(&numMapEnemies, sizeof(uint16_t), 1, file);    
    MY_TRACE_FLAG("\tNum enemies on map: %i\n", numMapEnemies);
    
    if (numMapEnemies > 0)
    {
        //allocate memory for enemies
        mapEnemies = (tMapEntity *)malloc(numMapEnemies * sizeof(tMapEntity));
        if (mapEnemies == NULL) {
            fclose(file);
            abort_on_error("Can allocate memory for %u enemies on map\n", numMapEnemies);
        }

        //read data by field to avoid padding problems
        for (uint16_t i = 0; i < numMapEnemies; i++)
        {
            fread(&mapEnemies[i].class,   sizeof(uint8_t),    1, file);
            fread(&mapEnemies[i].type,    sizeof(uint8_t),    1, file);
            fread(&mapEnemies[i].x,       sizeof(uint16_t),   1, file);
            fread(&mapEnemies[i].y,       sizeof(uint16_t),   1, file);
            fread(&mapEnemies[i].dir,     sizeof(uint8_t),    1, file);

            MY_TRACE_FLAG("\t\tEnemy Class: %u Type: %u X: %i Y: %i Dir: %u\n", mapEnemies[i].class, mapEnemies[i].type, mapEnemies[i].x, mapEnemies[i].y, mapEnemies[i].dir);

            //create entity
            entity_create(mapEnemies[i].class, mapEnemies[i].type, (tVector){mapEnemies[i].x, mapEnemies[i].y}, mapEnemies[i].dir);
        }
    
        free(mapEnemies);
        mapEnemies = NULL;
    }

    //clean resources    
    fclose(file);    
    free(mapIds);    
    free(mapObjects);    
    free(tilesWithProperty);
    mapIds = NULL;
    mapObjects = NULL;
    tilesWithProperty = NULL;

    //allocate tiles bitmaps    
    tiles = (BITMAP **)malloc(mapHeader.tileCount * sizeof(BITMAP));
    //load map tileSheet    
    mapTileSheet = load_bmp(tileFile, desktop_palette);
    //create tiles from tilesheet image
    for (uint8_t i = 0; i < mapHeader.tileCount; i++)
    {
        tiles[i] = create_sub_bitmap(mapTileSheet, (i % mapHeader.tileColumns) * mapHeader.tile_height, (int)(i / mapHeader.tileColumns) * mapHeader.tile_width, mapHeader.tile_width, mapHeader.tile_height);
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

void map_draw(BITMAP *buffer, tScroll *scroll, bool frontLayer)
{
    tTile *tile;
    bool memTileStopScrollX = false;
    bool memTileStopScrollY = false;
    int16_t sx = scroll->pos.x % mapHeader.tile_width;      //tile pos x on scroll
    int16_t sy = scroll->pos.y % mapHeader.tile_height;     //tile pos y on scroll
    int16_t tx = scroll->pos.x / mapHeader.tile_width;      //tile num x on scroll
    int16_t ty = scroll->pos.y / mapHeader.tile_height;     //tile num y on scroll   
    
    //TODO: replace clear all buffer with color only positions with no tiles?
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
                //get map tile on current position
                tile = &map[((y + ty) * mapHeader.map_width) + x + tx];
                
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
                        //check if tile animation 
                        if (CHECK_FLAG(tile->tileProperty, E_TILE_PROP_ANIMATION))
                            //draw tile animation frame                            
                            draw_sprite(buffer, tiles[tileAnimation[tile->tileAnimationId].frames[tileAnimation[tile->tileAnimationId].anim.frame].frameId], (x * mapHeader.tile_width) - sx, (y * mapHeader.tile_height) - sy);                        
                        else if (CHECK_FLAG(tile->tileProperty, E_TILE_PROP_NO_SCROLL_Y))
                        {
                            //memorize tile with stop scroll
                            memTileStopScrollY = true;                                   
                        }
                        else if (CHECK_FLAG(tile->tileProperty, E_TILE_PROP_NO_SCROLL_X))
                        {
                            //memorize tile with stop scroll
                            memTileStopScrollX = true;                            
                        }
                        else
                        {
                            //draw tile id
                            draw_sprite(buffer, tiles[tile->tileId - 1], (x * mapHeader.tile_width) - sx, (y * mapHeader.tile_height) - sy);                        
                        }
                    }
                }
            }    
        }
    }

    //sets stop scroll if any tile with this property (only back layer)
    if (!frontLayer)
    {
        scroll->stopScrollY = memTileStopScrollY;
        scroll->stopScrollX = memTileStopScrollX;
    }
}


uint16_t map_tile_exists(tVector checkPosition)
{
    return (checkPosition.x / mapHeader.tile_width) < mapHeader.map_width && (checkPosition.y / mapHeader.tile_height) < mapHeader.map_height && checkPosition.x >= 0 && checkPosition.y >= 0;
}

uint8_t map_get_tile_property(tVector checkPosition)
{
    if (map[((checkPosition.y / mapHeader.tile_height) * mapHeader.map_width) + (checkPosition.x / mapHeader.tile_width)].tileId == 0)
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