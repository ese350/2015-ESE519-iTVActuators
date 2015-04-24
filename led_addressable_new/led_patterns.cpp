#include "led_patterns.h"

/*global timer to mark the duration*/
Timer gTimer;

Timeout timeout;

volatile bool timed_out = false;

void init_grid()
{
   // int col;
   int i, j, k, l;
    
    ///*Address init for the first strip*/
//    for(col = 0; col < COLS ; col++)
//        grid[0][col] = COLS*0 + col;
    
    //Initialize addresses    
    for(i = 0; i < SELECTEDROWS; i++)
    {
        for(j = 0; j < SELECTEDCOLS; j++)
        {
            if(j % 2 == 0)
            {
                grid_matrix[i][j] = (SELECTEDROWS * j) + 65 - i;
            }
            else
            {
                grid_matrix[i][j] = (SELECTEDROWS * j) + i;
            }
        }
    }
    
    //Initialize pixel grid
    for(i = 0; i < PIXELROWS; i++)
    {
        for(j = 0; j < PIXELCOLS; j++)
        {
            for(k = 0; k < PIXELSIZE_ROWS; k++)
            {
                for(l = 0; l < PIXELSIZE_COLS; l++)
                {
                    pixel_grid[i][j].leds[k][l] = grid_matrix[PIXELSIZE_ROWS*i + k][PIXELSIZE_COLS*j + l];
                }
            }
            pixel_grid[i][j].selected = 0;
        }
    } 
    ///*Address init for the second strip*/
//    for(col = 0; col < COLS; col++)
//        grid[1][col] = COLS*1 + col;
}

void clear_strip()
{
    for (int i=0; i<((strip.numPixels())); i++)
        strip.setPixelColor(i, strip.Color(0,0,0));
    strip.show();
}

void timeout_isr()
{
    timed_out = true;
}

void create_pattern(pattern_enum_t type, direction_enum_t direction, uint8_t r, uint8_t g, uint8_t b, uint32_t delay, uint32_t duration)
{
    clear_strip();
    
    switch(type)
    {
    case P_FLASH:
        create_flash(direction, r, g, b, delay, duration);
        break;
    
    case P_WAVE:
        create_wave(direction, r, g, b, delay, duration);
        break;
        
    case P_WAVE_FB:
        create_wave_fb(direction, r, g, b, delay, duration);
        break;
    
    //case P_PULSE:
//        create_pulse(direction, r, g, b, delay, duration);
        
    case P_RISEFALL:
        create_risefall(direction, r, g, b, delay, duration);
        break;
    
    case P_CUSTOM:
        create_custom(direction, r, g, b, delay, duration);
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
void create_flash(direction_enum_t direction, uint8_t r, uint8_t g, uint8_t b, uint32_t delay, uint32_t duration)
{
    int i, j;
    uint8_t start_x, start_y, end_x, end_y;
    
    start_x = 0;
    start_y = 0;
    end_x = SELECTEDROWS;
    end_y = SELECTEDCOLS;
        
    gTimer.start(); // begin timer
    
    /*While the time is less than the duration specified keep flashing*/
    while(gTimer.read_ms() < duration)
    {
        for (i= start_x; i < end_x; i++)
        {
            for (j= start_y; j < end_y; j++)
            {
                strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b)); //strip 1
            }       
        }
            strip.show();
            wait_ms(delay);
    
        /*Shutting down the leds*/
        clear_strip();
        wait_ms(delay);
    }
    gTimer.stop();
    gTimer.reset();
}

