/**
 @author   Huynh Trung Tin <tinvuong51003405@gmail.com>
 Nguyen Van Hien <nvhien1992@gmail.com>
 @copyright Copyright (C) 2014 <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b> , All rights reserved 
 */

#ifndef REMOTE_COM_RESOURCES_H_
#define REMOTE_COM_RESOURCES_H_

#include "remote_com.h"

/*=======================================================================
 ===========================SHARED RESOURCES=============================
 =======================================================================*/
#define  SERIAL_BUF_SIZE		2048

extern char* rx_buf;
extern uint16_t rx_buf_counter;

/*=======================================================================
 ===========================DEFINE FUNCTION==============================
 =======================================================================*/
/**
 * 
 */
void RCOM_get_step_info(job_type_t job_type, uint8_t current_step,
		RCOM_step_info_t* step_info);

/**
 * 
 */
RCOM_step_result_t RCOM_step_excution(RCOM_step_info_t* step_info);

/**
 * 
 */
int uart_rx_callback(unsigned char c);

/**
 * 
 */
void uart_writef(char* frame);

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
void RCOM_enable_rx_buf(void);

/**
 * 
 */
void RCOM_disable_rx_buf(void);

/**
 * 
 */
void RCOM_set_domain(char* new_domain);

/**
 * 
 */
void RCOM_set_rx_buf(char* buf_addr);

/**
 * 
 */
char* RCOM_get_rx_buf(void);

/**
 * 
 */
uint16_t RCOM_get_rx_buf_len(void);

#endif //REMOTE_COM_RESOURCES_H_
