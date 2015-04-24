#include "mbed.h"
#include "rtos.h"
#include "LPD8806.h"
#include "TLC59711.h" 
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
//#include <time.h>       /* time */
#include <vector>

//#define ROWS 1          // Rows of leds; each seperate strip is considered as a row
//#define COLS 264         // number of leds per row

#define SELECTEDROWS 66
#define SELECTEDCOLS 8

#define NUMLEDS SELECTEDROWS*SELECTEDCOLS

#define MAXROWS 66
#define MAXCOLS 8

#define PIXELSIZE_ROWS 6
#define PIXELSIZE_COLS 2

#define PIXELROWS MAXROWS/PIXELSIZE_ROWS
#define PIXELCOLS MAXCOLS/PIXELSIZE_COLS

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
    P_WAVE_FB,
    P_RISEFALL,
    P_CUSTOM
}pattern_enum_t;

typedef struct
{
    int leds[PIXELSIZE_ROWS][PIXELSIZE_COLS];
    bool selected;
}pixel_struct_t;

void init_grid();
void create_pattern(pattern_enum_t, direction_enum_t, uint8_t, uint8_t, uint8_t, uint32_t, uint32_t);
void create_wave(direction_enum_t, uint8_t, uint8_t, uint8_t, uint32_t, uint32_t);
void create_flash(direction_enum_t, uint8_t, uint8_t, uint8_t, uint32_t, uint32_t);
void create_wave_fb(direction_enum_t, uint8_t, uint8_t, uint8_t, uint32_t, uint32_t);
void create_risefall(direction_enum_t, uint8_t, uint8_t, uint8_t, uint32_t, uint32_t);

void create_custom(direction_enum_t, uint8_t, uint8_t, uint8_t, uint32_t, uint32_t);

void clear_strip();

// Total leds used for light effects; ROWS * COLS
extern LPD8806 strip;
extern TLC59711 tlc;
//extern int grid[ROWS][COLS]; 
extern int grid_matrix[MAXROWS][MAXCOLS];
extern pixel_struct_t pixel_grid[PIXELROWS][PIXELCOLS];

void clear_selected_pixels();
void show_pixel(pixel_struct_t*, uint8_t, uint8_t, uint8_t, uint32_t);
pixel_struct_t* get_pixel(int, int);



