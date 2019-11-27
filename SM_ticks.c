/*
 * Implementation of of the State Machines tick functions
 */

#include <avr/io.h>
#include "SM_ticks.h"
#include "nokia5110.h"
#include <stdio.h>
#include <util/delay.h>

// http://maxembedded.com/2011/06/the-adc-of-the-avr/
unsigned short readADC(uint8_t channel) {
    channel &= 0x07;
    ADMUX = (ADMUX & 0xF8) | channel;
    ADCSRA |= (1 << ADSC); // Start Conversion
    while ( (ADCSRA) & (1 << ADSC) ); // Wait until conversion complete
    return ADC;
}

// Joystick tick implementation
int JSTick(int state) {
    volatile unsigned short analog_X;
    volatile unsigned short analog_Y;
    
    analog_X = readADC(0);
    analog_Y = readADC(1);
    
    switch (state) { // State Transitions
        case WAIT:
            if (analog_X >= 800) { state = RIGHT; }
            else if (analog_X <= 80) { state = LEFT; }
            else if (analog_Y >= 800) { state = UP; }
            else if (analog_Y <= 80) { state = DOWN; }
            else { state = WAIT; }
            break;
            
        case UP:
            if (analog_Y >= 800) { state = UP; }
            else { state = WAIT; }
            break;
            
        case DOWN:
            if (analog_Y <= 80) { state = DOWN; }
            else { state = WAIT; }
            break;
            
        case LEFT:
            if (analog_X <= 80) { state = LEFT; }
            else { state = WAIT; }
            break;
            
        case RIGHT:
            if (analog_X >= 800) { state = RIGHT; }
            else { state = WAIT; }
            break;
            
        default:
            state = WAIT;
            break;
    }
    
    switch (state) { // State Actions
        case WAIT:
            position = 0x00;
            break;
            
        case UP:
            position = 0x01;
            break;
            
        case DOWN:
            position = 0x02;
            break;
            
        case LEFT:
            position = 0x04;
            break;
            
        case RIGHT:
            position = 0x08;
            break;
            
        default:
            break;
    }
    return state;
}

int nokiaDisplayTick(int state) {
    
    /*
     nokia_lcd_clear();
     nokia_lcd_set_pixel(horiPos, vertPos, 1);
     nokia_lcd_render();
     */
    unsigned char x = 2;
    //unsigned char y = 6;
    nokia_lcd_set_cursor(8,3);
    nokia_lcd_write_char(x + '0', 1);
    //nokia_lcd_write_char(y + '0', 1);
    //nokia_lcd_write_char(y + '0', 1);
    nokia_lcd_render();
    return 0;
}

// Manage Grid tick implementation
/*
 | 0 | 1 | 2 | 3 |
 |---|---|---|---|
 | 4 | 5 | 6 | 7 |
 |---|---|---|---|
 | 8 | 9 |10 |11 |
 |---|---|---|---|
 |12 |13 |14 |15 |
 
 Each cell is associated with the above array indices
 */
int manageGrid(int state) {
    switch (state) { // State Transitions
        case WAIT_POS:
            if (position == 0x00) { state = WAIT_POS; }
            else if (position == 0x01) { state = CHECK_U; }
            else if (position == 0x02) { state = CHECK_D; }
            else if (position == 0x04) { state = CHECK_L; }
            else if (position == 0x08) { state = CHECK_R; }
            else { state = WAIT_POS; }
            break;
            
        case CHECK_U:
            if (position == 0x01) { state = CHECK_U; }
            else { state = WAIT_POS; }
            break;
            
        case CHECK_D:
            if (position == 0x02) { state = CHECK_D; }
            else { state = WAIT_POS; }
            break;
            
        case CHECK_L:
            if (position == 0x04) { state = CHECK_L; }
            else { state = WAIT_POS; }
            break;
            
        case CHECK_R:
            if (position == 0x08) { state = CHECK_R; }
            else { state = WAIT_POS; }
            break;
            
        default:
            state = WAIT_POS;
            break;
    }
    switch (state) { // State Actions
        case WAIT_POS:
            break;
            
        case CHECK_U:
            if (vertPos > 0) { vertPos--; }
            break;
            
        case CHECK_D:
            if (vertPos < 47) { vertPos++; }
            break;
            
        case CHECK_L:
            if (horiPos > 0) { horiPos--; }
            break;
            
        case CHECK_R:
            if (horiPos < 83) { horiPos++; }
            break;
            
        default:
            state = WAIT_POS;
            break;
    }
    return state;
}

// TODO: Implement manageGrid, updateScore, and Display tasks
