/*
 * remote_com.c
 *
 *  Created on: Apr 7, 2015
 *      Author: nvhie_000
 */
#include <cstring>
#include "remote_com.h"
#include "message_type.h"

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
char *VN_phone_code = "+84";

static bool set_APN_auto(void);

static bool set_APN_auto(void) {
	char *msp_name_tmp = sim900_get_MSP_name();
	if (!msp_name_tmp) {
		return FALSE ;
	}
	uint8_t index = 0;
	for (index = 0; index < MSP_NUMBER; index++) {
		if (strcmp(msp_name_tmp, msp_name[index]) == 0) {
			break;
		}
		if (index == MSP_NUMBER - 1) {
			NOTIFY("MSP is not supported!\n");
			return FALSE ;
		}
	}
	sim900_set_apn_para(apn_name[index], apn_usrname[index],
			apn_password[index]);

	return TRUE ;
}

void remote_com_app(pointer rcom_msg_queue, pointer controller_msg_queue,
		sync_buffer_t *rcom_to_controller_sync_buffer) {
	SWM_msg_t msg;
	if (_lwmsgq_receive(rcom_msg_queue, (_mqx_max_type_ptr) &msg,
			LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, NULL ) != MQX_OK) {
		DEBUG("ERR: receive msg failed\n");
	}

	char *result = NULL;
	char *rcom_to_controller_buffer = NULL;

	switch (msg.cmd) {
	case RCOM_START_CMD:
		DEBUG("RCOM starting...\n");
		if (!sim900_start()) {
			DEBUG("\tERR: SIM start failed\n");
			msg.result_type = FAIL;
			_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
			return;
		}
		msg.result_type = SUCCESS;
		_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RCOM_STOP_CMD:
		DEBUG("RCOM stoping...\n");
		sim900_stop();
		break;
	case RCOM_CONFIGURE_CMD:
		DEBUG("RCOM configuring...\n");
		if (!sim900_default_config()) {
			DEBUG("\tERR: SIM configuration failed\n");
			msg.result_type = FAIL;
			_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
			return;
		}

		DEBUG("RCOM checking SIM inserted...\n");
		if (!sim900_check_SIM_inserted()) {
			DEBUG("\tERR: SIM removed\n");
			msg.result_type = FAIL;
			_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
			return;
		}

		DEBUG("RCOM setting APN auto-ly...\n");
		if (!set_APN_auto()) {
			DEBUG("\tERR: Can't set APN\n");
			msg.result_type = FAIL;
			_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
			return;
		}

		msg.result_type = SUCCESS;
		_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RCOM_CONNECT_INTERNET_CMD:
		DEBUG("RCOM connecting internet...\n");
		if (!sim900_connect_internet()) {
			DEBUG("\tERR: Can't connect internet\n");
			msg.result_type = FAIL;
			_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
			return;
		}

		msg.result_type = SUCCESS;
		_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RCOM_SEND_SMS_MSG_CMD:
		DEBUG("RCOM sending SMS msg...\n");
//		sim900_send_sms("0947380243", "nvhien");
		break;
	case RCOM_HAVE_SMS_MSG_CMD:
		DEBUG("RCOM reading SMS msg...\n");
		result = sim900_read_SMS_msg(msg.content.value);
		rcom_to_controller_buffer = (char*) sync_buffer_get_resource_to_write(
				rcom_to_controller_sync_buffer);
		SIM900_SMS_msg_t sms_msg;
		sms_msg.phone_number = rcom_to_controller_buffer
				+ sizeof(SIM900_SMS_msg_t);
		sim900_get_phone_number_from_SMS_msg(result, sms_msg.phone_number);
		sim900_normalize_phone_number(sms_msg.phone_number, VN_phone_code);
		sms_msg.content = sms_msg.phone_number + strlen(sms_msg.phone_number)
				+ 1;
		sim900_get_content_from_SMS_msg(result, sms_msg.content);
		memcpy(rcom_to_controller_buffer, (void*) &sms_msg,
				sizeof(SIM900_SMS_msg_t));
		sync_buffer_allow_read(rcom_to_controller_sync_buffer);
		msg.result_type = SUCCESS;
		msg.content.ptr = (void*) rcom_to_controller_sync_buffer;
		_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RCOM_MAKE_MISSED_CALL_CMD:
		DEBUG("RCOM making missed voice call...\n");
//		sim900_make_missed_voice_call(buffer);
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
		if (!result) {
			msg.result_type = FAIL;
			_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
			return;
		}
		sync_buffer_write(rcom_to_controller_sync_buffer, (uint8_t*) result,
				strlen(result) + 1);
		sync_buffer_allow_read(rcom_to_controller_sync_buffer);
		msg.result_type = SUCCESS;
		msg.content.ptr = (void*) rcom_to_controller_sync_buffer;
		_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	default:
		break;
	}
	return;
}

void remote_com_RI_processing(pointer rcom_msg_queue,
		pointer controller_msg_queue,
		sync_buffer_t *ri_to_controller_sync_buffer) {
	char phone_number_or_sms_index[16];
	SWM_msg_t msg;

	switch (sim900_process_RI_task(phone_number_or_sms_index)) {
	case INCOMING_VOICE_CALL:
		msg.cmd = RCOM_HAVE_VOICE_CALL_CMD;
		sim900_normalize_phone_number(phone_number_or_sms_index, VN_phone_code);
		sync_buffer_write(ri_to_controller_sync_buffer,
				(uint8_t*) phone_number_or_sms_index,
				strlen(phone_number_or_sms_index) + 1);
		sync_buffer_allow_read(ri_to_controller_sync_buffer);
		msg.content.ptr = (void*) ri_to_controller_sync_buffer;
		_lwmsgq_send(controller_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	case RECEIVED_SMS_MSG:
		msg.cmd = RCOM_HAVE_SMS_MSG_CMD;
		msg.content.value = (uint8_t) *phone_number_or_sms_index;
		_lwmsgq_send(rcom_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	default:
		DEBUG("RI_UNKNOWN\n");
		break;
	}
}
