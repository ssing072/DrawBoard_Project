/* Nokia 5110 LCD AVR Library example
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by SkewPL, http://skew.tk
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "ADC.h"
#include "nokia5110.h"
#include "timer.h"
#include "io.h"
#include <avr/eeprom.h>

#define A4 (~PINA & 0x08)
#define A5 (~PINA & 0x10)

unsigned char Sketching = 0;
unsigned char MenuOn = 0;
unsigned char getX = 0;
unsigned char getY = 0;
unsigned char saveFile = 0;

typedef struct _task {
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct)(int);
} task;

static task task1, task2, task3, task4;
task *tasks[] = { &task1, &task2, &task3, &task4};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);


unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a % b;
		if( c == 0 ) { return b; }
		a = b;
		b = c;
	}
	return 0;
}

unsigned char x = 42;
unsigned char y = 24;
unsigned char x_prev = 42;
unsigned char y_prev = 24;
unsigned char draw = 0;
unsigned short x_adc;
unsigned short y_adc;
unsigned short b_adc;

enum X_States{xWait};
	
int X_draw(int state){
	if(getY == 1){
		x_adc = get_ADC(0);
		switch(state){
			case xWait:
			state = xWait;
			break;
		}
		switch(state){
			case xWait:
			if(x_adc < 400){
				if(y > 0){
					y--;
				}
			}
			else if(x_adc > 700){
				if(y < 47){
					y++;
				}
			}
			break;
		}
	}
	return state;
}

enum Y_States{yWait};

int Y_draw(int state){
	if(getY == 1){
		y_adc = get_ADC(1);
		switch(state){
			case xWait:
			state = xWait;
			break;
		}
		switch(state){
			case xWait:
			if(y_adc < 400){
				if(x < 83){
					x++;
				}
			}
			else if(y_adc > 700){
				if(x > 0){
					x--;
				}
			}
			break;
		}
	}
	return state;
	
}

void clearSelect(){
	LCD_WriteCommand(0x80 + 1 - 1);
	LCD_WriteData(' ');
	LCD_WriteCommand(0x80 + 7 - 1);
	LCD_WriteData(' ');
	LCD_WriteCommand(0x80 + 13 - 1);
	LCD_WriteData(' ');
	LCD_WriteCommand(0xB8 + 19 - 9);
	LCD_WriteData(' ');
	LCD_WriteCommand(0xB8 + 26 - 9);
	LCD_WriteData(' ');
}

void clearSaves(){
	LCD_WriteCommand(0x80 + 1 - 1);
	LCD_WriteData(' ');
	LCD_WriteCommand(0x80 + 4 - 1);
	LCD_WriteData(' ');
	LCD_WriteCommand(0x80 + 7 - 1);
	LCD_WriteData(' ');
	LCD_WriteCommand(0x80 + 10 - 1);
	LCD_WriteData(' ');
	LCD_WriteCommand(0xB8 + 17 - 9);
	LCD_WriteData(' ');
}


enum Draw_States{Move, Draw, Delete, Save, Exit, ReleaseButton, Saving, Save1, Save2, Save3, Save4, None, Back};
unsigned char isPixel = 0;
unsigned char prev_state;
unsigned char prev_save;
int Sketch(int state){
	if(Sketching == 1){
		b_adc = get_ADC(2);
		switch(state){
			case Move:
				if(A4){
					state = ReleaseButton;
				}
				else{
					state = Move;
				}
			break;
			case Draw:
				if(A4){
					state = ReleaseButton;
				}
				else{
					state = Draw;
				}
			break;
			case Delete:
				if(A4){
					state = ReleaseButton;
				}
				else{
					state = Delete;
				}
			break;
			case ReleaseButton:
				if(A4){
					state = ReleaseButton;
				}
				else{
					if(prev_state == Move){
						state = Draw;
					}
					else if(prev_state == Draw){
						state = Delete;
					}
					else if(prev_state == Delete){
						state = Save;
					}
					else if(prev_state == Save){
						state = Exit;
					}
					else if(prev_state == Exit){
						state = Move;
					}
				}
			break;
			case Save:
				if(A5){
					state = Saving;
					MenuOn = 0;
					getX = 0;
					getY = 0;
					Sketching = 1;
					LCD_ClearScreen();
					LCD_DisplayString(1, " S1 S2 S3 S4     Back");
				}
				else if(A4){
					state = ReleaseButton;
				}
				else{
					state = Save;
				}
			break;
			case Exit:
				if(A5){
					state = Move;
					LCD_ClearScreen();
					LCD_DisplayString(1, "Exiting...");
					Sketching = 0;
					MenuOn = 1;
					delay_ms(3000);
				}
				else if(A4){
					state = ReleaseButton;
				}
				else{
					state = Exit;
				}
			break;
			case Saving:
			if(A4){
				state = Saving;
			}
			else{
				if(prev_save == Save1){
					state = Save2;
				}
				else if(prev_save == Save2){
					state = Save3;
				}
				else if(prev_save == Save3){
					state = Save4;
				}
				else if(prev_save == Save4){
					state = None;
				}
				else if(prev_save == None){
					state = Save1;
				}
			}
			break;
			case Save1:
			if(A5){
				nokia_lcd_save(1);

			} 
			else if(A4){
				state = Saving;
			}
			else{
				state = Save1;
			}
			break;
			case Save2:
			if(A5){
				nokia_lcd_save(2);
			}
			else if(A4){
				state = Saving;
			}
			else{
				state = Save2;
			}
			break;
			case Save3:
			if(A5){
				nokia_lcd_save(3);
			}
			else if(A4){
				state = Saving;
			}
			else{
				state = Save3;
			}
			break;
			case Save4:
			if(A5){
				nokia_lcd_save(4);
			}
			else if(A4){
				state = Saving;
			}
			else{
				state = Save4;
			}
			break;
			case None:
			if(A5){
				state = Back;
			}
			else if(A4){
				state = Saving;
			}
			else{
				state = None;
			}
			break;
			case Back:
			if(A5){
				state = Back;
			}
			else{
				LCD_ClearScreen();
				LCD_DisplayString(1," Move  Draw  Del   Save   Exit");
				MenuOn = 0;
				getX = 1;
				getY = 1;
				Sketching = 1;
				state = Move;
				
			}
			break;
		}
		switch(state){
			case Move:
				getX = 1;
				getY = 1;
				prev_state = Move;
				nokia_lcd_set_pixel(x_prev,y_prev, isPixel);
				isPixel = nokia_lcd_get_pixel(x, y);
				nokia_lcd_set_pixel(x,y,1);
				nokia_lcd_render();
				x_prev = x;
				y_prev = y;
				clearSelect();
				LCD_WriteCommand(0x80 + 1 - 1);
				LCD_WriteData(0);
			break;
			case Draw:
				prev_state = Draw;
				nokia_lcd_set_pixel(x,y,1);
				nokia_lcd_render();
				clearSelect();
				LCD_WriteCommand(0x80 + 7 - 1);
				LCD_WriteData(0);
				x_prev = x;
				y_prev = y;
			break;
			case Delete:
				prev_state = Delete;
				//LCD_DisplayString(1,"Delete");
				clearSelect();
				LCD_WriteCommand(0x80 + 13 - 1);
				LCD_WriteData(0);
				nokia_lcd_set_pixel(x_prev,y_prev,0);
				nokia_lcd_set_pixel(x,y,1);
				nokia_lcd_render();
				x_prev = x;
				y_prev = y;
			break;
			case Save:
				getX = 0;
				prev_save = None;
				getY = 0;
				prev_state = Save;
				clearSelect();
				LCD_WriteCommand(0xB8 + 19 - 9);
				LCD_WriteData(0);
			break;
			case Exit:
				prev_state = Exit;
				clearSelect();
				LCD_WriteCommand(0xB8 + 26 - 9);
				LCD_WriteData(0);
			break;
			case ReleaseButton:
			break;
			case Saving:
			break;
			case Save1:
			prev_save = Save1;
			clearSaves();
			LCD_WriteCommand(0x80 + 1 - 1);
			LCD_WriteData(0);
			
			break;
			case Save2:
			prev_save = Save2;
			clearSaves();
			LCD_WriteCommand(0x80 + 4 - 1);
			LCD_WriteData(0);
			
			break;
			case Save3:
			prev_save = Save3;
			clearSaves();
			LCD_WriteCommand(0x80 + 7 - 1);
			LCD_WriteData(0);
			break;
			case Save4:
			prev_save = Save4;
			clearSaves();
			LCD_WriteCommand(0x80 + 10 - 1);
			LCD_WriteData(0);
			break;
			case None:
			prev_save = None;
			clearSaves();
			LCD_WriteCommand(0xB8 + 17 - 9);
			LCD_WriteData(0);
			break;
		}
	}
	return state;
}





enum menuStates {mainMenu, SketchState, View, WaitSketch, WaitView, View1, View2, View3, View4, Leave, Viewing, ReleasePressed};
unsigned char prev_leave;
unsigned char count2 = 0;
int Menu(int state){
	if(MenuOn == 1){
		switch(state){
			case mainMenu:
			if(count2 < 2){
				state = mainMenu;
			}
			else{
				state = SketchState;
			}
			break;
			case SketchState:
			if(A4){
				state = WaitView;
			}
			else{
				state = SketchState;
			}
			break;
			case View:
			if(A5){
				LCD_ClearScreen();
				LCD_DisplayString(1, " S1 S2 S3 S4     Exit");
				state = View1;
			}
			else if(A4){
				state = WaitSketch;
			}
			else{
				state = View;
			}
			break;
			case WaitSketch:
			if(A4){
				state = WaitSketch;
			}
			else{
				state = SketchState;
			}
			break;
			case WaitView:
			if(A4){
				state = WaitView;
			}
			else{
				state = View;
			}
			break;
			case View1:
			if(A4){
				state = Viewing;
			}
			else{
				state = View1;
			}
			break;
			case View2:
			if(A4){
				state = Viewing;
			}
			else{
				state = View2;
			}
			break;
			case View3:
			if(A4){
				state = Viewing;
			}
			else{
				state = View3;
			}
			break;
			case View4:
			if(A4){
				state = Viewing;
			}
			else{
				state = View4;
			}
			break;
			case Leave:
			if(A5){
				state = ReleasePressed;
			}
			else if(A4){
				state = Viewing;
			}
			else{
				state = Leave;
			}
			break;
			case Viewing:
			if(A4){
				state = Viewing;
			}
			else{
				if(prev_leave == View1){
					state = View2;
				}
				else if(prev_leave == View2){
					state = View3;
				}
				else if(prev_leave == View3){
					state = View4;
				}
				else if(prev_leave == View4){
					state = Leave;
				}
				else if(prev_leave == Leave){
					state = View1;
				}
			}
			break;
			case ReleasePressed:
			if(A5){
				state = ReleasePressed;
			}
			else{
				state = mainMenu;
			}
			break;
			
			
		}
		switch(state){
			case mainMenu:
			LCD_ClearScreen();
			LCD_DisplayString(1, "     Sketch           View");
			nokia_lcd_clear();
			nokia_lcd_render();
			count2++;
			break;
			case SketchState:
			Sketching = 0;
			getX = 0;
			getY = 0;
			saveFile = 0;
			nokia_lcd_set_cursor(10,15);
			nokia_lcd_write_string("SketchBoard", 1);
			nokia_lcd_render();
			LCD_WriteCommand(0xB8 + 22 - 9);
			LCD_WriteData(' ');
			LCD_WriteCommand(0x80 + 4);
			LCD_WriteData(0);
			count2 = 0;
			if(A5){
				Sketching = 1;
				getX = 1;
				getY = 1;
				MenuOn = 0;
				saveFile = 0;
				nokia_lcd_clear();
				LCD_DisplayString(1," Move  Draw  Del   Save   Exit");
				state = mainMenu;
			}
			break;
			case View:
			LCD_WriteCommand(0x80 + 4);
			LCD_WriteData(' ');
			LCD_WriteCommand(0xB8 + 22 - 9);
			LCD_WriteData(0);
			break;
			case WaitSketch:
			break;
			case WaitView:
			break;
			case View1:
			prev_leave = View1;
			clearSaves();
			LCD_WriteCommand(0x80 + 1 - 1);
			LCD_WriteData(0);
			if(A5){
				nokia_printsave(1);
				nokia_lcd_render();
			}
			break;
			case View2:
			prev_leave = View2;
			clearSaves();
			LCD_WriteCommand(0x80 + 4 - 1);
			LCD_WriteData(0);
			if(A5){
				nokia_printsave(2);
			}
			break;
			case View3:
			prev_leave = View3;
			clearSaves();
			LCD_WriteCommand(0x80 + 7 - 1);
			LCD_WriteData(0);
			if(A5){
				nokia_printsave(3);
			}
			break;
			case View4:
			prev_leave = View4;
			clearSaves();
			LCD_WriteCommand(0x80 + 10 - 1);
			LCD_WriteData(0);
			if(A5){
				nokia_printsave(4);
			}
			break;
			case Leave:
			prev_leave = Leave;
			clearSaves();
			LCD_WriteCommand(0xB8 + 17 - 9);
			LCD_WriteData(0);
			break;
			case ReleasePressed:
			break;
			
		}
	}	
	return state;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	

	
	task1.state = mainMenu;
	task1.period = 150;
	task1.elapsedTime = task1.period;
	task1.TickFct = &Menu;
	
	task2.state = xWait;
	task2.period = 200;
	task2.elapsedTime = task2.period;
	task2.TickFct = &X_draw;
	
	task3.state = yWait;
	task3.period = 200;
	task3.elapsedTime = task3.period;
	task3.TickFct = &Y_draw;
	
	task4.state = Move;
	task4.period = 150;
	task4.elapsedTime = task4.period;
	task4.TickFct = &Sketch;
	
	
	
	unsigned char Select[8] = { 0x08, 0x0C, 0x0E, 0x0F, 0x0E, 0x0C, 0x08, 0x00}; 

	LCD_init();
	LCD_Custom_Char(0, Select);
	LCD_WriteCommand(0x0C);
	
    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_render();
	
	ADC_Init();
	
	unsigned short i;
	unsigned long GCD = tasks[0]->period;
	for(i=1;i<numTasks; i++){
		GCD = findGCD(GCD, tasks[i]->period);
	}
	
	TimerSet(GCD);
	TimerOn();
	MenuOn = 1;
	Sketching = 0;
	getX = 0;
	getY = 0;
	saveFile = 0;
	while(1){
		for (i = 0; i < numTasks; i++){
			if((tasks[i]->elapsedTime == tasks[i]->period)){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += GCD;//GCD
		}
		while(!TimerFlag){};
		TimerFlag = 0;
	}
}