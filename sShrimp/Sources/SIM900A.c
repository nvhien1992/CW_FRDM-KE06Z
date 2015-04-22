#include <cstring>
#include <cstdlib>
#include "mqxlite.h"
#include "remote_com_resources.h"
#include "SIM900A.h"

#define NOTIFY_EN (1)
#define DEBUG_EN (1)
#include "debug.h"

SIM900_pins_t *SIM900_pins;
SIM900_params_t SIM900_params;

/* definitions depend on design of SIM900 board */
/* nPW */
#define NPW_SET()	SIM900_pins->nPW.ClrVal(NULL)
#define NPW_CLR()	SIM900_pins->nPW.SetVal(NULL)
/* nRST */
#define NRST_SET()	SIM900_pins->nRST.ClrVal(NULL)
#define NRST_CLR()	SIM900_pins->nRST.SetVal(NULL)
/* DTR */
#define DTR_SET()	SIM900_pins->DTR.ClrVal(NULL)
#define DTR_CLR()	SIM900_pins->DTR.SetVal(NULL)
/* voltage lv on STATUS pin */
#define STATUS_HIGH_LV	FALSE
#define STATUS_LOW_LV	TRUE
/* voltage lv on RI pin */
#define RI_HIGH_LV	FALSE
#define RI_LOW_LV	TRUE

static LWEVENT_STRUCT RI_event;
#define RI_EVT_BIT_MASK	0x01

static bool RI_interrupt_flag = FALSE;
char RI_unsolicited_buf[32];
RCOM_buff_t RI_rx_buf = { RI_unsolicited_buf, sizeof(RI_unsolicited_buf), 0,
		TRUE };

/*=======================================================================
 ==========================DEFINE PRIVATE FUNCTIONS======================
 =======================================================================*/
/**
 * @brief turn on sim900.
 */
static void sim900_power_on();

/**
 * @brief turn off sim900.
 */
static void sim900_power_off(void);

/**
 * 
 */
static void sim900_clear_RI_rx_buf(void);
static int sim900_get_recv_SMS_msg_index(void);
static bool sim900_get_incoming_call_number(char *incoming_pn);
/**
 * @brief get response content from webserver. Response format:
 * "+HTTPREAD:<data_size>\r\n<response_content>\r\nOK\r\n".
 * 
 * @return NULL if fail, else response content will be returned.
 */
static char* sim900_get_http_response_content(void);

/*=======================================================================
 ========================IMPLEMENT PRIVATE FUNCTIONS=====================
 =======================================================================*/
static void sim900_clear_RI_rx_buf(void) {
	RI_rx_buf.buffer_counter = 0;
	memset(RI_rx_buf.buffer_ptr, 0, RI_rx_buf.buffer_max_size);
}

static int sim900_get_recv_SMS_msg_index(void) {
	/* check recv buffer */
	char *pstr = strstr(RI_rx_buf.buffer_ptr, "+CMTI: \"SM\",");
	if (!pstr) {
		return FALSE;
	}

	pstr += strlen("+CMTI: \"SM\",");

	return atoi(pstr);
}

static bool sim900_get_incoming_call_number(char *incoming_pn) {
	if (!incoming_pn) {
		return FALSE;
	}

	/* check recv buffer */
	char *pstr = strstr(RI_rx_buf.buffer_ptr, "+CLIP: \"");
	if (!pstr) {
		return FALSE;
	}

	pstr += strlen("+CLIP: \"");

	/* normalize the phone number: +84... -> 0... */
	if (*pstr == '+') {
		pstr += 2;
		*pstr = '0';
	}

	uint8_t indx = 0;
	while (pstr[indx] != '\"') {
		incoming_pn[indx] = pstr[indx];
		indx++;
	}
	incoming_pn[indx] = '\0';

	return TRUE;
}

static char* sim900_get_http_response_content(void) {
	/* check recv buffer */
	char *pstr = strstr(RCOM_get_rx_buf(), "+HTTPREAD:");
	if (!pstr) {
		return FALSE;
	}

	/* get data size */
	int data_size = 0;
	sscanf(pstr, "+HTTPREAD:%d", &data_size);

	/* make pointer behind +HTTPREAD:_\r\n */
	pstr = strstr(pstr, "\n") + 1;

	/* get response content */
	char *out_buff = RCOM_get_rx_buf();

	uint8_t index = 0;
	for (index = 0; index < data_size; index++) {
		out_buff[index] = pstr[index];
	}
	out_buff[data_size] = '\0';

	return out_buff;
}

static void sim900_power_on() {
	NPW_SET();
}

static void sim900_power_off(void) {
	NPW_CLR();
}

/*=======================================================================
 ========================IMPLEMENT PUBLIC FUNCTIONS======================
 =======================================================================*/
void sim900_init(SIM900_pins_t *defined_pins) {
	SIM900_pins = defined_pins;
	SIM900_pins->RI.Disable(NULL);
	_lwevent_create(&RI_event, LWEVENT_AUTO_CLEAR);
}

