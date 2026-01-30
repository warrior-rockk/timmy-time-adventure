#pragma little endian

struct mapHeader {
	u16 tile_width;
	u16 tile_height;
	u16 tiles_X;
	u16 tiles_Y;
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
    u16 numEntities;
    entity entities[numEntities];
};

level level @ 0x00;