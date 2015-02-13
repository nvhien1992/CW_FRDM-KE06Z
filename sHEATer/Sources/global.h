/*
 * global.h
 *
 *  Created on: Feb 8, 2015
 *      Author: nvhie_000
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>
#include <stdbool.h>
#include "mqxlite.h"
#include "button.h"
#include "sensor.h"
#include "on_off_device.h"

#define MAX_BUTTONS 2

typedef enum {
	UNKNOWN = 0x00,
	RUN_STOP_BTN = 0x01,
	ALARM_BTN = 0x02,
	ADD_BTN = 0x03,
	SUB_BTN = 0x04,
	TEMP_SS = 0x05,
	ALARM = 0x06,
	HEATER = 0x80,
} dev_id_e;

typedef enum {
	ADC_EVT_BIT_MASK = 0x01,
	BTN_EVT_BIT_MASK = 0x02,
	A_MIN_EVT_BIT_MASK = 0x03,
} event_bit_mask_e;

extern button_t button_table[MAX_BUTTONS];
extern temp_sensor_t LM35;

extern LWEVENT_STRUCT adc_lwevent;
extern LWEVENT_STRUCT btn_lwevent;
extern LWEVENT_STRUCT a_min_lwevent;

#endif /* GLOBAL_H_ */
