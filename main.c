/*
 * Author : Jacob Halvorson
 * CS120B Final Project: Embedded Systems version of the popular mobile game "2048"
 * Written for ATmega1284 
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "io.h"
#include "timer.h"
#include "bit.h"
#include "keypad.h"
#include "scheduler.h"
#include "SM_ticks.h"

void ADC_init() {
	ADMUX = (1 << REFS0);
	
	// ADC Enable and prescalar 128
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

int main(void) {
	//Initialize I/O
	DDRA = 0x00; PORTA = 0xFF; // Init PORTA as Input
	DDRB = 0xFF; PORTB = 0x00; // Init PORTB as Output
    
	// Declare an array of tasks
	static task taskJS;
	task *tasks[] = {&taskJS};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	unsigned long int gcd = 0;
	
	// Task 1 (Joystick Input)
	taskJS.state = WAIT;
	taskJS.period = 100;
	taskJS.elapsedTime = taskJS.period;
	taskJS.tick = &JSTick;
	
	// Find GCD of the tasks periods
	//gcd = findGCD(taskJS.period);
	gcd = 100;
	// Set the timer to the GCD
	TimerSet(gcd);
	TimerOn();
	
	ADC_init();
	
	unsigned short i; // Scheduler for-loop iterator
    while (1) {
	    for (i = 0; i < numTasks; ++i) {
		    if (tasks[i]->elapsedTime == tasks[i]->period) {
			    tasks[i]->state = tasks[i]->tick(tasks[i]->state); // set next state
			    tasks[i]->elapsedTime = 0; // reset elapsedTime
		    }
		    tasks[i]->elapsedTime += gcd;
	    }
	    while(!TimerFlag);
	    TimerFlag = 0;
    }
    return 0; // Error: Program should not exit
}

