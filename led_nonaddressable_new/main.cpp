#include "MRF24J40.h" 

#include "main.h"
#include <vector>

Serial pc(USBTX, USBRX);    // Serial communication for debugging purposes

MRF24J40 mrf(p11,p12,p13,p14,p21); // pins for zig bee
TLC59711 tlc(1, p10, p8);

// Used for sending and receiving
char rxBuffer[128];
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
    uint32_t R,G,B;
    clear_strip();
    
    while(1)
    {
        rxLen = rf_receive(rxBuffer, 128);
        if(rxLen > 0 && rxBuffer[0] == 'x') {
            char* rxBuffer1 = &rxBuffer[1];
            pc.printf("Received string %s \r\n",rxBuffer);
            std::vector<std::string> msg;
            /*split the buffer based on the delimiter ',' and save into extract*/
            msg = split(std::string(rxBuffer1),'/',msg);
            int timestamp = std::atoi(msg[0].c_str());
            if(timestamp % 75 == 50)  
            {
                getColor(&R,&G,&B,"G");
                nonaddr_ff(R, G, B, 50, 1000);
            }
            else if(timestamp % 75 == 20)
            {
                getColor(&R,&G,&B,"G");
                nonaddr_ff(R, G, B, 200, 2000);
            }
            else if(timestamp %75 == 25)
            {
//                getColor(&R,&G,&B,"B");
                nonaddr_wave(DIR_NONE, 400, 45000, 65535, 500, 1000);
            }
            else if(timestamp %75 == 65)
            {
                nonaddr_bwave(DIR_NONE, 400, 5000, 270, 500, 1000);
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
