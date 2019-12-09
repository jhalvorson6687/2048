/*
 * Implementation of the State Machines tick functions
 */

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include "SM_ticks.h"
#include "io.h"
#include "nokia5110.h"

// http://maxembedded.com/2011/06/the-adc-of-the-avr/
unsigned short readADC(uint8_t channel) {
    channel &= 0x07;
    ADMUX = (ADMUX & 0xF8) | channel;
    ADCSRA |= (1 << ADSC); // Start Conversion
    while ( (ADCSRA) & (1 << ADSC) ); // Wait until conversion complete
    return ADC;
}

void drawBoard() {
    // Draw vertical lines across screen
    for (int i = 0; i <= 84; i += 21) {
        for (int j = 0; j < 48; ++j) {
            if (i == 84) {
                nokia_lcd_set_pixel(83, j, 1);
            }
            else {
                nokia_lcd_set_pixel(i, j, 1);
            }
        }
    }
    // Draw horizontal lines across screen
    for (int h = 0; h <= 48; h += 12) {
        for (int k = 0; k < 84; ++k) {
            if (h == 48) {
                nokia_lcd_set_pixel(k, 47, 1);
            }
            else {
                nokia_lcd_set_pixel(k, h, 1);
            }
        }
    }
}

// Joystick tick implementation
int jsTick(int state) {
    unsigned short analog_X;
    unsigned short analog_Y;
    
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

// Nokia Display tick implementation
int nokiaDisplayTick(int state) {
    switch (state) { // State Transitions
        case MENU:
            if (~PINA & 0x80) { state = GRID; }
            else { state = MENU; }
            break;
            
        case GRID:
            if (~PINA & 0x20) { state = RESET; }
            else if (gameWon()) { state = GAME_WON; }
            else if (gameOver()) { state = GAME_OVER; }
            else { state = GRID; }
            break;
            
        case GAME_OVER:
            if (~PINA & 0x80) { state = RESET; }
            else { state = GAME_OVER; }
            break;
            
        case GAME_WON:
            if (~PINA & 0x80) { state = GRID; }
            else { state = GAME_WON; }
            break;
            
        case RESET:
            state = GRID;
            break;
            
        default:
            state = MENU;
    }
    switch (state) { // State Actions
        case MENU:
            displayMenu();
            break;
            
        case GRID:
            displayGrid();
            break;
            
        case GAME_OVER:
            displayGameOver();
            break;
            
        case GAME_WON:
            displayGameWon();
            break;
            
        case RESET:
            resetGrid();
            break;
            
        default:
            break;
    }
    
    return state;
}

// Manage Grid tick implementation
int manageGridTick(int state) {
    unsigned char tmpA = ~PINA & 0xFF;
    
    switch (state) { // State Transitions
        case WAIT_START:
            if (tmpA == 0xC0) { state = CHEAT; } // Enter cheat mode if PA6 is held while joystick is pressed
            else if (tmpA == 0x80) { state = WAIT_POS; } // Wait until joystick is pressed down
            else { state = WAIT_START; }
            break;
        case WAIT_POS:
            if (position == 0x00) { state = WAIT_POS; }
            else if (position == 0x01) { state = CHECK_U; }
            else if (position == 0x02) { state = CHECK_D; }
            else if (position == 0x04) { state = CHECK_L; }
            else if (position == 0x08) { state = CHECK_R; }
            else { state = WAIT_POS; }
            break;
            
        case CHECK_U:
            if (position != 0x00) { state = RELEASE; }
            else { state = WAIT_POS; }
            break;
            
        case CHECK_D:
            if (position != 0x00) { state = RELEASE; }
            else { state = WAIT_POS; }
            break;
            
        case CHECK_L:
            if (position != 0x00) { state = RELEASE; }
            else { state = WAIT_POS; }
            break;
            
        case CHECK_R:
            if (position != 0x00) { state = RELEASE; }
            else { state = WAIT_POS; }
            break;
            
        case RELEASE:
            if (position == 0x00) { state = WAIT_POS; }
            else { state = RELEASE; }
            break;
            
        case CHEAT:
            state = WAIT_POS;
            break;
            
        default:
            state = WAIT_POS;
            break;
    }
    switch (state) { // State Actions
            int valMove1;
            int valMove2;
            int valMove3;
            int valMove4;
            
        case WAIT_START:
            break;
            
        case WAIT_POS:
            break;
            
        case CHECK_U:
            // Check Column 1
            valMove1 = moveTiles(0, 4, 8, 12);
            // Check Column 2
            valMove2 = moveTiles(1, 5, 9, 13);
            // Check Column 3
            valMove3 = moveTiles(2, 6, 10, 14);
            // Check Column 4
            valMove4 = moveTiles(3, 7, 11, 15);
            
            if ( (tilesInPlay < 16) && (valMove1 || valMove2 || valMove3 || valMove4) ) {
                generateRandTile();
            }
            break;
            
        case CHECK_D:
            // Check Column 1
            valMove1 = moveTiles(12, 8, 4, 0);
            // Check Column 2
            valMove2 = moveTiles(13, 9, 5, 1);
            // Check Column 3
            valMove3 = moveTiles(14, 10, 6, 2);
            // Check Column 4
            valMove4 = moveTiles(15, 11, 7, 3);
            
            if ( (tilesInPlay < 16) && (valMove1 || valMove2 || valMove3 || valMove4) ) {
                generateRandTile();
            }
            break;
            
        case CHECK_L:
            // Check Row 1
            valMove1 = moveTiles(0, 1, 2, 3);
            // Check Row 2
            valMove2 = moveTiles(4, 5, 6, 7);
            // Check Row 3
            valMove3 = moveTiles(8, 9, 10, 11);
            // Check Row 4
            valMove4 = moveTiles(12, 13, 14, 15);
            
            if ( (tilesInPlay < 16) && (valMove1 || valMove2 || valMove3 || valMove4) ) {
                generateRandTile();
            }
            break;
            
        case CHECK_R:
            // Check Row 1
            valMove1 = moveTiles(3, 2, 1, 0);
            // Check Row 2
            valMove2 = moveTiles(7, 6, 5, 4);
            // Check Row 3
            valMove3 = moveTiles(11, 10, 9, 8);
            // Check Row 4
            valMove4 = moveTiles(15, 14, 13, 12);
            
            if ( (tilesInPlay < 16) && (valMove1 || valMove2 || valMove3 || valMove4) ) {
                generateRandTile();
            }
            break;
            
        case RELEASE:
            break;
            
        case CHEAT:
            cheatMode();
            break;
            
        default:
            break;
    }
    return state;
}

// LCD Screen tick implementation
int lcdScreenTick(int state) {
    LCD_DisplayString(1, "Score:");
    LCD_DisplayString(17, "High Score:");
    
    // Display Current Score
    if (score < 10) {
        LCD_Cursor(8);
        LCD_WriteData(score + '0');
    }
    else if (score < 100) {
        unsigned char firstDigit = score / 10;
        unsigned char secondDigit = score % 10;
        
        LCD_Cursor(8);
        LCD_WriteData(firstDigit + '0');
        LCD_Cursor(9);
        LCD_WriteData(secondDigit + '0');
    }
    else if (score < 1000) {
        int value = score;
        unsigned char thirdDigit = value % 10;
        value /= 10;
        unsigned char secondDigit = value % 10;
        value /= 10;
        unsigned char firstDigit = value % 10;
        
        LCD_Cursor(8);
        LCD_WriteData(firstDigit + '0');
        LCD_Cursor(9);
        LCD_WriteData(secondDigit + '0');
        LCD_Cursor(10);
        LCD_WriteData(thirdDigit + '0');
    }
    else {
        int value = score;
        unsigned char fourthDigit = value % 10;
        value /= 10;
        unsigned char thirdDigit = value % 10;
        value /= 10;
        unsigned char secondDigit = value % 10;
        value /= 10;
        unsigned char firstDigit = value % 10;
        
        LCD_Cursor(8);
        LCD_WriteData(firstDigit + '0');
        LCD_Cursor(9);
        LCD_WriteData(secondDigit + '0');
        LCD_Cursor(10);
        LCD_WriteData(thirdDigit + '0');
        LCD_Cursor(11);
        LCD_WriteData(fourthDigit + '0');
    }
    
    // Display High Score
    if (highscore < 10) {
        LCD_Cursor(29);
        LCD_WriteData(highscore + '0');
    }
    else if (highscore < 100) {
        unsigned char firstDigit = highscore / 10;
        unsigned char secondDigit = highscore % 10;
        
        LCD_Cursor(29);
        LCD_WriteData(firstDigit + '0');
        LCD_Cursor(30);
        LCD_WriteData(secondDigit + '0');
    }
    else if (highscore < 1000) {
        int value = highscore;
        unsigned char thirdDigit = value % 10;
        value /= 10;
        unsigned char secondDigit = value % 10;
        value /= 10;
        unsigned char firstDigit = value % 10;
        
        LCD_Cursor(29);
        LCD_WriteData(firstDigit + '0');
        LCD_Cursor(30);
        LCD_WriteData(secondDigit + '0');
        LCD_Cursor(31);
        LCD_WriteData(thirdDigit + '0');
    }
    else {
        int value = highscore;
        unsigned char fourthDigit = value % 10;
        value /= 10;
        unsigned char thirdDigit = value % 10;
        value /= 10;
        unsigned char secondDigit = value % 10;
        value /= 10;
        unsigned char firstDigit = value % 10;
        
        LCD_Cursor(29);
        LCD_WriteData(firstDigit + '0');
        LCD_Cursor(30);
        LCD_WriteData(secondDigit + '0');
        LCD_Cursor(31);
        LCD_WriteData(thirdDigit + '0');
        LCD_Cursor(32);
        LCD_WriteData(fourthDigit + '0');
    }
    if (score > highscore) {
        writeEEPROM(score);
    }
    return 0;
}

/*
 | 0 | 1 | 2 | 3 |
 |---|---|---|---|
 | 4 | 5 | 6 | 7 |
 |---|---|---|---|
 | 8 | 9 |10 |11 |
 |---|---|---|---|
 |12 |13 |14 |15 |
 
 Each cell above is associated with the array indices in gridValues
 */
int moveTiles(unsigned short indexOne, unsigned short indexTwo, unsigned short indexThree, unsigned short indexFour) {
    validMove = TRUE;
    
    // Check 0 number case
    if ( (gridValues[indexOne] == 0) && (gridValues[indexTwo] == 0) && (gridValues[indexThree] == 0) && (gridValues[indexFour] == 0) ) {
        validMove = FALSE;
    }
    
    // Check 1 number cases
    else if ( (gridValues[indexOne] == 0) && (gridValues[indexTwo] == 0) && (gridValues[indexThree] == 0) && (gridValues[indexFour] != 0) ) {
        gridValues[indexOne] = gridValues[indexFour];
        gridValues[indexFour] = 0;
    }
    else if ( (gridValues[indexOne] == 0) && (gridValues[indexTwo] == 0) && (gridValues[indexThree] != 0) && (gridValues[indexFour] == 0) ) {
        gridValues[indexOne] = gridValues[indexThree];
        gridValues[indexThree] = 0;
    }
    else if ( (gridValues[indexOne] == 0) && (gridValues[indexTwo] != 0) && (gridValues[indexThree] == 0) && (gridValues[indexFour] == 0) ) {
        gridValues[indexOne] = gridValues[indexTwo];
        gridValues[indexTwo] = 0;
    }
    else if ( (gridValues[indexOne] != 0) && (gridValues[indexTwo] == 0) && (gridValues[indexThree] == 0) && (gridValues[indexFour] == 0) ) {
        validMove = FALSE;
    }
    
    // Check 2 number cases
    else if ( (gridValues[indexOne] == 0) && (gridValues[indexTwo] == 0) && (gridValues[indexThree] != 0) && (gridValues[indexFour] != 0)) {
        if (gridValues[indexFour] == gridValues[indexThree]) {
            gridValues[indexOne] = gridValues[indexFour] * 2;
            gridValues[indexFour] = 0;
            gridValues[indexThree] = 0;
            tilesInPlay--;
            score += gridValues[indexOne];
        }
        else {
            gridValues[indexOne] = gridValues[indexThree];
            gridValues[indexTwo] = gridValues[indexFour];
            gridValues[indexThree] = gridValues[indexFour] = 0;
        }
    }
    else if ( (gridValues[indexOne] == 0) && (gridValues[indexTwo] != 0) && (gridValues[indexThree] == 0) && (gridValues[indexFour] != 0)) {
        if (gridValues[indexFour] == gridValues[indexTwo]) {
            gridValues[indexOne] = gridValues[indexFour] * 2;
            gridValues[indexFour] = 0;
            gridValues[indexTwo] = 0;
            tilesInPlay--;
            score += gridValues[indexOne];
        }
        else {
            gridValues[indexOne] = gridValues[indexTwo];
            gridValues[indexTwo] = gridValues[indexFour];
            gridValues[indexFour] = 0;
        }
    }
    else if ( (gridValues[indexOne] != 0) && (gridValues[indexTwo] == 0) && (gridValues[indexThree] == 0) && (gridValues[indexFour] != 0)) {
        if (gridValues[indexFour] == gridValues[indexOne]) {
            gridValues[indexOne] *= 2;
            gridValues[indexFour] = 0;
            tilesInPlay--;
            score += gridValues[indexOne];
        }
        else {
            gridValues[indexTwo] = gridValues[indexFour];
            gridValues[indexFour] = 0;
        }
    }
    else if ( (gridValues[indexOne] == 0) && (gridValues[indexTwo] != 0) && (gridValues[indexThree] != 0) && (gridValues[indexFour] == 0)) {
        if (gridValues[indexThree] == gridValues[indexTwo]) {
            gridValues[indexOne] = gridValues[indexThree] * 2;
            gridValues[indexThree] = 0;
            gridValues[indexTwo] = 0;
            tilesInPlay--;
            score += gridValues[indexOne];
        }
        else {
            gridValues[indexOne] = gridValues[indexTwo];
            gridValues[indexTwo] = gridValues[indexThree];
            gridValues[indexThree] = 0;
        }
    }
    else if ( (gridValues[indexOne] != 0) && (gridValues[indexTwo] == 0) && (gridValues[indexThree] != 0) && (gridValues[indexFour] == 0)) {
        if (gridValues[indexThree] == gridValues[indexOne]) {
            gridValues[indexOne] = gridValues[indexThree] * 2;
            gridValues[indexThree] = 0;
            tilesInPlay--;
            score += gridValues[indexOne];
        }
        else {
            gridValues[indexTwo] = gridValues[indexThree];
            gridValues[indexThree] = 0;
        }
    }
    else if ( (gridValues[indexOne] != 0) && (gridValues[indexTwo] != 0) && (gridValues[indexThree] == 0) && (gridValues[indexFour] == 0)) {
        if (gridValues[indexTwo] == gridValues[indexOne]) {
            gridValues[indexOne] = gridValues[indexTwo] * 2;
            gridValues[indexTwo] = 0;
            tilesInPlay--;
            score += gridValues[indexOne];
        }
        else {
            validMove = FALSE;
        }
    }
    
    // Check 3 number cases
    else if ( (gridValues[indexOne] == 0) && (gridValues[indexTwo] != 0) && (gridValues[indexThree] != 0) && (gridValues[indexFour] != 0) ) {
        if (gridValues[indexTwo] == gridValues [indexThree]) {
            gridValues[indexOne] = gridValues[indexTwo] * 2;
            gridValues[indexTwo] = gridValues[indexFour];
            gridValues[indexFour] = gridValues[indexThree] = 0;
            tilesInPlay--;
            score += gridValues[indexOne];
        }
        else if (gridValues[indexThree] == gridValues [indexFour]) {
            gridValues[indexOne] = gridValues[indexTwo];
            gridValues[indexTwo] = gridValues[indexThree] * 2;
            gridValues[indexFour] = gridValues[indexThree] = 0;
            tilesInPlay--;
            score += gridValues[indexOne];
        }
        else {
            gridValues[indexOne] = gridValues[indexTwo];
            gridValues[indexTwo] = gridValues[indexThree];
            gridValues[indexThree] = gridValues[indexFour];
            gridValues[indexFour] = 0;
        }
    }
    else if ( (gridValues[indexOne] != 0) && (gridValues[indexTwo] != 0) && (gridValues[indexThree] == 0) && (gridValues[indexFour] != 0) ) {
        if (gridValues[indexOne] == gridValues[indexTwo]) {
            gridValues[indexOne] *= 2;
            gridValues[indexTwo] = gridValues[indexFour];
            gridValues[indexFour] = 0;
            tilesInPlay--;
            score += gridValues[indexOne];
        }
        else if (gridValues[indexTwo] == gridValues[indexFour]) {
            gridValues[indexTwo] *= 2;
            gridValues[indexFour] = 0;
            tilesInPlay--;
            score += gridValues[indexTwo];
        }
        else {
            gridValues[indexThree] = gridValues[indexFour];
            gridValues[indexFour] = 0;
        }
    }
    else if ( (gridValues[indexOne] != 0) && (gridValues[indexTwo] == 0) && (gridValues[indexThree] != 0) && (gridValues[indexFour] != 0) ) {
        if (gridValues[indexOne] == gridValues[indexThree]) {
            gridValues[indexOne] *= 2;
            gridValues[indexTwo] = gridValues[indexFour];
            gridValues[indexThree] = gridValues[indexFour] = 0;
            tilesInPlay--;
            score += gridValues[indexOne];
        }
        else if (gridValues[indexThree] == gridValues[indexFour]) {
            gridValues[indexTwo] = gridValues[indexThree] * 2;
            gridValues[indexThree] = gridValues[indexFour] = 0;
            tilesInPlay--;
            score += gridValues[indexTwo];
        }
        else {
            gridValues[indexTwo] = gridValues[indexThree];
            gridValues[indexThree] = gridValues[indexFour];
            gridValues[indexFour] = 0;
        }
    }
    else if ( (gridValues[indexOne] != 0) && (gridValues[indexTwo] != 0) && (gridValues[indexThree] != 0) && (gridValues[indexFour] == 0) ) {
        if (gridValues[indexOne] == gridValues[indexTwo]) {
            gridValues[indexOne] *= 2;
            gridValues[indexTwo] = gridValues[indexThree];
            gridValues[indexThree] = 0;
            tilesInPlay--;
            score += gridValues[indexOne];
        }
        else if (gridValues[indexTwo] == gridValues[indexThree]) {
            gridValues[indexTwo] *= 2;
            gridValues[indexThree] = 0;
            tilesInPlay--;
            score += gridValues[indexTwo];
        }
        else {
            validMove = FALSE;
        }
    }
    
    // Check 4 number cases
    else if ( (gridValues[indexOne] != 0) && (gridValues[indexTwo] != 0) && (gridValues[indexThree] != 0) && (gridValues[indexFour] != 0) ) {
        if ( (gridValues[indexOne] == gridValues[indexTwo]) && (gridValues[indexThree] == gridValues[indexFour]) ) {
            gridValues[indexOne] *= 2;
            gridValues[indexTwo] = gridValues[indexThree] * 2;
            gridValues[indexThree] = 0;
            gridValues[indexFour] = 0;
            tilesInPlay -= 2;
            score += gridValues[indexOne];
            score += gridValues[indexTwo];
        }
        else if (gridValues[indexOne] == gridValues[indexTwo]) {
            gridValues[indexOne] *= 2;
            gridValues[indexTwo] = gridValues[indexThree];
            gridValues[indexThree] = gridValues[indexFour];
            gridValues[indexFour] = 0;
            tilesInPlay--;
            score += gridValues[indexOne];
        }
        else if (gridValues[indexTwo] == gridValues[indexThree]) {
            gridValues[indexTwo] *= 2;
            gridValues[indexThree] = gridValues[indexFour];
            gridValues[indexFour] = 0;
            tilesInPlay--;
            score += gridValues[indexTwo];
        }
        else if (gridValues[indexThree] == gridValues[indexFour]) {
            gridValues[indexThree] *= 2;
            gridValues[indexFour] = 0;
            tilesInPlay--;
            score += gridValues[indexThree];
        }
        else {
            validMove = FALSE;
        }
    }
    return validMove;
}

void generateRandTile() {
    unsigned char gridIndex = rand() % 16; // Generate random index from 0 to 15
    unsigned char newTileNum = (rand() % 10) ? 2 : 4; // New tile will be 4 10% of the time
    
    // Generate new index until empty tile is found
    while(gridValues[gridIndex] != 0) {
        gridIndex = rand() % 16;
    }
    gridValues[gridIndex] = newTileNum;
    tilesInPlay++;
}

void displayGrid() {
    char intToStr[5]; // Buffer to hold gridValues to display as a string on Nokia display
    uint8_t xPos; // Variable for x coordinate of set cursor function. Used to center value based on digits
    
    nokia_lcd_clear();
    drawBoard();
    
    // Write to column 1
    if (gridValues[0] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[0]);
    }
    if (gridValues[0] > 1000) { xPos = 0; } // Center 4 digit value
    else if (gridValues[0] > 100) { xPos = 2; } // Center 3 digit value
    else if (gridValues[0] > 10) { xPos = 5; } // Center 2 digit value
    else { xPos = 8;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,3);
    nokia_lcd_write_string(intToStr, 1);
    
    if (gridValues[4] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';    }
    else {
        sprintf(intToStr, "%hu", gridValues[4]);
    }
    if (gridValues[4] > 1000) { xPos = 0; } // Center 4 digit value
    else if (gridValues[4] > 100) { xPos = 2; } // Center 3 digit value
    else if (gridValues[4] > 10) { xPos = 5; } // Center 2 digit value
    else { xPos = 8;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,15);
    nokia_lcd_write_string(intToStr, 1);
    
    if (gridValues[8] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[8]);
    }
    if (gridValues[8] > 1000) { xPos = 0; } // Center 4 digit value
    else if (gridValues[8] > 100) { xPos = 2; } // Center 3 digit value
    else if (gridValues[8] > 10) { xPos = 5; } // Center 2 digit value
    else { xPos = 8;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,27);
    nokia_lcd_write_string(intToStr, 1);
    
    if (gridValues[12] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[12]);
    }
    if (gridValues[12] > 1000) { xPos = 0; } // Center 4 digit val
    else if (gridValues[12] > 100) { xPos = 2; } // Center 3 digit value
    else if (gridValues[12] > 10) { xPos = 5; } // Center 2 digit value
    else { xPos = 8;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,39);
    nokia_lcd_write_string(intToStr, 1);
    
    // Write to column 2
    if (gridValues[1] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[1]);
    }
    if (gridValues[1] > 1000) { xPos = 21; } // Center 4 digit value
    else if (gridValues[1] > 100) { xPos = 23; } // Center 3 digit value
    else if (gridValues[1] > 10) { xPos = 26; } // Center 2 digit value
    else { xPos = 29;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,3);
    nokia_lcd_write_string(intToStr, 1);
    
    if (gridValues[5] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[5]);
    }
    if (gridValues[5] > 1000) { xPos = 21; } // Center 4 digit value
    else if (gridValues[5] > 100) { xPos = 23; } // Center 3 digit value
    else if (gridValues[5] > 10) { xPos = 26; } // Center 2 digit value
    else { xPos = 29;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,15);
    nokia_lcd_write_string(intToStr, 1);
    
    if (gridValues[9] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[9]);
    }
    if (gridValues[9] > 1000) { xPos = 21; } // Center 4 digit value
    else if (gridValues[9] > 100) { xPos = 23; } // Center 3 digit value
    else if (gridValues[9] > 10) { xPos = 26; } // Center 2 digit value
    else { xPos = 29;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,27);
    nokia_lcd_write_string(intToStr, 1);
    
    if (gridValues[13] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[13]);
    }
    if (gridValues[13] > 1000) { xPos = 21; } // Center 4 digit value
    else if (gridValues[13] > 100) { xPos = 23; } // Center 3 digit value
    else if (gridValues[13] > 10) { xPos = 26; } // Center 2 digit value
    else { xPos = 29;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,39);
    nokia_lcd_write_string(intToStr, 1);
    
    // Write to column 3
    if (gridValues[2] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[2]);
    }
    if (gridValues[2] > 1000) { xPos = 42; } // Center 4 digit value
    else if (gridValues[2] > 100) { xPos = 44; } // Center 3 digit value
    else if (gridValues[2] > 10) { xPos = 47; } // Center 2 digit value
    else { xPos = 50;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,3);
    nokia_lcd_write_string(intToStr, 1);
    
    if (gridValues[6] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[6]);
    }
    if (gridValues[6] > 1000) { xPos = 42; } // Center 4 digit value
    else if (gridValues[6] > 100) { xPos = 44; } // Center 3 digit value
    else if (gridValues[6] > 10) { xPos = 47; } // Center 2 digit value
    else { xPos = 50;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,15);
    nokia_lcd_write_string(intToStr, 1);
    
    if (gridValues[10] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[10]);
    }
    if (gridValues[10] > 1000) { xPos = 42; } // Center 4 digit value
    else if (gridValues[10] > 100) { xPos = 44; } // Center 3 digit value
    else if (gridValues[10] > 10) { xPos = 47; } // Center 2 digit value
    else { xPos = 50;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,27);
    nokia_lcd_write_string(intToStr, 1);
    
    if (gridValues[14] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[14]);
    }
    if (gridValues[14] > 1000) { xPos = 42; } // Center 4 digit value
    else if (gridValues[14] > 100) { xPos = 44; } // Center 3 digit value
    else if (gridValues[14] > 10) { xPos = 47; } // Center 2 digit value
    else { xPos = 50;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,39);
    nokia_lcd_write_string(intToStr, 1);
    
    // Write to column 4
    if (gridValues[3] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[3]);
    }
    if (gridValues[3] > 1000) { xPos = 61; } // Center 4 digit value
    else if (gridValues[3] > 100) { xPos = 65; } // Center 3 digit value
    else if (gridValues[3] > 10) { xPos = 68; } // Center 2 digit value
    else { xPos = 71;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,3);
    nokia_lcd_write_string(intToStr, 1);
    
    if (gridValues[7] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[7]);
    }
    if (gridValues[7] > 1000) { xPos = 61; } // Center 4 digit value
    else if (gridValues[7] > 100) { xPos = 65; } // Center 3 digit value
    else if (gridValues[7] > 10) { xPos = 68; } // Center 2 digit value
    else { xPos = 71;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,15);
    nokia_lcd_write_string(intToStr, 1);
    
    if (gridValues[11] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[11]);
    }
    if (gridValues[11] > 1000) { xPos = 61; } // Center 4 digit value
    else if (gridValues[11] > 100) { xPos = 65; } // Center 3 digit value
    else if (gridValues[11] > 10) { xPos = 68; } // Center 2 digit value
    else { xPos = 71;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,27);
    nokia_lcd_write_string(intToStr, 1);
    
    if (gridValues[15] == 0) {
        intToStr[0] = ' ';
        intToStr[1] = '\0';
    }
    else {
        sprintf(intToStr, "%hu", gridValues[15]);
    }
    if (gridValues[15] > 1000) { xPos = 61; } // Center 4 digit value
    else if (gridValues[15] > 100) { xPos = 65; } // Center 3 digit value
    else if (gridValues[15] > 10) { xPos = 68; } // Center 2 digit value
    else { xPos = 71;} // Center 1 digit value
    nokia_lcd_set_cursor(xPos,39);
    nokia_lcd_write_string(intToStr, 1);
    
    nokia_lcd_render();
}

