/*
 * remote_com.c
 *
 *  Created on: Apr 7, 2015
 *      Author: nvhie_000
 */
#include <cstring>
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

char result_buffer[256];

static RCOM_result_type_t set_APN_auto(void);

static RCOM_result_type_t set_APN_auto(void) {
	char msp_name_tmp[16];
	if (sim900_get_MSP_name(msp_name_tmp) == RCOM_FAIL) {
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

void remote_com_app(SWM_msg_t *swm_msg, void *dest_msg_queue) {
	RCOM_result_type_t res_type = RCOM_FAIL;
	switch (swm_msg->cmd) {
	case RCOM_START:
		DEBUG("RCOM starting...\n");
		sim900_start();
		sim900_check_SIM_inserted(res_type);
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
//		sim900_send_sms(((SIM900_SMS_package_t*) swm_msg)->phone_number,
//				((SIM900_SMS_package_t*) swm_msg)->msg_content);
		break;
	case RCOM_READ_SMS_MSG:
		DEBUG("RCOM reading SMS msg...\n");
		sim900_read_SMS_msg((uint8_t) swm_msg->content.value, result_buffer);
		break;
	case RCOM_MAKE_MISSED_CALL:
		DEBUG("RCOM making missed voice call...\n");
//		DEBUG("%s\n", swm_msg->content.value_ptr);
//		sim900_make_missed_voice_call(swm_msg->content.value_ptr);
		break;
	case RCOM_REPORT_DATA:
		break;
	case RCOM_UPDATE_DATA:
		break;
	case RCOM_POLL_COMMAND:
		break;
	case RCOM_GET_TIMESTAMP:
		DEBUG("RCOM getting timestamp...\n");
		sim900_HTTP_GET(URL_time_stamp, result_buffer);
		DEBUG("%s\n", result_buffer);
		break;
	default:
		break;
	}
	DEBUG("end\n");
}

