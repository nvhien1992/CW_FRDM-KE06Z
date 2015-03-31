/**
 @author   Huynh Trung Tin <tinvuong51003405@gmail.com>
 Nguyen Van Hien <nvhien1992@gmail.com>
 @copyright Copyright (C) 2014 <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b> , All rights reserved 
 */

#ifndef REMOTE_COM_RESOURCES_H_
#define REMOTE_COM_RESOURCES_H_

#include "remote_com.h"
#include "uart_ll.h"

/*=======================================================================
 ===========================SHARED RESOURCES=============================
 =======================================================================*/
#define  SERIAL_BUF_SIZE	2048

extern char *rx_buf;

/*=======================================================================
 ===========================DEFINE FUNCTION==============================
 =======================================================================*/
/**
 * 
 */
void RCOM_get_step_info(RCOM_job_type_t job_type, uint8_t current_step,
		RCOM_step_info_t* step_info);

/**
 * 
 */
RCOM_step_result_t RCOM_step_excution(RCOM_step_info_t* step_info);

/**
 * 
 */
void RCOM_uart_rx_callback(void);

/**
 * 
 */
void RCOM_uart_get_char(void);

/**
 * 
 */
void RCOM_state_machine(void);

/**
 * 
 */
void RCOM_uart_writef(char* frame);

/**
 * 
 */
void RCOM_set_uart(uart_t *defined_uart);

/**
 * 
 */
void RCOM_init(void);

/**
 * 
 */
bool RCOM_SM_is_enable(void);

/**
 * 
 */
void RCOM_SM_enable(void);

/**
 * 
 */
void RCOM_SM_disable(void);

/**
 * 
 */
//void RCOM_enable_rx_buf(void);

/**
 * 
 */
//void RCOM_disable_rx_buf(void);

/**
 * 
 */
void RCOM_set_apn_para(char* apn_name, char* apn_user, char* apn_pass);

/**
 * 
 */
void RCOM_set_domain(char* new_domain);

/**
 * 
 */
void RCOM_set_phone_number(char* phone_number);

/**
 * 
 */
void RCOM_set_sms_msg(char* msg);

/**
 * 
 */
void RCOM_set_rx_buf(char* buf_addr, uint16_t max_num_char);

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
uint16_t RCOM_get_rx_buf_len(void);

//extern RCOM_t RCOM_info;

#endif //REMOTE_COM_RESOURCES_H_
