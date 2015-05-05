/*
 * remote_com.c
 *
 *  Created on: Apr 7, 2015
 *      Author: nvhie_000
 */
#include <cstring>
#include "mqxlite.h"
#include "remote_com.h"

#define NOTIFY_EN (1)
#define DEBUG_EN (1)
#include "debug.h"

/* The number of the Mobile Service Providers supported */
#define MSP_NUMBER (3)

/* The number of the default phone number be used to send SMS msg or make missed voice call */
#define USR_PN_NUMBER (1)

char *msp_name[MSP_NUMBER] = { "VIETTEL", "MOBIPHONE", "VINAPHONE" };
char *apn_name[MSP_NUMBER] = { "v-internet", "m-i090", "m3-world" };
char *apn_usrname[MSP_NUMBER] = { "", "mms", "mms" };
char *apn_password[MSP_NUMBER] = { "", "mms", "mms" };
char *URL_time_stamp = "i3s.edu.vn/swm/local-com/index.php?comType=timestamp";
char *URL_post_report = "i3s.edu.vn/swm/local-com/index.php?comType=report";
char *default_usr_phone_number[USR_PN_NUMBER] = { "0947380247", };

static bool add_result_to_cir_queue(cir_queue_t *cir_queue, char *response);
static RCOM_result_type_t set_APN_auto(void);

static bool add_result_to_cir_queue(cir_queue_t *cir_queue, char *response) {
	if (!response) {
		return false;
	}

	uint16_t data_size = strlen(response);

	if (cir_queue_overflowed_if_adding(cir_queue, data_size)) {
		return false;
	}

	cir_queue_add_byte(cir_queue, data_size);
	cir_queue_add_data(cir_queue, (uint8_t*) response, data_size);

	return true;
}

static RCOM_result_type_t set_APN_auto(void) {
	char *msp_name_tmp = sim900_get_MSP_name();
	if (!msp_name_tmp) {
		DEBUG("NULL buffer\n");
		return RCOM_FAIL;
	}
	uint8_t index = 0;
	for (index = 0; index < MSP_NUMBER; index++) {
		if (strcmp(msp_name_tmp, msp_name[index]) == 0) {
			break;
		}
		if (index == MSP_NUMBER - 1) {
			NOTIFY("MSP is not supported!\n");
			return RCOM_FAIL;
		}
	}
	sim900_set_apn_para(apn_name[index], apn_usrname[index],
			apn_password[index]);

	return RCOM_SUCCESS_WITHOUT_DATA;
}