/****************************************
* wave
*
* creates a flow effect that is in sync with the super bowl touchdown scene; 
* the rgb value can be changed so that the color can match the color of the team that scored
*
* @param r      red color value
* @param g      green color  
* @param b      blue color   
* @param delay  delay between updating the led layout to "move the color" by 1 led
*
****************************************/
void create_wave(direction_enum_t direction, uint8_t r, uint8_t g, uint8_t b, uint32_t delay, uint32_t duration)
{
    int i, j;
    timeout.attach_us(&timeout_isr, duration * 1000);
    switch(direction)
    {
    case DIR_UP:
    
        for(i = SELECTEDROWS - 1; i >= 0; i--)
        {
            for(j = 0; j < SELECTEDCOLS; j++)
            {
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }                
                strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
            }
            strip.show();
            wait_ms(1);
            for(j = 0; j < SELECTEDCOLS; j++)
            {
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
            }
            strip.show();
            wait_ms(delay);
        }
        break;
    
    case DIR_DOWN:
        
        for(i = 0; i < SELECTEDROWS; i++)
        {
            for(j = 0; j < SELECTEDCOLS; j++)
            {
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
            }
            strip.show();
            wait_ms(1);
            for(j = 0; j < SELECTEDCOLS; j++)
            {
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
            }
            strip.show();
            wait_ms(delay);
        }
        break; 
    
    case DIR_LEFT:
    
        for(j = SELECTEDCOLS - 1; j >= 0; j--)
        {
            for(i = 0; i < SELECTEDROWS; i++)
            {
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
            }
            strip.show();
            wait_ms(1);
            for(i = 0; i < SELECTEDROWS; i++)
            {
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
            }
            strip.show();
            wait_ms(delay);
        }
    
        break;
    
    case DIR_RIGHT:
    
        for(j = 0; j < SELECTEDCOLS; j++)
        {
            for(i = 0; i < SELECTEDROWS; i++)
            {
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
            }
            strip.show();
            wait_ms(1);
            for(i = 0; i < SELECTEDROWS; i++)
            {
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
            }
            strip.show();
            wait_ms(delay);
        }
        break;
    
    }
    
    /*Shutting down the leds*/
    clear_strip();
    wait_ms(delay);
    
}

void create_wave_fb(direction_enum_t direction, uint8_t r, uint8_t g, uint8_t b, uint32_t delay, uint32_t duration)
{

//    int i, j;
    timeout.attach_us(&timeout_isr, duration * 1000);
    clear_strip();
    
//    for(i = SELECTEDROWS - 1; i >= 0; i--)
//    {
//        for(j = 0; j < SELECTEDCOLS; j++)
//        {
//            strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
//        }
//        strip.show();
//        wait_ms(1);
//        for(j = 0; j < SELECTEDCOLS; j++)
//        {
//            strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
//        }
//        strip.show();
//        wait_ms(delay);
//    }
//    
//    /*Shutting down the leds*/
//    clear_strip();
    
    create_wave(DIR_UP, r, g, b, delay, duration);
    
    create_wave(DIR_DOWN, r, g, b, delay, duration);
    
    //for(i = 0; i < SELECTEDROWS; i++)
//        {
//            for(j = 0; j < SELECTEDCOLS; j++)
//            {
//                strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
//            }
//            strip.show();
//            wait_ms(1);
//            for(j = 0; j < SELECTEDCOLS; j++)
//            {
//                strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
//            }
//            strip.show();
//            wait_ms(delay);
//        }
//    /*Shutting down the leds*/
//    clear_strip();
//    wait_ms(delay);
}

void create_risefall(direction_enum_t direction, uint8_t r, uint8_t g, uint8_t b, uint32_t delay, uint32_t duration)
{
    int i, j;
    
    timeout.attach_us(&timeout_isr, duration * 1000);
    clear_strip();
    
    for(i = SELECTEDROWS - 1; i >= 0; i--)
    {
        for(j = 0; j < SELECTEDCOLS; j++)
        {
            if(timed_out)
            {
                clear_strip();
                timed_out = false;
                return;
            }
            strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
        }
        strip.show();
        wait_ms(1);
        for(j = 0; j < SELECTEDCOLS; j++)
        {
            if(timed_out)
            {
                clear_strip();
                timed_out = false;
                return;
            }
            strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
        }
        strip.show();
        wait_ms(delay);
    }
    for(i = 0; i < SELECTEDROWS; i++)
    {
        for(j = 0; j < SELECTEDCOLS; j++)
        {
            if(timed_out)
            {
                clear_strip();
                timed_out = false;
                return;
            }
            strip.setPixelColor(grid_matrix[i][j], strip.Color(0, 0, 0));
        }
        strip.show();
        wait_ms(1);
        for(j = 0; j < SELECTEDCOLS; j++)
        {
            if(timed_out)
            {
                clear_strip();
                timed_out = false;
                return;
            }
            strip.setPixelColor(grid_matrix[i][j], strip.Color(0, 0, 0));
        }
        strip.show();
        wait_ms(delay);
    }
    
    clear_strip();
    wait_ms(delay);
}

