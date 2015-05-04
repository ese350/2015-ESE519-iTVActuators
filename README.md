Blog Link: http://i-tv-actuators-upenn.blogspot.com/

The code is arranged in the form of 2 main folders:

1. led_addressable_new: Has all the code contents needed to light up the addressable LEDs. Files under this include:
    a. LPD8806, MRF24J40, TLC59711, mbed-rtos, mbed libraries to provide support for the ZigBee modules, mbed, etc.
    b. led_patterns.cpp: Has function descriptions for all the patterns possible, like: flash, wave, flow, etc.
    c. led_patterns.h: Has function definitions for all possible patterns, enum definitions for pattern type and pattern direction.
    d. main.cpp: This is the code that has to be run to implement our project with addressable LED strips.
    e. The remaining files are helper headers.

2. led_nonaddressable_new: Has all the code contents needed to light up the non-addressable LEDs. Files under this include:
    a. LPD8806, MRF24J40, TLC59711, mbed libraries to provide support for ZigBee modules, PWM modules, mbed, etc.
    b. main.cpp: This is the code that has to be run to implement our project with non-addressable LED strips.
    c. main.h: Includes pattern function definitions and enum descriptions.
    d. nonaddressable.cpp: Has function descriptions for all the patterns possible.
    e. The remaining files are helper headers.

3. webapp: This folder has the css and json files that run the web application.
