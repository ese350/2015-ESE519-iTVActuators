#include "MRF24J40.h" 
#include "led_patterns.h"
#include "main.h"

#ifndef TEST

//extern "C" void mbed_reset();

//using namespace std;

Serial pc(USBTX, USBRX);    // Serial communication for debugging purposes

LPD8806 strip = LPD8806(NUMLEDS);

MRF24J40 mrf(p11,p12,p13,p14,p21); // pins for zig bee
int grid_matrix[MAXROWS][MAXCOLS] = {0};
pixel_struct_t pixel_grid[PIXELROWS][PIXELCOLS] = {0};

// Used for sending and receiving
char rxBuffer[256];
int rxLen;

/*Extracts the individual strings and saves them into a vector*/
std::vector<std::string> split(const std::string &str, char delimiter, std::vector<std::string> extract)
{
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        extract.push_back(item);
    }
    return extract;
}

//Parses the received string and creates a pattern based on the parameters
void parser(char s[])
{
    char *p, *q;
    char st[200];
    int i = 0;
    int patternShape = -1;
    int red = -1, green = -1, blue = -1;
    unsigned int delay = 0, period = 0;
    pattern_enum_t type = P_FLASH;
    direction_enum_t patternDirection = DIR_NONE;
    int timestamp = -1;


    //Processing the received character array
    p = strtok (s, ",");
    while(p != NULL)
    {
        switch(i){
        case 0: {
            pc.printf("%s\r\n", p);
            if(strcmp(p, "flash") == 0)
            {
                type = P_FLASH;
                delay = 25;
                select_all_pixels();
            }
            else if(strcmp(p, "flash_slow") == 0)
            {
                type = P_FLASH;
                delay = 100;
                select_all_pixels();
            }
            else if(strcmp(p, "wave") == 0)
            {
                type = P_WAVE;
                delay = 25;
                select_all_pixels();
            }
            else if(strcmp(p, "wave_slow") == 0)
            {
                type = P_WAVE;
                delay = 100;
                select_all_pixels();
            }
            else if(strcmp(p, "wave_fb") == 0)
            {
                type = P_WAVE_FB;
                delay = 25;
                select_all_pixels();
            }
            else if(strcmp(p, "wave_rf") == 0)
            {
                type = P_RISEFALL;
                delay = 25;
                select_all_pixels();
            }
            else if(strcmp(p, "flow") == 0)
            {
                type = P_WAVE_FLOW;
                delay = 25;
                select_all_pixels();
            }
            else{
                pc.printf("Pattern does not exist in the dictionary\r\n");
                return;
            }
            break;}
        case 1:{
            sscanf(p, "%d", &red);
            break;}
        case 2:{
            sscanf(p, "%d", &green);
            break;}
        case 3:{
            sscanf(p, "%d", &blue);
            break;}
        case 4:{
            if (strcmp(p, "up") == 0)
                patternDirection = DIR_UP;
            else if(strcmp(p, "down") == 0)
                patternDirection = DIR_DOWN;
            else if(strcmp(p, "left") == 0)
                patternDirection = DIR_LEFT;
            else if(strcmp(p, "right") == 0)
                patternDirection = DIR_RIGHT;
            else if(strcmp(p, "none") == 0)
                patternDirection = DIR_NONE;
            else{
                pc.printf("Invalid direction entered \r\n");
                return;
            }
            break;}
        case 5:{
            sscanf(p, "%d", &patternShape);
            break;}
        case 6:{
            sscanf(p, "%d", &timestamp);
            break;}
        case 7:{
            sscanf(p, "%u", &period);
            break;}
        case 8:{
            sscanf(p, "[%199[^]]]", st);
            
            clear_all_pixels();
            q = strtok(st, ";");
            while(q != NULL)
            {
                int val = std::atoi(q);
                pc.printf("%d\r\n", val);
                pixel_grid[val/PIXELCOLS][val%PIXELCOLS].selected = 1;
                q = strtok(NULL, ";");
            }
            break;}
        default:{
            pc.printf("Invalid string received\r\n");
            return;}
        }
        i++;
        p = strtok(NULL, ",");
        }//End of while

    

    //Converting int to uint8_t for r, g, b 

    uint8_t redUint, greenUint, blueUint;

    redUint = red;
    greenUint = green;
    blueUint = blue;

//    select_pattern(patternShape);

    //Create Pattern based on the values recevied from the server
    create_pattern(type, patternDirection, redUint, greenUint, blueUint, delay, period);
    }


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

