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

/**
 * @brief Set id for alarm. This function must be called first.
 * 
 * @param[in] id The alarm id.
 */
void set_alarm_id(uint8_t id);

/**
 * @brief Enable alarm.
 */
void enable_alarm(void);

/**
 * @brief Disable alarm.
 */
void disable_alarm(void);

/**
 * @brief Restart alarm.
 */
void restart_alarm(void);

/**
 * @brief The callback function called in the interrupt of timer.
 * 
 * @param[in] dest_queue The pointer to msg queue.
 */
void alarm_callback_timer_isr(void *dest_queue);

#endif /* ALARM_H_ */
