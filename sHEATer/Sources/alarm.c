/*
 * alarm.c
 *
 *  Created on: Mar 10, 2015
 *      Author: nvhie_000
 */
#include "alarm.h"
#include "global.h"

#define TIMER_PERIOD 10 //ms
#define A_MIN (60*1000/TIMER_PERIOD) //1min

static uint16_t time_cycle_count = 0;
static bool alarm_en = false;

void enable_alarm(void) {
	alarm_en = true;
}

void disable_alarm(void) {
	alarm_en = false;
	time_cycle_count = 0;
}

void restart_alarm(void) {
	alarm_en = true;
	time_cycle_count = 0;	
}

void alarm_callback_timer_isr(void *dest_queue) {
	if(!alarm_en) {
		return;
	}
	
	time_cycle_count++;
	if (time_cycle_count == A_MIN) {
		time_cycle_count = 0;
		_mqx_uint msg = (_mqx_uint) ((uint32_t) (ALARM << 16));
		_lwmsgq_send((pointer) dest_queue, &msg, 0);
	}
}