#include "remote_com_resources.h"
#include "sim900_int.h"

#define NOTIFY_EN (1)
#define DEBUG_EN (1)
#include "debug.h"

SIM900_pins_t *SIM900_pins;

void sim900_power_on() {
	SIM900_pins->nPW.ClrVal(NULL );
}

void sim900_power_off(void) {
	SIM900_pins->nPW.SetVal(NULL );
}

//void sim900_term_http_session(void) {
//	sim900_send_more("AT+HTTPTERM\r\n");
//}

//void sim900_send_cmd(char* cmd) {
//	RCOM_clear_rx_buf();
//	RCOM_uart_writef(cmd);
//	DEBUG("cmd: %s\n", cmd);
//}

void sim900_send_more(char* data) {
	RCOM_uart_writef(data);
	DEBUG("data: %s\n", data);
}

char* sim900_get_http_res_content(void) {
	uint8_t line_feed_num = 0;
	bool has_HPlus = FALSE;
	uint16_t line_feed_pos[2];

	/* find +H */
	uint16_t start = 0;
	for (start = 0; start < RCOM_get_rx_buf_len(); start++) {
		if ((rx_buf[start] == '+') && (rx_buf[start + 1] == 'H')) {
			has_HPlus = TRUE;
			break;
		}
	}

	/* has +H */
	if (has_HPlus) {
		uint16_t i = 0;
		for (i = start; i < RCOM_get_rx_buf_len(); i++) { //find line_feed_pos
			if (rx_buf[i] == '\n') {
				line_feed_pos[line_feed_num] = i;
				line_feed_num++;
				if (line_feed_num == 2)
					break;
			}
		}

		if (line_feed_num == 2) {
			//set new end string
			rx_buf[line_feed_pos[1] - 1] = '\0';
			return &rx_buf[line_feed_pos[0] + 1];
		}
	}

	return NULL ;
}

void sim900_pin_init(SIM900_pins_t *defined_pins) {
	SIM900_pins = defined_pins;
//	SIM900_pins->RI.Init(NULL );
}

void sim900_start_device(RCOM_job_result_t *job_result) {
	RCOM_step_info_t step_info;
	char cmd[128];
	step_info.command = cmd;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_START_DEVICE_STEP_NUM;
			current_step++) {
		switch (current_step) {
		case 0:
			sim900_power_off();
			SIM900_pins->nRST.SetVal(NULL );

			RCOM_get_step_info(START_DEVICE, current_step, &step_info);
			RCOM_step_excution(&step_info);
			break;
		case 1: //3s
			sim900_power_on();
			SIM900_pins->nRST.ClrVal(NULL );

			RCOM_get_step_info(START_DEVICE, current_step, &step_info);
			RCOM_step_excution(&step_info);
			break;
		default:
			break;
		}
	}
	job_result->result_type = JOB_SUCCESS_WITHOUT_DATA;
	return;
}

void sim900_config_device(RCOM_job_result_t *job_result) {
	RCOM_step_info_t step_info;
	char cmd[128];
	step_info.command = cmd;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_CONFIG_DEVICE_STEP_NUM;
			current_step++) {
		RCOM_get_step_info(CONFIG_DEVICE, current_step, &step_info);
		if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
			job_result->result_type = JOB_FAIL;
			job_result->content.int_result = current_step;
			DEBUG("step failed: %d\n", current_step);
			return;
		}
	}
	job_result->result_type = JOB_SUCCESS_WITHOUT_DATA;
}

void sim900_connect_internet(RCOM_job_result_t *job_result) {
	RCOM_step_info_t step_info;
	char cmd[128];
	step_info.command = cmd;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_CONNECT_INTERNET_STEP_NUM;
			current_step++) {
		RCOM_get_step_info(CONNECT_INTERNET, current_step, &step_info);
		if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
			job_result->result_type = JOB_FAIL;
			job_result->content.int_result = current_step;
			DEBUG("step failed: %d\n", current_step);
			return;
		}
	}
	job_result->result_type = JOB_SUCCESS_WITHOUT_DATA;
}

