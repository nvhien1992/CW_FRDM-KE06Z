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
	RCOM_START,
	RCOM_STOP,
	RCOM_CONFIGURE,
	RCOM_CONNECT_INTERNET,
	RCOM_HAVE_VOICE_CALL,
	RCOM_HAVE_SMS_MSG,
	RCOM_SEND_SMS_MSG,
	RCOM_READ_SMS_MSG,
	RCOM_MAKE_MISSED_CALL,
	RCOM_REPORT_DATA,
	RCOM_UPDATE_DATA,
	RCOM_POLL_COMMAND,
	RCOM_GET_TIMESTAMP,
} SWM_cmd_t;

typedef struct {
	SWM_cmd_t cmd;
	union {
		uint32_t value;
		char* value_ptr;
	} content;
} SWM_msg_t;

#endif /* MESSAGE_TYPE_H_ */