void sim900_control_ring_indicator(bool is_enabled) {
	is_enabled ? SIM900_pins->RI.Enable(NULL) : SIM900_pins->RI.Disable(NULL);
}

RCOM_result_type_t sim900_start(void) {
	RCOM_step_info_t step_info;

	if (SIM900_pins->STATUS.GetVal(NULL) == STATUS_HIGH_LV) {
		DEBUG("Stop device before starting!\n");
		sim900_stop();
	} else {
		/* disable ring indicator */
		SIM900_pins->RI.Disable(NULL);
	}

	uint8_t current_step = 0;
	for (current_step = 0; current_step < 3; current_step++) {
		switch (current_step) {
		case 0: //1s
			sim900_power_on();

			step_info.execution_method = TIMEOUT_ONLY;
			step_info.command = "";
			step_info.timeout = 1;
			break;
		case 1: //2s
			NRST_CLR();

			step_info.execution_method = TIMEOUT_ONLY;
			step_info.command = "";
			step_info.timeout = 2;
			break;
		case 2: //10s
			NRST_SET();

			step_info.execution_method = TIMEOUT_ONLY;
			step_info.command = "";
			step_info.timeout = 10;
			break;
		default:
			return RCOM_FAIL;
		}
		RCOM_step_excution(&step_info);
	}

	if (SIM900_pins->STATUS.GetVal(NULL) == STATUS_LOW_LV) {
		DEBUG("Start SIM900 failed\n");
		return RCOM_FAIL;
	}

	/* enable ring indicator */
	SIM900_pins->RI.Enable(NULL);

	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_default_config(void) {
	RCOM_step_info_t step_info;

	uint8_t current_step = 0;
	for (current_step = 0; current_step < 5; current_step++) {
		switch (current_step) {
		case 0:
		case 1:
			step_info.execution_method = TIMEOUT_ONLY;
			step_info.command = "AT\r\n";
			step_info.timeout = 1;
			break;
		case 2: //disable echo mode
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "ATE0\r\n";
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 3: //set SMS text mode
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+CMGF=1\r\n";
			step_info.timeout = 5;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 4: //show incoming call number
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+CLIP=1\r\n";
			step_info.timeout = 2;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		default:
			return RCOM_FAIL;
		}

		if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
			DEBUG("step failed: %d\n", current_step);
			return RCOM_FAIL;
		}
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_connect_internet(void) {
	RCOM_step_info_t step_info;
	char cmd[48];

	uint8_t current_step = 0;
	for (current_step = 0; current_step < 9; current_step++) {
		/* re-create buffer */
		step_info.command = cmd;
		switch (current_step) {
		case 0:
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+CGATT=0\r\n";
			step_info.timeout = 10;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 1:
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+CGATT=1\r\n";
			step_info.timeout = 10;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 2:
			step_info.execution_method = TIMEOUT_ONLY;
			step_info.command = "AT+SAPBR=0,1\r\n";
			step_info.timeout = 1;
			break;
		case 3: // Set CONTYPE
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n";
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 4: //Set APN_NAME
			step_info.execution_method = SM_AND_TIMEOUT;
			sprintf(step_info.command, "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n",
					SIM900_params.apn_name);
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 5: //Set APN_USER
			step_info.execution_method = SM_AND_TIMEOUT;
			sprintf(step_info.command, "AT+SAPBR=3,1,\"USER\",\"%s\"\r\n",
					SIM900_params.apn_user);
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 6: //Set APN_PASSWORD
			step_info.execution_method = SM_AND_TIMEOUT;
			sprintf(step_info.command, "AT+SAPBR=3,1,\"PWD\",\"%s\"\r\n",
					SIM900_params.apn_pass);
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 7: //Query IP
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+SAPBR=2,1\r\n";
			step_info.timeout = 1;
			step_info.expected_response = "\r\n+SAPBR: 1";
			break;
		case 8: //Open bearer (at least 3sec)
//			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.execution_method = TIMEOUT_ONLY;
			step_info.command = "AT+SAPBR=1,1\r\n";
			step_info.timeout = 3;
//			step_info.expected_response = "\r\nOK\r\n";
			break;
		default:
			return RCOM_FAIL;
		}

		if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
			DEBUG("step failed: %d\n", current_step);
			return RCOM_FAIL;
		}
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_stop(void) {
	RCOM_step_info_t step_info;

	/* disable ring indicator */
	SIM900_pins->RI.Disable(NULL);

	uint8_t current_step = 0;
	for (current_step = 0; current_step < 3; current_step++) {
		switch (current_step) {
		case 0: //2s
			NRST_CLR();

			step_info.execution_method = TIMEOUT_ONLY;
			step_info.command = "";
			step_info.timeout = 2;
			break;
		case 1: //5s
			NRST_SET();

			step_info.execution_method = TIMEOUT_ONLY;
			step_info.command = "";
			step_info.timeout = 5;
			break;
		case 2: //1s
			sim900_power_off();

			step_info.execution_method = TIMEOUT_ONLY;
			step_info.command = "";
			step_info.timeout = 1;
			break;
		default:
			return RCOM_FAIL;
		}
		RCOM_step_excution(&step_info);
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_enable_DTR_sleep_mode(void) {
	RCOM_step_info_t step_info;

	step_info.execution_method = SM_AND_TIMEOUT;
	step_info.command = "AT+CSCLK=1\r\n";
	step_info.timeout = 1;
	step_info.expected_response = "\r\nOK\r\n";

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return RCOM_FAIL;
	}

	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_disable_sleep_mode(void) {
	RCOM_step_info_t step_info;

	step_info.execution_method = SM_AND_TIMEOUT;
	step_info.command = "AT+CSCLK=0\r\n";
	step_info.timeout = 1;
	step_info.expected_response = "\r\nOK\r\n";

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return RCOM_FAIL;
	}

	return RCOM_SUCCESS_WITHOUT_DATA;
}

void sim900_control_sleep_manually(bool sleep_sim900) {
	sleep_sim900 ? DTR_SET() : DTR_CLR();
}

void sim900_RI_callback(void) {
	DEBUG("RI int\n");
	RI_interrupt_flag = TRUE;
	RCOM_uart_get_char();
	_lwevent_set(&RI_event, RI_EVT_BIT_MASK);
}

SIM900_RI_result_t sim900_process_RI_task(char* RI_result) {
	_lwevent_wait_for(&RI_event, RI_EVT_BIT_MASK, TRUE, NULL);

	/* wait 150ms to determine received SMS or not */
	uint8_t time_detect_SMS = 150; //ms
	_time_delay_ticks(time_detect_SMS / RCOM_get_systick());

	RI_interrupt_flag = FALSE;
	if (SIM900_pins->RI.GetVal(NULL) == RI_HIGH_LV) { //received SMS
		DEBUG("Received SMS msg\n");

		/* get sms index */
		int index = sim900_get_recv_SMS_msg_index();
		if (index < 0) {
			return RI_UNKNOWN;
		}
		*RI_result = index;

		/* clear RI's rx buffer */
		sim900_clear_RI_rx_buf();

		return RECEIVED_SMS_MSG;
	} else { //incoming call
		DEBUG("Incoming call\n");

		/* get phone number before hanging up */
		if (!sim900_get_incoming_call_number(RI_result)) {
			return RI_UNKNOWN;
		}

		/* clear RI's rx buffer */
		sim900_clear_RI_rx_buf();

		_time_delay_ticks(1000 / RCOM_get_systick());
		sim900_hang_up_voice_call();

		return INCOMING_VOICE_CALL;
	}
}

void sim900_uart_callback(void) {
	if (RI_interrupt_flag) {
		RCOM_store_rchar_into_buffer(&RI_rx_buf);
		RCOM_uart_get_char();
	}
}

void sim900_set_apn_para(char* apn_name, char* apn_user, char* apn_pass) {
	SIM900_params.apn_name = apn_name;
	SIM900_params.apn_user = apn_user;
	SIM900_params.apn_pass = apn_pass;
}

bool sim900_check_SIM_inserted(void) {
	RCOM_step_info_t step_info;

	step_info.execution_method = SM_AND_TIMEOUT;
	step_info.command = "AT+CSMINS?\r\n";
	step_info.timeout = 1;
	step_info.expected_response = "\r\nOK\r\n";

	RCOM_clear_rx_buf();
	RCOM_enable_rx_buf();
	
	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return FALSE;
	}

	/* check recv buffer */
	char *pstr = strstr(RCOM_get_rx_buf(), "+CSMINS:");
	if (!pstr) {
		return FALSE;
	}

	/* get SIM status */
	int evt_code, SIM_status;
	sscanf(pstr, "+CSMINS:%d,%d", &evt_code, &SIM_status);

	return (SIM_status == 1);
}

char* sim900_get_MSP_name(void) {
	RCOM_step_info_t step_info;

	step_info.execution_method = SM_AND_TIMEOUT;
	step_info.command = "AT+CSPN?\r\n";
	step_info.timeout = 1;
	step_info.expected_response = "\r\nOK\r\n";

	RCOM_clear_rx_buf();
	RCOM_enable_rx_buf();

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return NULL;
	}

	/* check recv buffer, response format: +CSPN: "<MSP>" */
	char *pstr = strstr(RCOM_get_rx_buf(), "+CSPN: \"");
	if (!pstr) {
		return NULL;
	}

	pstr += strlen("+CSPN: \"");

	char *MSP_name = RCOM_get_rx_buf();
	uint8_t index = 0;
	while (pstr[index] != '\"') {
		MSP_name[index] = pstr[index];
		index++;
	}
	MSP_name[index] = '\0';

	return MSP_name;
}

RCOM_result_type_t sim900_terminate_HTTP_session(void) {
	RCOM_step_info_t step_info;

	step_info.execution_method = SM_AND_TIMEOUT;
	step_info.command = "AT+HTTPTERM\r\n";
	step_info.timeout = 1;
	step_info.expected_response = "\r\nOK\r\n";

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return RCOM_FAIL;
	}

	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_create_HTTP_POST_session(char *URL) {
	char cmd[32 + strlen(URL)];
	RCOM_step_info_t step_info;

	uint8_t current_step = 0;
	for (current_step = 0; current_step < 4; current_step++) {
		/* re-create buffer */
		step_info.command = cmd;

		switch (current_step) {
		case 0: //INIT
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+HTTPINIT\r\n";
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 1: //set profile
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+HTTPPARA=\"CID\",1\r\n";
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 2: //set URL
			step_info.execution_method = SM_AND_TIMEOUT;
			sprintf(step_info.command, "AT+HTTPPARA=\"URL\",\"%s\"\r\n", URL);
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 3: //finish creating HTTP POST session.
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command =
					"AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"\r\n";
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
		default:
			return RCOM_FAIL;
		}

		if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
			sim900_terminate_HTTP_session();
			DEBUG("step failed: %d\n", current_step);
			return RCOM_FAIL;
		}
	}

	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_send_HTTP_data_size(uint16_t data_size) {
	RCOM_step_info_t step_info;
	char cmd[32];

	step_info.command = cmd;
	step_info.execution_method = SM_AND_TIMEOUT;
	sprintf(step_info.command, "AT+HTTPDATA=%d,%d\r\n", data_size, 50000);
	step_info.timeout = 1;
	step_info.expected_response = "\r\nDOWNLOAD\r\n";

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		sim900_terminate_HTTP_session();
		return RCOM_FAIL;
	}

	return RCOM_SUCCESS_WITHOUT_DATA;
}

void sim900_uart_send_data(char *data) {
	RCOM_uart_writef(data);
}

char* sim900_HTTP_POST_action(void) {
	RCOM_step_info_t step_info;

	uint8_t current_step = 0;
	for (current_step = 0; current_step < 3; current_step++) {
		switch (current_step) {
		case 0: //waiting to send data to SIM900
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "";
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 1: //send data to webserver.
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+HTTPACTION=1\r\n";
			step_info.timeout = 30;
			step_info.expected_response = "HTTPACTION:1,200";
			break;
		case 2: //get response from webserver.
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+HTTPREAD\r\n";
			step_info.timeout = 5;
			step_info.expected_response = "\r\nOK\r\n";
			RCOM_clear_rx_buf();
			RCOM_enable_rx_buf();
			break;
		default:
			return NULL;
		}

		if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
			sim900_terminate_HTTP_session();
			DEBUG("step failed: %d\n", current_step);
			return NULL;
		}
	}

	char *result = sim900_get_http_response_content();

	/* terminate session */
	if (sim900_terminate_HTTP_session() == RCOM_FAIL) {
		DEBUG("Can not terminate session\n");
	}

	return result;
}