void sim900_stop_device(RCOM_job_result_t *job_result) {
	RCOM_step_info_t step_info;
	char cmd[128];
	step_info.command = cmd;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_STOP_DEVICE_STEP_NUM;
			current_step++) {
		switch (current_step) {
		case 0: //3s
			SIM900_pins->nRST.SetVal(NULL );
			RCOM_get_step_info(STOP_DEVICE, current_step, &step_info);
			RCOM_step_excution(&step_info);
			break;
		case 1: //3s
			SIM900_pins->nRST.ClrVal(NULL );
			RCOM_get_step_info(STOP_DEVICE, current_step, &step_info);
			RCOM_step_excution(&step_info);
			break;
		case 2: //1s
			sim900_power_off();
			RCOM_get_step_info(STOP_DEVICE, current_step, &step_info);
			RCOM_step_excution(&step_info);
			break;
		default:
			break;
		}
	}
	job_result->result_type = JOB_SUCCESS_WITHOUT_DATA;
	return;
}

//void sim900_post_report(gw_rpt_para_t* pRptPara, RCOM_job_result_t *job_result) {
//	RCOM_step_info_t step_info;
//	char cmd[128];
//step_info.command = cmd;
//	RCOM_clear_rx_buf();
//
//	char str_temp[150];
//	uint16_t data_size = 0;
//	uint8_t i = 0;
//	bool job_failed = FALSE;
//	uint8_t current_step = 0;
//	for (current_step = 0; currernt_step < SIM900_HTTP_POST_STEP_NUM;
//			current_step++) {
//		switch (current_step) {
//		case 0:
//		case 1:
//		case 2:
//		case 3:
//			RCOM_get_step_info(REPORT_DATA, current_step, &step_info);
//			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
//				current_step = 7;
//				job_failed = TRUE;
//job_result->content.int_result = current_step;
//			}
//			break;
//		case 4:
//			/* calculate the total number of sending bytes */
//			/* communication Type */
//			sprintf(str_temp, "comType=report");
//			data_size += strlen(str_temp);
//			/* farm-Name */
//			sprintf(str_temp, "&fN=%s", (char *) GWM_GetName());
//			data_size += strlen(str_temp);
//			/* received-Data-Number  */
//			sprintf(str_temp, "&rDN=%lx", pRptPara->dataIndex); //NEED TIN DOUBLE CHECK
//			data_size += strlen(str_temp);
//			/* number-Data */
//			sprintf(str_temp, "&nD=%x", pRptPara->numData);
//			data_size += strlen(str_temp);
//			/* report-Data */
//			for (i = 0; i < pRptPara->numData; i++) {
//				/* report-Data-NodeId */
//				sprintf(str_temp, "&n%d=%x", i, pRptPara->rptData[i].ndId);
//				data_size += strlen(str_temp);
//				/* report-Data-SensorId */
//				sprintf(str_temp, "&d%d=%x", i, pRptPara->rptData[i].ssId);
//				data_size += strlen(str_temp);
//				/* report-Data-SensorValue */
//				sprintf(str_temp, "&v%d=%x", i, pRptPara->rptData[i].ssVal);
//				data_size += strlen(str_temp);
//				/* report-Data-SensingTime */
//				sprintf(str_temp, "&sT%d=%lx", i, pRptPara->rptData[i].senTime);
//				data_size += strlen(str_temp);
//			}
//			/* Send MessageSize */
//			sprintf(str_temp, "AT+HTTPDATA=%d,%ld\r\n", data_size, 50000);
//			sim900_send_more(str_temp); //WHY send_more. send_cmd???
//
//			RCOM_get_step_info(REPORT_DATA, current_step, &step_info);
//			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
//				current_step = 7;
//				job_failed = TRUE;
//job_result->content.int_result = current_step;
//			}
//			break;
//		case 5:
//			/* send report message */
//			/* communication Type */
//			sprintf(str_temp, "comType=report");
//			sim900_send_more(str_temp);
//			/* farm-Name */
//			sprintf(str_temp, "&fN=%s", (char *) GWM_GetName());
//			sim900_send_more(str_temp);
//			/* received-Data-Number  */
//			sprintf(str_temp, "&rDN=%lx", pRptPara->dataIndex); //NEED TIN DOUBLE CHECK
//			sim900_send_more(str_temp);
//			/* number-Data */
//			sprintf(str_temp, "&nD=%x", pRptPara->numData);
//			sim900_send_more(str_temp);
//			/* report-Data */
//			for (i = 0; i < pRptPara->numData; i++) {
//				/* report-Data-NodeId */
//				sprintf(str_temp, "&n%d=%x", i, pRptPara->rptData[i].ndId);
//				sim900_send_more(str_temp);
//				/* report-Data-SensorId */
//				sprintf(str_temp, "&d%d=%x", i, pRptPara->rptData[i].ssId);
//				sim900_send_more(str_temp);
//				/* report-Data-SensorValue */
//				sprintf(str_temp, "&v%d=%x", i, pRptPara->rptData[i].ssVal); //COULD IT SUPPORT negative number ???
//				sim900_send_more(str_temp);
//				/* report-Data-SensingTime */
//				sprintf(str_temp, "&sT%d=%lx", i, pRptPara->rptData[i].senTime);
//				sim900_send_more(str_temp);
//			}
//			RCOM_get_step_info(REPORT_DATA, current_step, &step_info);
//			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
//				current_step = 7;
//				job_failed = TRUE;
//job_result->content.int_result = current_step;
//			}
//			break;
//		case 6:
//		case 7:
//			RCOM_get_step_info(REPORT_DATA, current_step, &step_info);
//			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
//				current_step = 7;
//				job_failed = TRUE;
//job_result->content.int_result = current_step;
//				break;
//			}
//			//TODO: Parse data from UART (Get HTTP response from web server)
//			break;
//		case 8:
//			RCOM_disable_rx_buf();
//			RCOM_get_step_info(REPORT_DATA, current_step, &step_info);
//			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
//				job_failed = TRUE;
//job_result->content.int_result = current_step;
//			}
//			break;
//		case 9:
//			RCOM_enable_rx_buf();
//			break;
//		default:
//			job_result->result_type = JOB_FAIL;
//			return;
//		}
//	}
//	if(job_failed) {
//		job_result->result_type = JOB_FAIL;
//	} else {
//		job_result->result_type = JOB_SUCCESS_WITH_DATA;
//	}
//}
//
//void SWM_Get_Unit_Str(ss_unit_t unit, char unit_str[]) {
//	/* Get unit prefix */
//	switch (unit & SS_UNIT_PREFIX_MASK) {
//	case SS_PREFIX_NORMAL: //None-prefix
//		strcpy(unit_str, "");
//		break;
//	case SS_PREFIX_KILO:
//		strcpy(unit_str, "K");
//		break;
//	case SS_PREFIX_MEGA:
//		strcpy(unit_str, "M");
//		break;
//	case SS_PREFIX_GIGA:
//		strcpy(unit_str, "G");
//		break;
//	case SS_PREFIX_MILI:
//		strcpy(unit_str, "m");
//		break;
//	case SS_PREFIX_MICRO:
//		strcpy(unit_str, "u");
//		break;
//	case SS_PREFIX_NANO:
//		strcpy(unit_str, "n");
//		break;
//	case SS_PREFIX_PICO:
//		strcpy(unit_str, "p");
//		break;
//	default:
//		strcpy(unit_str, "");
//		break;
//	}
//	/* Get unit */
//	switch (unit & SS_UNIT_MASK) {
//	case SS_UNIT_ON_OFF:
//		strcat(unit_str, "ON/OFF");
//		break;
//	case SS_UNIT_PPM:
//		strcat(unit_str, "ppm");
//		break;
//	case SS_UNIT_PERCENT:
//		strcat(unit_str, "%");
//		break;
//	case SS_UNIT_TMP_C:
//		strcat(unit_str, "oC");
//		break;
//	case SS_UNIT_TMP_K:
//		strcat(unit_str, "K");
//		break;
//	case SS_UNIT_TMP_F:
//		strcat(unit_str, "F");
//		break;
//	case SS_UNIT_PH:
//		strcat(unit_str, "pH");
//		break;
//	case SS_UNIT_MG_L:
//		strcat(unit_str, "mg/l");
//		break;
//	case SS_UNIT_LUX:
//		strcat(unit_str, "lux");
//		break;
//	case SS_UNIT_VOLTAGE:
//		strcat(unit_str, "V");
//		break;
//	case SS_UNIT_AMPERE:
//		strcat(unit_str, "A");
//		break;
//	default:
//		break;
//	}
//}
//
//void sim900_post_update(RCOM_job_result_t *job_result) {
//	RCOM_step_info_t step_info;
//	char cmd[128];
//step_info.command = cmd;
//	RCOM_clear_rx_buf();
//
//	char str_temp[150];
//	char unit_str[10];
//	uint16_t data_size = 0;
//	bool job_failed = FALSE;
//	uint8_t i, j;
//	swm_gw_mng_t* pGateway;
//	node_stt_t nodeStt;
//	uint8_t current_step = 0;
//	for (current_step = 0; current_step < SIM900_HTTP_POST_STEP_NUM;
//			current_step++) {
//		switch (current_step) {
//		case 0:
//		case 1:
//		case 2:
//		case 3:
//			RCOM_get_step_info(UPDATE_DATA, current_step, &step_info);
//			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
//				current_step = 7;
//				job_failed = TRUE;
//job_result->content.int_result = current_step;
//			}
//			break;
//		case 4:
//			pGateway = GWM_GetGWMng();
//			/* calculate the total number of sending bytes */
//			/* communication Type */
//			sprintf(str_temp, "comType=update");
//			data_size += strlen(str_temp);
//			/* farm-Name */
//			sprintf(str_temp, "&fN=%s", (char *) GWM_GetName());
//			data_size += strlen(str_temp);
//			for (i = 0; i < pGateway->numNode; i++) {
//				nodeStt = pGateway->nodeStt[i];
//				for (j = 0; j < nodeStt.numDev; j++) {
//					/* update-Data-NodeId */
//					sprintf(str_temp, "&n%d=%x", i + j, nodeStt.id);
//					data_size += strlen(str_temp);
//					/* update-Data-Total-Data-Number */
//					//sprintf(str_temp, "&tDN%d=%x", i+j, );
//					//data_size += strlen(str_temp);
//					/* update-Data-Last-Data-Index */
//					//sprintf(str_temp, "&lDI%d=%x", i+j, );
//					//data_size += strlen(str_temp);
//					/* update-Data-activeTime */
//					sprintf(str_temp, "&aT%d=%lx", i + j, nodeStt.activedTime);
//					data_size += strlen(str_temp);
//					/* update-Data-SensorId */
//					sprintf(str_temp, "&d%d=%x", i + j, nodeStt.ssStt[j].id);
//					data_size += strlen(str_temp);
//					/* update-Data-Unit */
//					SWM_Get_Unit_Str(nodeStt.ssStt[j].unit, unit_str);
//					sprintf(str_temp, "&u%d=%s", i + j, unit_str);
//					data_size += strlen(str_temp);
//					/* update-Data-low-Emergency-Set */
//					sprintf(str_temp, "&lES%d=%x", i + j,
//							nodeStt.ssStt[j].lowEmgSet);
//					data_size += strlen(str_temp);
//					/* update-Data-low-Emergency-Clear */
//					sprintf(str_temp, "&lEC%d=%x", i + j,
//							nodeStt.ssStt[j].lowEmgClear);
//					data_size += strlen(str_temp);
//					/* update-Data-low-Warning-Set */
//					sprintf(str_temp, "&lWS%d=%x", i + j,
//							nodeStt.ssStt[j].lowWarnSet);
//					data_size += strlen(str_temp);
//					/* update-Data-low-Warning-Clear */
//					sprintf(str_temp, "&lWC%d=%x", i + j,
//							nodeStt.ssStt[j].lowWarnClear);
//					data_size += strlen(str_temp);
//					/* update-Data-high-Warning-Clear */
//					sprintf(str_temp, "&hWC%d=%x", i + j,
//							nodeStt.ssStt[j].highWarnClear);
//					data_size += strlen(str_temp);
//					/* update-Data-high-Warning-Set */
//					sprintf(str_temp, "&hWS%d=%x", i + j,
//							nodeStt.ssStt[j].highWarnSet);
//					data_size += strlen(str_temp);
//					/* update-Data-highEmergencyClear */
//					sprintf(str_temp, "&hEC%d=%x", i + j,
//							nodeStt.ssStt[j].highEmgClear);
//					data_size += strlen(str_temp);
//					/* update-Data-high-Emergency-Set */
//					sprintf(str_temp, "&hES%d=%x", i + j,
//							nodeStt.ssStt[j].highEmgSet);
//					data_size += strlen(str_temp);
//				}
//			}
//			/* Send MessageSize */
//			sprintf(str_temp, "AT+HTTPDATA=%d,%ld\r\n", data_size, 50000);
//			sim900_send_more(str_temp);
//			RCOM_get_step_info(UPDATE_DATA, current_step, &step_info);
//			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
//				current_step = 7;
//				job_failed = TRUE;
//job_result->content.int_result = current_step;
//			}
//			break;
//		case 5:
//			pGateway = GWM_GetGWMng();
//			/* communication Type */
//			sprintf(str_temp, "comType=update");
//			sim900_send_more(str_temp);
//			/* farm-Name */
//			sprintf(str_temp, "&fN=%s", (char *) GWM_GetName());
//			sim900_send_http_data(str_temp, 1);
//			for (i = 0; i < pGateway->numNode; i++) {
//				nodeStt = pGateway->nodeStt[i];
//				for (j = 0; j < nodeStt.numDev; j++) {
//					/* update-Data-NodeId */
//					sprintf(str_temp, "&n%d=%x", i + j, nodeStt.id);
//					sim900_send_more(str_temp);
//					/* update-Data-Total-Data-Number */
//					//sprintf(str_temp, "&tDN%d=%x", i+j, );
//					//sim900_send_more(str_temp);
//					/* update-Data-Last-Data-Index */
//					//sprintf(str_temp, "&lDI%d=%x", i+j, );
//					//sim900_send_more(str_temp);
//					/* update-Data-activeTime */
//					sprintf(str_temp, "&aT%d=%lx", i + j, nodeStt.activedTime);
//					sim900_send_more(str_temp);
//					/* update-Data-SensorId */
//					sprintf(str_temp, "&d%d=%x", i + j, nodeStt.ssStt[j].id);
//					sim900_send_more(str_temp);
//					/* update-Data-Unit */
//					SWM_Get_Unit_Str(nodeStt.ssStt[j].unit, unit_str);
//					sprintf(str_temp, "&u%d=%s", i + j, unit_str);
//					sim900_send_more(str_temp);
//					/* update-Data-low-Emergency-Set */
//					sprintf(str_temp, "&lES%d=%x", i + j,
//							nodeStt.ssStt[j].lowEmgSet);
//					sim900_send_more(str_temp);
//					/* update-Data-low-Emergency-Clear */
//					sprintf(str_temp, "&lEC%d=%x", i + j,
//							nodeStt.ssStt[j].lowEmgClear);
//					sim900_send_more(str_temp);
//					/* update-Data-low-Warning-Set */
//					sprintf(str_temp, "&lWS%d=%x", i + j,
//							nodeStt.ssStt[j].lowWarnSet);
//					sim900_send_more(str_temp);
//					/* update-Data-low-Warning-Clear */
//					sprintf(str_temp, "&lWC%d=%x", i + j,
//							nodeStt.ssStt[j].lowWarnClear);
//					sim900_send_more(str_temp);
//					/* update-Data-high-Warning-Clear */
//					sprintf(str_temp, "&hWC%d=%x", i + j,
//							nodeStt.ssStt[j].highWarnClear);
//					sim900_send_more(str_temp);
//					/* update-Data-high-Warning-Set */
//					sprintf(str_temp, "&hWS%d=%x", i + j,
//							nodeStt.ssStt[j].highWarnSet);
//					sim900_send_more(str_temp);
//					/* update-Data-highEmergencyClear */
//					sprintf(str_temp, "&hEC%d=%x", i + j,
//							nodeStt.ssStt[j].highEmgClear);
//					sim900_send_more(str_temp);
//					/* update-Data-high-Emergency-Set */
//					sprintf(str_temp, "&hES%d=%x", i + j,
//							nodeStt.ssStt[j].highEmgSet);
//					sim900_send_more(str_temp);
//				}
//			}
//			RCOM_get_step_info(UPDATE_DATA, current_step, &step_info);
//			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
//				current_step = 7;
//				job_failed = TRUE;
//job_result->content.int_result = current_step;
//			}
//			break;
//		case 6:
//		case 7:
//			RCOM_get_step_info(UPDATE_DATA, current_step, &step_info);
//			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
//				current_step = 7;
//				job_failed = TRUE;
//job_result->content.int_result = current_step;
//				break;
//			}
//			//TODO: Parse HTTP response in step 7
//			break;
//		case 8:
//			RCOM_disable_rx_buf();
//			RCOM_get_step_info(UPDATE_DATA, current_step, &step_info);
//			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
//				job_failed = TRUE;
//job_result->content.int_result = current_step;
//			}
//			break;
//		case 9:
//			RCOM_enable_rx_buf();
//			break;
//		default:
//			job_result->result_type = JOB_FAIL;
//			return;
//		}
//	}
//	if(job_failed) {
//		job_result->result_type = JOB_FAIL;
//	} else {
//		job_result->result_type = JOB_SUCCESS_WITH_DATA;
//	}
//}

