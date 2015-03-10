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

void enable_alarm(void);
void disable_alarm(void);
void restart_alarm(void);
void alarm_callback_timer_isr(void *dest_queue);

#endif /* ALARM_H_ */
