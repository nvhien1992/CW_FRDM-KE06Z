/*
 * spiw.h
 *
 *  Created on: Jul 7, 2015
 *      Author: nvhie_000
 */

#ifndef SPIW_H_
#define SPIW_H_

/* User include*/
#include "PE_Types.h"
#include "mqxlite.h"

////////////////////////////////////////////////
#define SPIW_NUM_SLAVE_DEVICE	1

////////////////////////////////////////////////
typedef uint8_t spiw_err_t;
#define SPIW_ERR_OK				0	/**! The function work well */
#define SPIW_ERR_BUSY			1	/**! SPI is busying so that the function is aborted */
#define SPIW_ERR_INVALID		2	/**! Input parameters of the function is invalid */
#define SPIW_ERR_NOT_RESPOND	3	/**! SPI is not respond as expected */
////////////////////////////////////////////////

#define EVT_BIT_MASK_SEND 0x01
#define EVT_BIT_MASK_RECEIVE 0x02
typedef struct {
	LDD_TDeviceData *spi_user_ptr;
	LDD_TDeviceData *chip_select;
} spiw_t;

typedef struct {
	_mqx_uint_ptr old_ceiling_ptr;
	_mqx_uint priority;
} spiw_queue;

/**
 \brief Reinit the SPI wrapper. Reset related Kinetis peripheral and all the local variables
 \return NONE
 */
void SPIW_Reinit(void);

/**
 \brief Acquire the SPI wrapper for communicating. 
 \para slaveDevIndex Index of the slave SPI device
 \retval SPIW_ERR_INVALID The slaveDevIndex is out of range
 \retval SPIW_ERR_BUSY The SPI wrapper was acquired by another device
 \retval SPIW_ERR_OK The SPI wrapper is acquired and corresponding chip select is actived
 */
spiw_err_t SPIW_Acquire(uint8_t slaveDevIndex);

/**
 \brief Acquire the SPI wrapper for communicating. If the SPI was acquired, this function wait until the SPI wrapper is released
 \para slaveDevIndex Index of the slave SPI device
 \retval SPIW_ERR_INVALID The slaveDevIndex is out of range
 \retval SPIW_ERR_OK The SPI wrapper is acquired and corresponding chip select is actived
 */
spiw_err_t SPIW_WaitAndAcquire(uint8_t slaveDevIndex);

/**
 \brief Send a buffer over SPI. Block the task until all data are sent
 \para numByte Number of sending byte
 \para sendBuf Sending buffer
 \retval SPIW_ERR_OK All data are sent correctly
 \retval SPIW_ERR_NOT_RESPOND End data transmision cannot be detected
 */
spiw_err_t SPIW_BlockedSend(uint16_t numByteSend, uint8_t sendBuf[]);

/**
 \brief Send a buffer and receive another one over SPI simultaneously. Block the task until all data are sent and received
 \para numByte Number of sending byte
 \para sendBuf Sending buffer
 \para recvBuf Receiving buffer
 \retval SPIW_ERR_OK All data are sent and received correctly
 \retval SPIW_ERR_NOT_RESPOND End data receiving cannot be detected
 */
spiw_err_t SPIW_BlockedSendReceive(uint16_t numByte, uint8_t sendBuf[],
		uint8_t recvBuf[]);

/**
 \brief Release the SPI wrapper and deactivate all the slave device chip select signals
 */
void SPIW_Release(void);

/**
 * @brief Called in send event after sending completely.
 */
void SPIW_send_callback_in_event(void);

/**
 * @brief Called in receiving event after receiving completely.
 */
void SPIW_recv_callback_in_event(void);

#endif /* SPIW_H_ */