void sim900_get_timestamp(RCOM_job_result_t *job_result) {
	RCOM_step_info_t step_info;
	char cmd[128];
	step_info.command = cmd;
	RCOM_clear_rx_buf();
	bool job_failed = FALSE;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_HTTP_GET_STEP_NUM;
			current_step++) {
		switch (current_step) {
		case 0:
		case 1:
		case 2:
		case 3:
			RCOM_get_step_info(GET_TIMESTAMP, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				current_step = 4;
				job_failed = TRUE;
				job_result->content.int_result = current_step;
				DEBUG("step failed: %d\n", current_step);
			}
			break;
		case 4:
			RCOM_get_step_info(GET_TIMESTAMP, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				current_step = 4;
				job_failed = TRUE;
				job_result->content.int_result = current_step;
				DEBUG("step failed: %d\n", current_step);
				break;
			}
			//TODO: Parse HTTP response in step 4
			break;
		case 5:
//			RCOM_disable_rx_buf();
			RCOM_get_step_info(GET_TIMESTAMP, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				job_failed = TRUE;
				job_result->content.int_result = current_step;
				DEBUG("step failed: %d\n", current_step);
			}
			break;
		case 6:
//			RCOM_enable_rx_buf();
			break;
		default:
			break;
		}
	}
	if (job_failed) {
		job_result->result_type = JOB_FAIL;
	} else {
		job_result->result_type = JOB_SUCCESS_WITH_DATA;
	}
}

