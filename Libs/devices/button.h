/*
 * button.h
 *
 *  Created on: Feb 8, 2015
 *      Author: nvhie_000
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "PE_Types.h"

#define BTN_NUM_BITS_IN_VALUE (16) //value has 16bits

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
	bool enable_btn_evt;
} button_t;

/**
 * 
 */
void enable_button_event(button_t *a_button);

/**
 * 
 */
void disable_button_event(button_t *a_button);

/**
 * 
 */
void enable_all_button_events(void);

/**
 * 
 */
void disable_all_button_events(void);

/**
 * 
 */
uint8_t get_button_id(button_t *a_button);

/**
 * 
 */
bool is_changed_status(button_t *a_button);

/**
 * 
 */
btn_status_t get_button_status(button_t *a_button);

/**
 * @brief The callback function called by timer isr to process button.
 * 
 * @param[in] button_table
 * @param[in] num_of_btns
 * @param[in] dest_queue Msgq receiving btn status, NULL if don't want to send button status to a msgq.
 */
void button_callback_timer_isr(button_t *button_table, uint8_t num_of_btns, void *dest_queue);

#endif /* BUTTON_H_ */