void displayGameOver() {
    nokia_lcd_clear();
    
    nokia_lcd_set_cursor(16, 1);
    nokia_lcd_write_string("GAME OVER!", 1);
    
    nokia_lcd_set_cursor(0, 21);
    nokia_lcd_write_string("Press joystick down to play ", 1);
    
    nokia_lcd_set_cursor(26, 38);
    nokia_lcd_write_string("again", 1);
    
    nokia_lcd_render();
}

void displayGameWon() {
    nokia_lcd_clear();
    
    nokia_lcd_set_cursor(21, 1);
    nokia_lcd_write_string("YOU WON!", 1);
    
    nokia_lcd_set_cursor(0, 21);
    nokia_lcd_write_string("Press joystick down to keep ", 1);
    
    nokia_lcd_set_cursor(23, 38);
    nokia_lcd_write_string("playing", 1);
    
    nokia_lcd_render();
}

void displayMenu() {
    nokia_lcd_clear();
    
    nokia_lcd_set_cursor(21, 1);
    nokia_lcd_write_string("2048", 2);
    
    nokia_lcd_set_cursor(0, 21);
    nokia_lcd_write_string("Press Joystick Down to Play", 1);
    
    nokia_lcd_render();
}

int gameOver() {
    unsigned char noAdjTiles = 0x01; // Boolean value: 1 when no adjacent tiles are equal
    unsigned char endGame = 0x00; // Return value: 1 if there are no more possible moves
    
    // Loop through first 3 Rows on grid
    for (int i = 0; i < 12; i++) {
        if (gridValues[i] == gridValues[i + 4]) { // Check below the tile at i
            noAdjTiles = 0x00; // False
            break;
        }
    }
    
    // Loop through first 3 Columns on grid
    for (int j = 0; j < 15; j++) {
        if ( (j == 3) || (j == 7) || (j == 11) ) {
            ++j;
        }
        if (gridValues[j] == gridValues[j + 1]) { // Check to right of tile at j
            noAdjTiles = 0x00; //False
            break;
        }
    }
    
    if ( (tilesInPlay == 16) && (noAdjTiles) ) {
        endGame = 0x01; // End the game
    }
    else {
        endGame = 0x00; // Continue game
    }
    
    return endGame;
}

