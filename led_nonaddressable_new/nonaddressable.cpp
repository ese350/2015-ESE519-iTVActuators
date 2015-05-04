#include "mbed.h" 
#include "main.h" 

Timer gTimer;

Timeout timeout;

volatile bool timed_out = false;

void timeout_isr()
{
    clear_strip();
    timed_out = true;
}

void create_pattern(pattern_enum_t type, uint32_t r, uint32_t g, uint32_t b, uint32_t delay, uint32_t duration)
{
    clear_strip();
    
    switch(type)
    {
    case P_FLASH:
        nonaddr_ff(r, g, b, delay, duration);
        break;
    
    case P_WAVE:
        nonaddr_wave(r, g, b, delay, duration);
        break;
        
    case P_WAVE_B:
        nonaddr_bwave(r, g, b, delay, duration);
        break;
    
    case P_ON:
        nonaddr_on(r, g, b, duration);
        break;
        
    case P_FADE:
        nonaddr_fade(r, g, b, delay, duration);
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

void nonaddr_wave(uint32_t r, uint32_t g, uint32_t b, uint32_t delay, uint32_t duration)
{
    timeout.attach_us(&timeout_isr, duration * 1000);
    timed_out = false;

    tlc.setLED(0,r, g, b);
    tlc.setLED(1,65535, 65535, 65535);
    tlc.write();
    wait_ms(delay);
    if(timed_out)
    {
        timed_out = false;
        return;
    }
    tlc.setLED(0,65535, 65535, 65535);
    tlc.setLED(1,r, g, b);
    tlc.write();
    wait_ms(delay);
    if(timed_out)
    {
        timed_out = false;
        return;
    }
    tlc.setLED(0,65535, 65535, 65535);
    tlc.setLED(1,65535, 65535, 65535);
    tlc.write();
    wait_ms(delay);  
}

void nonaddr_bwave(uint32_t r, uint32_t g, uint32_t b, uint32_t delay, uint32_t duration)
{
    timeout.attach_us(&timeout_isr, duration * 1000);
    timed_out = false;

    tlc.setLED(1,r, g, b);
    tlc.setLED(0,65535, 65535, 65535);
    tlc.write();
    wait_ms(delay);
    if(timed_out)
    {
        timed_out = false;
        return;
    }
    tlc.setLED(1,65535, 65535, 65535);
    tlc.setLED(0,r, g, b);
    tlc.write();
    wait_ms(delay);
    if(timed_out)
    {
        timed_out = false;
        return;
    }
    tlc.setLED(1,65535, 65535, 65535);
    tlc.setLED(0,65535, 65535, 65535);
    tlc.write();
    wait_ms(delay);  
}

void nonaddr_fade(uint32_t r, uint32_t g, uint32_t b, uint32_t delay, uint32_t duration)
{
    uint32_t scaledr, scaledg, scaledb;
    timeout.attach_us(&timeout_isr, duration * 1000);
    timed_out = false;
    float scaler = 0.1;
    bool increment_flag = true;
    while(!timed_out)
    {
        scaledr = (uint32_t)(scaler * (65535 - r));
        scaledg = (uint32_t)(scaler * (65535 - g));
        scaledb = (uint32_t)(scaler * (65535 - b));
        tlc.setLED(0, scaledr, scaledg, scaledb);
        tlc.setLED(1, scaledr, scaledg, scaledb);
        tlc.write();
        wait_ms(delay);
        if(scaler == 1)
        {
            increment_flag = false;
        }
        if(increment_flag)
        {
            scaler += 0.05;
        }
        else
        {
            scaler -= 0.05;
        }
    }
    clear_strip();
}

//Turns on all strips with the specified colour
void nonaddr_on(uint32_t r, uint32_t g, uint32_t b, uint32_t duration)
{
    timeout.attach_us(&timeout_isr, duration * 1000);
    tlc.setLED(0, r, g, b);
    tlc.setLED(1, r, g, b);
    tlc.setLED(2, r, g, b);
    tlc.setLED(3, r, g, b);
    tlc.write();
}