/**
 @file uart_ll.c
 @brief
 @author <b>Nguyen Van Hien</b> <nvhien1992@gmail.com>
 @copyright Copyright &copy; 2015, <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b>, All rights reserved. 
 */
#include <stdarg.h>
#include <cstring>
#include <stdio.h>
#include "uart_ll.h"
#include "PE_Error.h"

#define MAX_CHAR_PRINT 128

void uart_init(uart_t *uart) {
	uart->dev_data = uart->Init(NULL);
}

bool uart_send(uart_t *uart, void *in_buff, uint16_t buff_size) {
	if(uart->SendBlock(uart->dev_data, in_buff, buff_size) != ERR_OK) {
		return FALSE;
	}
	if(uart->Main != NULL) {
		uart->Main(uart->dev_data);
	}
	return TRUE;
}

bool uart_receive(uart_t *uart, void *out_buff, uint16_t buff_size) {
	if(uart->ReceiveBlock(uart->dev_data, out_buff, buff_size) != ERR_OK) {
		return FALSE;
	}
	if(uart->Main != NULL) {
		uart->Main(uart->dev_data);
	}
	return TRUE;
}

void uart_printf(uart_t *uart, const char *format, ...) {
	char buffer[MAX_CHAR_PRINT];
	va_list args;

	va_start(args, format);
	vsnprintf(buffer, MAX_CHAR_PRINT, format, args);
	va_end(args);

	/* print buffer to lcd */
	uart_send(uart, buffer, strlen(buffer));
}

