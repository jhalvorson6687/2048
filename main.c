/*
 * Author : Jacob Halvorson
 * CS120B Final Project: Embedded Systems version of the popular mobile game "2048"
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "io.h"
#include "timer.h"
#include "scheduler.h"
#include "SM_ticks.h"
#include "nokia5110.h"

void ADC_init() {
    ADMUX = (1 << REFS0);
    
    // ADC Enable and prescalar 128
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

int main(void) {
    //Initialize I/O
    DDRA = 0x00; PORTA = 0xFF; // Initialize PORTA as Input
    DDRB = 0xFF; PORTB = 0x00; // Initialize PORTB as Output
    DDRC = 0xFF; PORTC = 0x00; // Initialize PORTC as Output
    DDRD = 0xFF; PORTD = 0x00; // Initialize PORTD as Output
    
    // Declare an array of tasks
    static task taskJS, taskManageGrid, taskNokiaDisp, taskLcdScreen;
    task *tasks[] = {&taskJS, &taskManageGrid, &taskNokiaDisp, &taskLcdScreen};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    // Task 1 (Joystick Input)
    taskJS.state = WAIT;
    taskJS.period = 100;
    taskJS.elapsedTime = taskJS.period;
    taskJS.tick = &jsTick;
    
    // Task 2 (Manage Grid)
    taskManageGrid.state = WAIT_START;
    taskManageGrid.period = 100;
    taskManageGrid.elapsedTime = taskManageGrid.period;
    taskManageGrid.tick = &manageGridTick;
    
    // Task 3 (Nokia Display)
    taskNokiaDisp.state = MENU;
    taskNokiaDisp.period = 100;
    taskNokiaDisp.elapsedTime = taskNokiaDisp.period;
    taskNokiaDisp.tick = &nokiaDisplayTick;
    
    // Task 4 (LCD Screen)
    taskLcdScreen.state = MENU;
    taskLcdScreen.period = 100;
    taskLcdScreen.elapsedTime = taskLcdScreen.period;
    taskLcdScreen.tick = &lcdScreenTick;
    
    // Find GCD of the tasks periods
    unsigned long int gcd = findGCD(taskJS.period, taskNokiaDisp.period);
    
    // Set the timer to the GCD
    TimerSet(gcd);
    TimerOn();
    
    // Initialize ADC
    ADC_init();
    
    // Initialize Nokia Display
    nokia_lcd_init();
    
    // Initialize LCD Screen
    LCD_init();
    
    // Set winning number to 2048 to start
    winningNum = 2048;
    
    // Set grid to start
    resetGrid();
    
    score = 0;
    highscore = eeprom_read_word((uint16_t*)0);
    
    srand((int)time(0));
    
    unsigned short i; // Scheduler for-loop iterator
    while (1) {
        if  (~PINA & 0x10) { resetHighscore(); }
        for (i = 0; i < numTasks; ++i) {
            if (tasks[i]->elapsedTime == tasks[i]->period) {
                tasks[i]->state = tasks[i]->tick(tasks[i]->state); // set next state
                tasks[i]->elapsedTime = 0; // reset elapsedTime
            }
            tasks[i]->elapsedTime += gcd;
        }
        while(!TimerFlag); // Wait GCD ms
        TimerFlag = 0;
    }
    return 0; // Error: Program should not exit
}

