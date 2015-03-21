#include "remote_com_resources.h"
#include "sim900_int.h"

/**
 * @brief 
 *   turn on sim900
 * @para:
 *   none
 * @return:
 *   none
 */
void sim900_power_on() {
	MOB_NPW_CLEAR();
}

/**
 * @brief 
 *   turn off sim900
 * @para:
 *   none
 * @return:
 *   none
 */
void sim900_power_off(void) {
	MOB_NPW_SET();
}

void sim900_term_http_session(void) {
	sim900_send_more("AT+HTTPTERM\r\n");
}

/**
 * @brief 
 *   Sending a command to SIM900
 * @param
 *   cmd: comamnd (string format)
 * @return
 */
uint8_t sim900_send_cmd(char* cmd) {
	rx_buf_counter = 0;
	memset(rx_buf, 0, SERIAL_BUF_SIZE);
	uart1_writef(cmd);
	PRINTF(cmd);
	return 1;
}

/**
 @brief Send more data to the SIM900
 @return None
 @attention This function do not clear RXBuf before sending 
 */
void sim900_send_more(char* data) {
	uart1_writef(data);
	PRINTF(data);
}

uint8_t sim900_send_http_data(char* data, uint8_t is_sent) {
	if (isSent == 1) {
		sim900_send_more(data);
	}
	return strlen(data);
}

