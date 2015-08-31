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

/**
 * @brief init lcd, 4 bit mode, increase counter.
 * 
 * @return false if lcd_pins is null, true otherwise.
 */
bool lcd_init(void);

/**
 * @brief set position of cursor.
 *
 * @param[in] line, should be between 1 and 4.
 * @param[in] pos, should be between 1 and 20.
 *
 * @return success=true or failed=false.
 */bool lcd_set_cursor(uint8_t line, uint8_t pos);

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
void lcd_clear(void);

/**
 * @brief Set cursor to 1-1. (no clearing)
 */
void lcd_return_home(void);

/**
 * @brief Set cursor to 1-1 without shifting. (no clearing)
 */
void lcd_return_home_wo_shifting(void);

/**
 * @brief store a custom code generated pattern to CGRAM.
 *
 * @param[in] pattern, an 8-elements array.
 * @param[in] addr, DDRAM address for generated pattern; should be between 0 and 7.
 * (CGRAM address = addr << 3).
 */
void lcd_gen_pattern(const uint8_t *pattern, uint8_t addr);

#endif /* LCD_H_ */
