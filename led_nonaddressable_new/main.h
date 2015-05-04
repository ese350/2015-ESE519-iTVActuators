#include <string>
#include "TLC59711.h"

#define ID 'y'


//Describes various types of basic patterns
typedef enum
{
    P_FLASH,
    P_WAVE,
    P_WAVE_B,
    P_ON,
    P_FADE
}pattern_enum_t;

void create_pattern(pattern_enum_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

void nonaddr_ff(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void nonaddr_wave(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void nonaddr_bwave(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void nonaddr_fade(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void nonaddr_on(uint32_t, uint32_t, uint32_t, uint32_t);

void clear_strip();

extern TLC59711 tlc;
extern Serial pc;