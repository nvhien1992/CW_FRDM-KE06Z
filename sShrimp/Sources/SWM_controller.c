/**
 @file SWM_controller.c
 @brief
 @author <b>Nguyen Van Hien</b> <nvhien1992@gmail.com>
 @copyright Copyright &copy; 2015, <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b>, All rights reserved. 
 */
#include <cstring>
#include "SWM_controller.h"
#include "message_type.h"

#define NOTIFY_EN (1)
#define DEBUG_EN (1)
#include "debug.h"

void controller_app(pointer rcom_msg_queue, pointer controller_msg_queue) {
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
	case RCOM_CONFIGURED:
		msg.cmd = RCOM_CONNECT_INTERNET_CMD;
		_lwmsgq_send(rcom_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RCOM_INTERNET_CONNECTED:
		break;
	default:
		break;
	}

	_lwmsgq_receive(controller_msg_queue, (_mqx_max_type_ptr) &msg,
			LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL );

	char *result = NULL;

	switch (msg.cmd) {
	case RCOM_GET_TIMESTAMP_CMD:
		if (msg.result_type == SUCCESS) {
			result = (char*) sync_buffer_read((sync_buffer_t*) msg.content.ptr);
			DEBUG("timestamp: %s\n", result);
			sync_buffer_allow_write((sync_buffer_t*) msg.content.ptr);
		}
		return;
	case RCOM_HAVE_SMS_MSG_CMD:
		if (msg.result_type == SUCCESS) {
			result = (char*) sync_buffer_read((sync_buffer_t*) msg.content.ptr);
			DEBUG(
					"pn: %s, content: %s\n", ((SIM900_SMS_msg_t*)result)->phone_number, ((SIM900_SMS_msg_t*)result)->content);
			sync_buffer_allow_write((sync_buffer_t*) msg.content.ptr);
		}
		return;
	case RCOM_HAVE_VOICE_CALL_CMD:
		DEBUG(
				"pn: %s\n", (char*) sync_buffer_read(((sync_buffer_t*)msg.content.ptr)));
		sync_buffer_allow_write(((sync_buffer_t*) msg.content.ptr));
		return;
	case RCOM_START_CMD:
		if (msg.result_type == SUCCESS) {
			RCOM_set_status(RCOM_STARTED);
		}
		return;
	case RCOM_CONFIGURE_CMD:
		if (msg.result_type == SUCCESS) {
			RCOM_set_status(RCOM_CONFIGURED);
		}
		return;
	case RCOM_CONNECT_INTERNET_CMD:
		if (msg.result_type == SUCCESS) {
			DEBUG("RCOM internet connected\n");
			RCOM_set_status(RCOM_INTERNET_CONNECTED);
		}
		return;
	default:
		return;
	}
}

