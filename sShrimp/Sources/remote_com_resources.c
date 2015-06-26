/**
 @file remote_com_resources.c
 @brief 
 @author <b>Nguyen Van Hien</b> <nvhien1992@gmail.com>
 @copyright Copyright &copy; 2015, <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b>, All rights reserved. 
 */
#include <stdio.h>
#include <cstring>
#include "mqxlite.h"
#include "remote_com_resources.h"

#define NOTIFY_EN (1)
#define DEBUG_EN (1)
#include "debug.h"

#define SM_EVT_BIT_MASK (0x01)

/* static vars */
static RCOM_SM_t RCOM_SM = { FALSE, NULL, 0, }; 

static uart_t *RCOM_uart;
static RCOM_buff_t rx_buf = { NULL, 0, 0, FALSE }; /*!< internal rx buffer. */
static unsigned char r_char;
static RCOM_status_t RCOM_status = RCOM_HAS_NOTHING;
static uint8_t systick_period = 5; /*!< 5ms as default. */

static LWEVENT_STRUCT SM_event; /*!< internal lwevent. */

/*=======================================================================
 ======================DEFINE PRIVATE FUNCTIONS==========================
 =======================================================================*/
/**
 * @private
 * @brief Simple state machine processing response from SIM900.
 */
static void RCOM_state_machine(void);

/**
 * @private
 * @brief Enable state machine.
 */
static void RCOM_SM_enable(void);

/**
 * @private
 * @brief Disable state machine.
 */
static void RCOM_SM_disable(void);

/*=======================================================================
 ======================IMPLEMENT PRIVATE FUNCTIONS=======================
 =======================================================================*/
static void RCOM_state_machine(void) {
	if (RCOM_SM.SM_status) {
		if (RCOM_SM.SM_buffer[RCOM_SM.SM_pointer] == r_char) {
			/* Increase RCOM state machine pointer */
			RCOM_SM.SM_pointer++;

			/* Check end of buffer */
			if (RCOM_SM.SM_buffer[RCOM_SM.SM_pointer] == '\0') {

				/* Send a successful notification to RCOM_step_excution */
				_lwevent_set(&SM_event, SM_EVT_BIT_MASK);

				/* Disable RCOM state machine */
				RCOM_SM_disable();

				return;
			}
		} else {
			if (RCOM_SM.SM_pointer > 0) {
				/* check r_char after reset pointer */
				if (RCOM_SM.SM_buffer[0] == r_char) {
					RCOM_SM.SM_pointer = 1;
				}
			} else {
				/* Reset pointer when receive incorrect character */
				RCOM_SM.SM_pointer = 0;
			}
		}
	}
}

static void RCOM_SM_enable(void) {
	RCOM_SM.SM_status = TRUE;
	RCOM_SM.SM_pointer = 0;
}

static void RCOM_SM_disable(void) {
	RCOM_SM.SM_status = FALSE;
	RCOM_SM.SM_pointer = 0;
}

/*=======================================================================
 ======================IMPLEMENT PUBLIC FUNCTIONS========================
 =======================================================================*/
void RCOM_uart_get_char(void) {
	uart_receive(RCOM_uart, &r_char, 1);
}

void RCOM_store_rchar_into_buffer(RCOM_buff_t *a_buffer) {
	if (a_buffer->enable_buffer) {
		if (a_buffer->buffer_counter == a_buffer->buffer_max_size) {
			a_buffer->enable_buffer = FALSE;
			return;
		}
		a_buffer->buffer_ptr[a_buffer->buffer_counter++] = r_char;
	}
}

RCOM_step_result_t RCOM_step_excution(RCOM_step_info_t* step_info) {
	_mqx_uint ticks_waitting = 0;

	switch (step_info->execution_method) {
	case SM_AND_TIMEOUT:
		/* copy expected response to RCOM-state-machine-buffer */
		RCOM_SM.SM_buffer = step_info->expected_response;

		/* Enable RCOM-state-machine*/
		RCOM_SM_enable();

		/* Register to receive char */
		RCOM_uart_get_char();

		/* send command to UART */
		RCOM_uart_writef(step_info->command);

		ticks_waitting = step_info->timeout * 1000 / systick_period;
		if (_lwevent_wait_ticks(&SM_event, SM_EVT_BIT_MASK, TRUE,
				ticks_waitting) != MQX_OK) {
			RCOM_SM_disable();
			RCOM_disable_rx_buf();
			return STEP_FAIL;
		}
		RCOM_disable_rx_buf();

		return STEP_SUCCESS;
	case TIMEOUT_ONLY:
		/* Register to receive char */
		RCOM_uart_get_char();

		/* send command to UART */
		RCOM_uart_writef(step_info->command);

		ticks_waitting = step_info->timeout * 1000 / systick_period;
		_lwevent_wait_ticks(&SM_event, SM_EVT_BIT_MASK, TRUE, ticks_waitting);

		RCOM_disable_rx_buf();

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

	RCOM_store_rchar_into_buffer(&rx_buf);

	if (RCOM_SM.SM_status || rx_buf.enable_buffer) {
		/* register to get new char */
		RCOM_uart_get_char();
	}
}

void RCOM_init(uint8_t systick_period_in_ms) {
	systick_period = systick_period_in_ms;
	uart_init(RCOM_uart);
	_lwevent_create(&SM_event, LWEVENT_AUTO_CLEAR);
}

void RCOM_set_status(RCOM_status_t new_status) {
	RCOM_status = new_status;
}

RCOM_status_t RCOM_get_status(void) {
	return RCOM_status;
}

void RCOM_uart_writef(char* frame) {
	uart_send(RCOM_uart, frame, strlen(frame));
}

void RCOM_set_uart(uart_t *defined_uart) {
	RCOM_uart = defined_uart;
}

void RCOM_set_rx_buf(char* buf_addr, uint16_t buf_max_size) {
	rx_buf.buffer_ptr = buf_addr;
	rx_buf.buffer_max_size = buf_max_size;
}

char* RCOM_get_rx_buf(void) {
	return rx_buf.buffer_ptr;
}

void RCOM_clear_rx_buf(void) {
	rx_buf.buffer_counter = 0;
	memset(rx_buf.buffer_ptr, 0, rx_buf.buffer_max_size);
}

void RCOM_enable_rx_buf(void) {
	rx_buf.enable_buffer = TRUE;
}

void RCOM_disable_rx_buf(void) {
	rx_buf.enable_buffer = FALSE;
}

uint16_t RCOM_get_rx_buf_data_len(void) {
	return rx_buf.buffer_counter;
}

uint8_t RCOM_get_systick(void) {
	return systick_period;
}
