/*
 * lcd_dev.h
 *
 *  Created on: Jul 3, 2015
 *      Author: nvhie_000
 */

#ifndef LCD_DEV_H_
#define LCD_DEV_H_

#include "LCD.h"

lcd_pins_t lcd_pins = { { LCD_RS_SetVal, LCD_RS_ClrVal, }, //lcd_rs
		{ LCD_EN_SetVal, LCD_EN_ClrVal, }, //lcd_en
		{ LCD_BL_SetVal, LCD_BL_ClrVal, }, //lcd_bl
		{ LCD_DB4_SetVal, LCD_DB4_ClrVal, }, //lcd_db4
		{ LCD_DB5_SetVal, LCD_DB5_ClrVal, }, //lcd_db5
		{ LCD_DB6_SetVal, LCD_DB6_ClrVal, }, //lcd_db6
		{ LCD_DB7_SetVal, LCD_DB7_ClrVal, }, //lcd_db7
		};

#endif /* LCD_DEV_H_ */
