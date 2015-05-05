/*
 * SWM_controller.c
 *
 *  Created on: Apr 22, 2015
 *      Author: nvhie_000
 */
#include <cstring>
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
	switch (RCOM_get_status()) {
	case RCOM_HAS_NOTHING:
		msg.cmd = RCOM_START_CMD;
		_lwmsgq_send(rcom_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RCOM_STARTED:
		msg.cmd = RCOM_CONFIGURE_CMD;
		_lwmsgq_send(rcom_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RCOM_INITIALIZED:
		msg.cmd = RCOM_CONNECT_INTERNET_CMD;
		_lwmsgq_send(rcom_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RCOM_INTERNET_CONNECTED:
		break;
	default:
		break;
	}

	_lwmsgq_receive(controller_msg_queue, (_mqx_max_type_ptr) &msg,
			LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);
	char buffer_temp[128];
	uint8_t data_size = 0;
	switch (msg.cmd) {
	case RCOM_GET_TIMESTAMP_CMD:
		if (msg.content.result_type == RCOM_SUCCESS_WITH_DATA) {
			data_size = cir_queue_get_byte(rcom_to_controller_queue);
			DEBUG("datasize: %d\n", data_size);
			cir_queue_get_data(rcom_to_controller_queue, (uint8_t*) buffer_temp,
					data_size);
			DEBUG("timestamp: %s\n", buffer_temp);
		}
		return;
	case RCOM_HAVE_SMS_MSG_CMD:
		if (msg.content.result_type == RCOM_SUCCESS_WITH_DATA) {
			data_size = cir_queue_get_byte(rcom_to_controller_queue);
			DEBUG("datasize: %d\n", data_size);
			cir_queue_get_data(rcom_to_controller_queue, (uint8_t*) buffer_temp,
					data_size);
			buffer_temp[data_size] = '\0';
			DEBUG("%d\n", cir_queue_get_data_size(rcom_to_controller_queue));
			DEBUG("sms content: %s\n", buffer_temp);
		}
		return;
	case RCOM_HAVE_VOICE_CALL_CMD:
		if (msg.content.result_type == RCOM_SUCCESS_WITH_DATA) {
			data_size = cir_queue_get_byte(rcom_to_controller_queue);
			DEBUG("datasize: %d\n", data_size);
			cir_queue_get_data(rcom_to_controller_queue, (uint8_t*) buffer_temp,
					data_size);
			buffer_temp[data_size] = '\0';
			DEBUG("%d\n", cir_queue_get_data_size(rcom_to_controller_queue));
			DEBUG("Phone number: %s\n", buffer_temp);
		}
		return;
	case RCOM_START_CMD:
		if (msg.content.result_type == RCOM_SUCCESS_WITHOUT_DATA) {
			RCOM_set_status(RCOM_STARTED);
		}
		return;
	case RCOM_CONFIGURE_CMD:
		if (msg.content.result_type == RCOM_SUCCESS_WITHOUT_DATA) {
			RCOM_set_status(RCOM_INITIALIZED);
		}
		return;
	case RCOM_CONNECT_INTERNET_CMD:
		if (msg.content.result_type == RCOM_SUCCESS_WITHOUT_DATA) {
			RCOM_set_status(RCOM_INTERNET_CONNECTED);
			msg.cmd = RCOM_GET_TIMESTAMP_CMD;
			_lwmsgq_send(rcom_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		}
		return;
	default:
		return;
	}
}

