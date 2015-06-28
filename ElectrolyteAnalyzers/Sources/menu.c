/**
 @file menu.c
 @brief Implementing functions for menu of the electrolyte analyzer.
 @author <b>Nguyen Van Hien</b> <nvhien1992@gmail.com>
 @copyright Copyright &copy; 2015, <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b>, All rights reserved. 
 */
#include "menu.h"
#include "button.h"
#include "LCD.h"
#include "YES_BTN.h"
#include "NO_BTN.h"
#include "LCD_RS.h"
#include "LCD_EN.h"
#include "LCD_BL.h"
#include "LCD_DB4.h"
#include "LCD_DB5.h"
#include "LCD_DB6.h"
#include "LCD_DB7.h"
#include "MISC_TIMER.h"

#define DEBUG_EN 1
#define NOTIFY_EN 1
#include "debug.h"

#define NUM_BTN_MESSAGES (32)
#define MESSAGE_SIZE_ALIGNED (1)
uint8_t btn_to_ctrl_msg_queue[sizeof(LWMSGQ_STRUCT)
		+ NUM_BTN_MESSAGES * MESSAGE_SIZE_ALIGNED];

#define YES_BTN_ID (0x01)
#define NO_BTN_ID (0x10)

button_t yes_btn = { YES_BTN_GetVal, //(*GetVal)()
		YES_BTN_ID, //dev_id
		0, //hold_time_count
		btn_no_pressed, //current_status
		btn_no_pressed, //old_status
		FALSE , //old_value_reg
		};

button_t no_btn = { NO_BTN_GetVal, //(*GetVal)()
		NO_BTN_ID, //dev_id
		0, //hold_time_count
		btn_no_pressed, //current_status
		btn_no_pressed, //old_status
		FALSE , //old_value_reg
		};

button_t* btn_table[2] = { NULL, NULL };

bool btn_enabled = TRUE;

lcd_pins_t lcd_pins = { { LCD_RS_SetVal, LCD_RS_ClrVal, }, //lcd_rs
		{ LCD_EN_SetVal, LCD_EN_ClrVal, }, //lcd_en
		{ LCD_BL_SetVal, LCD_BL_ClrVal, }, //lcd_bl
		{ LCD_DB4_SetVal, LCD_DB4_ClrVal, }, //lcd_db4
		{ LCD_DB5_SetVal, LCD_DB5_ClrVal, }, //lcd_db5
		{ LCD_DB6_SetVal, LCD_DB6_ClrVal, }, //lcd_db6
		{ LCD_DB7_SetInput, LCD_DB7_SetOutput, LCD_DB7_SetVal, LCD_DB7_ClrVal,
				LCD_DB7_GetVal, }, //lcd_db7
		};

/* global vars */
_mqx_uint recv_msg = 0;
uint16_t recv_btn_id = 0;
btn_status_t recv_btn_status = btn_no_pressed;

/* menu defines */
#define MEASUREMENT_MENU		(0x02)
#define PRINT_FUNCTIONS_MENU 	(0x03)
#define QC_URINE_SAMPLE_MENU 	(0x05)
#define DAILY_MAINTENANCE_MENU 	(0x06)
#define OPERATOR_FUNCTIONS_MENU (0x07)
#define PROGRAM_INSTRUMENT_MENU (0x08)
#define SERVICE_FUNCTIONS_MENU 	(0x09)
#define CALIBRATION_MENU 		(0x0A)
uint8_t next_main_menu = MEASUREMENT_MENU;

void analyzer_init(void) {
	/* power up analyzer */
	power_up_sequence();
	
	/* init buttons */
	btn_table[0] = &yes_btn;
	btn_table[1] = &no_btn;

	/* init timer to process buttons */
	MISC_TIMER_Init(NULL);

	/* init LCD */
	init_lcd(&lcd_pins);

	/* init msgq */
	_lwmsgq_init((pointer) btn_to_ctrl_msg_queue, NUM_BTN_MESSAGES,
			MESSAGE_SIZE_ALIGNED);

	/* default main menu */
	next_main_menu = MEASUREMENT_MENU;
}

