/********************************************************************
* Map System
*
* 11/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "map.h"

// Definimos una estructura para el encabezado del mapa
struct mapHeader{
    uint16_t tile_width;
    uint16_t tile_height;
    uint16_t map_width;
    uint16_t map_height;
} mapHeader;

typedef struct {
    uint8_t class;
    uint8_t type;
    uint16_t x;
    uint16_t y;
    uint8_t dir;
} tMapObject;

uint8_t *map;
tMapObject *mapObjects;
uint16_t mapWidth;
BITMAP *tiles[NUM_TILES];

void map_load()
{
    //load tiles
    /*tiles[0] = load_bmp("res/tiles/001.bmp", desktop_palette);
    tiles[1] = load_bmp("res/tiles/002.bmp", NULL);
    tiles[2] = load_bmp("res/tiles/003.bmp", NULL);*/
    
    //load map tileSheet
    BITMAP *mapTileSheet;
    mapTileSheet = load_bmp("res/tiles/tsheet.bmp", desktop_palette);
    for (uint8_t i = 0; i < NUM_TILES; i++)
    {
        tiles[i] = create_sub_bitmap(mapTileSheet, (i % TILES_ROW) * TILE_H, (int)(i / TILES_ROW) * TILE_W, TILE_W, TILE_H);
    }

    //load map file
    FILE *file = fopen("res/maps/level00.bin", "rb");
    if (!file) {
        abort_on_error("Error al abrir el archivo");
    }

    //read map file header
    //MapHeader header;
    if (fread(&mapHeader, sizeof(mapHeader), 1, file) != 1) {
        fclose(file);
        abort_on_error("Error al leer el encabezado.\n");
    }

    //test
    TRACE("Tile dimensions: %u x %u px\n", mapHeader.tile_width, mapHeader.tile_height);
    TRACE("Map dimensions: %u x %u tiles\n", mapHeader.map_width, mapHeader.map_height);

    mapWidth = mapHeader.map_width;

    //Calculate number of tiles and reservate memory
    uint16_t total_tiles = mapHeader.map_width * mapHeader.map_height;
    map = (uint8_t *)malloc(total_tiles * sizeof(uint8_t));

    if (map == NULL) {
        fclose(file);
        abort_on_error("Error: No se pudo asignar memoria para %u tiles.\n", total_tiles);
    }

    //read the full tile array 
    size_t read_count = fread(map, sizeof(uint8_t), total_tiles, file);
    if (read_count != total_tiles) {
        abort_on_error("Error: Se esperaba leer %u tiles, pero se leyeron %zu.\n", total_tiles, read_count);
    }
    

    //read map objects
    uint16_t numMapObjects;
    fread(&numMapObjects, sizeof(uint16_t), 1, file);
    TRACE("Num objects: %i\n", numMapObjects);
    
    //allocate memory for objects
    mapObjects = (tMapObject *)malloc(numMapObjects * sizeof(tMapObject));
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

        TRACE("Class: %u Type: %u X: %i Y: %i Dir: %u\n", mapObjects[i].class, mapObjects[i].type, mapObjects[i].x, mapObjects[i].y, mapObjects[i].dir);

        //create entity
        entity_create(mapObjects[i].class, mapObjects[i].type, (tVector){mapObjects[i].x, mapObjects[i].y}, mapObjects[i].dir);
    }

    //TODO: clean
    //free(tile_array);
    fclose(file);    
}

void map_draw(BITMAP *buffer, tScroll *scroll, tVector screenSize)
{
    uint8_t tileNum;
    //int16_t lx = (GAME_W / TILE_W) + 1; //limit scroll x
    //int16_t ly = (GAME_H / TILE_H);     //limit scroll y
    int16_t sx = scroll->pos.x % TILE_W; //tile pos x on scroll
    int16_t sy = scroll->pos.y % TILE_H; //tile pos y on scroll
    int16_t tx = scroll->pos.x / TILE_W; //tile num x on scroll
    int16_t ty = scroll->pos.y / TILE_H; //tile num y on scroll   
    tVector screenLimit = {(screenSize.x / TILE_W) + 1, (screenSize.y / TILE_H)};
    
    clear_to_color(buffer, 1);

    for (int y = 0; y < screenLimit.y; y++)
    {
        for (int x = 0; x < screenLimit.x; x++)        
        {
            //tileNum = map[y + ty][x + tx];
            tileNum = map[((y + ty) * mapWidth) + x + tx];

            /* blit tile*/
            if (tileNum != 0)            
                draw_sprite(buffer, tiles[tileNum - 1], (x * 16) - sx, (y * 16) - sy);
        }    
    }
}

//checks if position exists on tile maps
uint16_t map_tile_exists(tVector *checkPosition)
{
    //return (checkPosition->x / TILE_W) < level.numTilesX && (checkPosition->y / TILE_H) < level.numTilesY && checkPosition->x >= 0 && checkPosition->y >= 0;     
    return (checkPosition->x / TILE_W) < mapHeader.map_width && (checkPosition->y / TILE_H) < mapHeader.map_height && checkPosition->x >= 0 && checkPosition->y >= 0;
}

//gets map tile code
uint16_t map_get_tile_code(tVector *checkPosition)
{
    //return map[checkPosition->y / TILE_H][checkPosition->x / TILE_W];
    return  map[((checkPosition->y / TILE_H) * mapWidth) + (checkPosition->x / TILE_W)];
}

tVector map_get_dimensions()
{
    return (tVector){mapHeader.map_width * TILE_W, mapHeader.map_height * TILE_H};
}

//TODO: get pixel color of position's map
/*uint8_t map_get_pixel(tVector *checkPosition)
{
    getpixel(hud.hsImage, mouse_x, mouse_y - HUD_Y);    
}*/