/*
 * global.h
 *
 *  Created on: Mar 17, 2015
 *      Author: nvhie_000
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>
#include <stdbool.h>
#include "alarm.h"

/* Definitions for LW Message Queue Component */
#define NUM_MESSAGES	16
#define MSG_SIZE		1

/* Use light weight message queues */
extern uint32_t ctrl_msg_queue[sizeof(LWMSGQ_STRUCT) / sizeof(uint32_t)
		+ NUM_MESSAGES * MSG_SIZE];

extern uint32_t remote_com_msg_queue[sizeof(LWMSGQ_STRUCT) / sizeof(uint32_t)
		+ NUM_MESSAGES * MSG_SIZE];

#endif /* GLOBAL_H_ */
