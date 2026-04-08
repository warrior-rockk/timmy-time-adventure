#pragma little endian

enum E_ENTITY_CLASS : u8
{
    E_ENT_CLASS_PLAYER,
    E_ENT_CLASS_OBJECT,
    E_ENT_CLASS_ENEMY,
    E_ENT_CLASS_PLATFORM,
};

struct mapHeader {
    u16 tile_width;
    u16 tile_height;
    u16 tiles_X;
    u16 tiles_Y;
    u16 backgroundColor;
    u16 tileCount;
    u16 tileColumns;  
    u16 numTilesData;
    u16 numTilesAnimation;
};

struct entity {
    E_ENTITY_CLASS class;
    u8 type;
    u16 x;
    u16 y;
    u8 dir;
    s16 spare;
};

struct tileData {
    u8 tileId;
    u16 tileData;
};

struct tileFrame {
    u8 tileId;
    u16 duration;
};

struct tileAnimation {
    u8 tileId;
    u8 numFrames;
    tileFrame frames[numFrames];
};

struct level {
    mapHeader header;
    u16 numTiles = header.tiles_X * header.tiles_Y;
    u8 tiles[numTiles];
    tileData tilesProperties[header.numTilesData];
    tileAnimation tilesAnimation[header.numTilesAnimation];
    u16 numObjects;
    entity objects[numObjects];
    u16 numEnemies;
    entity enemies[numEnemies];
};

level level @ 0x00;