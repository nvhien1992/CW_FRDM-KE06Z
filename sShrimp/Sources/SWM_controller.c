/*
 * SWM_controller.c
 *
 *  Created on: Apr 22, 2015
 *      Author: nvhie_000
 */
#include "SWM_controller.h"
#include "SIM900A.h"
#include "message_type.h"

#define NOTIFY_EN (1)
#define DEBUG_EN (1)
#include "debug.h"

void controller_app(pointer rcom_msg_queue, pointer controller_msg_queue,
		cir_queue_t *rcom_to_controller_queue,
		cir_queue_t *controller_to_rcom_queue) {
	SWM_msg_t msg;
	_lwmsgq_receive(controller_msg_queue, (_mqx_max_type_ptr) &msg,
			LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);

//	char *result = NULL;
	switch (msg.cmd) {
//	case :
//		break;
	default:
		break;
	}
}

