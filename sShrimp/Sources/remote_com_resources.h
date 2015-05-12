/**
 @author Nguyen Van Hien <nvhien1992@gmail.com>
 @copyright Copyright (C) 2015 <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b> , All rights reserved 
 */

#ifndef REMOTE_COM_RESOURCES_H_
#define REMOTE_COM_RESOURCES_H_

#include "PE_Types.h"
#include "uart_ll.h"

/*=======================================================================
 ===========================PUBLIC RCOM DEFINITIONS======================
 =======================================================================*/
typedef enum {
	RCOM_HAS_NOTHING, //
	RCOM_STARTED, //
	RCOM_CONFIGURED, //
	RCOM_INTERNET_CONNECTED //
} RCOM_status_t;

/*=======================================================================
 ==========================PRIVATE RCOM DEFINITIONS======================
 =======================================================================*/
typedef enum {
	SM_AND_TIMEOUT, //
	TIMEOUT_ONLY, //
	NOTHING, //
} RCOM_exe_method_t;

typedef enum {
	STEP_SUCCESS, //
	STEP_FAIL, //
} RCOM_step_result_t;

typedef struct {
	RCOM_exe_method_t execution_method;
	char* command;
	uint32_t timeout;
	char* expected_response;
} RCOM_step_info_t;

typedef struct {
	bool SM_status;
	char* SM_buffer;
	uint32_t SM_pointer;
} RCOM_SM_t;

typedef struct {
	char* buffer_ptr;
	uint16_t buffer_max_size;
	uint16_t buffer_counter;
	bool enable_buffer;
} RCOM_buff_t;

/*=======================================================================
 ==========================PUBLIC RCOM FUNCTIONS=========================
 =======================================================================*/
/**
 * 
 */
void RCOM_init(uint8_t systick_period_in_ms);

/**
 * 
 */
void RCOM_set_status(RCOM_status_t new_status);

/**
 * 
 */
RCOM_status_t RCOM_get_status(void);

/**
 * 
 */
void RCOM_uart_rx_callback(void);

/**
 * 
 */
void RCOM_set_uart(uart_t *defined_uart);

/**
 * 
 */
void RCOM_set_rx_buf(char* buf_addr, uint16_t buf_max_size);

/*=======================================================================
 ==========================PRIVATE RCOM FUNCTIONS========================
 =======================================================================*/
/**
 * 
 */
RCOM_step_result_t RCOM_step_excution(RCOM_step_info_t* step_info);

/**
 * 
 */
void RCOM_uart_writef(char* frame);

/**
 * 
 */
void RCOM_uart_get_char(void);

/**
 * 
 */
void RCOM_store_rchar_into_buffer(RCOM_buff_t *a_buffer);

/**
 * 
 */
char* RCOM_get_rx_buf(void);

/**
 * 
 */
void RCOM_clear_rx_buf(void);

/**
 * 
 */
void RCOM_enable_rx_buf(void);

/**
 * 
 */
void RCOM_disable_rx_buf(void);

/**
 * 
 */
uint16_t RCOM_get_rx_buf_data_len(void);

/**
 * 
 */
uint8_t RCOM_get_systick(void);

#endif //REMOTE_COM_RESOURCES_H_
