#pragma little endian

struct mapHeader {
	u16 tile_width;
	u16 tile_height;
	u16 tiles_X;
	u16 tiles_Y;
	u16 backgroundColor;
};

struct entity {
    u8 class;
    u8 type;
    u16 x;
    u16 y;
    u8 dir;
};

struct level {
    mapHeader header;
    u16 numTiles = header.tiles_X * header.tiles_Y;
    u8 tiles[numTiles];
    u16 numObjects;
    entity objects[numObjects];
    u16 numEnemies;
    entity enemies[numEnemies];
};

level level @ 0x00;