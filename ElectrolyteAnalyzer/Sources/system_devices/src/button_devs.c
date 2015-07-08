/*
 * button_devs.c
 *
 *  Created on: Jul 3, 2015
 *      Author: nvhie_000
 */
#include "button_devs.h"
#include "button.h"
#include "YES_BTN.h"
#include "NO_BTN.h"

/* Definitions for LW Message Queue Component */
#define NUM_BTN_MESSAGES	(2)
#define MSG_SIZE_ALIGNED	(1)

/* Use light weight message queues */
uint8_t btns_msg_queue[sizeof(LWMSGQ_STRUCT)
		+ NUM_BTN_MESSAGES * sizeof(uint32_t)];

#define MAX_BTNS (2)

button_t yes_btn = { YES_BTN_GetVal, //(*GetVal)()
		YES_BTN_ID, //dev_id
		0, //hold_time_count
		btn_no_pressed, //current_status
		btn_no_pressed, //old_status
		FALSE, //old_value_reg
		{},	//new_value_reg[]
		TRUE , //enable_btn_evt
		};

button_t no_btn = { NO_BTN_GetVal, //(*GetVal)()
		NO_BTN_ID, //dev_id
		0, //hold_time_count
		btn_no_pressed, //current_status
		btn_no_pressed, //old_status
		FALSE, //old_value_reg
		{}, //new_value_reg[]
		TRUE , //enable_btn_evt
		};

button_t* btn_table[2] = { NULL, NULL };

void buttons_init(void) {
	btn_table[0] = &yes_btn;
	btn_table[1] = &no_btn;
	disable_buttons();
	_lwmsgq_init((pointer) btns_msg_queue, NUM_BTN_MESSAGES, MSG_SIZE_ALIGNED);
}

void buttons_processing_in_timer_isr(void) {
	button_callback_timer_isr(*btn_table, MAX_BTNS, btns_msg_queue);
}

void enable_buttons(void) {
	enable_all_button_events();
}

void disable_buttons(void) {
	disable_all_button_events();
}