char* sim900_HTTP_POST(char *URL, char *data) {
	int data_size = strlen(data);
	int cmd_size = 32;
	if (strlen(URL) < data_size) {
		cmd_size += data_size;
	} else {
		cmd_size += strlen(URL);
	}
	char cmd[cmd_size];
	RCOM_step_info_t step_info;

	uint8_t current_step = 0;
	for (current_step = 0; current_step < 8; current_step++) {
		/* re-create buffer */
		step_info.command = cmd;

		switch (current_step) {
		case 0:
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+HTTPINIT\r\n";
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 1:
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+HTTPPARA=\"CID\",1\r\n";
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 2:
			step_info.execution_method = SM_AND_TIMEOUT;
			sprintf(step_info.command, "AT+HTTPPARA=\"URL\",\"%s\"\r\n", URL);
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 3: //finish creating HTTP POST session.
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command =
					"AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"\r\n";
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 4: //calculate the total number of sending bytes
			step_info.execution_method = SM_AND_TIMEOUT;
			sprintf(step_info.command, "AT+HTTPDATA=%d,%d\r\n", data_size,
					50000);
			step_info.timeout = 2;
			step_info.expected_response = "\r\nDOWNLOAD\r\n";
			break;
		case 5: //send report message to SIM900
			step_info.execution_method = SM_AND_TIMEOUT;
			sprintf(step_info.command, "%s", data);
			step_info.timeout = 2;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 6: //send data to webserver.
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+HTTPACTION=1\r\n";
			step_info.timeout = 30;
			step_info.expected_response = "HTTPACTION:1,200";
			break;
		case 7: //get response from webserver.
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+HTTPREAD\r\n";
			step_info.timeout = 5;
			step_info.expected_response = "\r\nOK\r\n";
			RCOM_clear_rx_buf();
			RCOM_enable_rx_buf();
			break;
		default:
			return NULL;
		}
		if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
			sim900_terminate_HTTP_session();
			DEBUG("step failed: %d\n", current_step);
			return NULL;
		}
	}

	char *result = sim900_get_http_response_content();

	/* terminate session */
	if (sim900_terminate_HTTP_session() == RCOM_FAIL) {
		DEBUG("Can not terminate session\n");
	}

	return result;
}