int gameWon() {
    unsigned char won = 0x00;
    
    for (int i = 0; i < 16; i++) {
        if (gridValues[i] == winningNum) {
            won = 0x01;
            winningNum *= 2; // Double winning number so game can continue
            break;
        }
    }
    
    return won;
}

void resetGrid() {
    // Set random grid value to 2 and the rest 0
    unsigned char firstValue = rand() % 16;
    for (int x = 0; x < 16; ++x) {
        if (x == firstValue) { gridValues[x] = 2; }
        else { gridValues[x] = 0; }
    }
    tilesInPlay = 1;
    score = 0;
    winningNum = 2048;
    LCD_ClearScreen();
    LCD_DisplayString(1, "Score:");
    LCD_DisplayString(17, "High Score:");
}

void cheatMode() {
    gridValues[0] = gridValues[1] = gridValues[2] = gridValues[3] = 1024;
    for (int i = 4; i < 16; ++i) {
        gridValues[i] = 0;
    }
    tilesInPlay = 4;
}

void writeEEPROM(uint16_t newScore) {
    eeprom_update_word((uint16_t*)0, newScore);
    highscore = newScore;
}

void resetHighscore() {
    eeprom_update_word((uint16_t*)0, 0);
    highscore = 0;
    LCD_ClearScreen();
    LCD_DisplayString(17, "High Score:");
}
