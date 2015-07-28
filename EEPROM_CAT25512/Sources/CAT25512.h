/*
 * CAT25512.h
 *
 *  Created on: Jul 7, 2015
 *      Author: nvhie_000
 */

#ifndef CAT25512_H_
#define CAT25512_H_

#include "spiw.h"

typedef uint8_t srom_err_t;
#define SROM_ERR_OK				(0x00)
#define SROM_ERR_DAMAGED		(0x01)
#define SROM_ERR_VALIDATE_FAIL	(0x02)

void SROM_Reinit(uint16_t a_page_size);

/**
 \brief Read a data block from the SPI EEPROM to a buffer in internal SRAM
 \para startAddress Start address of the data block
 \para numByte Number of bytes
 \para readBuf The buffer storing read data
 \retval SROM_ERR_OK Read OK
 \retval SROM_ERR_DAMAGED The SROM does not respond (not yet)
 */
srom_err_t SROM_Read(uint32_t startAddress, uint16_t numByteRead,
		uint8_t readBuf[]);

/**
 \brief Write a data block from a buffer in internal SRAM to the SPI EEPROM 
 \para startAddress Start address of the data block
 \para numByte Number of bytes
 \para readBuf The buffer storing written data
 \retval SROM_ERR_OK Write OK
 \retval SROM_ERR_DAMAGED The SROM does not respond (not yet)
 */
srom_err_t SROM_Write(uint32_t startAddress, uint16_t numByteWrite,
		uint8_t writeBuf[]);

/**
 \brief Read a data block from the SPI EEPROM to a buffer in internal SRAM. Read data again and compare with previous read data to ensure the data correcness
 \para startAddress Start address of the data block
 \para numByte Number of bytes
 \para readBuf The buffer storing read data
 \para numRetry Number of retry (read and reread) if validating is fail
 \retval SROM_ERR_OK Read OK
 \retval SROM_ERR_DAMAGED The SROM does not respond (not yet)
 \retval SROM_ERR_VALIDATE_FAIL Reread data are always differ with previous read data
 */
srom_err_t SROM_ReadValidated(uint32_t startAddress, uint16_t numByteRead,
		uint8_t readBuf[], uint8_t numRetry);

/**
 \brief Write a data block from a buffer in internal SRAM to the SPI EEPROM. Reread data and compare with written data to ensure the data correcness
 \para startAddress Start address of the data block
 \para numByte Number of bytes
 \para readBuf The buffer storing written data
 \para numRetry Number of retry (write and reread) if validating is fail
 \retval SROM_ERR_OK Read OK
 \retval SROM_ERR_DAMAGED The SROM does not respond (not yet)
 \retval SROM_ERR_VALIDATE_FAIL Reread data are always differ with written data
 */
srom_err_t SROM_WriteValidated(uint32_t startAddress, uint16_t numByte,
		uint8_t writeBuf[], uint8_t numRetry);

#endif /* CAT25512_H_ */
