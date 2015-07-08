/*
 * button.c
 *
 *  Created on: Feb 8, 2015
 *      Author: nvhie_000
 */
#include "mqxlite.h"
#include "button.h"

#define DEBUG_EN 1
#define NOTIFY_EN 1
#include "debug.h"

#define TIMER_PERIOD (10) //ms
#define BTN_SAMPLING_PERIOD (10 / TIMER_PERIOD) //sampling every 10ms (timer_period = 10ms)
#define BTN_HOLD_TIMEOUT (500 / TIMER_PERIOD) //btn is on hold after 0.5s.
#define BTN_HOLD_PERIOD (100 / TIMER_PERIOD) //resend evt after 100ms when btn is on hold.
const bool btn_active_state = FALSE;

static uint16_t sampling_time_count = 0;
static uint16_t button_hold_time_count = 0;

static void button_processing(button_t *a_button);
static void button_hold_processing(button_t *a_button, void *dest_queue);

static bool enable_all_btn_evts = TRUE;

static void button_processing(button_t *a_button) {
	/* sampling */
	a_button->new_value_reg[2] = a_button->new_value_reg[1];
	a_button->new_value_reg[1] = a_button->new_value_reg[0];
	a_button->new_value_reg[0] = a_button->GetVal(NULL);

	/* update hold_time_count value */
	if (a_button->hold_time_count != 0) {
		a_button->hold_time_count--;
		if (a_button->hold_time_count == 0) { //time out, button is hold.
			a_button->current_status = btn_on_hold;
		}
	}

	/* processing */
	if ((a_button->new_value_reg[0] == a_button->new_value_reg[1])
			&& (a_button->new_value_reg[1] == a_button->new_value_reg[2])) { //new stable state

		if (a_button->new_value_reg[0] != a_button->old_value_reg) { //change state

			a_button->old_value_reg = a_button->new_value_reg[0];
			if (a_button->new_value_reg[0] == btn_active_state) { //change from inactive->active
				a_button->hold_time_count = BTN_HOLD_TIMEOUT; //set time out value
			} else { //change from active->inactive
				if (a_button->hold_time_count > 0) { //button is pressed
					a_button->current_status = btn_pressed;
				} else { //button is un-hold
					a_button->current_status = btn_no_pressed;
				}
				a_button->hold_time_count = 0;
			}
		}
	} //end if()
}

static void button_hold_processing(button_t *a_button, void *dest_queue) {
	button_hold_time_count++;
	if (button_hold_time_count == BTN_HOLD_PERIOD) {
		button_hold_time_count = 0;
		a_button->old_status = btn_no_pressed;
		if (!dest_queue || !(a_button->enable_btn_evt)
				|| !enable_all_btn_evts) {
			return;
		}
		_mqx_uint msg = (_mqx_uint) (((uint32_t) a_button->dev_id
				<< BTN_NUM_BITS_IN_VALUE)
		| (uint16_t) get_button_status(a_button));
		_lwmsgq_send((pointer) dest_queue, &msg, 0);
	}
}

void enable_button_event(button_t *a_button) {
	a_button->enable_btn_evt = TRUE;
}

void disable_button_event(button_t *a_button) {
	a_button->enable_btn_evt = FALSE;
}

void enable_all_button_events(void) {
	enable_all_btn_evts = TRUE;
}

void disable_all_button_events(void) {
	enable_all_btn_evts = FALSE;
}

uint8_t get_button_id(button_t *a_button) {
	return a_button->dev_id;
}

bool is_changed_status(button_t *a_button) {
	if (a_button->current_status != a_button->old_status) {
		return TRUE;
	}
	return FALSE;
}

btn_status_t get_button_status(button_t *a_button) {
	btn_status_t status = a_button->current_status;

	if (a_button->current_status == btn_pressed) {
		a_button->current_status = btn_no_pressed;
	}

	a_button->old_status = a_button->current_status;

	return status;
}

void button_callback_timer_isr(button_t *button_table, uint8_t num_of_btns,
		void *dest_queue) {
	sampling_time_count = sampling_time_count + 1;

	if (sampling_time_count == BTN_SAMPLING_PERIOD) {
		sampling_time_count = 0;
		uint8_t i = 0;
		for (i = 0; i < num_of_btns; i++) {
			button_processing(&button_table[i]);
			if (is_changed_status(&button_table[i])) {
				if (!dest_queue || !(&button_table[i].enable_btn_evt)
						|| !enable_all_btn_evts) {
					return;
				}
				_mqx_uint msg =
						(_mqx_uint) (((uint32_t) button_table[i].dev_id
								<< BTN_NUM_BITS_IN_VALUE)| (uint16_t) get_button_status(&button_table[i]));
				_lwmsgq_send((pointer) dest_queue, &msg, 0);
			} else {
				if (button_table[i].current_status == btn_on_hold) {
					button_hold_processing(&button_table[i], dest_queue);
				}
			}
		} //end for()
	} //end if()
}
