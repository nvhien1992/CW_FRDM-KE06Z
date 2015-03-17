/*
 * alarm.h
 *
 *  Created on: Mar 10, 2015
 *      Author: nvhie_000
 */

#ifndef ALARM_H_
#define ALARM_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint16_t interval_in_sec; //only support the interval set in second.
	uint16_t time_cycle_count;
} time_property_t;

typedef struct {
	uint8_t alarm_id;
	time_property_t time_properties;
	bool is_active;
} alarm_t;

/**
 * @brief Set id for alarm. This function must be called firstly.
 * 
 * @param[in|out] a_alarm Target alarm.
 * @param[in] id The alarm id.
 */
void set_alarm_id(alarm_t *a_alarm, uint8_t id);

/**
 * @brief Set interval for alarm.
 * 
 * @param[in|out] a_alarm Target alarm.
 * @param[in] interval_in_sec Interval
 */
void set_alarm_interval(alarm_t *a_alarm, uint16_t interval_in_sec);

/**
 * @brief Enable alarm.
 * 
 * @param[in|out] a_alarm Target alarm.
 */
void enable_alarm(alarm_t *a_alarm);

/**
 * @brief Disable alarm.
 * 
 * @param[in|out] a_alarm Target alarm.
 */
void disable_alarm(alarm_t *a_alarm);

/**
 * @brief Restart alarm.
 * 
 * @param[in|out] a_alarm Target alarm.
 */
void restart_alarm(alarm_t *a_alarm);

/**
 * @brief The callback function called in the interrupt of timer.
 * 
 * @param[in|out] a_alarm Target alarm.
 * @param[in] dest_queue The pointer to msg queue.
 */
void alarm_callback_timer_isr(alarm_t *a_alarm, void *dest_queue);

#endif /* ALARM_H_ */
