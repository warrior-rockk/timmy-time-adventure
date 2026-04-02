#define cControlLoggerMaxFrames  1000

enum E_KEY_STATE : u8
{
    E_K_ST_MEM_PRESS,    
    E_K_ST_PRESS,        
    E_K_ST_DOWN,         
    E_K_ST_UP, 
    E_KEY_ST_NUM,          
};

enum E_GAME_KEYS : u8 
{
    E_G_KEY_UP,
    E_G_KEY_DOWN,
    E_G_KEY_LEFT,
    E_G_KEY_RIGHT,
    E_G_KEY_JUMP,
    E_G_KEY_ACTION,
    E_G_KEY_PAUSE,
    E_G_KEY_EXIT,
    E_G_KEY_D,
    E_G_KEY_S,
    E_G_KEY_I,
    E_GAME_KEYS_NUM,
};

struct inputLogEvent
{
    u16 frameTime;    //Frame time stamp
    E_GAME_KEYS controlCode;   //Input code
    E_KEY_STATE controlEvent;  //Input event 
};

inputLogEvent data[cControlLoggerMaxFrames] @ 0x00;