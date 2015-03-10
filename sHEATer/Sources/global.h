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
#include "alarm.h"

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

extern button_t button_table[MAX_BUTTONS];
extern sensor_t LM35;
extern sensor_t Ve_ref;

/* Definitions for LW Message Queue Component */
#define NUM_MESSAGES		16
#define MSG_SIZE			1

/* Use light weight message queues */
extern uint32_t ctrl_msg_queue[sizeof(LWMSGQ_STRUCT) / sizeof(uint32_t)
		+ NUM_MESSAGES * MSG_SIZE];

#endif /* GLOBAL_H_ */
