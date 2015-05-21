/**
 @file uart_ll.h
 @brief
 @author <b>Nguyen Van Hien</b> <nvhien1992@gmail.com>
 @copyright Copyright (C) 2015 <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b> , All rights reserved 
 */

#ifndef UART_LL_H_
#define UART_LL_H_

#include "PE_Types.h"

typedef struct {
	LDD_TDeviceData* (*Init)(LDD_TUserData *usr_data);
	LDD_TError (*SendBlock)(LDD_TDeviceData *dev_data, LDD_TData *in_buff,
			uint16_t buff_size);
	LDD_TError (*ReceiveBlock)(LDD_TDeviceData *dev_data, LDD_TData *out_buff,
			uint16_t buff_size);
	void (*Main)(LDD_TDeviceData *dev_data); //be used if polling. set NULL if not use.
	LDD_TDeviceData *dev_data;
} uart_t;

void uart_init(uart_t *uart);

bool uart_send(uart_t *uart, void *in_buff, uint16_t buff_size);

bool uart_receive(uart_t *uart, void *out_buff, uint16_t buff_size);

void uart_printf(uart_t *uart, const char *format, ...);

#endif /* UART_LL_H_ */
