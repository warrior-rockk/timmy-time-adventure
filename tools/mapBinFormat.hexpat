#pragma little endian

enum E_ENTITY_CLASS : u8
{
    E_ENT_CLASS_TRIGGER,
    E_ENT_CLASS_PLAYER,
    E_ENT_CLASS_OBJECT,
    E_ENT_CLASS_ENEMY,
    E_ENT_CLASS_PLATFORM,
};

enum E_OBJECT_TYPE : u8
{
    E_ROCK_OBJECT_TYPE,
    E_ROCK_2_OBJECT_TYPE,
    E_EGG_OBJECT_TYPE,
    E_END_LEVEL_OBJECT_TYPE,
    E_CHECKPOINT_OBJECT_TYPE,
    E_ITEM_OBJECT_TYPE,
    E_STOP_SCROLL_OBJECT_TYPE,
    E_ROCK_3_OBJECT_TYPE,
    E_WAGON_OBJECT_TYPE,
    E_DYNAMITE_OBJECT_TYPE,
    E_BACKCOLOR_OBJECT_TYPE,
    E_BRIDGE_OBJECT_TYPE,
    E_ROCK_EXPLOSION_OBJECT_TYPE,
    E_GAME_OVER_OBJECT_TYPE,
    E_ROCK_FALL_OBJECT_TYPE,
    E_SPIKE_FALL_OBJECT_TYPE,
    E_BOX_OBJECT_TYPE,
    E_HIDDEN_OBJECT_TYPE,
    E_QUICKSAND_OBJECT_TYPE,

    E_OBJECTS_TYPE_NUM,
};

enum E_ENEMY_TYPE : u8
{
    E_PTERO_ENEMY_TYPE,
    E_RAPTOR_ENEMY_TYPE,
    E_SPIDER_ENEMY_TYPE,
    E_PIRANHA_ENEMY_TYPE,
    E_COWBOY_ENEMY_TYPE,
    E_EAGLE_ENEMY_TYPE,
    E_BULLET_ENEMY_TYPE,
    E_TUMBLE_ENEMY_TYPE,
    E_TRICE_ENEMY_TYPE,
    E_SCORPION_ENEMY_TYPE,
    E_INDIAN_AXE_ENEMY_TYPE,
    E_AXE_ENEMY_TYPE,
    E_BAT_ENEMY_TYPE,
    E_INDIAN_BOW_ENEMY_TYPE,
    E_ARROW_ENEMY_TYPE,
    E_EGYPTIAN_ENEMY_TYPE,
    
    E_ENEMIES_TYPE_NUM,
};

struct mapHeader {
    u16 tile_width;
    u16 tile_height;
    u16 tiles_X;
    u16 tiles_Y;
    u16 backgroundColor;
    u16 levelTime;
    u16 tileCount;
    u16 tileColumns;  
    u16 numTilesData;
    u16 numTilesAnimation;
};

struct entity {
    E_ENTITY_CLASS class;
    match (class)
    {
        (E_ENTITY_CLASS::E_ENT_CLASS_PLAYER): u8 type;
        (E_ENTITY_CLASS::E_ENT_CLASS_TRIGGER): E_OBJECT_TYPE type;
        (E_ENTITY_CLASS::E_ENT_CLASS_OBJECT): E_OBJECT_TYPE type;
        (E_ENTITY_CLASS::E_ENT_CLASS_ENEMY): E_ENEMY_TYPE type;        
    }    
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