int rf_receive(char *data, uint8_t maxLength)
{
    uint8_t len = mrf.Receive((uint8_t *)data, maxLength);
    uint8_t header[8]= {1, 8, 0, 0xA1, 0xB2, 0xC3, 0xD4, 0x00};
    if(len > 10) {
        //Remove the header and footer of the message
        for(uint8_t i = 0; i < len-2; i++) {
            if(i<8) {
                //Make sure our header is valid first
                if(data[i] != header[i])
                    return 0;
            } else {
                data[i-8] = data[i];
            }
        }
        data[len - 10] = '\0';
//        pc.printf("Received: %s length:%d\r\n", data, ((int)len)-10);
    }
    return ((int)len)-10;
}

/****************************************
* main
*
* sets up the communication between the  main mbed
* inits the leds to check if the leds are working properly
* continuously checks for a command from the main mbed
* if new command received, then executes the appropriate function
*
****************************************/
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
    //clear_strip();
//    show_pixel(&pixel_grid[0][0], R, G, B, 500);
//    wait_ms(1000);

    pixel_grid[0][0].selected = 1;
    pixel_grid[1][1].selected = 1;
    pixel_grid[2][2].selected = 1;
    pixel_grid[2][0].selected = 1;
    pixel_grid[0][2].selected = 1;

    //pixel_grid[0][0].selected = 1;
//    pixel_grid[1][1].selected = 1;
//    pixel_grid[2][2].selected = 1;
//    pixel_grid[3][3].selected = 1;
//    pixel_grid[4][2].selected = 1;
//    pixel_grid[5][1].selected = 1;
//    pixel_grid[6][0].selected = 1;

//    select_all_pixels();
    
    create_pattern(P_WAVE_FLOW, DIR_RIGHT, R, G, B, 50, 5000);
    
//    pixel_grid[10][0].selected = 1;
//    pixel_grid[10][1].selected = 1;
//    pixel_grid[10][2].selected = 1;
//    pixel_grid[10][3].selected = 1;
//    pixel_grid[9][1].selected = 1;
//    pixel_grid[9][2].selected = 1;
//    pixel_grid[9][3].selected = 1;
//    pixel_grid[8][2].selected = 1;
//    pixel_grid[8][3].selected = 1;
//    pixel_grid[7][3].selected = 1;
    
//
//    create_pattern(P_CUSTOM, DIR_UP, 150, 80, 200, 50, 1000);
//    
    //pixel_grid[0][0].selected = 1;
//    pixel_grid[1][1].selected = 1;
//    pixel_grid[2][2].selected = 1;
//    pixel_grid[2][0].selected = 1;
//    pixel_grid[0][2].selected = 1;
//    
//    getColor(&R,&G,&B,"G");
//    
//    create_pattern(P_CUSTOM, DIR_UP, R, G, B, 25, 1000);
//    
//    pixel_grid[2][0].selected = 1;
//    pixel_grid[2][1].selected = 1;
//    pixel_grid[2][2].selected = 1;
//    pixel_grid[2][3].selected = 1;
//    
//    getColor(&R,&G,&B,"B");
//    
//    create_pattern(P_CUSTOM, DIR_UP, R, G, B, 50, 1000);
    
//    wait(0.1);
    
    clear_strip();
        
    while(1)
    {
        //wait for messages
        rxLen = rf_receive(rxBuffer, 128);
        if(rxLen > 0)
        {
            if(rxBuffer[0] == ID) 
            {
                char* rxBuffer1 = &rxBuffer[1];
                pc.printf("Received string %s \r\n",rxBuffer);
                parser(rxBuffer1);
            }
        }
    }
}

#endif