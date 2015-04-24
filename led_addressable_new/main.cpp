#include "MRF24J40.h" 
#include "led_patterns.h"
#include "main.h"

extern "C" void mbed_reset();

//using namespace std;

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

Serial pc(USBTX, USBRX);    // Serial communication for debugging purposes

LPD8806 strip = LPD8806(NUMLEDS);

MRF24J40 mrf(p11,p12,p13,p14,p21); // pins for zig bee
int grid_matrix[MAXROWS][MAXCOLS] = {0};
pixel_struct_t pixel_grid[PIXELROWS][PIXELCOLS] = {0};

// Used for sending and receiving
char rxBuffer[128];
int rxLen;


void parser(char s[])
{
    char *p;
    int i = 0;
    int patternShape = -1;
    int red = -1, green = -1, blue = -1;
    pattern_enum_t pattern = P_FLASH;
    direction_enum_t patternDirection = DIR_NONE;
    int timestamp = -1;


    //Processing the received character array
    p = strtok (s, ",");
    while(p != NULL)
    {
        switch(i){
        case 0: {
            pc.printf(p);
            sscanf(p, "%d", &patternShape);
            break;}
        case 1:{
            pc.printf(p);
            sscanf(p, "%d", &red);
            break;}
        case 2:{
            pc.printf(p);
            sscanf(p, "%d", &green);
            break;}
        case 3:{
//            pc.printf(p);
            sscanf(p, "%d", &blue);
            break;}
        case 4:{
//            pc.printf(p);
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
//            pc.printf(p);
            if(strcmp(p, "FLASH") == 0)
                pattern = P_FLASH;
            else if(strcmp(p, "WAVE") == 0)
                pattern = P_WAVE;
            else if(strcmp(p, "WAVE_FB") == 0)
                pattern = P_WAVE_FB;
            else if(strcmp(p, "WAVE_RISEFALL") == 0)
                pattern = P_RISEFALL;
            else if(strcmp(p, "WAVE_CUSTOM") == 0)
                pattern = P_CUSTOM;
            else{
                pc.printf("Pattern does not exist in the dictionary\r\n");
                return;
            }

            break;}
        case 6:{
//            pc.printf(p);
            sscanf(p, "%d", &timestamp);
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


    //Changing delay and period of pattern based on the timestamp

    uint16_t delay = 0, period = 0;


    if(timestamp % 75 == 5)
    {
        delay = 50;
        period = 1000;
    }
    else if(timestamp % 75 == 10)
    {
        delay = 25;
        period = 2000;
    }
    else if(timestamp % 75 == 15)
    {
        delay = 200;
        period = 2000;
    }
    else if(timestamp % 75 == 20)
    {
        delay = 10;
        period = 2000;
    }
    else if(timestamp % 75 == 25)
    {
        delay = 25;
        period = 2000;
    }
    else if(timestamp % 75 == 30)
    {
        delay = 25;
        period = 2000;
    }

    else if(timestamp % 75 == 35)
    {
        delay = 25;
        period = 1000;
    }
    else if(timestamp % 75 == 40)
    {
        delay = 25;
        period = 1000;
    }
    else if(timestamp % 75 == 45)
    {
        delay = 25;
        period = 2000;
    }
    else
    {
        delay = 0;
        period = 0;
        pc.printf("Invalid Timestamp\r\n");
        return;
    }

    
    //TODO AS: Figuring out how to deal with deciding the pattern shape


    //Create Pattern based on the values recevied from the server
    create_pattern(pattern, patternDirection, redUint, greenUint, blueUint, delay, period);
        
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
    getColor(&R,&G,&B,"R");
    clear_strip();    
    //create_pattern(P_FLASH, DIR_NONE, R, G, B, 25, 500);
//    strip.setPixelColor(33, R, G, B);
//    strip.show();
    //strip.setPixelColor(0, R, G, B);
//    strip.show();
//    pc.printf("%d\r\n",strip.getPixelColor(33));
    wait_ms(1000);
//    show_pixel(&pixel_grid[0][0], R, G, B, 500);

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
        if(rxLen > 0 && rxBuffer[0] == 'x') {
            char* rxBuffer1 = &rxBuffer[1];
            pc.printf("Received string %s \r\n",rxBuffer);
            parser(rxBuffer1);
            //std::vector<std::string> msg;
//            /*split the buffer based on the delimiter ',' and save into extract*/
//            msg = split(std::string(rxBuffer1),'/',msg);
//            int timestamp = std::atoi(msg[0].c_str());
//            if(timestamp % 75 == 5)
//            {
//                create_pattern(P_FLASH, DIR_NONE, 255, 0, 0, 50, 1000);
//                mbed_reset();
//            }
//            else if(timestamp % 75 == 10)
//            {
//                create_pattern(P_WAVE, DIR_UP, 255, 0, 0, 25, 2000);
//                mbed_reset();
//            }
//            else if(timestamp % 75 == 15)
//            {
//                getColor(&R,&G,&B,"B");
//                create_pattern(P_FLASH, DIR_NONE, R, G, B, 200, 2000);
//                mbed_reset();
//            }
//            else if(timestamp % 75 == 20)
//            {
//                //getColor(&R,&G,&B,"G");
////                create_pattern(P_RISEFALL, DIR_NONE, R, G, B, 10, 2000);
////                mbed_reset();
//            }
//            else if(timestamp % 75 == 25)
//            {
//                //pixel_grid[0][0].selected = 1;
////                pixel_grid[1][1].selected = 1;
////                pixel_grid[2][2].selected = 1;
////                pixel_grid[2][0].selected = 1;
////                pixel_grid[0][2].selected = 1;
////                
////                getColor(&R,&G,&B,"G");
////                
////                create_pattern(P_CUSTOM, DIR_UP, R, G, B, 25, 2000);
////                mbed_reset();
//            }
//            else if(timestamp % 75 == 30)  
//            {
//                pixel_grid[0][0].selected = 1;
//                pixel_grid[1][0].selected = 1;
//                pixel_grid[2][0].selected = 1;
//                pixel_grid[3][0].selected = 1;
//                pixel_grid[4][0].selected = 1;
//                pixel_grid[0][3].selected = 1;
//                pixel_grid[1][3].selected = 1;
//                pixel_grid[2][3].selected = 1;
//                pixel_grid[3][3].selected = 1;
//                pixel_grid[4][3].selected = 1;
//                pixel_grid[1][1].selected = 1;
//                pixel_grid[1][2].selected = 1;
//                
//                create_pattern(P_CUSTOM, DIR_UP, 186, 105, 99, 25, 2000);
//                mbed_reset();
//                
//            }
//            else if(timestamp % 75 == 35)  
//            {
//                getColor(&R,&G,&B,"Y");
//                create_pattern(P_WAVE, DIR_LEFT, 255, 0, 0, 25, 1000);
//                mbed_reset();
//            }
//            else if(timestamp % 75 == 40)  
//            {
//                getColor(&R,&G,&B,"Y");
//                create_pattern(P_WAVE, DIR_RIGHT, 255, 0, 0, 25, 1000);
//                mbed_reset();
//            }
//            else if(timestamp % 75 == 45)  
//            {
//                pixel_grid[10][0].selected = 1;
//                pixel_grid[10][1].selected = 1;
//                pixel_grid[10][2].selected = 1;
//                pixel_grid[10][3].selected = 1;
//                pixel_grid[9][1].selected = 1;
//                pixel_grid[9][2].selected = 1;
//                pixel_grid[9][3].selected = 1;
//                pixel_grid[8][2].selected = 1;
//                pixel_grid[8][3].selected = 1;
//                pixel_grid[7][3].selected = 1;
//                
//                create_pattern(P_CUSTOM, DIR_UP, 150, 80, 200, 25, 2000);
//                mbed_reset();
//            }
        }
    }
}