char* sim900_HTTP_GET(char *URL) {
	char cmd[32 + strlen(URL)];
	RCOM_step_info_t step_info;

	uint8_t current_step = 0;
	for (current_step = 0; current_step < 5; current_step++) {
		/* re-create buffer */
		step_info.command = cmd;

		switch (current_step) {
		case 0: //INIT
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+HTTPINIT\r\n";
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 1: //set profile
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+HTTPPARA=\"CID\",1\r\n";
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 2: //set URL
			step_info.execution_method = SM_AND_TIMEOUT;
			sprintf(step_info.command, "AT+HTTPPARA=\"URL\",\"%s\"\r\n", URL);
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		case 3: //ACTION
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+HTTPACTION=0\r\n";
			step_info.timeout = 30;
			step_info.expected_response = "HTTPACTION:0,200";
			break;
		case 4: //get HTTP data response
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+HTTPREAD\r\n";
			step_info.timeout = 5;
			step_info.expected_response = "\r\nOK\r\n";
			RCOM_clear_rx_buf();
			RCOM_enable_rx_buf();
			break;
		default:
			return NULL;
		}

		if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
			sim900_terminate_HTTP_session();
			DEBUG("step failed: %d\n", current_step);
			return NULL;
		}
	}

	char *result = sim900_get_http_response_content();

	if (sim900_terminate_HTTP_session() == RCOM_FAIL) {
		DEBUG("Can not terminate session\n");
	}

	return result;
}

