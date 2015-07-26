/*
 * main_menu.c
 *
 *  Created on: Jun 30, 2015
 *      Author: nvhie_000
 */
#include "main_menu.h"
#include "menu_id.h"
#include "lcd_dev.h"
#include "button_devs.h"

menu_id_t next_main_menu = MEASUREMENT_MENU;

void menus_init(void) {
	buttons_init();
	uint8_t indx = 0;
	for (indx = 0; indx < 2; indx++) {
		lcd_init(&lcd_pins);
	}
}

void main_menu(void) {
	switch (next_main_menu) {
	case MEASUREMENT_MENU:
		measurement_menu_enter(&next_main_menu);
		break;
	case PRINT_FUNCTIONS_MENU:
		print_functions_menu_enter(&next_main_menu);
		break;
	case QC_URINE_SAMPLE_MENU:
		qc_sample_menu_enter(&next_main_menu);
		break;
	case DAILY_MAINTENANCE_MENU:
		daily_maintenance_menu_enter(&next_main_menu);
		break;
	case OPERATOR_FUNCTIONS_MENU:
		operator_functions_menu_enter(&next_main_menu);
		break;
	case PROGRAM_INSTRUMENT_MENU:
		program_instrument_menu_enter(&next_main_menu);
		break;
	case SERVICE_FUNCTIONS_MENU:
		service_functions_menu_enter(&next_main_menu);
		break;
	case CALIBRATION_MENU:
		calibration_menu_enter(&next_main_menu);
		break;
	default:
		measurement_menu_enter(&next_main_menu);
		break;
	}
}
