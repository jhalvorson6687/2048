/*
 * Author : Jacob Halvorson
 * CS120B Final Project: Embedded Systems version of the popular mobile game "2048"
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
#include "nokia5110.h"

void ADC_init() {
    ADMUX = (1 << REFS0);
    
    // ADC Enable and prescalar 128
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
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
    nokia_lcd_render();
}

int main(void) {
    //Initialize I/O
    DDRA = 0x00; PORTA = 0xFF; // Init PORTA as Input
    DDRB = 0xFF; PORTB = 0x00; // Init PORTB as Output
    
    // Declare an array of tasks
    static task taskJS, taskManageGrid, taskNokiaDisp;
    task *tasks[] = {&taskJS, &taskManageGrid, &taskNokiaDisp};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    unsigned long int gcd = 0;
    
    // Task 1 (Joystick Input)
    taskJS.state = WAIT;
    taskJS.period = 50;
    taskJS.elapsedTime = taskJS.period;
    taskJS.tick = &JSTick;
    
    // Task 2 (Manage Grid)
    taskManageGrid.state = WAIT_POS;
    taskManageGrid.period = 50;
    taskManageGrid.elapsedTime = taskManageGrid.period;
    taskManageGrid.tick = &manageGrid;
    
    // Task 3 (Nokia Display)
    taskNokiaDisp.state = WAIT;
    taskNokiaDisp.period = 50;
    taskNokiaDisp.elapsedTime = taskNokiaDisp.period;
    taskNokiaDisp.tick = &nokiaDisplayTick;
    
    // Find GCD of the tasks periods
    gcd = findGCD(taskJS.period, taskNokiaDisp.period);
    
    // Set the timer to the GCD
    TimerSet(gcd);
    TimerOn();
    
    // Initialize ADC
    ADC_init();
    
    // Initialize Nokia Display
    nokia_lcd_init();
    
    drawBoard();
    
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

