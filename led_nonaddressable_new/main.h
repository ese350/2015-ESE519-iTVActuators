#include <string>
#include <sstream>
#include "TLC59711.h"

#define SELECTEDROWS 66
#define SELECTEDCOLS 8

#define MAXROWS 66
#define MAXCOLS 8

#define PIXELSIZE_X 2
#define PIXELSIZE_Y 6

#define PIXELROWS MAXROWS/PIXELSIZE_Y
#define PIXELCOLS MAXCOLS/PIXELSIZE_X


typedef enum
{
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_OTHER    
}direction_enum_t;

typedef enum
{
    P_FLASH,
    P_WAVE,
    P_WAVE_B
}pattern_enum_t;


void nonaddr_ff(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void nonaddr_wave(direction_enum_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void nonaddr_bwave(direction_enum_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

void clear_strip();

extern TLC59711 tlc;
extern Serial pc;