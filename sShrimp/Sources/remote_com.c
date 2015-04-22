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

char *msp_name[MSP_NUMBER] = { "VIETTEL", "MOBIPHONE", "VINAPHONE" };
char *apn_name[MSP_NUMBER] = { "v-internet", "m-i090", "m3-world" };
char *apn_usrname[MSP_NUMBER] = { "", "mms", "mms" };
char *apn_password[MSP_NUMBER] = { "", "mms", "mms" };
char *URL_time_stamp = "i3s.edu.vn/swm/local-com/index.php?comType=timestamp";
char *URL_post_report = "i3s.edu.vn/swm/local-com/index.php?comType=report";

static RCOM_result_type_t set_APN_auto(void);

static RCOM_result_type_t set_APN_auto(void) {
	char *msp_name_tmp = sim900_get_MSP_name();
	if (msp_name_tmp == NULL) {
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
	case RCOM_START:
		DEBUG("RCOM starting...\n");
		cir_queue_clear(rcom_to_controller_queue);
		sim900_start();
		DEBUG("Checking SIM inserted...\n");
		if (!sim900_check_SIM_inserted()) {
			NOTIFY("SIM removed\n");
		}
		DEBUG("Setting APN auto-ly...\n");
		set_APN_auto();
		break;
	case RCOM_STOP:
		DEBUG("RCOM stoping...\n");
		sim900_stop();
		break;
	case RCOM_CONFIGURE:
		DEBUG("RCOM configuring...\n");
		sim900_default_config();
		break;
	case RCOM_CONNECT_INTERNET:
		DEBUG("RCOM connecting internet...\n");
		sim900_connect_internet();
		break;
	case RCOM_SEND_SMS_MSG:
		DEBUG("RCOM sending SMS msg...\n");
//		data_size = cir_queue_preview_byte(controller_to_rcom_queue, TRUE);
//		cir_queue_preview_data(controller_to_rcom_queue, (uint8_t*) buffer,
//				data_size);
//		sim900_send_sms("0947380243", "nvhien");
		break;
	case RCOM_READ_SMS_MSG:
		DEBUG("RCOM reading SMS msg...\n");
		result = sim900_read_SMS_msg(msg.content.value);
		DEBUG("%s\n", result);
		break;
	case RCOM_MAKE_MISSED_CALL:
		DEBUG("RCOM making missed voice call...\n");
		data_size = cir_queue_preview_byte(controller_to_rcom_queue, FALSE);
		cir_queue_preview_data(controller_to_rcom_queue, (uint8_t*) buffer,
				data_size);
		buffer[data_size] = '\0';
		DEBUG("pn: %s\n", buffer);
		sim900_make_missed_voice_call(buffer);
		break;
	case RCOM_REPORT_DATA:
		break;
	case RCOM_UPDATE_DATA:
		break;
	case RCOM_POLL_COMMAND:
		break;
	case RCOM_GET_TIMESTAMP:
		DEBUG("RCOM getting timestamp...\n");
		result = sim900_HTTP_GET(URL_time_stamp);
		DEBUG("%s\n", result);
		break;
	default:
		break;
	}
	DEBUG("end\n");
}

void remote_com_RI_processing(pointer rcom_msg_queue,
		pointer controller_msg_queue, cir_queue_t *rcom_to_controller_queue,
		cir_queue_t *controller_to_rcom_queue) {
	char phone_number_or_sms_content[16];
	SWM_msg_t msg;
	uint8_t sms_index = 0;
	switch (sim900_process_RI_task(phone_number_or_sms_content)) {
	case INCOMING_VOICE_CALL:
		DEBUG("Phone number: %s\n", phone_number_or_sms_content);
		break;
	case RECEIVED_SMS_MSG:
		sms_index = (uint8_t) *phone_number_or_sms_content;
		DEBUG("SMS index: %d\n", sms_index);
		msg.cmd = RCOM_READ_SMS_MSG;
		msg.content.value = sms_index;
		_lwmsgq_send(rcom_msg_queue, (_mqx_max_type_ptr) &msg, 0);
		break;
	default:
		DEBUG("RI_UNKNOWN\n");
		break;
	}
}

