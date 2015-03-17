/*
 * alarm.c
 *
 *  Created on: Mar 10, 2015
 *      Author: nvhie_000
 */
#include "mqxlite.h"
#include "alarm.h"

#define TIMER_PERIOD 10 //ms

void set_alarm_id(alarm_t *a_alarm, uint8_t id) {
	a_alarm->alarm_id = id;
}

void set_alarm_interval(alarm_t *a_alarm, uint16_t interval_in_sec) {
	a_alarm->time_properties.interval_in_sec = interval_in_sec;
}

void enable_alarm(alarm_t *a_alarm) {
	a_alarm->is_active = true;
}

void disable_alarm(alarm_t *a_alarm) {
	a_alarm->is_active = false;
	a_alarm->time_properties.time_cycle_count = 0;
}

void restart_alarm(alarm_t *a_alarm) {
	a_alarm->is_active = true;
	a_alarm->time_properties.time_cycle_count = 0;
}

void alarm_callback_timer_isr(alarm_t *a_alarm, void *dest_queue) {
	if (!a_alarm->is_active) {
		return;
	}

	a_alarm->time_properties.time_cycle_count++;
	if (a_alarm->time_properties.time_cycle_count
			== (a_alarm->time_properties.interval_in_sec * 1000 / TIMER_PERIOD)) {
		a_alarm->time_properties.time_cycle_count = 0;
		_mqx_uint msg = (_mqx_uint) ((uint32_t) (a_alarm->alarm_id << 16));
		_lwmsgq_send((pointer) dest_queue, &msg, 0);
	}
}