void yes_no_btn_processing(void) {
	if (btn_enabled) {
		button_callback_timer_isr(*btn_table, 2, btn_to_ctrl_msg_queue);
	} else {
		button_callback_timer_isr(*btn_table, 2, NULL);
	}
}

void enable_button(void) {
	btn_enabled = TRUE;
}

void disable_button(void) {
	btn_enabled = FALSE;
}

void main_menu(void) {
	switch (next_main_menu) {
	case MEASUREMENT_MENU:
		measurement_menu();
		break;
	case PRINT_FUNCTIONS_MENU:
		print_functions_menu();
		break;
	case QC_URINE_SAMPLE_MENU:
		qc_std_dialysate_urine_sample_menu();
		break;
	case DAILY_MAINTENANCE_MENU:
		daily_maintenance_menu();
		break;
	case OPERATOR_FUNCTIONS_MENU:
		operator_functions_menu();
		break;
	case PROGRAM_INSTRUMENT_MENU:
		program_instrument_menu();
		break;
	case SERVICE_FUNCTIONS_MENU:
		service_functions_menu();
		break;
	case CALIBRATION_MENU:
		calibration_menu();
		break;
	default:
		measurement_menu();
		break;
	}
}

void measurement_menu(void) {
	lcd_printf("Na K Cl READY?");
	
	//TODO open flap
	
	enable_button();
	_lwmsgq_receive((pointer) btn_to_ctrl_msg_queue, &recv_msg,
			LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);
	disable_button();
	
	recv_btn_id = (uint16_t) (recv_msg >> BTN_NUM_BITS_IN_VALUE);
	recv_btn_status = (btn_status_t) recv_msg;
	switch (recv_btn_id) {
	case YES_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			//TODO measure samples
			lcd_printf("[WIP...]");
			_time_delay_ticks(200);
			next_main_menu = MEASUREMENT_MENU;
		}
		return;
	case NO_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			next_main_menu = PRINT_FUNCTIONS_MENU;
		}
		return;
	default:
		return;
	}
}

void print_functions_menu(void) {
	lcd_printf("PRINT FUNCTIONS?");
	
	enable_button();
	_lwmsgq_receive((pointer) btn_to_ctrl_msg_queue, &recv_msg,
			LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);
	disable_button();
	
	recv_btn_id = (uint16_t) (recv_msg >> BTN_NUM_BITS_IN_VALUE);
	recv_btn_status = (btn_status_t) recv_msg;
	switch (recv_btn_id) {
	case YES_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			//TODO print functions
			lcd_printf("[WIP...]");
			_time_delay_ticks(200);
			next_main_menu = MEASUREMENT_MENU;
		}
		return;
	case NO_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			next_main_menu = QC_URINE_SAMPLE_MENU;
		}
		return;
	default:
		return;
	}
}

void qc_std_dialysate_urine_sample_menu(void) {
	lcd_printf("QC/STD/DIALYSATE/URINE SAMPLE?");
	
	enable_button();
	_lwmsgq_receive((pointer) btn_to_ctrl_msg_queue, &recv_msg,
			LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);
	disable_button();
	
	recv_btn_id = (uint16_t) (recv_msg >> BTN_NUM_BITS_IN_VALUE);
	recv_btn_status = (btn_status_t) recv_msg;
	switch (recv_btn_id) {
	case YES_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			//TODO qc/std/dialysate/urine sample
			lcd_printf("[WIP...]");
			_time_delay_ticks(200);
			next_main_menu = MEASUREMENT_MENU;
		}
		return;
	case NO_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			next_main_menu = DAILY_MAINTENANCE_MENU;
		}
		return;
	default:
		return;
	}
}