//void sim900_post_report(gw_rpt_para_t* pRptPara, RCOM_job_result_t *job_result) {
//	RCOM_step_info_t step_info;
//	char cmd[150];
//
//	char str_temp[150];
//	uint16_t data_size = 0;
//	uint8_t i = 0;
//	uint8_t current_step = 0;
//	for (current_step = 0; current_step < 8; current_step++) {
//		/* re-create buffer */
//		step_info.command = cmd;
//
//		switch (current_step) {
//		case 0:
//			step_info.execution_method = SM_AND_TIMEOUT;
//			step_info.command = "AT+HTTPINIT\r\n";
//			step_info.timeout = 1;
//			step_info.expected_response = "\r\nOK\r\n";
//			break;
//		case 1:
//			step_info.execution_method = SM_AND_TIMEOUT;
//			step_info.command = "AT+HTTPPARA=\"CID\",1\r\n";
//			step_info.timeout = 1;
//			step_info.expected_response = "\r\nOK\r\n";
//			break;
//		case 2:
//			step_info.execution_method = SM_AND_TIMEOUT;
//			sprintf(step_info.command, "AT+HTTPPARA=\"URL\",\"%s\"\r\n",
//					SIM900_params.URL);
//			step_info.timeout = 1;
//			step_info.expected_response = "\r\nOK\r\n";
//			break;
//		case 3: //finish creating HTTP POST session.
//			step_info.execution_method = SM_AND_TIMEOUT;
//			step_info.command =
//					"AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"\r\n";
//			step_info.timeout = 1;
//			step_info.expected_response = "\r\nOK\r\n";
//			break;
//		case 4: //calculate the total number of sending bytes
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
//
//			step_info.execution_method = SM_AND_TIMEOUT;
//			sprintf(step_info.command, "AT+HTTPDATA=%d,%ld\r\n", data_size,
//					50000);
//			step_info.timeout = 1;
//			step_info.expected_response = "\r\nDOWNLOAD\r\n";
//			break;
//		case 5: //send report message to SIM900
//			/* communication Type */
//			sprintf(str_temp, "comType=report");
//			RCOM_uart_writef(str_temp);
//			/* farm-Name */
//			sprintf(str_temp, "&fN=%s", (char *) GWM_GetName());
//			RCOM_uart_writef(str_temp);
//			/* received-Data-Number  */
//			sprintf(str_temp, "&rDN=%lx", pRptPara->dataIndex); //NEED TIN DOUBLE CHECK
//			RCOM_uart_writef(str_temp);
//			/* number-Data */
//			sprintf(str_temp, "&nD=%x", pRptPara->numData);
//			RCOM_uart_writef(str_temp);
//			/* report-Data */
//			for (i = 0; i < pRptPara->numData; i++) {
//				/* report-Data-NodeId */
//				sprintf(str_temp, "&n%d=%x", i, pRptPara->rptData[i].ndId);
//				RCOM_uart_writef(str_temp);
//				/* report-Data-SensorId */
//				sprintf(str_temp, "&d%d=%x", i, pRptPara->rptData[i].ssId);
//				RCOM_uart_writef(str_temp);
//				/* report-Data-SensorValue */
//				sprintf(str_temp, "&v%d=%x", i, pRptPara->rptData[i].ssVal); //COULD IT SUPPORT negative number ???
//				RCOM_uart_writef(str_temp);
//				/* report-Data-SensingTime */
//				sprintf(str_temp, "&sT%d=%lx", i, pRptPara->rptData[i].senTime);
//				RCOM_uart_writef(str_temp);
//			}
//
//			step_info.execution_method = SM_AND_TIMEOUT;
//			step_info.command = "";
//			step_info.timeout = 1;
//			step_info.expected_response = "\r\nOK\r\n";
//			break;
//		case 6: //send data to webserver.
//			step_info.execution_method = SM_AND_TIMEOUT;
//			step_info.command = "AT+HTTPACTION=1\r\n";
//			step_info.timeout = 20;
//			step_info.expected_response = "HTTPACTION:1,200";
//			break;
//		case 7: //get response from webserver.
//			step_info.execution_method = SM_AND_TIMEOUT;
//			step_info.command = "AT+HTTPREAD\r\n";
//			step_info.timeout = 5;
//			step_info.expected_response = "\r\nOK\r\n";
//			break;
//		default:
//			return;
//		}
//		if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
//			sim900_terminate_HTTP_session(job_result);
//			job_result->result_type = JOB_FAIL;
//			job_result->content.int_result = current_step;
//			DEBUG("step failed: %d\n", current_step);
//			return;
//		}
//	}
//
//TODO
//	DEBUG("");
//
//	/* terminate session */
//	sim900_terminate_HTTP_session(job_result);
//	if (job_result == JOB_FAIL) {
//		DEBUG("Can not terminate session\n");
//	}
//
//	job_result->result_type = JOB_SUCCESS_WITH_DATA;
//}

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

