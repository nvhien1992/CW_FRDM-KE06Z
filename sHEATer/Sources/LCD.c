/*
 * LCD.c
 *
 *  Created on: Feb 2, 2015
 *      Author: nvhie_000
 */
#include <stdarg.h>
#include <cstdio>
#include "LCD.h"
#include "LCD_RS.h"
//#include "LCD_RW.h"
#include "LCD_EN.h"
#include "LCD_BL.h"
#include "LCD_DB4.h"
#include "LCD_DB5.h"
#include "LCD_DB6.h"
#include "LCD_DB7.h"

#define LCD20x4
//#define LCD16x2

#ifdef LCD20x4
#define MAX_CHAR_IN_LINE 20
#define MAX_LINE 4
#elif LCD16x2
#define MAX_CHAR_IN_LINE 16
#define MAX_LINE 2
#endif

/*--------------------- LCD-specific data, definitions -----------------------*/
/* define cLCD's pin state */
enum RS_state_e {
	cmd = 0, data = 1
};
enum RW_state_e {
	write = 0, read = 1
};
/* end define pin state */

/** define cLCD's instructions **/
enum instruction_e {
	/* clear Display :
	 * Write 20H to DDRAM and set DDRAM address to 00h from AC.
	 * RS = 0, RW = 0.
	 * 1.5 ms.
	 */
	cLCD_cmd_ClearDisplay = 0x01,
	/* return Home :
	 * set DDRAM address to 00h from AC, return cursor to it's original location if shifted (contents of DDRAM not changed).
	 * RS = 0, RW = 0.
	 * 1.5 ms.
	 */
	cLCD_cmd_ReturnHome = 0x02,
	/* Entry mode set :
	 * Set cursor move direction and specifies display shift.
	 * RS = 0, RW = 0.
	 * 37 us.
	 */
	cLCD_cmd_EntryMode_inc = 0x06,
	cLCD_cmd_EntryMode_dec = 0x04,
	cLCD_cmd_EntryMode_SL = 0x07,
	cLCD_cmd_EntryMode_SR = 0x05,
	/* Display ON/OFF :
	 * RS = 0, RW = 0, 0 0 0 0 1 D C B
	 * D : display on (1) / off (0)
	 * C : cursor on (1) / off (0)
	 * B : cursor blink on (1) / off (0)
	 * 37 us
	 */
	cLCD_cmd_DCB_off = 0x08,
	cLCD_cmd_D_on = 0x0C,
	cLCD_cmd_C_on = 0x0A,
	cLCD_cmd_B_on = 0x09,
	/* Cursor or display shift :
	 * shift cursor/display left/right
	 * RS = 0, RW = 0, 0 0 0 1 S/C R/L x x.
	 * 37 us.
	 */
	cLCD_cmd_cursorSL = 0x10, // AC-1
	cLCD_cmd_cursorSR = 0x14, // AC+1
	cLCD_cmd_displaySL = 0x18, // AC = AC
	cLCD_cmd_displaySR = 0x1C, // AC = AC
	/* Function set :
	 * RS = 0, RW = 0, 0 0 1 DL N F x x
	 * DL : 1 - 8bit, 0 - 4bit.
	 * N : 1 - 2 line, 0 - 1 line.
	 * F : 1 - 5x11 dots, 0 - 5x8 dots.
	 * 37 us.
	 */
	cLCD_cmd_FS_4bit_2line = 0x28,
	/* Set CGRAM Address :
	 * RS = 0, RW = 0, 0 1 AC5 AC4 AC3 AC2 AC1 AC0.
	 * Set CGRAM address to Ac.
	 * 37us.
	 */
	/* Set DDRAM Address :
	 * RS = 0, RW = 0, 1 AC6 AC5 AC4 AC3 AC2 AC1 AC0
	 * Set DDRAM address to Ac.
	 * 1 line mode: ?
	 * 2 line mode:
	 * _20x4:
	 * 00 - 13 (line 1, pos 1-20)
	 * 40 - 53 (line 2)
	 * 14 - 27 (line 3)
	 * 54 - 67 (line 4)
	 * _16x2:
	 * 00 - 0F (line 1, pos 1-16)
	 * 40 - 4F (line 2)
	 * 37 us.
	 */
	cLCD_DDRAM_line1 = 0x00,
	cLCD_DDRAM_line2 = 0x40,
#ifdef LCD20x4	
	cLCD_DDRAM_line3 = 0x14,
	cLCD_DDRAM_line4 = 0x54,
#endif
/* Read busy flag and AC
 * RS = 0, RW = 1, 0us, output BF AC6 - AC0
 * BF = 0 : not busy.
 */
/* Write data to RAM :
 * RS = 1, RW = 1, D7-D0 : data.
 * After write AC +/- 1.
 */
/* Read data from RAM */
};

/* private variables */
static uint8_t current_line = 1;
static uint8_t current_pos = 1;