/**
Creates a custom pattern based on the selected pixels in the grid
*/
void create_custom(direction_enum_t direction, uint8_t r, uint8_t g, uint8_t b, uint32_t delay, uint32_t duration)
{
    timeout.attach_us(&timeout_isr, duration * 1000);
    int i, j;
    bool column_selected, row_selected;
    pixel_struct_t* pixel_ptr;
    
    switch(direction)
    {
    case DIR_UP:
        column_selected = 0;
        clear_strip();
        
        for(i = SELECTEDROWS - 1; i >= 0; i--)
        {
            column_selected = 0;
            for(j = 0; j < SELECTEDCOLS; j++)
            {
                pixel_ptr = get_pixel(j, i);
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                if(pixel_ptr->selected)
                {
                    column_selected = 1;
                    strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
                }
            }
            strip.show();
            wait_ms(1);
            for(j = 0; j < SELECTEDCOLS; j++)
            {
                pixel_ptr = get_pixel(j, i);
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                if(pixel_ptr->selected)
                {
                    strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
                }
            }
            strip.show();
            if(column_selected)
            {
                wait_ms(delay);
            }
        }
        clear_selected_pixels();
        break;
    
    case DIR_DOWN:
        column_selected = 0;
        for(i = 0; i < SELECTEDROWS; i++)
        {
            for(j = 0; j < SELECTEDCOLS; j++)
            {
                pixel_ptr = get_pixel(j, i);
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                if(pixel_ptr->selected)
                {
                    column_selected = 1;
                    strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
                }
            }
            strip.show();
            wait_ms(1);
            for(j = 0; j < SELECTEDCOLS; j++)
            {
                pixel_ptr = get_pixel(j, i);
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                if(pixel_ptr->selected)
                {
                    strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
                }
            }
            strip.show();
            if(column_selected)
            {
                wait_ms(delay);
            }
        }
        clear_selected_pixels();
        break; 
    
    case DIR_LEFT:
        row_selected = 0;
        for(j = SELECTEDCOLS - 1; j >= 0; j--)
        {
            for(i = 0; i < SELECTEDROWS; i++)
            {
                pixel_ptr = get_pixel(j, i);
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                if(pixel_ptr->selected)
                {
                    row_selected = 1;
                    strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
                }
            }
            strip.show();
            wait_ms(1);
            for(i = 0; i < SELECTEDROWS; i++)
            {
                pixel_ptr = get_pixel(j, i);
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                if(pixel_ptr->selected)
                {
                    strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
                }
            }
            strip.show();
            if(row_selected)
            {
                wait_ms(delay);
            }
        }
        clear_selected_pixels();
        break;
    
    case DIR_RIGHT:
    
        row_selected = 0;
        for(j = 0; j < SELECTEDCOLS; j++)
        {
            for(i = 0; i < SELECTEDROWS; i++)
            {
                pixel_ptr = get_pixel(j, i);
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                if(pixel_ptr->selected)
                {
                    row_selected = 1;
                    strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
                }
            }
            strip.show();
            wait_ms(1);
            for(i = 0; i < SELECTEDROWS; i++)
            {
                pixel_ptr = get_pixel(j, i);
                if(timed_out)
                {
                    clear_strip();
                    timed_out = false;
                    return;
                }
                if(pixel_ptr->selected)
                {
                    strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b));
                }
            }
            strip.show();
            if(row_selected)
            {
                wait_ms(delay);
            }
        }
        clear_selected_pixels();
        break;
    
    }
    
    /*Shutting down the leds*/
    clear_strip();
    wait_ms(delay);
}

void clear_selected_pixels()
{
    int i, j;
    for (i = 0; i < PIXELROWS; i++)
    {
        for (j = 0; j < PIXELCOLS; j++)
        {
            pixel_grid[i][j].selected = 0;
        } 
    }
}

void show_pixel(pixel_struct_t* pixel_ptr, uint8_t r, uint8_t g, uint8_t b, uint32_t delay)
{
    for(int i = 0; i < PIXELSIZE_ROWS; i++)
    {
        for(int j = 0; j < PIXELSIZE_COLS; j++)
        {
//            printf("%d\r\n", pixel_ptr->leds[i][j]);
            strip.setPixelColor(pixel_ptr->leds[i][j], strip.Color(r, g, b));
        }
        strip.show();
        //wait_ms(1);
//        for(int j = 0; j < PIXELSIZE_COLS; j++)
//        {
//            strip.setPixelColor(pixel_ptr->leds[i][j], strip.Color(r, g, b));
//        }
//        strip.show();
    }
    wait_ms(delay);
    clear_strip();
    wait_ms(delay);
}

// Get pointer to pixel at the input coordinates
pixel_struct_t* get_pixel(int x, int y)
{
    int pixely = y/PIXELSIZE_ROWS;
    int pixelx = x/PIXELSIZE_COLS;
    return &pixel_grid[pixely][pixelx];
}