//void sim900_post_update(RCOM_job_result_t *job_result) {
//	RCOM_step_info_t step_info;
//	char cmd[150];
//
//	char str_temp[150];
//	char unit_str[10];
//	uint16_t data_size = 0;
//	bool job_failed = FALSE;
//	uint8_t i, j;
//	swm_gw_mng_t* pGateway;
//	node_stt_t nodeStt;
//	uint8_t current_step = 0;
//	for (current_step = 0; current_step < 8; current_step++) {
//		/* re-create buffer */
//		step_info.command = cmd;
//
//		switch (current_step) {
//		case 0:
//			step_info.execution_method = SM_AND_TIMEOUT;
//			step_info.command = "AT+HTTPINIT\r\n";
//			step_info.timeout = 1;
//			step_info.expected_response = "\r\nOK\r\n";
//			break;
//		case 1:
//			step_info.execution_method = SM_AND_TIMEOUT;
//			step_info.command = "AT+HTTPPARA=\"CID\",1\r\n";
//			step_info.timeout = 1;
//			step_info.expected_response = "\r\nOK\r\n";
//			break;
//		case 2:
//			step_info.execution_method = SM_AND_TIMEOUT;
//			sprintf(step_info.command, "AT+HTTPPARA=\"URL\",\"%s\"\r\n",
//					SIM900_params.URL);
//			step_info.timeout = 1;
//			step_info.expected_response = "\r\nOK\r\n";
//			break;
//		case 3: //finish creating HTTP POST session.
//			step_info.execution_method = SM_AND_TIMEOUT;
//			step_info.command =
//					"AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"\r\n";
//			step_info.timeout = 1;
//			step_info.expected_response = "\r\nOK\r\n";
//			break;
//		case 4: //send data length to SIM900
//			pGateway = GWM_GetGWMng();
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
//			step_info.execution_method = SM_AND_TIMEOUT;
//			sprintf(step_info.command, "AT+HTTPDATA=%d,%ld\r\n", data_size,
//					50000);
//			step_info.timeout = 1;
//			step_info.expected_response = "\r\nDOWNLOAD\r\n";
//			break;
//		case 5: //send data to SIM900
//			pGateway = GWM_GetGWMng();
//			/* communication Type */
//			sprintf(str_temp, "comType=update");
//			RCOM_uart_writef(str_temp);
//			/* farm-Name */
//			sprintf(str_temp, "&fN=%s", (char *) GWM_GetName());
//			sim900_send_http_data(str_temp, 1);
//			for (i = 0; i < pGateway->numNode; i++) {
//				nodeStt = pGateway->nodeStt[i];
//				for (j = 0; j < nodeStt.numDev; j++) {
//					/* update-Data-NodeId */
//					sprintf(str_temp, "&n%d=%x", i + j, nodeStt.id);
//					RCOM_uart_writef(str_temp);
//					/* update-Data-Total-Data-Number */
//					//sprintf(str_temp, "&tDN%d=%x", i+j, );
//					//RCOM_uart_writef(str_temp);
//					/* update-Data-Last-Data-Index */
//					//sprintf(str_temp, "&lDI%d=%x", i+j, );
//					//RCOM_uart_writef(str_temp);
//					/* update-Data-activeTime */
//					sprintf(str_temp, "&aT%d=%lx", i + j, nodeStt.activedTime);
//					RCOM_uart_writef(str_temp);
//					/* update-Data-SensorId */
//					sprintf(str_temp, "&d%d=%x", i + j, nodeStt.ssStt[j].id);
//					RCOM_uart_writef(str_temp);
//					/* update-Data-Unit */
//					SWM_Get_Unit_Str(nodeStt.ssStt[j].unit, unit_str);
//					sprintf(str_temp, "&u%d=%s", i + j, unit_str);
//					RCOM_uart_writef(str_temp);
//					/* update-Data-low-Emergency-Set */
//					sprintf(str_temp, "&lES%d=%x", i + j,
//							nodeStt.ssStt[j].lowEmgSet);
//					RCOM_uart_writef(str_temp);
//					/* update-Data-low-Emergency-Clear */
//					sprintf(str_temp, "&lEC%d=%x", i + j,
//							nodeStt.ssStt[j].lowEmgClear);
//					RCOM_uart_writef(str_temp);
//					/* update-Data-low-Warning-Set */
//					sprintf(str_temp, "&lWS%d=%x", i + j,
//							nodeStt.ssStt[j].lowWarnSet);
//					RCOM_uart_writef(str_temp);
//					/* update-Data-low-Warning-Clear */
//					sprintf(str_temp, "&lWC%d=%x", i + j,
//							nodeStt.ssStt[j].lowWarnClear);
//					RCOM_uart_writef(str_temp);
//					/* update-Data-high-Warning-Clear */
//					sprintf(str_temp, "&hWC%d=%x", i + j,
//							nodeStt.ssStt[j].highWarnClear);
//					RCOM_uart_writef(str_temp);
//					/* update-Data-high-Warning-Set */
//					sprintf(str_temp, "&hWS%d=%x", i + j,
//							nodeStt.ssStt[j].highWarnSet);
//					RCOM_uart_writef(str_temp);
//					/* update-Data-highEmergencyClear */
//					sprintf(str_temp, "&hEC%d=%x", i + j,
//							nodeStt.ssStt[j].highEmgClear);
//					RCOM_uart_writef(str_temp);
//					/* update-Data-high-Emergency-Set */
//					sprintf(str_temp, "&hES%d=%x", i + j,
//							nodeStt.ssStt[j].highEmgSet);
//					RCOM_uart_writef(str_temp);
//				}
//			}
//
//			step_info.execution_method = SM_AND_TIMEOUT;
//			step_info.command = "";
//			step_info.timeout = 1;
//			step_info.expected_response = "\r\nOK\r\n";
//			break;
//		case 6: //send data from SIM900 to webserver.
//			step_info.execution_method = SM_AND_TIMEOUT;
//			step_info.command = "AT+HTTPACTION=1\r\n";
//			step_info.timeout = 20;
//			step_info.expected_response = "HTTPACTION:1,200";
//			break;
//		case 7: //get response from webserver.
//			step_info.execution_method = SM_AND_TIMEOUT;
//			step_info.command = "AT+HTTPREAD\r\n";
//			step_info.timeout = 5;
//			step_info.expected_response = "\r\nOK\r\n";
//			break;
//		default:
//			return;
//		}
//
//		if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
//			sim900_terminate_HTTP_session(job_result);
//			job_result->result_type = JOB_FAIL;
//			job_result->content.int_result = current_step;
//			DEBUG("step failed: %d\n", current_step);
//			return;
//		}
//	}
//
//TODO
//
//	/* terminate session */
//	sim900_terminate_HTTP_session(job_result);
//	if (job_result == JOB_FAIL) {
//		DEBUG("Can not terminate session\n");
//	}
//
//	job_result->result_type = JOB_SUCCESS_WITH_DATA;
//}

