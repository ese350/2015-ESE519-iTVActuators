#include "led_patterns.h"

/*global timer to mark the duration*/
Timer gTimer;

Timeout timeout;

volatile bool timed_out = false;

//Initialize addresses and the pixel grid struct
void init_grid()
{
    int i, j, k, l;
    
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
        
    case P_RISEFALL:
        create_risefall(direction, r, g, b, delay, duration);
        break;
        
    case P_WAVE_FLOW:
        create_wave_flow(direction, r, g, b, delay, duration);
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
    pixel_struct_t* pixel_ptr = NULL;
        
    gTimer.start(); // begin timer
    
    /*While the time is less than the duration specified keep flashing*/
    while(gTimer.read_ms() < duration)
    {
        for (i= 0; i < SELECTEDROWS; i++)
        {
            for (j= 0; j < SELECTEDCOLS; j++)
            {
                
                pixel_ptr = get_pixel(j, i);
                if(pixel_ptr->selected)
                {
                    strip.setPixelColor(grid_matrix[i][j], strip.Color(r, g, b)); //strip 1
                }
            }       
        }
            strip.show();
            wait_ms(delay);
        
        /*Shutting down the leds*/
        clear_strip();
        wait_ms(delay);
    }
    clear_all_pixels();
    gTimer.stop();
    gTimer.reset();
}

/****************************************
* wave
*
* creates a flow effect; 
*
* @param r      red color value
* @param g      green color  
* @param b      blue color   
* @param delay  delay between updating the led layout to "move the color" by 1 led
* @param duration How long should the wave effect be active in milliseconds
*
****************************************/
void create_wave(direction_enum_t direction, uint8_t r, uint8_t g, uint8_t b, uint32_t delay, uint32_t duration)
{
    int i, j;
    timed_out = false;
    timeout.attach_us(&timeout_isr, duration * 1000);
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
        clear_all_pixels();
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
        clear_all_pixels();
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
        clear_all_pixels();
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
        clear_all_pixels();
        break;
    
    }
    
    /*Shutting down the leds*/
    clear_strip();
    wait_ms(delay);
}

/****************************************
* back and forth wave
*
* creates a back and forth wave pattern; 
*
* @param r      red color value
* @param g      green color  
* @param b      blue color   
* @param delay  delay between updating the led layout to "move the color" by 1 led
* @param duration How long should the wave effect be active in milliseconds
*
****************************************/
void create_wave_fb(direction_enum_t direction, uint8_t r, uint8_t g, uint8_t b, uint32_t delay, uint32_t duration)
{
    timeout.attach_us(&timeout_isr, duration * 1000);
    clear_strip();
    switch(direction)
    {
        case DIR_UP:
            create_wave(DIR_UP, r, g, b, delay, duration);
            select_all_pixels();
            create_wave(DIR_DOWN, r, g, b, delay, duration);
            break;
        case DIR_DOWN:
            create_wave(DIR_DOWN, r, g, b, delay, duration);
            select_all_pixels();
            create_wave(DIR_UP, r, g, b, delay, duration);
            break;
        case DIR_LEFT:
            create_wave(DIR_LEFT, r, g, b, delay, duration);
            select_all_pixels();
            create_wave(DIR_RIGHT, r, g, b, delay, duration);
            break;
        case DIR_RIGHT:
            create_wave(DIR_RIGHT, r, g, b, delay, duration);
            select_all_pixels();
            create_wave(DIR_LEFT, r, g, b, delay, duration);
            break;
        default:
            break;
    }
}

/****************************************
* rise and fall
*
* creates a rising wave followed by a falling wave; 
*
* @param r      red color value
* @param g      green color  
* @param b      blue color   
* @param delay  delay between updating the led layout to "move the color" by 1 led
* @param duration How long should the wave effect be active in milliseconds
*
****************************************/
void create_risefall(direction_enum_t direction, uint8_t r, uint8_t g, uint8_t b, uint32_t delay, uint32_t duration)
{
    int i, j;
    bool column_selected;
    pixel_struct_t* pixel_ptr = NULL;
    timed_out = false;
    timeout.attach_us(&timeout_isr, duration * 1000);
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
                strip.setPixelColor(grid_matrix[i][j], strip.Color(0, 0, 0));
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
                strip.setPixelColor(grid_matrix[i][j], strip.Color(0, 0, 0));
            }
        }
        strip.show();
        if(column_selected)
        {
            wait_ms(delay);
        }
    }
    clear_all_pixels();
    clear_strip();
    wait_ms(delay);
}

