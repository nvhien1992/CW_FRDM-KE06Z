/*
 * i2cw.h
 *
 *  Created on: Aug 2, 2015
 *      Author: nvhie
 */

#ifndef I2CW_H_
#define I2CW_H_

#include "PE_Types.h"
#include "mqxlite.h"
#include "I2CP.h"

typedef uint8_t i2cw_err_t;
#define I2CW_ERR_OK				0	/**! The function work well */
#define I2CW_ERR_BUSY			1	/**! I2C is busying so that the function is aborted */
#define I2CW_ERR_INVALID		2	/**! Input parameters of the function is invalid */
#define I2CW_ERR_NOT_RESPOND	3	/**! I2C is not respond as expected */

typedef enum {
	addr_7bits,
	addr_10bits
} I2CW_addr_bits_t;

bool I2CW_Reinit(void);

i2cw_err_t I2CW_Acquire(void);

i2cw_err_t I2CW_WaitAndAcquire(void);

void I2CW_SetSlaveAddress(I2CW_addr_bits_t addr_bits_mode, uint16_t slave_addr);

i2cw_err_t I2CW_BlockedMasterSend(uint16_t numByte, uint8_t sendBuf[], bool send_stop);

i2cw_err_t I2CW_BlockedMasterReceive(uint16_t numByte, uint8_t recvBuf[]);

void I2CW_Release(void);

void I2CW_MasterSend_Success(void);

void I2CW_MasterReceive_Success(void);

#endif /* I2CW_H_ */