RCOM_result_type_t sim900_send_sms(char* phone_number, char* msg) {
	char cmd[32 + strlen(msg)];
	RCOM_step_info_t step_info;

	uint8_t current_step = 0;
	for (current_step = 0; current_step < 2; current_step++) {
		/* re-create buffer */
		step_info.command = cmd;

		switch (current_step) {
		case 0:
			step_info.execution_method = SM_AND_TIMEOUT;
			sprintf(step_info.command, "AT+CMGS=\"%s\"\r", phone_number);
			step_info.timeout = 2;
			step_info.expected_response = ">";
			break;
		case 1:
			step_info.execution_method = SM_AND_TIMEOUT;
			sprintf(step_info.command, "%s\x1A\r", msg);
			step_info.timeout = 10;
			step_info.expected_response = "\r\nOK\r\n";
			break;
		default:
			return RCOM_FAIL;
		}

		if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
			DEBUG("step failed: %d\n", current_step);
			return RCOM_FAIL;
		}
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_make_missed_voice_call(char* phone_number) {
	RCOM_step_info_t step_info;
	char cmd[32];

	uint8_t current_step = 0;
	for (current_step = 0; current_step < 3; current_step++) {
		/* re-create buffer */
		step_info.command = cmd;

		switch (current_step) {
		case 0: //enable the presentation of the COL.
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+COLP=1\r\n";
			step_info.timeout = 2;
			step_info.expected_response = "\r\nOK\r\n";
			RCOM_step_excution(&step_info);
			break;
		case 1: //calling and hang up immediately when hanging on.
			step_info.execution_method = SM_AND_TIMEOUT;
			sprintf(step_info.command, "ATD%s;\r\n", phone_number);
			step_info.timeout = 20;
			step_info.expected_response = "+COLP:";
			RCOM_step_excution(&step_info);
			break;
		case 2: //end calling
			step_info.execution_method = SM_AND_TIMEOUT;
			step_info.command = "AT+HVOIC\r\n";
			step_info.timeout = 1;
			step_info.expected_response = "\r\nOK\r\n";
			if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
				DEBUG("step failed: %d\n", current_step);
				return RCOM_FAIL;
			}
			break;
		default:
			return RCOM_FAIL;
		}
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

char* sim900_read_SMS_msg(uint8_t sms_index) {
	RCOM_step_info_t step_info;
	char cmd[16];
	step_info.command = cmd;

	step_info.execution_method = SM_AND_TIMEOUT;
	sprintf(step_info.command, "AT+CMGR=%d,0\r\n", sms_index);
	step_info.timeout = 5;
	step_info.expected_response = "\r\nOK\r\n";

	RCOM_clear_rx_buf();
	RCOM_enable_rx_buf();

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return NULL;
	}

	return RCOM_get_rx_buf();
}

RCOM_result_type_t sim900_show_incoming_call_number(void) {
	RCOM_step_info_t step_info;

	step_info.execution_method = SM_AND_TIMEOUT;
	step_info.command = "AT+CLIP=1\r\n";
	step_info.timeout = 2;
	step_info.expected_response = "\r\nOK\r\n";

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return RCOM_FAIL;
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_not_show_incoming_call_number(void) {
	RCOM_step_info_t step_info;

	step_info.execution_method = SM_AND_TIMEOUT;
	step_info.command = "AT+CLIP=0\r\n";
	step_info.timeout = 2;
	step_info.expected_response = "\r\nOK\r\n";

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return RCOM_FAIL;
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_enable_echo_mode(void) {
	RCOM_step_info_t step_info;

	step_info.execution_method = SM_AND_TIMEOUT;
	step_info.command = "ATE1\r\n";
	step_info.timeout = 1;
	step_info.expected_response = "\r\nOK\r\n";

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return RCOM_FAIL;
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_disable_echo_mode(void) {
	RCOM_step_info_t step_info;

	step_info.execution_method = SM_AND_TIMEOUT;
	step_info.command = "ATE0\r\n";
	step_info.timeout = 1;
	step_info.expected_response = "\r\nOK\r\n";

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return RCOM_FAIL;
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_set_SMS_text_mode(void) {
	RCOM_step_info_t step_info;

	step_info.execution_method = SM_AND_TIMEOUT;
	step_info.command = "AT+CMGF=1\r\n";
	step_info.timeout = 2;
	step_info.expected_response = "\r\nOK\r\n";

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return RCOM_FAIL;
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_set_SMS_PDU_mode(void) {
	RCOM_step_info_t step_info;

	step_info.execution_method = SM_AND_TIMEOUT;
	step_info.command = "AT+CMGF=0\r\n";
	step_info.timeout = 2;
	step_info.expected_response = "\r\nOK\r\n";

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return RCOM_FAIL;
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_hang_up_voice_call(void) {
	RCOM_step_info_t step_info;

	step_info.execution_method = SM_AND_TIMEOUT;
	step_info.command = "AT+HVOIC\r\n";
	step_info.timeout = 1;
	step_info.expected_response = "\r\nOK\r\n";

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return RCOM_FAIL;
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_delete_group_SMS_message(SIM900_del_SMS_t del_type) {
	RCOM_step_info_t step_info;

	switch (del_type) {
	case DEL_ALL:
		step_info.command = "AT+CMGDA=\"DEL ALL\"\r\n";
		break;
	case DEL_READ:
		step_info.command = "AT+CMGDA=\"DEL READ\"\r\n";
		break;
	case DEL_UNREAD:
		step_info.command = "AT+CMGDA=\"DEL UNREAD\"\r\n";
		break;
	case DEL_SENT:
		step_info.command = "AT+CMGDA=\"DEL SENT\"\r\n";
		break;
	case DEL_UNSENT:
		step_info.command = "AT+CMGDA=\"DEL UNSENT\"\r\n";
		break;
	case DEL_INBOX:
		step_info.command = "AT+CMGDA=\"DEL INBOX\"\r\n";
		break;
	default:
		return RCOM_FAIL;
	}
	step_info.execution_method = SM_AND_TIMEOUT;
	step_info.timeout = 1;
	step_info.expected_response = "\r\nOK\r\n";

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return RCOM_FAIL;
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

RCOM_result_type_t sim900_delete_an_SMS_message(uint8_t index) {
	RCOM_step_info_t step_info;
	char cmd[16];
	step_info.command = cmd;

	step_info.execution_method = SM_AND_TIMEOUT;
	sprintf(step_info.command, "AT+CMGD=%d,0\r\n", index);
	step_info.timeout = 2;
	step_info.expected_response = "\r\nOK\r\n";

	if (STEP_SUCCESS != RCOM_step_excution(&step_info)) {
		return RCOM_FAIL;
	}
	return RCOM_SUCCESS_WITHOUT_DATA;
}

