/*
 * button.h
 *
 *  Created on: Feb 8, 2015
 *      Author: nvhie_000
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdint.h>
#include <stdbool.h>
#include "PE_Types.h"

typedef enum {
	btn_no_pressed = 0,
	btn_pressed = 1,
	btn_on_hold = 2,
} btn_status_t;

typedef struct {
	bool (*GetVal)(LDD_TDeviceData *dev_data);
	uint8_t dev_id;
	uint16_t hold_time_count;
	btn_status_t current_status;
	btn_status_t old_status;
	bool old_value_reg;
	bool new_value_reg[3];
} button_t;

uint8_t get_button_id(button_t *a_button);

bool is_changed_status(button_t *a_button);

btn_status_t get_button_status(button_t *a_button);

void button_callback_timer_isr(button_t *button_table, uint8_t num_of_btns, void *dest_queue);

#endif /* BUTTON_H_ */
