/**
 @author   Huynh Trung Tin <tinvuong51003405@gmail.com>
 Nguyen Van Hien <nvhien1992@gmail.com>
 @copyright Copyright (C) 2014 <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b> , All rights reserved 
 */
#include <stdio.h>
#include <cstring>
#include "mqxlite.h"
#include "remote_com_resources.h"

#define NOTIFY_EN (1)
#define DEBUG_EN (1)
#include "debug.h"

#define SYS_TICK_PERIOD (5) //ms
#define SM_EVT_BIT_MASK (0x01)

/* extern vars */
char* rx_buf = NULL;

/* static vars */
RCOM_t RCOM_info;
uart_t *RCOM_uart;
//static bool enable_recv = FALSE;
uint16_t max_char_in_rx = 0;
uint16_t rx_buf_counter = 0;
_mqx_uint ticks_waitting = 0;
unsigned char r_char;
LWEVENT_STRUCT SM_event;

/*=======================================================================
 =========================IMPLEMENT FUNCTIONS============================
 =======================================================================*/
void RCOM_get_step_info(RCOM_job_type_t job_type, uint8_t current_step,
		RCOM_step_info_t* step_info) {
	switch (job_type) {
	case START_DEVICE:
		switch (current_step) {
		case 0:
			step_info->execution_method = TIMEOUT;
			step_info->command = "";
			step_info->timeout = 3;
			break;
		case 1:
			step_info->execution_method = TIMEOUT;
			step_info->command = "";
			step_info->timeout = 15;
			break;
		default:
			break;
		}
		break;
	case CONFIG_DEVICE:
		switch (current_step) {
		case 0:
			step_info->execution_method = TIMEOUT;
			step_info->command = "AT\r\n";
			step_info->timeout = 1;
			break;
		case 1:
			step_info->execution_method = TIMEOUT;
			step_info->command = "AT\r\n";
			step_info->timeout = 1;
			break;
		case 2:
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "ATE0\r\n";
			step_info->timeout = 1;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 3:
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "AT+CMGF=1\r\n";
			step_info->timeout = 5;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		default:
			break;
		}
		break;  //end CONFIG_DEVICE
	case CONNECT_INTERNET:
		switch (current_step) {
		case 0:
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "AT+CGATT=0\r\n";
			step_info->timeout = 10;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 1:
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "AT+CGATT=1\r\n";
			step_info->timeout = 5;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 2:
			step_info->execution_method = TIMEOUT;
			step_info->command = "AT+SAPBR=0,1\r\n";
			step_info->timeout = 1;
			break;
		case 3: // Set CONTYPE
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n";
			step_info->timeout = 1;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 4: //Set APN_NAME
			step_info->execution_method = SM_AND_TIMEOUT;
			sprintf(step_info->command, "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n",
					RCOM_info.apn_name);
			step_info->timeout = 1;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 5: //Set APN_USER
			step_info->execution_method = SM_AND_TIMEOUT;
			sprintf(step_info->command, "AT+SAPBR=3,1,\"USER\",\"%s\"\r\n",
					RCOM_info.apn_user);
			step_info->timeout = 1;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 6: //Set APN_PASSWORD
			step_info->execution_method = SM_AND_TIMEOUT;
			sprintf(step_info->command, "AT+SAPBR=3,1,\"PWD\",\"%s\"\r\n",
					RCOM_info.apn_pass);
			step_info->timeout = 1;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 7: //Query IP
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "AT+SAPBR=2,1\r\n";
			step_info->timeout = 1;
			step_info->expected_response = "\r\n+SAPBR: 1";
			break;
		case 8: //Open bearer
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "AT+SAPBR=1,1\r\n";
			step_info->timeout = 60;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		default:
			break;
		}
		break;  //end CONNECT_INTERNET
	case STOP_DEVICE:
		switch (current_step) {
		case 0:
			step_info->execution_method = TIMEOUT;
			step_info->command = "";
			step_info->timeout = 3;
			break;
		case 1:
			step_info->execution_method = TIMEOUT;
			step_info->command = "";
			step_info->timeout = 3;
			break;
		case 2:
			step_info->execution_method = TIMEOUT;
			step_info->command = "";
			step_info->timeout = 1;
			break;
		default:
			break;
		}
		break;  //end STEP_DEVICE
	case REPORT_DATA: //Use HTTP POST
	case UPDATE_DATA: //Use HTTP POST
		switch (current_step) {
		case 0:
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "AT+HTTPINIT\r\n";
			step_info->timeout = 1;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 1:
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "AT+HTTPPARA=\"CID\",1\r\n";
			step_info->timeout = 1;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 2:
			step_info->execution_method = SM_AND_TIMEOUT;
			sprintf(step_info->command,
					"AT+HTTPPARA=\"URL\",\"%s/local-com/index.php\"\r\n",
					RCOM_info.web_domain);
			step_info->timeout = 1;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 3:
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command =
					"AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"\r\n";
			step_info->timeout = 1;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 4:
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "";
			step_info->timeout = 1;
			step_info->expected_response = "\r\nDOWNLOAD\r\n";
			break;
		case 5:
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "";
			step_info->timeout = 1;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 6:
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "AT+HTTPACTION=1\r\n";
			step_info->timeout = 20;
			step_info->expected_response = "HTTPACTION:1,200";
			break;
		case 7:
			step_info->execution_method = TIMEOUT;
			step_info->command = "AT+HTTPREAD\r\n";
			step_info->timeout = 5;
			break;
		case 8:   //Terminate HTTP session
			step_info->execution_method = TIMEOUT;
			step_info->command = "";
			step_info->timeout = 1;
			break;
		case 9:
		default:
			break;
		}
		break;
	case POLL_COMMAND:  //Use HTTP GET
	case GET_TIMESTAMP: //Use HTTP GET
		switch (current_step) {
		case 0:
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "AT+HTTPINIT\r\n";
			step_info->timeout = 1;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 1:
			step_info->execution_method = SM_AND_TIMEOUT;
			strcpy(step_info->command, "AT+HTTPPARA=\"CID\",1\r\n");
			step_info->timeout = 1;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 2:
			step_info->execution_method = SM_AND_TIMEOUT;
			sprintf(step_info->command,
					"AT+HTTPPARA=\"URL\",\"%s/local-com/index.php?comType=timestamp\"\r\n",
					RCOM_info.web_domain);
			step_info->timeout = 1;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		case 3:
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "AT+HTTPACTION=0\r\n";
			step_info->timeout = 20;
			step_info->expected_response = "HTTPACTION:0,200";
			break;
		case 4:
			step_info->execution_method = TIMEOUT;
			step_info->command = "AT+HTTPREAD\r\n";
			step_info->timeout = 5;
			break;
		case 5:   //Terminate HTTP session
			step_info->execution_method = TIMEOUT;
			step_info->command = "AT+HTTPTERM\r\n";
			step_info->timeout = 1;
			break;
		case 6:
		default:
			break;
		}
		break;
	case SEND_SMS:
		switch (current_step) {
		case 0:
			step_info->execution_method = SM_AND_TIMEOUT;
			sprintf(step_info->command, "AT+CMGS=\"%s\"\r",
					RCOM_info.phone_number);
			step_info->timeout = 2;
			step_info->expected_response = ">";
			break;
		case 1:
			step_info->execution_method = SM_AND_TIMEOUT;
			sprintf(step_info->command, "%s\x1A\r", RCOM_info.sms_msg);
			step_info->timeout = 10;
			step_info->expected_response = "\r\nOK\r\n";
			break;
		default:
			break;
		}
		break;  //end SEND_SMS
	case MAKE_CALL:
		switch (current_step) {
		case 0:
			step_info->execution_method = SM_AND_TIMEOUT;
			sprintf(step_info->command, "ATD%s;\r\n", RCOM_info.phone_number);
			step_info->timeout = 30;
			step_info->expected_response = "\r\nBUSY\r\n";
			break;
		case 1:
			step_info->execution_method = SM_AND_TIMEOUT;
			step_info->command = "ATH\r\n";
			step_info->timeout = 1;
			step_info->expected_response = "\r\nBUSY\r\n";
			break;
		default:
			break;
		}
		break;  //end MAKE_CALL
	case TURN_OFF_DEVICE:
		break;
	case TERMINATE_HTTP_SESSION:
		break;
	default:
		break;
	}
}

