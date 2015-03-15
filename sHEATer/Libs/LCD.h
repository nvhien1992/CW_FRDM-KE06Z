/*
 * LCD.h
 *
 *  Created on: Feb 2, 2015
 *      Author: nvhie_000
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief init lcd, 4 bit mode, increase counter.
 */
void init_lcd(void);

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
