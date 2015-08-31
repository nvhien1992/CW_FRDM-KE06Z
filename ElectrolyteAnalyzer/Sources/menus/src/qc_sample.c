/*
 * qc_sample.c
 *
 *  Created on: Jun 30, 2015
 *      Author: nvhie_000
 */
#include "qc_sample.h"
#include "button_devs.h"
#include "LCD.h"

const char* qc_menu = "QC/STD/DIALYSATE/URINE SAMPLE?";

void qc_sample_menu_enter(menu_id_t* next_main_menu) {
	lcd_clear();
	lcd_printf(qc_menu);
	btn_id_t btn_id = waiting_button_pressed();
	if(btn_id == YES_BTN_ID) {
		lcd_clear();
		lcd_printf("[WIP...]");
		*next_main_menu = MEASUREMENT_MENU;
		_time_delay_ticks(1000);
	} else if (btn_id == NO_BTN_ID) {
		*next_main_menu = DAILY_MAINTENANCE_MENU;
	}
}
