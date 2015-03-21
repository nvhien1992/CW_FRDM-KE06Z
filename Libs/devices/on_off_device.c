/*
 * on_off_device.c
 *
 *  Created on: Feb 12, 2015
 *      Author: nvhie_000
 */
#include "on_off_device.h"

#define DEBUG_EN 1
#define NOTIFY_EN 1
#include "debug.h"

#define MAX_ON_OFF_DEV 2
#define TIMER_PERIOD 10 //ms
const bool active_state = FALSE;

on_off_dev_t *on_off_dev_table[MAX_ON_OFF_DEV];
bool table_inited = FALSE;

static void blink_processing(on_off_dev_t *on_off_dev);
static void assign_dev(on_off_dev_t *on_off_dev);
static void remove_dev(on_off_dev_t *on_off_dev);
static void init_table(void);

static void init_table(void) {
	uint8_t i = 0;
	for (i = 0; i < MAX_ON_OFF_DEV; i++) {
		on_off_dev_table[i] = NULL;
	}
}

static void assign_dev(on_off_dev_t *on_off_dev) {
	uint8_t i = 0;
	for (i = 0; i < MAX_ON_OFF_DEV; i++) {
		if(on_off_dev_table[i] == NULL) {
			on_off_dev_table[i] = on_off_dev;
			return;
		}
	}	
}

static void remove_dev(on_off_dev_t *on_off_dev) {
	uint8_t i = 0;
	for (i = 0; i < MAX_ON_OFF_DEV; i++) {
		if(on_off_dev_table[i] == on_off_dev) {
			on_off_dev_table[i] = NULL;
			return;
		}
	}	
}

static void blink_processing(on_off_dev_t *on_off_dev) {
	on_off_dev->time_cycle_count++;
	if (on_off_dev->time_cycle_count
			== (on_off_dev->period_in_ms / (2 * TIMER_PERIOD))) {
		on_off_dev->time_cycle_count = 0;
		/* toggle */
		if(on_off_dev->is_on_in_blink) {
			on_off_dev->is_on_in_blink = FALSE;
			if (active_state) {
				on_off_dev->ClrVal(NULL);
			} else {
				on_off_dev->SetVal(NULL);
			}
		} else {
			on_off_dev->is_on_in_blink = TRUE;
			if (active_state) {
				on_off_dev->SetVal(NULL);
			} else {
				on_off_dev->ClrVal(NULL);
			}
		}
	}
}

void turn_on(on_off_dev_t *on_off_dev) {
	if(on_off_dev->current_status == BLINK) {
		remove_dev(on_off_dev);
	}
	on_off_dev->current_status = ON;
	if (active_state) {
		on_off_dev->SetVal(NULL);
	} else {
		on_off_dev->ClrVal(NULL);
	}
}

void turn_off(on_off_dev_t *on_off_dev) {
	if(on_off_dev->current_status == BLINK) {
		remove_dev(on_off_dev);
	}
	on_off_dev->current_status = OFF;
	if (active_state) {
		on_off_dev->ClrVal(NULL);
	} else {
		on_off_dev->SetVal(NULL);
	}
}

void toggle(on_off_dev_t *on_off_dev) {
	if (on_off_dev->current_status == ON) {
		turn_off(on_off_dev);
	} else if (on_off_dev->current_status == OFF) {
		turn_on(on_off_dev);
	}
}

void blink(on_off_dev_t *on_off_dev, uint8_t freq_in_hz) {
	if(!table_inited) {
		table_inited = TRUE;
		init_table();
	}
	if (on_off_dev->current_status != BLINK) {
		on_off_dev->current_status = BLINK;
		on_off_dev->time_cycle_count = 0;
		on_off_dev->period_in_ms = 1000 / freq_in_hz; //ms
		if (on_off_dev->period_in_ms < (2 * TIMER_PERIOD)) { //can't blink with freq running faster than 2*timer freq.
			return;
		}
		on_off_dev->current_status = BLINK;
		assign_dev(on_off_dev);
	}
}

void on_off_blink_callback_timer_isr(void) {
	uint8_t i = 0;
	for (i = 0; i < MAX_ON_OFF_DEV; i++) {
		if(on_off_dev_table[i] != NULL) {
			blink_processing(on_off_dev_table[i]);
		}
	}
}