char* sim900_get_http_res_content(void) {
	uint8_t hasHPlus;
	uint16_t i;
	uint16_t start;
	uint8_t line_feed_num;
	uint16_t line_feed_pos[2];

	line_feed_num = 0;
	hasHPlus = 0;
	/* find +H */
	for (start = 0; start < rx_buf_counter; start++) {
		if ((rx_buf[start] == '+') && (rx_buf[start + 1] == 'H')) {
			hasHPlus = 1;
			break;
		}
	}

	/* has +H */
	if (hasHPlus == 1) {
		for (i = start; i < rx_buf_counter; i++) { //find line_feed_pos
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

	return NULL;
}

/**
 * @brief 
 *   config gpio for sim900 connection
 * @para   
 *   none
 * @return
 *   none
 */
void sim900_gpio_init(void) {
	//Configure MOB_nRST_PIN as output
	MOB_NRST_SEL &= ~MOB_NRST_PIN;
	MOB_NRST_DIR |= MOB_NRST_PIN;
	//Configure MOB_nPW_PIN as output
	MOB_NPW_SEL &= ~MOB_NPW_PIN;
	MOB_NPW_DIR |= MOB_NPW_PIN;
	//Configure MOB_STATUS_PIN as input
	MOB_STATUS_SEL &= ~MOB_STATUS_PIN;
	MOB_STATUS_DIR &= ~MOB_STATUS_PIN;
	//Configure MOB_STATUS_PIN as input
	MOB_RI_SEL &= ~MOB_RI_PIN;
	MOB_RI_DIR &= ~MOB_RI_PIN;
	//Configure GPIO for LED
	GPIO_SetPinMode(8, 0, false, true, true, false, false);      //Busy LED
	GPIO_SetPinMode(8, 1, false, true, true, false, false);      //has GPRS led
	GPIO_SetPinZero(8, 0);
	GPIO_SetPinZero(8, 1);
}

void sim900_start_device(RCOM_job_result_t *job_result) {
	RCOM_step_info_t step_info;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_START_DEVICE_STEP_NUM;
			current_step++) {
		switch (current_step) {
		case 0:
			sim900_gpio_init();
			uart_init(BAUD2UBR(9600)); //set the baud rate as necessary
			uart_set_input(uart1_rx_callback); //set the callback function		
			sim900_power_off();
			MOB_NRST_SET();

			RCOM_get_step_info(START_DEVICE, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		case 1: //3s
			sim900_power_on();

			RCOM_get_step_info(START_DEVICE, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		default:
			break;
		}
	}

	job_result->result_type = JOB_SUCCESS_WITHOUT_DATA;
	return;
}

/**
 * @brief  reinit sim900
 *	Init GPIO, UART
 *	Power up
 *	Create a square wave at nRST pin
 */
void sim900_config_device(RCOM_job_result_t *job_result) {
	RCOM_step_info_t step_info;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_CONFIG_DEVICE_STEP_NUM;
			current_step++) {
		switch (current_step) {
		case 0:
		case 1:
		case 2:
		case 3:
			RCOM_get_step_info(CONFIG_DEVICE, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		default:
			break;
		}
	}
	job_result->result_type = JOB_SUCCESS_WITHOUT_DATA;
	return;
}

/**
 * @brief  sim900 gprs config
 */
void sim900_connect_internet(RCOM_job_result_t *job_result) {
	RCOM_step_info_t step_info;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_CONNECT_INTERNET_STEP_NUM;
			current_step++) {
		switch (current_step) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			RCOM_get_step_info(CONNECT_INTERNET, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		default:
			break;
		}
	}
	job_result->result_type = JOB_SUCCESS_WITHOUT_DATA;
	return;
}

/**
 * @brief  stop SIM900
 *	Create a square wave at nRST pin
 *	Power down
 */
void sim900_stop_device(RCOM_job_result_t *job_result) {
	RCOM_step_info_t step_info;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_STOP_DEVICE_STEP_NUM;
			current_step++) {
		switch (current_step) {
		case 0: //3s
			MOB_NRST_SET();
			RCOM_get_step_info(STOP_DEVICE, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		case 1: //3s
			MOB_NRST_CLEAR();
			RCOM_get_step_info(STOP_DEVICE, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		case 2: //1s
			sim900_power_off();
			RCOM_get_step_info(STOP_DEVICE, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		default:
			break;
		}
	}
	job_result->result_type = JOB_SUCCESS_WITHOUT_DATA;
	return;
}

/*  
 * @description:  send report with gprs			            								
 *    method: http post											  				
 *    Data Format (To send Webserver):													
 *    fN=...&rDN=...&nD=...&n0=...&d0=...&v0=...&sT0=...  v.v. 
 *    fN: farm Name
 *    rDN: received Data Number
 *    nD: Number data of report			
 *    n0: ID of node 0
 *    d0: ID of sensor 0
 *    v0: Value of sensor 0
 *    sT0:Sensing time of sensor 0
 *    n1: ID of node 1
 *    d1: ID of sensor 1
 *    v1: Value of sensor 1
 *    sT1:Sensing time of sensor 1

 *    ...		
 * @para:
 *   domain: domain of webserver 					
 *   pRptPara: pointer of report para structure
 *   curStep: current step in post report operation
 * @return:
 *   1:success
 *   0:fail	
 */
void sim900_post_report(gw_rpt_para_t* pRptPara, RCOM_job_result_t *job_result) {
	RCOM_step_info_t step_info;
	char strTemp[150];
	uint16_t dataSize = 0;
	uint8_t i = 0;
	uint8_t current_step = 0;
	for (current_step = 0; currernt_step < SIM900_HTTP_POST_STEP_NUM;
			current_step++) {
		switch (current_step) {
		case 0:
		case 1:
		case 2:
		case 3:
			RCOM_get_step_info(REPORT_DATA, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				//TODO: call function to stop http session
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		case 4:
			/* calculate the total number of sending bytes */
			/* communication Type */
			sprintf(strTemp, "comType=report");
			dataSize += sim900_send_http_data(strTemp, 0);
			/* farm-Name */
			sprintf(strTemp, "&fN=%s", (char *) GWM_GetName());
			dataSize += sim900_send_http_data(strTemp, 0);
			/* received-Data-Number  */
			sprintf(strTemp, "&rDN=%lx", pRptPara->dataIndex); //NEED TIN DOUBLE CHECK
			dataSize += sim900_send_http_data(strTemp, 0);
			/* number-Data */
			sprintf(strTemp, "&nD=%x", pRptPara->numData);
			dataSize += sim900_send_http_data(strTemp, 0);
			/* report-Data */
			for (i = 0; i < pRptPara->numData; i++) {
				/* report-Data-NodeId */
				sprintf(strTemp, "&n%d=%x", i, pRptPara->rptData[i].ndId);
				dataSize += sim900_send_http_data(strTemp, 0);
				/* report-Data-SensorId */
				sprintf(strTemp, "&d%d=%x", i, pRptPara->rptData[i].ssId);
				dataSize += sim900_send_http_data(strTemp, 0);
				/* report-Data-SensorValue */
				sprintf(strTemp, "&v%d=%x", i, pRptPara->rptData[i].ssVal);
				dataSize += sim900_send_http_data(strTemp, 0);
				/* report-Data-SensingTime */
				sprintf(strTemp, "&sT%d=%lx", i, pRptPara->rptData[i].senTime);
				dataSize += sim900_send_http_data(strTemp, 0);
			}
			/* Send MessageSize */
			sprintf(strTemp, "AT+HTTPDATA=%d,%ld\r\n", dataSize, 50000);
			sim900_send_more(strTemp); //WHY send_more. send_cmd???

			RCOM_get_step_info(REPORT_DATA, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				//TODO: call function to stop http session
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		case 5:
			/* send report message */
			/* communication Type */
			sprintf(strTemp, "comType=report");
			sim900_send_http_data(strTemp, 1);
			/* farm-Name */
			sprintf(strTemp, "&fN=%s", (char *) GWM_GetName());
			sim900_send_http_data(strTemp, 1);
			/* received-Data-Number  */
			sprintf(strTemp, "&rDN=%lx", pRptPara->dataIndex); //NEED TIN DOUBLE CHECK
			sim900_send_http_data(strTemp, 1);
			/* number-Data */
			sprintf(strTemp, "&nD=%x", pRptPara->numData);
			sim900_send_http_data(strTemp, 1);
			/* report-Data */
			for (i = 0; i < pRptPara->numData; i++) {
				/* report-Data-NodeId */
				sprintf(strTemp, "&n%d=%x", i, pRptPara->rptData[i].ndId);
				sim900_send_http_data(strTemp, 1);
				/* report-Data-SensorId */
				sprintf(strTemp, "&d%d=%x", i, pRptPara->rptData[i].ssId);
				sim900_send_http_data(strTemp, 1);
				/* report-Data-SensorValue */
				sprintf(strTemp, "&v%d=%x", i, pRptPara->rptData[i].ssVal); //COULD IT SUPPORT negative number ???
				sim900_send_http_data(strTemp, 1);
				/* report-Data-SensingTime */
				sprintf(strTemp, "&sT%d=%lx", i, pRptPara->rptData[i].senTime);
				sim900_send_http_data(strTemp, 1);
			}
			RCOM_get_step_info(REPORT_DATA, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				//TODO: call function to stop http session
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		case 6:
		case 7:
			RCOM_get_step_info(REPORT_DATA, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				//TODO: call function to stop http session
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			//TODO: Parse data from UART (Get HTTP response from web server)
			break;
		case 8:
			COM_DisableRxBuf();
			sim900_term_http_session(); //this does not clear rx_buf
			RCOM_get_step_info(REPORT_DATA, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				//TODO: call function to stop http session
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		case 9:
			COM_EnableRxBuf();
			break;
		default:
			return 0;
		}
	}
	job_result->result_type = JOB_SUCCESS_WITH_DATA;
	return;
}

/*
 * @brief	Get unit (in string format) from unit (in int format)
 */
void SWM_Get_Unit_Str(ss_unit_t unit, char unit_str[]) {
	/* Get unit prefix */
	switch (unit & SS_UNIT_PREFIX_MASK) {
	case SS_PREFIX_NORMAL: //None-prefix
		strcpy(unit_str, "");
		break;
	case SS_PREFIX_KILO:
		strcpy(unit_str, "K");
		break;
	case SS_PREFIX_MEGA:
		strcpy(unit_str, "M");
		break;
	case SS_PREFIX_GIGA:
		strcpy(unit_str, "G");
		break;
	case SS_PREFIX_MILI:
		strcpy(unit_str, "m");
		break;
	case SS_PREFIX_MICRO:
		strcpy(unit_str, "u");
		break;
	case SS_PREFIX_NANO:
		strcpy(unit_str, "n");
		break;
	case SS_PREFIX_PICO:
		strcpy(unit_str, "p");
		break;
	default:
		strcpy(unit_str, "");
		break;
	}
	/* Get unit */
	switch (unit & SS_UNIT_MASK) {
	case SS_UNIT_ON_OFF:
		strcat(unit_str, "ON/OFF");
		break;
	case SS_UNIT_PPM:
		strcat(unit_str, "ppm");
		break;
	case SS_UNIT_PERCENT:
		strcat(unit_str, "%");
		break;
	case SS_UNIT_TMP_C:
		strcat(unit_str, "oC");
		break;
	case SS_UNIT_TMP_K:
		strcat(unit_str, "K");
		break;
	case SS_UNIT_TMP_F:
		strcat(unit_str, "F");
		break;
	case SS_UNIT_PH:
		strcat(unit_str, "pH");
		break;
	case SS_UNIT_MG_L:
		strcat(unit_str, "mg/l");
		break;
	case SS_UNIT_LUX:
		strcat(unit_str, "lux");
		break;
	case SS_UNIT_VOLTAGE:
		strcat(unit_str, "V");
		break;
	case SS_UNIT_AMPERE:
		strcat(unit_str, "A");
		break;
	default:
		break;
	}
}

/**
 * @brief   send update with gprs	
 *   -method: http post	
 *   Data Format (To send Webserver):
 *      fN=...&n0=...&aT0=...&d0=...&u0=...&lES0=...&lEC0=...
 *              &lWS0=...&lWC0=...&hWC0=...&hWS0=...&hEC0=...&hES0=... .v.v. 
 *      fN: farm Name
 *      n0: ID of node 0
 *      aT0: actived Time of node 0
 *      d0: ID of sensor 0
 *      u0: unit of sensor 0
 *      lES0: low Emergency Set of sensor 0
 *      lEC0: low Emergency Clear of sensor 0
 *      lWS0: low Warning Set of sensor 0
 *      lWC0: low Warning Clear of sensor 0
 *      hWC0: high Warning Clear of sensor 0
 *      hWS0: high Warning Set of sensor 0
 *      hEC0: high Emergency Clear of sensor 0
 *      hES0: high Emergency Set of sensor 0
 *      ...	
 * @para:	
 *   domain: domain of webserver
 *   curStep: current step in post update operation 	
 * @return:
 *    1: success
 *    0: fail		
 */
void sim900_post_update(RCOM_job_result_t *job_result) {
	RCOM_step_info_t step_info;
	char strTemp[150];
	char unit_str[10];
	uint16_t dataSize = 0;
	uint8_t i, j;
	swm_gw_mng_t* pGateway;
	node_stt_t nodeStt;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_HTTP_POST_STEP_NUM;
			current_step++) {
		switch (current_step) {
		case 0:
		case 1:
		case 2:
		case 3:
			RCOM_get_step_info(UPDATE_DATA, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				//TODO: call function to stop http session
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		case 4:
			pGateway = GWM_GetGWMng();
			/* calculate the total number of sending bytes */
			/* communication Type */
			sprintf(strTemp, "comType=update");
			dataSize += sim900_send_http_data(strTemp, 0);
			/* farm-Name */
			sprintf(strTemp, "&fN=%s", (char *) GWM_GetName());
			dataSize += sim900_send_http_data(strTemp, 0);
			for (i = 0; i < pGateway->numNode; i++) {
				nodeStt = pGateway->nodeStt[i];
				for (j = 0; j < nodeStt.numDev; j++) {
					/* update-Data-NodeId */
					sprintf(strTemp, "&n%d=%x", i + j, nodeStt.id);
					dataSize += sim900_send_http_data(strTemp, 0);
					/* update-Data-Total-Data-Number */
					//sprintf(strTemp, "&tDN%d=%x", i+j, );
					//dataSize += sim900_send_http_data(strTemp, 0);
					/* update-Data-Last-Data-Index */
					//sprintf(strTemp, "&lDI%d=%x", i+j, );
					//dataSize += sim900_send_http_data(strTemp, 0);
					/* update-Data-activeTime */
					sprintf(strTemp, "&aT%d=%lx", i + j, nodeStt.activedTime);
					dataSize += sim900_send_http_data(strTemp, 0);
					/* update-Data-SensorId */
					sprintf(strTemp, "&d%d=%x", i + j, nodeStt.ssStt[j].id);
					dataSize += sim900_send_http_data(strTemp, 0);
					/* update-Data-Unit */
					SWM_Get_Unit_Str(nodeStt.ssStt[j].unit, unit_str);
					sprintf(strTemp, "&u%d=%s", i + j, unit_str);
					dataSize += sim900_send_http_data(strTemp, 0);
					/* update-Data-low-Emergency-Set */
					sprintf(strTemp, "&lES%d=%x", i + j,
							nodeStt.ssStt[j].lowEmgSet);
					dataSize += sim900_send_http_data(strTemp, 0);
					/* update-Data-low-Emergency-Clear */
					sprintf(strTemp, "&lEC%d=%x", i + j,
							nodeStt.ssStt[j].lowEmgClear);
					dataSize += sim900_send_http_data(strTemp, 0);
					/* update-Data-low-Warning-Set */
					sprintf(strTemp, "&lWS%d=%x", i + j,
							nodeStt.ssStt[j].lowWarnSet);
					dataSize += sim900_send_http_data(strTemp, 0);
					/* update-Data-low-Warning-Clear */
					sprintf(strTemp, "&lWC%d=%x", i + j,
							nodeStt.ssStt[j].lowWarnClear);
					dataSize += sim900_send_http_data(strTemp, 0);
					/* update-Data-high-Warning-Clear */
					sprintf(strTemp, "&hWC%d=%x", i + j,
							nodeStt.ssStt[j].highWarnClear);
					dataSize += sim900_send_http_data(strTemp, 0);
					/* update-Data-high-Warning-Set */
					sprintf(strTemp, "&hWS%d=%x", i + j,
							nodeStt.ssStt[j].highWarnSet);
					dataSize += sim900_send_http_data(strTemp, 0);
					/* update-Data-highEmergencyClear */
					sprintf(strTemp, "&hEC%d=%x", i + j,
							nodeStt.ssStt[j].highEmgClear);
					dataSize += sim900_send_http_data(strTemp, 0);
					/* update-Data-high-Emergency-Set */
					sprintf(strTemp, "&hES%d=%x", i + j,
							nodeStt.ssStt[j].highEmgSet);
					dataSize += sim900_send_http_data(strTemp, 0);
				}
			}
			/* Send MessageSize */
			sprintf(strTemp, "AT+HTTPDATA=%d,%ld\r\n", dataSize, 50000);
			sim900_send_more(strTemp);
			RCOM_get_step_info(UPDATE_DATA, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				//TODO: call function to stop http session
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		case 5:
			pGateway = GWM_GetGWMng();
			/* communication Type */
			sprintf(strTemp, "comType=update");
			sim900_send_http_data(strTemp, 1);
			/* farm-Name */
			sprintf(strTemp, "&fN=%s", (char *) GWM_GetName());
			sim900_send_http_data(strTemp, 1);
			for (i = 0; i < pGateway->numNode; i++) {
				nodeStt = pGateway->nodeStt[i];
				for (j = 0; j < nodeStt.numDev; j++) {
					/* update-Data-NodeId */
					sprintf(strTemp, "&n%d=%x", i + j, nodeStt.id);
					sim900_send_http_data(strTemp, 1);
					/* update-Data-Total-Data-Number */
					//sprintf(strTemp, "&tDN%d=%x", i+j, );
					//sim900_send_http_data(strTemp, 1);
					/* update-Data-Last-Data-Index */
					//sprintf(strTemp, "&lDI%d=%x", i+j, );
					//sim900_send_http_data(strTemp, 1);
					/* update-Data-activeTime */
					sprintf(strTemp, "&aT%d=%lx", i + j, nodeStt.activedTime);
					sim900_send_http_data(strTemp, 1);
					/* update-Data-SensorId */
					sprintf(strTemp, "&d%d=%x", i + j, nodeStt.ssStt[j].id);
					sim900_send_http_data(strTemp, 1);
					/* update-Data-Unit */
					SWM_Get_Unit_Str(nodeStt.ssStt[j].unit, unit_str);
					sprintf(strTemp, "&u%d=%s", i + j, unit_str);
					sim900_send_http_data(strTemp, 1);
					/* update-Data-low-Emergency-Set */
					sprintf(strTemp, "&lES%d=%x", i + j,
							nodeStt.ssStt[j].lowEmgSet);
					sim900_send_http_data(strTemp, 1);
					/* update-Data-low-Emergency-Clear */
					sprintf(strTemp, "&lEC%d=%x", i + j,
							nodeStt.ssStt[j].lowEmgClear);
					sim900_send_http_data(strTemp, 1);
					/* update-Data-low-Warning-Set */
					sprintf(strTemp, "&lWS%d=%x", i + j,
							nodeStt.ssStt[j].lowWarnSet);
					sim900_send_http_data(strTemp, 1);
					/* update-Data-low-Warning-Clear */
					sprintf(strTemp, "&lWC%d=%x", i + j,
							nodeStt.ssStt[j].lowWarnClear);
					sim900_send_http_data(strTemp, 1);
					/* update-Data-high-Warning-Clear */
					sprintf(strTemp, "&hWC%d=%x", i + j,
							nodeStt.ssStt[j].highWarnClear);
					sim900_send_http_data(strTemp, 1);
					/* update-Data-high-Warning-Set */
					sprintf(strTemp, "&hWS%d=%x", i + j,
							nodeStt.ssStt[j].highWarnSet);
					sim900_send_http_data(strTemp, 1);
					/* update-Data-highEmergencyClear */
					sprintf(strTemp, "&hEC%d=%x", i + j,
							nodeStt.ssStt[j].highEmgClear);
					sim900_send_http_data(strTemp, 1);
					/* update-Data-high-Emergency-Set */
					sprintf(strTemp, "&hES%d=%x", i + j,
							nodeStt.ssStt[j].highEmgSet);
					sim900_send_http_data(strTemp, 1);
				}
			}
			RCOM_get_step_info(UPDATE_DATA, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				//TODO: call function to stop http session
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		case 6:
		case 7:
			RCOM_get_step_info(UPDATE_DATA, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				//TODO: call function to stop http session
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			//TODO: Parse HTTP response in step 7
			break;
		case 8:
			COM_DisableRxBuf();
			sim900_term_http_session(); //this do not clear RxBUF
			RCOM_get_step_info(UPDATE_DATA, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				//TODO: call function to stop http session
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		case 9:
			COM_EnableRxBuf();
			break;
		default:
			break;
		}
	}
	job_result->result_type = JOB_SUCCESS_WITH_DATA;
	return;
}

/**
 * @brief   request Webserver to get time stamp	
 *   method: http get
 * @para:	
 *   domain: domain of webserver
 *   curStep: current step in get timestamp operation
 * @return:
 */
void sim900_get_timestamp(RCOM_job_result_t *job_result) {
	RCOM_step_info_t step_info;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_HTTP_GET_STEP_NUM;
			current_step++) {
		switch (current_step) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			RCOM_get_step_info(GET_TIMESTAMP, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				//TODO: call function to stop http session
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			//TODO: Parse HTTP response in step 4
			break;
		case 5:
			COM_DisableRxBuf();
			sim900_term_http_session(); //this do not clear rxBuf
			RCOM_get_step_info(GET_TIMESTAMP, current_step, &step_info);
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				//TODO: call function to stop http session
				job_result->result_type = JOB_FAIL;
				job_result->int_result = current_step;
				return;
			}
			break;
		case 6:
			COM_EnableRxBuf();
			break;
		default:
			return 0;
		}
	}
	job_result->result_type = JOB_SUCCESS_WITH_DATA;
	return;
}

/**
 * @brief   send a warning SMS message
 * @para:	
 *   phoneNum: phone number which is received message
 *   message: warning message
 *   curStep: current step in send sms operation
 * @return:
 *   1:success
 *   0:fail	
 */
void sim900_send_sms(RCOM_job_result_t *job_result) {
	RCOM_step_info_t step_info;
	uint8_t current_step = 0;
	for (current_step = 0; current_step < SIM900_SEND_SMS_STEP_NUM;
			current_step++) {
		switch (current_step) {
		case 0:
			sim900_send_cmd("AT+CMGS=\"");
			sim900_send_more(phoneNum);
			sim900_send_more("\"\r");
			break;
		case 1:
			sim900_send_more(message);
			sim900_send_more("\x1A\r");
			break;
		default:
			break;
		}
	}
	job_result->result_type = JOB_SUCCESS_WITHOUT_DATA;
	return;
}

/**
 * @brief   make a warning voice call
 * @para:	
 *   phoneNumOne: the first phone number
 *   phoneNumSecond: the second phone number
 * @return:
 *   1:success
 *   0:fail	
 * @attention This is emergency situation, block everthing until DONE 
 */
uint8_t sim900_make_voice_call(char* phoneNum, uint8_t curStep) {
	switch (curStep) {
	case 0:
		sim900_send_cmd("ATD");
		sim900_send_more(phoneNum);
		sim900_send_more(";\r\n");
		break;
	case 1:
		sim900_send_cmd("ATH\r\n");
		break;
	default:
		return 0;
	}
	if (1 == curStep) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * @return
 *      1: need to terminate session
 *      0: don't need to terminate session
 */
uint8_t sim900_need_terminate_session(RCOM_job_type_t job_type) {
	switch (job_type) {
	case REPORT_DATA:
		return 1;
	case UPDATE_DATA:
		return 1;
	case POLL_COMMAND:
		return 1;
	case GET_TIMESTAMP:
		return 1;
	default:
		return 0;
	}
	return 1;
}

