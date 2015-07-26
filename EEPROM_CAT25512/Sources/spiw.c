/*
 * spiw.c
 *
 *  Created on: Jul 7, 2015
 *      Author: nvhie_000
 */
#include <stdio.h>
#include "spiw.h"
#include "SPIP.h" /* SPI_LDD renamed. Low level controller of SPI */
#include "SPI_CS.h" /* BitsIO_LDD renamed. This component is low level controller of chip select signals */

// declare event 
LWEVENT_STRUCT an_event_send;
LWEVENT_STRUCT an_event_receive;
// declare the mutex
MUTEX_ATTR_STRUCT spi_mutex_attr;
MUTEX_STRUCT spi_mutex;
//spi queue
spiw_queue spi_queue[SPIW_NUM_SLAVE_DEVICE];
_mqx_uint num_piority;
int index = 0;
LDD_TDeviceData* dev_data;

void SPIW_Reinit(void) {
	/* destroy mutex */
//	if (_mutex_destroy(&spi_mutex) != MQX_OK) {
//		printf("Destroy print mutex failed.\n");
//	}
	/* Initialize mutex attributes */
//	if (_mutatr_init(&spi_mutex_attr) != MQX_OK) {
//		printf("Initialize mutex attributes failed.\n");
//	}
	/* Initialize the mutex */
//	if (_mutex_init(&spi_mutex, &spi_mutex_attr) != MQX_OK) {
//		printf("Initialize print mutex failed.\n");
//	}
	_lwevent_create(&an_event_send, LWEVENT_AUTO_CLEAR);
	_lwevent_create(&an_event_receive, LWEVENT_AUTO_CLEAR);
	dev_data = SPIP_Init(NULL );
	SPIP_Enable(dev_data);
}

spiw_err_t SPIW_Acquire(uint8_t slaveDevIndex) {
	/* The slaveDevIndex is out of range */
	if (slaveDevIndex > SPIW_NUM_SLAVE_DEVICE) {
		return SPIW_ERR_INVALID;
	}
	/* The SPI wrapper was acquired by another device */
	else if (_mutex_lock(&spi_mutex) != MQX_OK) {

		return SPIW_ERR_BUSY;
	}
	/* The SPI wrapper is acquired and corresponding chip select is actived */
	else if (_mutex_lock(&spi_mutex) == MQX_OK) {
		/* Active CS */

		spi_queue[index].priority = 0;
		spi_queue[index].old_ceiling_ptr = NULL;
	}

	return SPIW_ERR_OK;
}

spiw_err_t SPIW_WaitAndAcquire(uint8_t slaveDevIndex) {
	/* The slaveDevIndex is out of range */
	if (slaveDevIndex > SPIW_NUM_SLAVE_DEVICE) {

		return SPIW_ERR_INVALID;
	}
	/* The SPI wrapper was acquired by another device */
	else if (_mutex_lock(&spi_mutex) != MQX_OK) {
		spi_queue[index].priority = num_piority++;
		spi_queue[index].old_ceiling_ptr = spi_queue[index - 1].old_ceiling_ptr;
	}

	return SPIW_ERR_OK;
}

spiw_err_t SPIW_BlockedSend(uint16_t numByteSend, uint8_t sendBuf[]) {
	/* Send block to SPI */
	SPIP_SendBlock(dev_data, sendBuf, numByteSend);
	/* Wait until send block succes, timeout = 10 ticks */
	if (_lwevent_wait_ticks(&an_event_send, EVT_BIT_MASK_SEND, TRUE,
			100) != MQX_OK) {
		printf("timeout snd\n");
		return SPIW_ERR_NOT_RESPOND;
	}

	return SPIW_ERR_OK;
}

spiw_err_t SPIW_BlockedSendReceive(uint16_t numByte, uint8_t sendBuf[],
		uint8_t recvBuf[]) {
	/* Wait to receive block */
	SPIP_ReceiveBlock(dev_data, recvBuf, numByte);
	
	/* Send block to receive*/
	SPIP_SendBlock(dev_data, sendBuf, numByte);
	/* Wait to send succes, timeout = 10ticks */
	if (_lwevent_wait_ticks(&an_event_send, EVT_BIT_MASK_SEND, TRUE,
			100) != MQX_OK) {
		printf("timeout s-r\n");
		return SPIW_ERR_NOT_RESPOND;
	}

	/* Wait to receive succes, timeout = 10ticks */
	if (_lwevent_wait_ticks(&an_event_receive, EVT_BIT_MASK_RECEIVE, TRUE,
			400) != MQX_OK) {
		printf("timeout r\n");
		return SPIW_ERR_NOT_RESPOND;
	}

	return SPIW_ERR_OK;
}

void SPIW_Release(void) {
	/* Inactive CS */
	_mutex_unlock(&spi_mutex);
}

void SPIW_send_callback_in_event(void) {
	_lwevent_set(&an_event_send, EVT_BIT_MASK_SEND);
}

void SPIW_recv_callback_in_event(void) {
	_lwevent_set(&an_event_receive, EVT_BIT_MASK_RECEIVE);
}