RCOM_step_result_t RCOM_step_excution(RCOM_step_info_t* step_info) {
	switch (step_info->execution_method) {
	case SM_AND_TIMEOUT:
		/* copy expected response to RCOM-state-machine-buffer */
		RCOM_info.SM_buffer = step_info->expected_response;
//		DEBUG("SM buff: %s\n", RCOM_info.SM_buffer);

		/* send command to UART */
//		DEBUG("%s\n", step_info->command);
		RCOM_uart_writef(step_info->command);
		
		/* Enable RCOM-state-machine */
		RCOM_SM_enable();
		
		/* Register to receive char */
		RCOM_uart_get_char();

		ticks_waitting = step_info->timeout * 1000 / SYS_TICK_PERIOD;
		if (_lwevent_wait_ticks(&SM_event, SM_EVT_BIT_MASK, TRUE,
				ticks_waitting) != MQX_OK) {
			RCOM_SM_disable();
			DEBUG("SM time out\n");
			return STEP_FAIL;
		}
		DEBUG("SM finished\n");

		return STEP_SUCCESS;
	case TIMEOUT:
		/* send command to UART */
//		DEBUG("%s\n", step_info->command);
		RCOM_uart_writef(step_info->command);
		
		ticks_waitting = step_info->timeout * 1000 / SYS_TICK_PERIOD;
		_lwevent_wait_ticks(&SM_event, SM_EVT_BIT_MASK, TRUE, ticks_waitting);

		return STEP_SUCCESS;
	case NOTHING:
		/* send command to UART */
		RCOM_uart_writef(step_info->command);
		return STEP_SUCCESS;
	default:
		return STEP_FAIL;
	}
}