/* declare private functions */
static void init_gpios(void);
static void wait_for_lcd(void);
static void send_4bits(uint8_t RS, uint8_t RW, uint8_t D7D4);
static void send_byte_wo_waiting(uint8_t RS, uint8_t RW, uint8_t D7D0);
static void send_byte(uint8_t RS, uint8_t RW, uint8_t D7D0);
static void delay_us(uint32_t usec);

/* implement private functions */
static void init_gpios(void) {
	LCD_RS_Init(NULL);
//	LCD_RW_Init(NULL);
	LCD_EN_Init(NULL);
	LCD_BL_Init(NULL);
	LCD_DB4_Init(NULL);
	LCD_DB5_Init(NULL);
	LCD_DB6_Init(NULL);
	LCD_DB7_Init(NULL);
}

static void wait_for_lcd(void) {
	/* wait at least 80us before checking BF */
	delay_us(80);

	/* set DB7 to input */
	LCD_DB7_SetInput(NULL);

	/* check BF */
	cmd ? LCD_RS_SetVal(NULL) : LCD_RS_ClrVal(NULL);
//	read ? LCD_RW_SetVal(NULL) : LCD_RW_ClrVal(NULL);

	/* addition delay */
	delay_us(1);

	while (1) {
		/* EN = 1 */
		LCD_EN_SetVal(NULL);

		/* Tpw = 480 ns */
		delay_us(1);

		/* check BF, breaking if DB7 = 0 */
		if (LCD_DB7_GetVal(NULL) == false) {
			break;
		}

		/* EN = 0 */
		LCD_EN_ClrVal(NULL);
	}/* end while : cLCD is ready */

	/* EN = 0 */
	LCD_EN_ClrVal(NULL);

	/* set DB7 to output */
	LCD_DB7_SetOutput(NULL);
}

static void send_4bits(uint8_t RS, uint8_t RW, uint8_t D7D4) {
	RS ? LCD_RS_SetVal(NULL) : LCD_RS_ClrVal(NULL);
//	RW ? LCD_RW_SetVal(NULL) : LCD_RW_ClrVal(NULL);

	/* addition delay */
	delay_us(1);

	/* EN = 1 */
	LCD_EN_SetVal(NULL);

	/* Send data */
	((D7D4 & 0x80) >> 7) ? LCD_DB7_SetVal(NULL) : LCD_DB7_ClrVal(NULL);
	((D7D4 & 0x40) >> 6) ? LCD_DB6_SetVal(NULL) : LCD_DB6_ClrVal(NULL);
	((D7D4 & 0x20) >> 5) ? LCD_DB5_SetVal(NULL) : LCD_DB5_ClrVal(NULL);
	((D7D4 & 0x10) >> 4) ? LCD_DB4_SetVal(NULL) : LCD_DB4_ClrVal(NULL);

	/* addition delay */
	delay_us(1);

	/* EN = 0 */
	LCD_EN_ClrVal(NULL);
}

static void send_byte_wo_waiting(uint8_t RS, uint8_t RW, uint8_t D7D0) {
	RS ? LCD_RS_SetVal(NULL) : LCD_RS_ClrVal(NULL);
//	RW ? LCD_RW_SetVal(NULL) : LCD_RW_ClrVal(NULL);

	/* addition delay */
	delay_us(1);

	/* Send 4 MSB bits D7-D4 */
	/* EN = 1 */
	LCD_EN_SetVal(NULL);

	((D7D0 & 0x80) >> 7) ? LCD_DB7_SetVal(NULL) : LCD_DB7_ClrVal(NULL);
	((D7D0 & 0x40) >> 6) ? LCD_DB6_SetVal(NULL) : LCD_DB6_ClrVal(NULL);
	((D7D0 & 0x20) >> 5) ? LCD_DB5_SetVal(NULL) : LCD_DB5_ClrVal(NULL);
	((D7D0 & 0x10) >> 4) ? LCD_DB4_SetVal(NULL) : LCD_DB4_ClrVal(NULL);

	/* addition delay */
	delay_us(1);

	/* EN = 0 */
	LCD_EN_ClrVal(NULL);

	/* tEC > 1.2 us */
	delay_us(2);

	/* Send next 4 bits D3-D0 */
	/* EN = 1 */
	LCD_EN_SetVal(NULL);

	((D7D0 & 0x08) >> 3) ? LCD_DB7_SetVal(NULL) : LCD_DB7_ClrVal(NULL);
	((D7D0 & 0x04) >> 2) ? LCD_DB6_SetVal(NULL) : LCD_DB6_ClrVal(NULL);
	((D7D0 & 0x02) >> 1) ? LCD_DB5_SetVal(NULL) : LCD_DB5_ClrVal(NULL);
	(D7D0 & 0x01) ? LCD_DB4_SetVal(NULL) : LCD_DB4_ClrVal(NULL);

	/* addition delay */
	delay_us(1);

	/* EN = 0 */
	LCD_EN_ClrVal(NULL);
}

static void send_byte(uint8_t RS, uint8_t RW, uint8_t D7D0) {
	/* wait for lcd is not busy */
	wait_for_lcd();

	/* send data */
	send_byte_wo_waiting(RS, RW, D7D0);
}