void daily_maintenance_menu(void) {
	lcd_printf("DAILY MAINTENANCE?");
	
	enable_button();
	_lwmsgq_receive((pointer) btn_to_ctrl_msg_queue, &recv_msg,
			LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);
	disable_button();
	
	recv_btn_id = (uint16_t) (recv_msg >> BTN_NUM_BITS_IN_VALUE);
	recv_btn_status = (btn_status_t) recv_msg;
	switch (recv_btn_id) {
	case YES_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			//TODO daily maintenance
			lcd_printf("[WIP...]");
			_time_delay_ticks(200);
			next_main_menu = MEASUREMENT_MENU;
		}
		return;
	case NO_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			next_main_menu = OPERATOR_FUNCTIONS_MENU;
		}
		return;
	default:
		return;
	}
}

void operator_functions_menu(void) {
	lcd_printf("OPERATOR FUNCTIONS?");
	
	enable_button();
	_lwmsgq_receive((pointer) btn_to_ctrl_msg_queue, &recv_msg,
			LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);
	disable_button();
	
	recv_btn_id = (uint16_t) (recv_msg >> BTN_NUM_BITS_IN_VALUE);
	recv_btn_status = (btn_status_t) recv_msg;
	switch (recv_btn_id) {
	case YES_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			//TODO operator functions
			lcd_printf("[WIP...]");
			_time_delay_ticks(200);
			next_main_menu = MEASUREMENT_MENU;
		}
		return;
	case NO_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			next_main_menu = PROGRAM_INSTRUMENT_MENU;
		}
		return;
	default:
		return;
	}
}

void program_instrument_menu(void) {
	lcd_printf("PROGRAM INSTRUMENT?");
	
	enable_button();
	_lwmsgq_receive((pointer) btn_to_ctrl_msg_queue, &recv_msg,
			LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);
	disable_button();
	
	recv_btn_id = (uint16_t) (recv_msg >> BTN_NUM_BITS_IN_VALUE);
	recv_btn_status = (btn_status_t) recv_msg;
	switch (recv_btn_id) {
	case YES_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			//TODO program instrument
			lcd_printf("[WIP...]");
			_time_delay_ticks(200);
			next_main_menu = MEASUREMENT_MENU;
		}
		return;
	case NO_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			next_main_menu = SERVICE_FUNCTIONS_MENU;
		}
		return;
	default:
		return;
	}
}

void service_functions_menu(void) {
	lcd_printf("SERVICE FUNCTIONS?");
	
	enable_button();
	_lwmsgq_receive((pointer) btn_to_ctrl_msg_queue, &recv_msg,
			LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);
	disable_button();
	
	recv_btn_id = (uint16_t) (recv_msg >> BTN_NUM_BITS_IN_VALUE);
	recv_btn_status = (btn_status_t) recv_msg;
	switch (recv_btn_id) {
	case YES_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			//TODO service functions
			lcd_printf("[WIP...]");
			_time_delay_ticks(200);
			next_main_menu = MEASUREMENT_MENU;
		}
		return;
	case NO_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			next_main_menu = CALIBRATION_MENU;
		}
		return;
	default:
		return;
	}
}

void calibration_menu(void) {
	lcd_printf("CALIBRATION?");
	
	enable_button();
	_lwmsgq_receive((pointer) btn_to_ctrl_msg_queue, &recv_msg,
			LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);
	disable_button();
	
	recv_btn_id = (uint16_t) (recv_msg >> BTN_NUM_BITS_IN_VALUE);
	recv_btn_status = (btn_status_t) recv_msg;
	switch (recv_btn_id) {
	case YES_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			//TODO calibration
			lcd_printf("[WIP...]");
			_time_delay_ticks(200);
			next_main_menu = MEASUREMENT_MENU;
		}
		return;
	case NO_BTN_ID:
		if (recv_btn_status == btn_pressed || recv_btn_status == btn_on_hold) {
			next_main_menu = MEASUREMENT_MENU;
		}
		return;
	default:
		next_main_menu = MEASUREMENT_MENU;
		return;
	}
}

void power_up_sequence(void) {
	//TODO
}
