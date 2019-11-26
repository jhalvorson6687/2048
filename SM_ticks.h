/*
 * Declarations of State Machine tick functions, states, and shared variables
 */

#ifndef SM_TICKS
#define SM_TICKS

//-----------Shared Variables-----------
unsigned char position; // position the joystick is moved in. (Up, Down, L, R)
uint8_t vertPos; // TODO: Replace with grid shared variable used for nokia display
uint8_t horiPos;
//---------End Shared Variables---------

//---------------SM States--------------------
enum JS_States {WAIT, UP, DOWN, LEFT, RIGHT};
enum manGrid_States {WAIT_POS, CHECK_U, CHECK_D, CHECK_L, CHECK_R};
//-------------End SM States------------------

//------------Tick function declarations---------------
int JSTick(int state); // Joystick input tick function
int nokiaDisplayTick(int state); // Nokia display tick function
int manageGrid(int state); // Manage grid tick function
//----------End Tick function declarations-------------

#endif // SM_TICKS