void create_wave_flow(direction_enum_t direction, uint8_t r, uint8_t g, uint8_t b, uint32_t delay, uint32_t duration)
{
    timed_out = false;
    timeout.attach_us(&timeout_isr, duration * 1000);
    while(!timed_out)
    {
        clear_strip();
        set_selected_pixels(r, g, b);
        strip.show();
        shift_array(direction);
        wait_ms(delay);
    }
    clear_strip();
}

//Clears all selected pixels
void clear_all_pixels()
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

//Sets the selected pixels with the specified r, g, b color values
void set_selected_pixels(uint8_t r, uint8_t g, uint8_t b)
{
    int i, j, k, l;
    for(i = 0; i < PIXELROWS; i++)
    {
        for(j = 0; j < PIXELCOLS; j++)
        {
            if(pixel_grid[i][j].selected)
            {
                for(k = 0; k < PIXELSIZE_ROWS; k++)
                {
                    for(l = 0; l < PIXELSIZE_COLS; l++)
                    {
                        strip.setPixelColor(pixel_grid[i][j].leds[k][l], strip.Color(r, g, b));
                    }
                }
            }
        }
    }
}


void shift_array(direction_enum_t direction)
{
    switch(direction)
    {
        case DIR_DOWN:
        {
            bool temp[PIXELCOLS] = {0};
            for(int i = PIXELROWS - 1; i >= 0; i--)
            {
                for(int j = 0; j < PIXELCOLS; j++)
                {
                    if(i == PIXELROWS - 1)
                    {
                        temp[j] = pixel_grid[i][j].selected;
                    }
                    if(i == 0)
                    {
                        pixel_grid[i][j].selected = temp[j];
                    }
                    else
                    {
                        pixel_grid[i][j].selected = pixel_grid[i-1][j].selected;
                    }
                }
            } 
            break;
        }
        case DIR_UP:
        {
            bool temp[PIXELCOLS] = {0};
            for(int i = 0; i < PIXELROWS; i++)
            {
                for(int j = 0; j < PIXELCOLS; j++)
                {
                    if(i == 0)
                    {
                        temp[j] = pixel_grid[i][j].selected;
                    }
                    if(i == PIXELROWS - 1)
                    {
                        pixel_grid[i][j].selected = temp[j];
                    }
                    else
                    {
                        pixel_grid[i][j].selected = pixel_grid[i+1][j].selected;
                    }
                }
            }
            break;
        }
        case DIR_RIGHT:
        {
            bool temp[PIXELROWS] = {0};
            for(int i = PIXELCOLS - 1; i >= 0; i--)
            {
                for(int j = 0; j < PIXELROWS; j++)
                {
                    if(i == PIXELCOLS - 1)
                    {
                        temp[j] = pixel_grid[j][i].selected;
                    }
                    if(i == 0)
                    {
                        pixel_grid[j][i].selected = temp[j];
                    }
                    else
                    {
                        pixel_grid[j][i].selected = pixel_grid[j][i-1].selected;
                    }
                }
            }
            break;
        }
        case DIR_LEFT:
        {
            bool temp[PIXELROWS] = {0};
            for(int i = 0; i < PIXELCOLS; i++)
            {
                for(int j = 0; j < PIXELROWS; j++)
                {
                    if(i == 0)
                    {
                        temp[j] = pixel_grid[j][i].selected;
                    }
                    if(i == PIXELCOLS - 1)
                    {
                        pixel_grid[j][i].selected = temp[j];
                    }
                    else
                    {
                        pixel_grid[j][i].selected = pixel_grid[j][i+1].selected;
                    }
                }
            }
            break;
        }
        default:
            break;
    }
}

//void select_pattern(int shape)
//{
//    switch(shape)
//    {
//        case 1:
//            break;
//        
//        case 2:
//            break;
//            
//        case 3:
//            break;
//            
//        default:
//            break;
//    }
//}

//Selects all pixels
void select_all_pixels()
{
    int i, j;
    for (i = 0; i < PIXELROWS; i++)
    {
        for (j = 0; j < PIXELCOLS; j++)
        {
            pixel_grid[i][j].selected = 1;
        } 
    }
}

//Display pixel with the specified color and duration
void show_pixel(pixel_struct_t* pixel_ptr, uint8_t r, uint8_t g, uint8_t b, uint32_t delay)
{
    for(int i = 0; i < PIXELSIZE_ROWS; i++)
    {
        for(int j = 0; j < PIXELSIZE_COLS; j++)
        {
            strip.setPixelColor(pixel_ptr->leds[i][j], strip.Color(r, g, b));
        }
        strip.show();
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