void sim900_send_sms(RCOM_job_result_t *job_result, char* phone_number,
		char* msg) {
	/* set phone number and msg needed to send */
	RCOM_set_phone_number(phone_number);
	RCOM_set_sms_msg(msg);

	RCOM_step_info_t step_info;
	char cmd[128];
	step_info.command = cmd;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_SEND_SMS_STEP_NUM;
			current_step++) {
		RCOM_get_step_info(SEND_SMS, current_step, &step_info);
		if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
			job_result->result_type = JOB_FAIL;
			job_result->content.int_result = current_step;
			DEBUG("step failed: %d\n", current_step);
			return;
		}
	}
	job_result->result_type = JOB_SUCCESS_WITHOUT_DATA;
}

void sim900_make_voice_call(RCOM_job_result_t *job_result, char* phone_number) {
	RCOM_set_phone_number(phone_number);
	RCOM_step_info_t step_info;
	char cmd[128];
	step_info.command = cmd;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_MAKE_VOICE_CALL_STEP_NUM;
			current_step++) {
		RCOM_get_step_info(MAKE_CALL, current_step, &step_info);
		if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
			job_result->result_type = JOB_FAIL;
			job_result->content.int_result = current_step;
			DEBUG("step failed: %d\n", current_step);
			return;
		}
	}
	job_result->result_type = JOB_SUCCESS_WITHOUT_DATA;
}

