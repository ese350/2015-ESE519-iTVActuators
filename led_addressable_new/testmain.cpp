#include "led_patterns.h"
#include "main.h"

#ifdef TEST

extern "C" void mbed_reset();

Serial pc(USBTX, USBRX);    // Serial communication for debugging purposes

LPD8806 strip = LPD8806(NUMLEDS);

int grid_matrix[MAXROWS][MAXCOLS] = {0};
pixel_struct_t pixel_grid[PIXELROWS][PIXELCOLS] = {0};

// Used for sending and receiving
char rxBuffer[128];
int rxLen;

/*Chooses RGB value based on string*/
void getColor(uint8_t *R,uint8_t *G,uint8_t *B,std::string str)
{
    char V = str[0];
    switch(V) {
        case 'R':
            *R = 255;
            *G = 0;
            *B = 0;
            break;
        case 'G':
            *R = 0;
            *G = 255;
            *B = 0;
            break;
        case 'B':
            *R = 0;
            *G = 0;
            *B = 255;
            break;
        case 'Y':
            *R = 255;
            *G = 255;
            *B = 0;
            break;
        case 'C':
            *R = 0;
            *G = 255;
            *B = 255;
            break;
        case 'M':
            *R = 255;
            *G = 0;
            *B = 255;
            break;
        case 'W':
            *R = 255;
            *G = 255;
            *B = 255;
            break;

        default:
            *R = 0;
            *G = 0;
            *B = 0;
            break;
    }
}

int main()
{
    uint8_t R,G,B;
    pc.baud(9600);
    strip.begin();
    init_grid();

    /*Shutting down the leds*/
    clear_strip();    
    getColor(&R,&G,&B,"G");
    wait_ms(10);

    pixel_grid[0][0].selected = 1;
    pixel_grid[1][1].selected = 1;
    pixel_grid[2][2].selected = 1;
    pixel_grid[2][0].selected = 1;
    pixel_grid[0][2].selected = 1;
    
    create_pattern(P_FLASH, DIR_NONE, R, G, B, 25, 2000);
    
    wait_ms(10);
    
    getColor(&R,&G,&B,"B");
    create_pattern(P_WAVE, DIR_UP, R, G, B, 25, 2000);
    
    wait_ms(10);
    
    getColor(&R,&G,&B,"Y");
    create_pattern(P_WAVE, DIR_RIGHT, R, G, B, 25, 2000);
    
    wait_ms(10);
    
    getColor(&R,&G,&B,"R");
    create_pattern(P_RISEFALL, DIR_UP, R, G, B, 25, 20000);
    
    wait_ms(10);
    getColor(&R,&G,&B,"B");
    create_pattern(P_FLASH, DIR_NONE, R, G, B, 200, 2000);
    
    wait_ms(10);
    getColor(&R,&G,&B,"G");
    pixel_grid[0][0].selected = 1;
    pixel_grid[1][1].selected = 1;
    pixel_grid[2][2].selected = 1;
    pixel_grid[2][0].selected = 1;
    pixel_grid[0][2].selected = 1;
    create_pattern(P_WAVE_FLOW, DIR_DOWN, R, G, B, 50, 2000);
       
    while(1)
    {
    }
}
#endif