void remote_com_app(pointer rcom_msg_queue, pointer controller_msg_queue,
		cir_queue_t *rcom_to_controller_queue,
		cir_queue_t *controller_to_rcom_queue) {
	SWM_msg_t msg;
	_lwmsgq_receive(rcom_msg_queue, (_mqx_max_type_ptr) &msg,
			LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL);

	uint8_t data_size = 0;
	char *result = NULL;
	char buffer[64];
	switch (msg.cmd) {
	case RCOM_START_CMD:
		DEBUG("RCOM starting...\n");
		cir_queue_clear(rcom_to_controller_queue);
		if (sim900_start() == RCOM_FAIL) {
			DEBUG("\tERR: SIM start failed\n");
			msg.content.value = (uint8_t) RCOM_FAIL;
			_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
			return;
		}
		msg.content.value = (uint8_t) RCOM_SUCCESS_WITHOUT_DATA;
		_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RCOM_STOP_CMD:
		DEBUG("RCOM stoping...\n");
		sim900_stop();
		break;
	case RCOM_CONFIGURE_CMD:
		DEBUG("RCOM configuring...\n");
		if (sim900_default_config() == RCOM_FAIL) {
			DEBUG("\tERR: SIM configuration failed\n");
			msg.content.value = (uint8_t) RCOM_FAIL;
			_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
			return;
		}

		DEBUG("RCOM checking SIM inserted...\n");
		if (!sim900_check_SIM_inserted()) {
			DEBUG("\tERR: SIM removed\n");
			msg.content.value = (uint8_t) RCOM_FAIL;
			_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
			return;
		}
		
		DEBUG("RCOM setting APN auto-ly...\n");
		if (set_APN_auto() == RCOM_FAIL) {
			DEBUG("\tERR: Can't set APN\n");
			msg.content.value = (uint8_t) RCOM_FAIL;
			_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
			return;
		}

		msg.content.value = (uint8_t) RCOM_SUCCESS_WITHOUT_DATA;
		_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RCOM_CONNECT_INTERNET_CMD:
		DEBUG("RCOM connecting internet...\n");
		if (sim900_connect_internet() == RCOM_FAIL) {
			DEBUG("\tERR: Can't connect internet\n");
			msg.content.value = (uint8_t) RCOM_FAIL;
			_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
			return;
		}

		msg.content.value = (uint8_t) RCOM_SUCCESS_WITHOUT_DATA;
		_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RCOM_SEND_SMS_MSG_CMD:
		DEBUG("RCOM sending SMS msg...\n");
//		data_size = cir_queue_preview_byte(controller_to_rcom_queue, TRUE);
//		cir_queue_preview_data(controller_to_rcom_queue, (uint8_t*) buffer,
//				data_size);
//		sim900_send_sms("0947380243", "nvhien");
		break;
	case RCOM_HAVE_SMS_MSG_CMD:
		DEBUG("RCOM reading SMS msg...\n");
		result = sim900_read_SMS_msg(msg.content.value);
		if (!add_result_to_cir_queue(rcom_to_controller_queue, result)) {
			DEBUG("\tERR: Read SMS msg failed\n");
			msg.content.value = (uint8_t) RCOM_FAIL;
			_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
			return;
		}
		msg.content.result_type = (uint8_t) RCOM_SUCCESS_WITH_DATA;
		_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RCOM_MAKE_MISSED_CALL_CMD:
		DEBUG("RCOM making missed voice call...\n");
		data_size = cir_queue_get_byte(controller_to_rcom_queue);
		cir_queue_get_data(controller_to_rcom_queue, (uint8_t*) buffer,
				data_size);
		buffer[data_size] = '\0';
		sim900_make_missed_voice_call(buffer);
		break;
	case RCOM_REPORT_DATA_CMD:
		break;
	case RCOM_UPDATE_DATA_CMD:
		break;
	case RCOM_POLL_COMMAND_CMD:
		break;
	case RCOM_GET_TIMESTAMP_CMD:
		DEBUG("RCOM getting timestamp...\n");
		result = sim900_HTTP_GET(URL_time_stamp);
		if (!add_result_to_cir_queue(rcom_to_controller_queue, result)) {
			DEBUG("\tERR: Get timestamp failed\n");
			msg.content.value = (uint8_t) RCOM_FAIL;
			_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
			return;
		}
		msg.content.result_type = (uint8_t) RCOM_SUCCESS_WITH_DATA;
		_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	default:
		break;
	}
}

void remote_com_RI_processing(pointer rcom_msg_queue,
		pointer controller_msg_queue, cir_queue_t *rcom_to_controller_queue) {
	char phone_number_or_sms_content[16];
	SWM_msg_t msg;

	switch (sim900_process_RI_task(phone_number_or_sms_content)) {
	case INCOMING_VOICE_CALL:
		if (!add_result_to_cir_queue(rcom_to_controller_queue,
				phone_number_or_sms_content)) {
			return;
		}
		msg.cmd = RCOM_HAVE_VOICE_CALL_CMD;
		msg.content.result_type = (uint8_t) RCOM_SUCCESS_WITH_DATA;
		_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RECEIVED_SMS_MSG:
		msg.cmd = RCOM_HAVE_SMS_MSG_CMD;
		msg.content.value = (uint8_t) *phone_number_or_sms_content;
		_lwmsgq_send(rcom_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	default:
		DEBUG("RI_UNKNOWN\n");
		break;
	}
}