static void delay_us(uint32_t usec) {
	uint32_t num_clocks = 48 * usec;
	uint32_t count = 0;

	for (count = 0; count < num_clocks; count++) {
		__asm__ __volatile__("nop");
	}
}
/* end implementing private functions */

/* implement public functions */
void init_lcd(void) {
	/* init GPIOs */
	init_gpios();

	/* Set back light */
	LCD_BL_SetVal(NULL);

	/* wait for >40ms */
	delay_us(100000);

	/* send function set 1 */
	send_4bits(cmd, write, 0x30);
	/* wait for >37us */
	delay_us(50);

	/* send function set 2 */
	send_byte_wo_waiting(cmd, write, cLCD_cmd_FS_4bit_2line);
	/* wait for >37us */
	delay_us(50);

	/* send function set 3 */
	send_byte_wo_waiting(cmd, write, cLCD_cmd_FS_4bit_2line);
	/* wait for >37us */
	delay_us(50);

	/* send display control, Display on */
	send_byte(cmd, write, cLCD_cmd_D_on);

	/* send display clear, clear all DDRAM to 20H, set DDRAM address to 0 from AC */
	send_byte(cmd, write, cLCD_cmd_ClearDisplay);

	/* send Entry mode set, AC++ */
	send_byte(cmd, write, cLCD_cmd_EntryMode_inc);

	/* additional delay */
	delay_us(1000);
}

bool set_cursor(uint8_t line, uint8_t pos) {
	uint8_t ddram_addr = 0;

	/* check line and pos */
	if ((line < 1) || (line > MAX_LINE)) {
		return false;
	}
	if ((pos < 1) || (pos > MAX_CHAR_IN_LINE)) {
		return false;
	}

	/* calculate DDRAM address */
	switch (line) {
	case 1:
		ddram_addr = cLCD_DDRAM_line1 + pos - 1;
		break;
	case 2:
		ddram_addr = cLCD_DDRAM_line2 + pos - 1;
		break;
#ifdef LCD20x4
	case 3:
		ddram_addr = cLCD_DDRAM_line3 + pos - 1;
		break;
	case 4:
		ddram_addr = cLCD_DDRAM_line4 + pos - 1;
		break;
#endif
	default:
		return false;
	}
	/* send set DDRAM address command */
	send_byte(cmd, write, ddram_addr | 0x80);

	/* save state */
	current_line = line;
	current_pos = pos;

	return true;
}

void lcd_putc(int8_t a_char) {
	uint8_t tab_pos;

	/* special characters */
	switch (a_char) {
	case '\n':
		if (current_line < MAX_LINE) {
			set_cursor(++current_line, 1);
		}
		break;
	case '\t':
		if (current_pos % 2 == 0) {
			tab_pos = current_pos + 2;
		} else {
			tab_pos = (current_pos / 2 + 1) * 2;
		}
		if (tab_pos < MAX_CHAR_IN_LINE) {
			set_cursor(current_line, tab_pos);
		}
		break;
	case '\r':
		set_cursor(current_line, 1);
		break;
	default:
		/* send data */
		send_byte(data, write, a_char);

		/* save state (only correct when in 2 line mode) */
		current_pos++;
		if (current_pos > MAX_CHAR_IN_LINE) {
			current_pos = 1;
			current_line++;
			if (current_line > MAX_LINE) {
				current_line = 1;
			}
			set_cursor(current_line, current_pos);
		}
		break;
	}
}

int16_t lcd_puts(int8_t *a_string) {
	int16_t num_char = 0;

	while (*a_string != '\0') {
		lcd_putc(*a_string++);
		num_char++;
	}

	return num_char;
}

int16_t lcd_printf(const char *format, ...) {
	int8_t buffer[MAX_CHAR_IN_LINE * MAX_LINE];
	int16_t retval;
	va_list args;

	va_start(args, format);
	vsnprintf((char*) buffer, MAX_CHAR_IN_LINE * MAX_LINE, format, args);
	va_end(args);

	/* print buffer to lcd */
	retval = lcd_puts(buffer);

	return retval;
}

void clear(void) {
	/* send clear command */
	send_byte(cmd, write, cLCD_cmd_ClearDisplay);

	/* save state */
	current_line = 1;
	current_pos = 1;

	/* additional delay */ //FIXME(later): why do we need this?
	delay_us(200);
}

void return_home(void) {
	/* send clear command */
	send_byte(cmd, write, cLCD_cmd_ReturnHome);

	/* save state */
	current_line = 1;
	current_pos = 1;
}

void return_home_wo_shifting(void) {
	set_cursor(1, 1);
}

void gen_pattern(const uint8_t *pattern, uint8_t addr) {
	uint16_t count;

	/* check addr */
	if (addr > 7) {
		return;
	}

	/* send set CGRAM address command */
	send_byte(cmd, write, (addr << 3) | 0x40);

	/* send pattern */
	for (count = 0; count < 8; count++) {
		send_byte(data, write, pattern[count]);
	}

	/* set cursor to current state */
	set_cursor(current_line, current_pos);
}
