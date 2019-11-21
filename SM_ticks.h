/*
 * Declarations of State Machine tick functions, states, and shared variables
 */

#ifndef SM_TICKS
#define SM_TICKS

//-----------Shared Variables-----------
unsigned char position; // position the joystick is moved in. (Up, Down, L, R)
//---------End Shared Variables---------

//---------------SM States--------------------
enum JS_States {WAIT, UP, DOWN, LEFT, RIGHT};
//-------------End SM States------------------

//------------Tick function declarations---------------
int JSTick(int state); // Joystick input tick function
//----------End Tick function declarations-------------

#endif // SM_TICKS
