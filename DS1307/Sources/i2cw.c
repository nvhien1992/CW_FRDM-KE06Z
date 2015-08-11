/*
 * i2cw.c
 *
 *  Created on: Aug 2, 2015
 *      Author: nvhie
 */
#include <stdio.h>
#include "i2cw.h"

// declare event 
LWEVENT_STRUCT i2c_evt;
#define EVT_BIT_MASK_SEND		(0x01)
#define EVT_BIT_MASK_RECEIVE	(0x02)

// declare the mutex
MUTEX_STRUCT i2c_mutex;
bool is_inited = FALSE;

// declare the i2c device
LDD_TDeviceData *I2CP_dev_data = NULL;

bool I2CW_Reinit(void) {
	/* destroy mutex and event */
	if (is_inited) {
		_mutex_unlock(&i2c_mutex);
		if (_mutex_destroy(&i2c_mutex) != MQX_OK) {
			printf("Destroy mutex failed.\n");
			return FALSE;
		}

		if (_lwevent_destroy(&i2c_evt) != MQX_OK) {
			printf("Destroy event failed.\n");
			return FALSE;
		}
	}
	/* initialize the mutex */
	if (_mutex_init(&i2c_mutex, NULL ) != MQX_OK) {
		printf("Initialize mutex failed.\n");
		return FALSE;
	}

	/* initialize events */
	if(_lwevent_create(&i2c_evt, LWEVENT_AUTO_CLEAR) != MQX_OK) {
		printf("Initialize event failed.\n");
		return FALSE;
	}

	/* init i2c */
	I2CP_dev_data = I2CP_Init(NULL );

	return TRUE ;
}

i2cw_err_t I2CW_Acquire(void) {
	/* The I2C wrapper is acquired and corresponding chip select is actived */
	if (_mutex_try_lock(&i2c_mutex) == MQX_OK) {
		return I2CW_ERR_OK;
	}
	/* The I2C wrapper was acquired by another device */
	return I2CW_ERR_BUSY;
}

i2cw_err_t I2CW_WaitAndAcquire(void) {
	/* The I2C wrapper was acquired by another device */
	if (_mutex_lock(&i2c_mutex) == MQX_OK) {
		return I2CW_ERR_OK;
	}

	return I2CW_ERR_INVALID;
}

i2cw_err_t I2CW_BlockedSend(uint16_t numByte, uint8_t sendBuf[], bool send_stop) {
	/* Send block to I2C */
	if (send_stop) {
		I2CP_MasterSendBlock(I2CP_dev_data, sendBuf, numByte,
				LDD_I2C_SEND_STOP);
	} else {
		I2CP_MasterSendBlock(I2CP_dev_data, sendBuf, numByte,
				LDD_I2C_NO_SEND_STOP);
	}

	/* Wait until send block succes, timeout = numByte*2ticks */
	if (_lwevent_wait_ticks(&i2c_evt, EVT_BIT_MASK_SEND, TRUE,
			numByte * 2) == MQX_OK) {
		return I2CW_ERR_OK;
	}
	return I2CW_ERR_NOT_RESPOND;
}

i2cw_err_t I2CW_BlockedReceive(uint16_t numByte, uint8_t recvBuf[]) {
	/* Wait to receive block */
	I2CP_MasterReceiveBlock(I2CP_dev_data, recvBuf, numByte, LDD_I2C_SEND_STOP);

	/* Wait to send succes, timeout = numByte * 2ticks */
	if (_lwevent_wait_ticks(&i2c_evt, EVT_BIT_MASK_SEND, TRUE,
			numByte * 2) != MQX_OK) {
		return I2CW_ERR_NOT_RESPOND;
	}

	/* Wait to receive succes, timeout = numByte * 2ticks */
	if (_lwevent_wait_ticks(&i2c_evt, EVT_BIT_MASK_RECEIVE, TRUE,
			numByte * 2) != MQX_OK) {
		return I2CW_ERR_NOT_RESPOND;
	}

	return I2CW_ERR_OK;
}

void I2CW_Release(void) {
	_mutex_unlock(&i2c_mutex);
}

void I2CW_SendBlock_Success(void) {
	/*event after send success*/
	_lwevent_set(&i2c_evt, EVT_BIT_MASK_SEND);
}

void I2CW_ReceiveBlock_Success(void) {
	/*event after receive success*/
	_lwevent_set(&i2c_evt, EVT_BIT_MASK_RECEIVE);
}
