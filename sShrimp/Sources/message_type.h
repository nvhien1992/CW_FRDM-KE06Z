/*
 * message_type.h
 *
 *  Created on: Apr 16, 2015
 *      Author: nvhie_000
 */

#ifndef MESSAGE_TYPE_H_
#define MESSAGE_TYPE_H_

#include "PE_Types.h"

typedef enum {
	RCOM_START_CMD,
	RCOM_STOP_CMD,
	RCOM_CONFIGURE_CMD,
	RCOM_CONNECT_INTERNET_CMD,
	RCOM_HAVE_VOICE_CALL_CMD,
	RCOM_HAVE_SMS_MSG_CMD,
	RCOM_SEND_SMS_MSG_CMD,
	RCOM_READ_SMS_MSG_CMD,
	RCOM_MAKE_MISSED_CALL_CMD,
	RCOM_REPORT_DATA_CMD,
	RCOM_UPDATE_DATA_CMD,
	RCOM_POLL_COMMAND_CMD,
	RCOM_GET_TIMESTAMP_CMD,
} SWM_cmd_t;

typedef enum {
	FAIL,
	SUCCESS,
} result_type_t;

typedef struct {
	SWM_cmd_t cmd;
	result_type_t result_type;
	union {
		uint32_t value;
		void *ptr;
	} content;
} SWM_msg_t;

#endif /* MESSAGE_TYPE_H_ */
