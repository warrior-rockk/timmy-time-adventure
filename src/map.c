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
typedef struct {
    uint16_t tile_width;
    uint16_t tile_height;
    uint16_t map_width;
    uint16_t map_height;
} MapHeader;

uint8_t *map;
uint16_t mapWidth;
BITMAP *tiles[NUM_TILES];

/*
static uint8_t map[MAP_TILE_H][MAP_TILE_W] =
{
    {22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,7,7,7,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,0,7,7,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22},
    {22,0,0,0,0,0,0,22,22,22,22,22,22,0,0,0,0,0,0,0,0,0,0,0,7,7,0,0,0,0,0,22},
    {22,0,0,0,0,0,22,22,22,22,22,22,22,22,0,0,0,0,0,0,0,0,0,0,22,22,0,0,0,0,0,22},
    {22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22}
};*/

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
    MapHeader header;
    if (fread(&header, sizeof(MapHeader), 1, file) != 1) {
        fclose(file);
        abort_on_error("Error al leer el encabezado.\n");
    }

    //test
    TRACE("Tile dimensions: %u x %u px\n", header.tile_width, header.tile_height);
    TRACE("Map dimensions: %u x %u tiles\n", header.map_width, header.map_height);

    mapWidth = header.map_width;

    //Calculate number of tiles and reservate memory
    uint16_t total_tiles = header.map_width * header.map_height;
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
    return (checkPosition->x / TILE_W) < MAP_TILE_W && (checkPosition->y / TILE_H) < MAP_TILE_H && checkPosition->x >= 0 && checkPosition->y >= 0;
}

//gets map tile code
uint16_t map_get_tile_code(tVector *checkPosition)
{
    //return map[checkPosition->y / TILE_H][checkPosition->x / TILE_W];
    return  map[((checkPosition->y / TILE_H) * mapWidth) + (checkPosition->x / TILE_W)];
}

//TODO: get pixel color of position's map
/*uint8_t map_get_pixel(tVector *checkPosition)
{
    getpixel(hud.hsImage, mouse_x, mouse_y - HUD_Y);    
}*/