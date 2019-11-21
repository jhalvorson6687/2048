/*
 * Implementation of of the State Machines tick functions
 */

#include <avr/io.h>
#include "SM_ticks.h"
#include <stdio.h>

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
			state = WAIT;
			break;
			
		case DOWN:
			state = WAIT;
			break;
		
		case LEFT: 
			state = WAIT;
			break;
			
		case RIGHT:
			state = WAIT;
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
	PORTB = position;
	return state;
}

// TODO: Implement manageGrid, updateScore, and Display tasks
