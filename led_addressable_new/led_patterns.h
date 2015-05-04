#include "mbed.h"
#include "rtos.h"
#include "LPD8806.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
//#include <time.h>       /* time */
#include <vector>


//Change if you want part of the strip to be enabled
#define SELECTEDROWS 66
#define SELECTEDCOLS 10

//Total number of LEDs
#define NUMLEDS SELECTEDROWS*SELECTEDCOLS

//Total number of rows and columns in the grid
#define MAXROWS 66
#define MAXCOLS 10

//Size of each pixel
#define PIXELSIZE_ROWS 6
#define PIXELSIZE_COLS 2

//Total number of pixel rows and columns
#define PIXELROWS MAXROWS/PIXELSIZE_ROWS
#define PIXELCOLS MAXCOLS/PIXELSIZE_COLS

//ID for addressable strip commands
#define ID 'x'

//Represents direction of the pattern to be displayed
typedef enum
{
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_OTHER    
}direction_enum_t;

//Represents type of pattern to be displayed
typedef enum
{
    P_FLASH,
    P_WAVE,
    P_WAVE_FB,
    P_RISEFALL,
    P_WAVE_FLOW
}pattern_enum_t;

//Represents a single pixel
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
void create_wave_flow(direction_enum_t, uint8_t, uint8_t, uint8_t, uint32_t, uint32_t);

void select_pattern(int);

void clear_strip();

void shift_array(direction_enum_t);

extern LPD8806 strip;
extern int grid_matrix[MAXROWS][MAXCOLS];
extern pixel_struct_t pixel_grid[PIXELROWS][PIXELCOLS];

void clear_all_pixels();
void select_all_pixels();

void set_selected_pixels(uint8_t, uint8_t, uint8_t);

void show_pixel(pixel_struct_t*, uint8_t, uint8_t, uint8_t, uint32_t);
pixel_struct_t* get_pixel(int, int);



