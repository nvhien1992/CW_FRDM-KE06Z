/*
 * spiw.c
 *
 *  Created on: Jul 8, 2015
 *      Author: Administrator
 */
#include <stdio.h>
#include "spiw.h"
#include "SPIP.h" /* SPI_LDD renamed. Low level controller of SPI */
#include "SPIWP_CS.h" /* BitsIO_LDD renamed. This component is low level controller of chip select signals */
// declare event 
LWEVENT_STRUCT an_event_send;
LWEVENT_STRUCT an_event_receive;

// declare the mutex
MUTEX_ATTR_STRUCT spi_mutex_attr;
MUTEX_STRUCT spi_mutex;

void SPIW_Reinit(void) {
	/* destroy mutex */
	if (_mutex_destroy(&spi_mutex) == MQX_OK) {
		printf("Destroy ok.\n");
	}
	/* Initialize mutex attributes */
	if (_mutatr_init(&spi_mutex_attr) != MQX_OK) {
		printf("Initialize mutex attributes failed.\n");
	}
	/* Initialize the mutex */
	if (_mutex_init(&spi_mutex, &spi_mutex_attr) != MQX_OK) {
		printf("Initialize print mutex failed.\n");
	}

	/* initialize events */
	_lwevent_create(&an_event_send, LWEVENT_AUTO_CLEAR);
	_lwevent_create(&an_event_receive, LWEVENT_AUTO_CLEAR);

	/* disable all CS */
	int i = 0;
	for (i = 0; i < SPIW_NUM_SLAVE_DEVICE; i++) {
		SPIWP_CS_SetBit(SPIWP_CS_DeviceData, i);
	}
}

spiw_err_t SPIW_Acquire(uint8_t slaveDevIndex) {
	/* The slaveDevIndex is out of range */
	if (slaveDevIndex >= SPIW_NUM_SLAVE_DEVICE) {
		return SPIW_ERR_INVALID;
	}
	/* The SPI wrapper is acquired and corresponding chip select is actived */
	if (_mutex_try_lock(&spi_mutex) == MQX_OK) {
		/* Active CS */
		SPIWP_CS_ClrBit(SPIWP_CS_DeviceData, slaveDevIndex); // CS low   
		return SPIW_ERR_OK;
	}
	/* The SPI wrapper was acquired by another device */
	return SPIW_ERR_BUSY;
}

spiw_err_t SPIW_WaitAndAcquire(uint8_t slaveDevIndex) {
	/* The slaveDevIndex is out of range */
	if (slaveDevIndex >= SPIW_NUM_SLAVE_DEVICE) {
		return SPIW_ERR_INVALID;
	}

	/* The SPI wrapper was acquired by another device */
	if (_mutex_lock(&spi_mutex) == MQX_OK) {
		/* Active CS */
		SPIWP_CS_ClrBit(SPIWP_CS_DeviceData, slaveDevIndex); // CS low 
		return SPIW_ERR_OK;
	}

	return SPIW_ERR_INVALID;
}

spiw_err_t SPIW_BlockedSend(uint16_t numByte, uint8_t sendBuf[]) {
	/* Send block to SPI */
	SPIP_SendBlock(SPIP_DeviceData, sendBuf, numByte);

	/* Wait until send block succes, timeout = numByte*2ticks */
	if (_lwevent_wait_ticks(&an_event_send, EVT_BIT_MASK_SEND, TRUE,
			numByte * 2) == MQX_OK) {
		return SPIW_ERR_OK;
	} else {
		return SPIW_ERR_NOT_RESPOND;
	}
}

spiw_err_t SPIW_BlockedSendReceive(uint16_t numByte, uint8_t sendBuf[],
		uint8_t recvBuf[]) {
	/* Wait to receive block */
	SPIP_ReceiveBlock(SPIP_DeviceData, recvBuf, numByte);

	/* Send block to receive*/
	SPIP_SendBlock(SPIP_DeviceData, sendBuf, numByte);

	/* Wait to send succes, timeout =numByte * 2ticks */
	if (_lwevent_wait_ticks(&an_event_send, EVT_BIT_MASK_SEND, TRUE,
			numByte * 2) != MQX_OK) {
		return SPIW_ERR_NOT_RESPOND;
	}

	/* Wait to receive succes, timeout = 10ticks */
	if (_lwevent_wait_ticks(&an_event_receive, EVT_BIT_MASK_RECEIVE, TRUE,
			numByte * 2) != MQX_OK) {
		return SPIW_ERR_NOT_RESPOND;
	}

	return SPIW_ERR_OK;
}

void SPIW_Release(void) {
	/* Inactive CS */
	int i = 0;
	for (i = 0; i < SPIW_NUM_SLAVE_DEVICE; i++) {
		SPIWP_CS_SetBit(SPIWP_CS_DeviceData, i);
	}
	_mutex_unlock(&spi_mutex);
}

void SendBlock_Success(void) {
	/*event after send success*/
	_lwevent_set(&an_event_send, EVT_BIT_MASK_SEND);
}

void ReceiveBlock_Success(void) {
	/*event after receive success*/
	_lwevent_set(&an_event_receive, EVT_BIT_MASK_RECEIVE);
}
