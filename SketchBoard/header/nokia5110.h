/* Nokia 5110 LCD AVR Library
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

#ifndef __NOKIA_5110_H__
#define __NOKIA_5110_H__

#include <avr/pgmspace.h>
#include <stdint.h>

/*
 * LCD's port
 */
#define PORT_LCD PORTB
#define DDR_LCD DDRB

/*
 * LCD's pins
 */
#define LCD_SCE PB2
#define LCD_RST PB4
#define LCD_DC PB0
#define LCD_DIN PB5
#define LCD_CLK PB7

#define LCD_CONTRAST 0x40

/*
 * Must be called once before any other function, initializes display
 */
void nokia_lcd_init(void);

/*
 * Clear screen
 */
void nokia_lcd_clear(void);

/**
 * Power of display
 * @lcd: lcd nokia struct
 * @on: 1 - on; 0 - off;
 */
void nokia_lcd_power(uint8_t on);

/**
 * Set single pixel
 * @x: horizontal pozition
 * @y: vertical position
 * @value: show/hide pixel
 */
void nokia_lcd_set_pixel(uint8_t x, uint8_t y, uint8_t value);

/**
 * Draw single char with 1-6 scale
 * @code: char code
 * @scale: size of char
 */
void nokia_lcd_write_char(char code, uint8_t scale);



/**
 * Draw string. Example: writeString("abc",3);
 * @str: sending string
 * @scale: size of text
 */
void nokia_lcd_write_string(const char *str, uint8_t scale);

/**
 * Set cursor position
 * @x: horizontal position
 * @y: vertical position
 */
void nokia_lcd_set_cursor(uint8_t x, uint8_t y);

/*
 * Render screen to display
 */
void nokia_lcd_render(void);
/*
*	Check if there exists a pixel at the location and return that
*/
int nokia_lcd_get_pixel(uint8_t x, uint8_t y);

/*
*Used to print the save files from EEPROM S1, S2, S3, S4. Use char to define which save 1, 2, 3, 4
*/
void nokia_printsave(unsigned char);


/*
*Used to save files in EEPROM S1, S2, S3, S4. Use char to define which save 1, 2, 3, 4
*/
void nokia_lcd_save(unsigned char);

#endif
