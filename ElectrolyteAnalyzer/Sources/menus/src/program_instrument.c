/*
 * program_instrument.c
 *
 *  Created on: Jun 30, 2015
 *      Author: nvhie_000
 */
#include "program_instrument.h"
#include "button_devs.h"
#include "LCD.h"

const char* program_ins_menu = "PROGRAM INSTRUMENT?";

void program_instrument_menu_enter(menu_id_t* next_main_menu) {
	lcd_clear();
	lcd_printf(program_ins_menu);
	btn_id_t btn_id = waiting_button_pressed();
	if(btn_id == YES_BTN_ID) {
		lcd_clear();
		lcd_printf("[WIP...]");
		*next_main_menu = MEASUREMENT_MENU;
		_time_delay_ticks(1000);
	} else if (btn_id == NO_BTN_ID) {
		*next_main_menu = SERVICE_FUNCTIONS_MENU;
	}
}
