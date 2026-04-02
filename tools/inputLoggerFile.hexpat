#define cControlLoggerMaxFrames  1000

enum eventType : u8 {
  K_PRESS,
  K_DOWN,
  K_UP,
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
    eventType controlEvent;  //Input event 
};

inputLogEvent data[cControlLoggerMaxFrames] @ 0x00;