#include "mbed.h" 
#include "main.h" 

Timer gTimer;

void create_pattern(pattern_enum_t type, direction_enum_t direction, uint8_t r, uint8_t g, uint8_t b, uint32_t delay, uint32_t duration)
{
    clear_strip();
    
    switch(type)
    {
    case P_FLASH:
        nonaddr_ff(r, g, b, delay, duration);
        break;
    
    case P_WAVE:
        nonaddr_wave(direction, r, g, b, delay, duration);
        break;
        
    case P_WAVE_B:
        nonaddr_bwave(direction, r, g, b, delay, duration);
        break;
        
    default:
        break;
    }
}

/****************************************
* flash
*
* The led strip flashes the light with the mentioned time-period and duration
*
* @param r      red color value
* @param g      green color  
* @param b      blue color   
* @param delay  delay between every flash in milliseconds duty cycle is 50%
* @param duration How long should the flash effect be active in milliseconds
****************************************/
void nonaddr_ff(uint32_t r, uint32_t g, uint32_t b, uint32_t delay, uint32_t duration)
{   
    gTimer.start(); // begin timer

    /*While the time is less than the duration specified keep flashing*/
    while(gTimer.read_ms() < duration)
    {
        tlc.setLED(0, r, g, b);
        tlc.setLED(1, r, g, b);
        tlc.write();
        wait_ms(delay);

    /*Shutting down the leds*/
    tlc.setLED(0, 65535, 65535 , 65535);
    tlc.setLED(1, 65535, 65535 , 65535);
    tlc.write();
    wait_ms(delay);
}
gTimer.stop();
gTimer.reset();   
}

void nonaddr_wave(direction_enum_t direction, uint32_t r, uint32_t g, uint32_t b, uint32_t delay, uint32_t duration)
{
    tlc.setLED(0,r, g, b);
    tlc.setLED(1,65535, 65535, 65535);
    tlc.write();
    wait_ms(delay);
    tlc.setLED(0,65535, 65535, 65535);
    tlc.setLED(1,r, g, b);
    tlc.write();
    wait_ms(delay);
    tlc.setLED(0,65535, 65535, 65535);
    tlc.setLED(1,65535, 65535, 65535);
    tlc.write();
    wait_ms(delay);  
}

void nonaddr_bwave(direction_enum_t direction, uint32_t r, uint32_t g, uint32_t b, uint32_t delay, uint32_t duration)
{
    tlc.setLED(1,r, g, b);
    tlc.setLED(0,65535, 65535, 65535);
    tlc.write();
    wait_ms(delay);
    tlc.setLED(1,65535, 65535, 65535);
    tlc.setLED(0,r, g, b);
    tlc.write();
    wait_ms(delay);
    tlc.setLED(1,65535, 65535, 65535);
    tlc.setLED(0,65535, 65535, 65535);
    tlc.write();
    wait_ms(delay);  
}