/*
 * Declarations of State Machine tick functions, states, helper functions, and shared variables
 */

#ifndef SM_TICKS
#define SM_TICKS

#include <avr/eeprom.h>

//-----------Shared Variables-----------
unsigned long winningNum; // Goal to reach
unsigned char position; // Position the joystick is moved in. (Up, Down, Left, Right)
unsigned short gridValues[16]; // Holds values for each position on the grid
unsigned char tilesInPlay; // Keep track of number of tiles in play
uint16_t score; // Score of the current game
uint16_t highscore; // Highest scored achieved stored in EEPROM
//---------End Shared Variables---------

//---------------SM States--------------------
enum JS_States {WAIT, UP, DOWN, LEFT, RIGHT};
enum manGrid_States {WAIT_START, WAIT_POS, CHECK_U, CHECK_D, CHECK_L, CHECK_R, RELEASE, CHEAT};
enum nokiaDisp_States {MENU, GRID, GAME_OVER, GAME_WON, RESET};
enum isValidMove {FALSE, TRUE} validMove;
//-------------End SM States------------------

//------------Tick function declarations---------------
int jsTick(int state);
int nokiaDisplayTick(int state);
int manageGridTick(int state);
int lcdScreenTick(int state);
//----------End Tick function declarations-------------

//------------Helper Functions--------------
void drawBoard();
unsigned short readADC(uint8_t channel);
int moveTiles(unsigned short indexOne, unsigned short indexTwo, unsigned short indexThree, unsigned short indexFour);
void generateRandTile();
void displayGrid();
void displayGameOver();
void displayGameWon();
void displayMenu();
int gameOver();
int gameWon();
void resetGrid();
void cheatMode();
void writeEEPROM(uint16_t newScore);
void resetHighscore();
//----------End Helper Functions------------

#endif // SM_TICKS

