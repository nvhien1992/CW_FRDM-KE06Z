/*
 * LCD.h
 *
 *  Created on: Feb 2, 2015
 *      Author: Nguyen Van Hien <nvhien1992@gmail.com>
 *  How to use:
 *  _All pins connecting to LCD must be initialized auto-ly.
 *  _Declare a var had lcd_pins_t type.
 *  _Assign all functions pointer of var with functions respectively.
 *  _Call init_lcd function to initialize.
 */

#ifndef LCD_H_
#define LCD_H_

#include "PE_Types.h"

typedef struct {
	void (*SetVal)(LDD_TDeviceData *arg);
	void (*ClrVal)(LDD_TDeviceData *arg);
} out_pin_t;

typedef struct {
	void (*SetInput)(LDD_TDeviceData *arg);
	void (*SetOutput)(LDD_TDeviceData *arg);
	void (*SetVal)(LDD_TDeviceData *arg);
	void (*ClrVal)(LDD_TDeviceData *arg);
	bool (*GetVal)(LDD_TDeviceData *arg);
} status_pin_t;

typedef struct {
	out_pin_t lcd_rs;
	out_pin_t lcd_en;
	out_pin_t lcd_bl;
	out_pin_t lcd_db4;
	out_pin_t lcd_db5;
	out_pin_t lcd_db6;
	status_pin_t lcd_db7;
} lcd_pins_t;

/**
 * @brief init lcd, 4 bit mode, increase counter.
 * 
 * @return false if lcd_pins is null, true otherwise.
 */
bool init_lcd(lcd_pins_t *lcd_pins);

/**
 * @brief set position of cursor.
 *
 * @param[in] line, should be between 1 and 4.
 * @param[in] pos, should be between 1 and 20.
 *
 * @return success=true or failed=false.
 */bool set_cursor(uint8_t line, uint8_t pos);

/**
 * @brief print a character to LCD.
 *
 * @param[in] a_char, a character.
 */
void lcd_putc(int8_t a_char);

/**
 * @brief print a string to LCD.
 *
 * @param[in] a_string, pointer to a string of character.
 *
 * @return number of character has been print to LCD.
 */
int16_t lcd_puts(int8_t *a_string);

/**
 * @brief lcd's printf (only for 20x4-1 characters)
 *
 * @param[in] format, in printf's style.
 *
 * @return number of characters has been printed, negative value if error.
 */
int16_t lcd_printf(const char *format, ...);

/**
 * @brief clear screen and set cursor to 1-1.
 */
void clear(void);

/**
 * @brief Set cursor to 1-1. (no clearing)
 */
void return_home(void);

/**
 * @brief Set cursor to 1-1 without shifting. (no clearing)
 */
void return_home_wo_shifting(void);

/**
 * @brief store a custom code generated pattern to CGRAM.
 *
 * @param[in] pattern, an 8-elements array.
 * @param[in] addr, DDRAM address for generated pattern; should be between 0 and 7.
 * (CGRAM address = addr << 3).
 */
void gen_pattern(const uint8_t *pattern, uint8_t addr);

#endif /* LCD_H_ */
