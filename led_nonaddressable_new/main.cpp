#include "MRF24J40.h" 

#include "main.h"
#include <vector>

Serial pc(USBTX, USBRX);    // Serial communication for debugging purposes

MRF24J40 mrf(p11,p12,p13,p14,p21); // pins for zig bee
TLC59711 tlc(1, p10, p8);

// Used for sending and receiving
char rxBuffer[128];
int rxLen;

//Parses the received string and creates a pattern based on the parameters
void parser(char s[])
{
    char *p;
    int i = 0;
    int red = -1, green = -1, blue = -1;
    pattern_enum_t type = P_FLASH;
    int timestamp = -1;
    unsigned int delay = 0, period = 0;


    //Processing the received character array
    p = strtok (s, ",");
    while(p != NULL)
    {
        switch(i){
        case 0: {
            sscanf(p, "%d", &type);
            pc.printf("%d\r\n", type);
            if(strcmp(p, "flash") == 0)
            {
                type = P_FLASH;
                delay = 25;
            }
            else if(strcmp(p, "flash_slow") == 0)
            {
                type = P_FLASH;
                delay = 100;
            }
            else if(strcmp(p, "wave") == 0)
            {
                type = P_WAVE;
                delay = 25;
            }
            else if(strcmp(p, "wave_slow") == 0)
            {
                type = P_WAVE;
                delay = 100;
            }
            else if(strcmp(p, "fade") == 0)
            {
                type = P_FADE;
                delay = 25;
            }
            else if(strcmp(p, "on") == 0)
            {
                type = P_ON;
                delay = 25;
            }
            else{
                pc.printf("Pattern does not exist in the dictionary\r\n");
                return;
            }
            break;}
        case 1:{
            sscanf(p, "%d", &red);
            pc.printf("%d\r\n", red);
            break;}
        case 2:{
            sscanf(p, "%d", &green);
            pc.printf("%d\r\n", green);
            break;}
        case 3:{
            sscanf(p, "%d", &blue);
            pc.printf("%d\r\n", blue);
            break;}
        case 4:{
            break;}
        case 5:{
            break;}
        case 6:{
            sscanf(p, "%d", &timestamp);
            pc.printf("%d\r\n", timestamp);
            break;}
        case 7:{
            sscanf(p, "%d", &period);
            pc.printf("%d\r\n", period);
            break;}
        default:{
            pc.printf("Invalid string received\r\n");
            return;}
        }
        i++;
        p = strtok(NULL, ",");
        }//End of while

    //Converting int to uint8_t for r, g, b 

    uint32_t redUint, greenUint, blueUint;

    redUint = red;
    greenUint = green;
    blueUint = blue;

    create_pattern(type, redUint, greenUint, blueUint, delay, period);        
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

/*Chooses RGB value based on string*/
void getColor(uint32_t *R,uint32_t *G,uint32_t *B,std::string str)
{
    char V = str[0];
    switch(V) {
        case 'R':
            *R = 0;
            *G = 65535;
            *B = 65535;
            break;
        case 'G':
            *R = 65535;
            *G = 0;
            *B = 65535;
            break;
        case 'B':
            *R = 65535;
            *G = 65535;
            *B = 0;
            break;
        case 'Y':
            *R = 0;
            *G = 0;
            *B = 65535;
            break;
        case 'C':
            *R = 65535;
            *G = 0;
            *B = 0;
            break;
        case 'M':
            *R = 0;
            *G = 65535;
            *B = 0;
            break;
        case 'W':
            *R = 0;
            *G = 0;
            *B = 0;
            break;

        default:
            *R = 65535;
            *G = 65535;
            *B = 65535;
            break;
    }
}


/****************************************
* main
*
* sets up the communication between the  main mbed
* continuously checks for a command from the main mbed
* if new command received, then executes the appropriate function
*
****************************************/
int main()
{
    pc.baud(9600);
    clear_strip();
    
    //tlc.setLED(0, 0, 65535, 65535);
//    tlc.setLED(1, 0, 65535, 65535);
//    tlc.write();
//    wait(3);
//    clear_strip();
    create_pattern(P_FADE, 65535, 0, 65535, 25, 2000);
    
    while(1)
    {
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

void clear_strip()
{
    tlc.setLED(0, 65535, 65535, 65535);
    tlc.setLED(1, 65535, 65535, 65535);
    tlc.setLED(2, 65535, 65535, 65535);
    tlc.setLED(3, 65535, 65535, 65535);
    tlc.write();
}