void RCOM_uart_rx_callback(void) {
	/* check received char */
	RCOM_state_machine();
	
	if (RCOM_SM_is_enable()) {
		/* register to get new char */
		RCOM_uart_get_char();
	}
}

void RCOM_uart_get_char(void) {
	uart_receive(RCOM_uart, &r_char, 1);
}

void RCOM_state_machine(void) {
	if (RCOM_SM_is_enable()) {
		if (RCOM_info.SM_buffer[RCOM_info.SM_pointer] == r_char) {
			/* Increase RCOM state machine pointer */
			RCOM_info.SM_pointer++;
			/* Check end of buffer */
			if (RCOM_info.SM_buffer[RCOM_info.SM_pointer] == '\0') {

				/* Send a successful notification to RCOM_step_excution */
				_lwevent_set_auto_clear(&SM_event, SM_EVT_BIT_MASK);

				/* Disable RCOM state machine */
				RCOM_SM_disable();

				return;
			}
		} else {
			/* Reset pointer when receive incorrect character */
			RCOM_info.SM_pointer = 0;
		}
	}
}

void RCOM_init(void) {
	uart_init(RCOM_uart);
	_lwevent_create(&SM_event, LWEVENT_AUTO_CLEAR);
}

void RCOM_uart_writef(char* frame) {
	uart_send(RCOM_uart, frame, strlen(frame));
}

void RCOM_set_uart(uart_t *defined_uart) {
	RCOM_uart = defined_uart;
}

bool RCOM_SM_is_enable(void) {
	return RCOM_info.SM_status;
}

void RCOM_SM_enable(void) {
	RCOM_info.SM_status = TRUE;
	RCOM_info.SM_pointer = 0;
}

void RCOM_SM_disable(void) {
	RCOM_info.SM_status = FALSE;
	RCOM_info.SM_pointer = 0;
}

void RCOM_set_apn_para(char* apn_name, char* apn_user, char* apn_pass) {
	RCOM_info.apn_name = apn_name;
	RCOM_info.apn_user = apn_user;
	RCOM_info.apn_pass = apn_pass;
}

void RCOM_set_domain(char* new_domain) {
	RCOM_info.web_domain = new_domain;
}

void RCOM_set_phone_number(char* phone_number) {
	RCOM_info.phone_number = phone_number;
}

void RCOM_set_sms_msg(char* msg) {
	RCOM_info.sms_msg = msg;
}

//void RCOM_enable_rx_buf(void) {
//	enable_recv = TRUE;
//}

//void RCOM_disable_rx_buf(void) {
//	enable_recv = FALSE;
//}

void RCOM_set_rx_buf(char* buf_addr, uint16_t max_num_char) {
	rx_buf = buf_addr;
	max_char_in_rx = max_num_char;
}

void RCOM_clear_rx_buf(void) {
	rx_buf_counter = 0;
	memset(rx_buf, 0, max_char_in_rx);
}

char* RCOM_get_rx_buf(void) {
	return rx_buf;
}

uint16_t RCOM_get_rx_buf_len(void) {
	return rx_buf_counter;
}
