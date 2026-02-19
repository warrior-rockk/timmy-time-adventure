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
tMapEntity *mapObjects;
tMapEntity *mapEnemies;
BITMAP **tiles;
BITMAP *mapTileSheet;
//temporal
uint8_t *mapIds;
tTile *tilesWithProperty;

void map_load(char *mapFile, char *tileFile)
{
    //load map file
    FILE *file = fopen(mapFile, "rb");
    if (!file) {
        abort_on_error("Error al abrir el archivo");
    }

    //read map file header    
    if (fread(&mapHeader, sizeof(mapHeader), 1, file) != 1) {
        fclose(file);
        abort_on_error("Error al leer el encabezado.\n");
    }
    
    MY_TRACE_FLAG("Loading map: %s with tileFile: %s\n", mapFile, tileFile);
    MY_TRACE_FLAG("\tTile dimensions: %u x %u px\n", mapHeader.tile_width, mapHeader.tile_height);
    MY_TRACE_FLAG("\tMap dimensions: %u x %u tiles\n", mapHeader.map_width, mapHeader.map_height);
    MY_TRACE_FLAG("\tBackground color: %u\n", mapHeader.backgroundColor);
    MY_TRACE_FLAG("\tTile count: %u\n", mapHeader.tileCount);
    MY_TRACE_FLAG("\tTiles with property: %u\n", mapHeader.numTilesWithProperty);

    //Calculate number of tiles and reservate memory
    uint16_t total_tiles = mapHeader.map_width * mapHeader.map_height;
    MY_TRACE_FLAG("\tTotal tiles on map: %i\n", total_tiles);

    mapIds  = (uint8_t *)malloc(total_tiles * sizeof(uint8_t));
    map     = (tTile *)malloc(total_tiles * sizeof(tTile));
    
    if (mapIds == NULL || map == NULL) {
        fclose(file);
        abort_on_error("Error: No se pudo asignar memoria para %u tiles.\n", total_tiles);
    }

    //read the full tile array id
    size_t read_count = fread(mapIds, sizeof(uint8_t), total_tiles, file);
    if (read_count != total_tiles) {
        abort_on_error("Error: Se esperaba leer %u tiles, pero se leyeron %zu.\n", total_tiles, read_count);
    }
    
    //allocate memory for temporal array of tile with property
    tilesWithProperty = (tTile *)malloc(mapHeader.numTilesWithProperty * sizeof(tTile)); 
    if (tilesWithProperty == NULL) {
        fclose(file);
        abort_on_error("Error: No se pudo asignar memoria para %u tiles con property.\n", mapHeader.numTilesWithProperty);
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
    
    //read map objects
    uint16_t numMapObjects;
    fread(&numMapObjects, sizeof(uint16_t), 1, file);
    MY_TRACE_FLAG("\tNum objects on map: %i\n", numMapObjects);
    
    //allocate memory for objects
    mapObjects = (tMapEntity *)malloc(numMapObjects * sizeof(tMapEntity));
    if (mapObjects == NULL) {
        fclose(file);
        abort_on_error("Error: No se pudo asignar memoria para %u objetos de mapa.\n", numMapObjects);
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
            abort_on_error("Error: No se pudo asignar memoria para %u objetos de mapa.\n", numMapEnemies);
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
    map = NULL;
    tiles = NULL;

    //initialize map data
    memset(&mapHeader, 0, sizeof(mapHeader));
    
    MY_TRACE_FLAG("Map unloaded\n");
}

void map_draw(BITMAP *buffer, tScroll *scroll, tVector screenSize)
{
    uint8_t tileNum;
    int16_t sx = scroll->pos.x % mapHeader.tile_width;      //tile pos x on scroll
    int16_t sy = scroll->pos.y % mapHeader.tile_height;     //tile pos y on scroll
    int16_t tx = scroll->pos.x / mapHeader.tile_width;      //tile num x on scroll
    int16_t ty = scroll->pos.y / mapHeader.tile_height;     //tile num y on scroll   

    //screen limit
    //TODO: precalculate on map_init not calculate each map_draw
    tVector screenLimit;
    screenLimit.x = mapHeader.map_width > (screenSize.x / mapHeader.tile_width) ? (screenSize.x / mapHeader.tile_width) + 1 : screenSize.x / mapHeader.tile_width;
    screenLimit.y =  mapHeader.map_height > (screenSize.y / mapHeader.tile_height) ? (screenSize.y / mapHeader.tile_height) + 1 : screenSize.y / mapHeader.tile_height;
        
    //TODO: replace clear all buffer with color only positions with no tiles?
    clear_to_color(buffer, mapHeader.backgroundColor);

    for (int y = 0; y < screenLimit.y; y++)
    {
        for (int x = 0; x < screenLimit.x; x++)        
        {
            tileNum = map[((y + ty) * mapHeader.map_width) + x + tx].tileId;
    
            /* blit tile*/
            if (tileNum != 0)            
                draw_sprite(buffer, tiles[tileNum - 1], (x * mapHeader.tile_width) - sx, (y * mapHeader.tile_height) - sy);
        }    
    }
}

//checks if position exists on tile maps
uint16_t map_tile_exists(tVector *checkPosition)
{
    return (checkPosition->x / mapHeader.tile_width) < mapHeader.map_width && (checkPosition->y / mapHeader.tile_height) < mapHeader.map_height && checkPosition->x >= 0 && checkPosition->y >= 0;
}

//gets map tile code
uint16_t map_get_tile_code(tVector *checkPosition)
{
    if (map[((checkPosition->y / mapHeader.tile_height) * mapHeader.map_width) + (checkPosition->x / mapHeader.tile_width)].tileId == 0)
        return E_TILE_PROP_NO_SOLID;
    else
    {
        return  map[((checkPosition->y / mapHeader.tile_height) * mapHeader.map_width) + (checkPosition->x / mapHeader.tile_width)].tileProperty;
    }
}

tVector map_get_dimensions()
{
    return (tVector){mapHeader.map_width * mapHeader.tile_width, mapHeader.map_height * mapHeader.tile_height};
}