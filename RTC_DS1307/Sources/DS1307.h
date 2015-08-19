/*
 * DS1307.h
 *
 *  Created on: Aug 2, 2015
 *      Author: nvhie
 */

#ifndef DS1307_H_
#define DS1307_H_

#include "i2cw.h"

typedef enum {
	SEC, MIN, HOUR, DAY, DATE, MONTH, YEAR, RTC_TIME
} RTC_time_t;

bool RTC_reinit(void);

bool RTC_set_time(RTC_time_t time_type, uint8_t *time);

bool RTC_get_time(RTC_time_t time_type, uint8_t *time);

#endif /* DS1307_H_ */
