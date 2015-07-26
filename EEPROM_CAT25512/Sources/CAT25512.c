/*
 * CAT25512.c
 *
 *  Created on: Jul 7, 2015
 *      Author: nvhie_000
 */
#include <cstring>
#include <stdio.h>
#include "CAT25512.h"
#include "mqxlite.h"
#include "SPI_CS.h"
#include "WP.h"

#define SYS_CLK (48) //MHz
/* cmds */
typedef uint8_t srom_cmd_t;
#define WRDI_CMD	(0x04)
#define WREN_CMD	(0x06)
#define WRSR_CMD	(0x01)
#define RDSR_CMD	(0x05)
#define WRITE_CMD	(0x02)
#define READ_CMD	(0x03)

/* masks in status register */
#define WPEN_MASK	(0x80) //bit 7
#define IPL_MASK	(0x40) //bit 6
#define LIP_MASK	(0x10) //bit 4
#define BP1_MASK	(0x08) //bit 3
#define BP0_MASK	(0x04) //bit 2
#define WEL_MASK	(0x02) //bit 1
#define NRDY_MASK	(0x01) //bit 0
/* a mem page size */
#define PAGE_SIZE	(64) //bytes
/* internal functions */
static void SROM_SR_write_bit(uint8_t SR_mask, uint8_t set_or_clear);
static uint8_t SROM_SR_read(void);
static void delay_5ms(void);
static void delay_us(uint32_t us);

static void delay_5ms(void) {
	_time_delay_ticks(1);
}

static void delay_us(uint32_t us) {
	uint32_t num_clocks = SYS_CLK * us;
	uint32_t count = 0;

	for (count = 0; count < num_clocks; count++) {
		__asm__ __volatile__("nop");
	}
}

static void SROM_SR_write_bit(uint8_t SR_mask, uint8_t set_or_clear) {
	uint8_t SR_data = SROM_SR_read();
	if (set_or_clear > 0) {
		SR_data |= SR_mask;
	} else {
		SR_data &= (~SR_mask);
	}
	uint8_t data_send[2];
	data_send[0] = WRSR_CMD;
	data_send[1] = SR_data;

	SPI_CS_ClrVal(NULL );
	delay_us(1);
	SPIW_BlockedSend(2, data_send);
	delay_us(1);
	SPI_CS_SetVal(NULL );

	/* delay 5ms after writing */
	delay_5ms();
}

static uint8_t SROM_SR_read(void) {
	uint8_t cmd = RDSR_CMD;
	uint8_t SR_data = 0xff;

	SPI_CS_ClrVal(NULL );
	SPIW_BlockedSend(1, &cmd);
	SPIW_BlockedSendReceive(1, &SR_data, &SR_data);
	SPI_CS_SetVal(NULL );

	return SR_data;
}

void SROM_Reinit(void) {
	SPIW_Reinit();
	SPI_CS_SetVal(NULL );
	WP_SetVal(NULL );
	SROM_write_enable();
}

void SROM_write_enable(void) {
	uint8_t cmd = WREN_CMD;

//	SPIW_Acquire();
	SPI_CS_ClrVal(NULL );
	delay_us(1);
	SPIW_BlockedSend(1, &cmd);
	delay_us(1);
	SPI_CS_SetVal(NULL );
//	SPIW_Release();

	delay_5ms();
}

void SROM_write_disable(void) {
	uint8_t cmd = WRDI_CMD;

//	SPIW_Acquire();
	SPI_CS_ClrVal(NULL );
	delay_us(1);
	SPIW_BlockedSend(1, &cmd);
	delay_us(1);
	SPI_CS_SetVal(NULL );
//	SPIW_Release();

	delay_5ms();
}

bool SROM_write_status(void) {
	uint8_t sr = SROM_SR_read();

	sr &= WEL_MASK;

	return (sr != 0);
}

srom_err_t SROM_Read(uint16_t startAddress, uint16_t numByteRead,
		uint8_t readBuf[]) {
	uint8_t data_send[3];
	data_send[0] = READ_CMD;
	data_send[1] = (uint8_t) (startAddress >> 8);
	data_send[2] = (uint8_t) startAddress;

	uint8_t i = 0;

//	SPIW_Acquire();
	SPI_CS_ClrVal(NULL );
	SPIW_BlockedSend(3, data_send);
	for (i = 0; i < numByteRead; i++) {
		SPIW_BlockedSendReceive(1, &readBuf[i], &readBuf[i]);
	}
	SPI_CS_SetVal(NULL );
//	SPIW_Release();

	return SROM_ERR_OK;
}

srom_err_t SROM_Write(uint16_t startAddress, uint16_t numByteWrite,
		uint8_t writeBuf[]) {
	uint8_t data_send[3 + numByteWrite];
	data_send[0] = WRITE_CMD;
	data_send[1] = (uint8_t) (startAddress >> 8);
	data_send[2] = (uint8_t) startAddress;
	memcpy(&data_send[3], writeBuf, numByteWrite);

//	SPIW_Acquire();
	if (numByteWrite > PAGE_SIZE) {
		SROM_SR_write_bit(IPL_MASK, 1);
	}
	SPI_CS_ClrVal(NULL );
	delay_us(1);
	SPIW_BlockedSend(numByteWrite + 3, data_send);
	delay_us(1);
	SPI_CS_SetVal(NULL );
//	SPIW_Release();

	delay_5ms();

	return SROM_ERR_OK;
}

srom_err_t SROM_ReadValidated(uint16_t startAddress, uint16_t numByteRead,
		uint8_t readBuf[], uint8_t numRetry) {
	uint8_t read_back = 0;
	uint8_t index = 0;
	bool retry = FALSE;
	uint8_t data_send[3];

	do {
		SROM_Read(startAddress, numByteRead, readBuf);

		/* send cmd and addr before read data back  */
		data_send[0] = READ_CMD;
		data_send[1] = (uint8_t) (startAddress >> 8);
		data_send[2] = (uint8_t) startAddress;
		SPI_CS_ClrVal(NULL );
		SPIW_BlockedSend(3, data_send);

		/* read back */
		for (index = 0; index < numByteRead; index++) {
			SPIW_BlockedSendReceive(1, &read_back, &read_back);
			if (read_back != readBuf[index]) {
				SPI_CS_SetVal(NULL );
				retry = TRUE;
				numRetry--;
				break;
			} else {
				retry = FALSE;
			}
		}
		SPI_CS_SetVal(NULL );
	} while (retry && numRetry > 0);

	if (numRetry == 0 || retry == TRUE) {
		return SROM_ERR_VALIDATE_FAIL;
	}

	return SROM_ERR_OK;
}

srom_err_t SROM_WriteValidated(uint16_t startAddress, uint16_t numByteWrite,
		uint8_t writeBuf[], uint8_t numRetry) {
	uint8_t read_back = 0;
	uint8_t index = 0;
	bool retry = FALSE;
	uint8_t data_send[3];

	SROM_Write(startAddress, numByteWrite, writeBuf);

	do {
		/* send cmd and addr before read data back */
		data_send[0] = READ_CMD;
		data_send[1] = (uint8_t) (startAddress >> 8);
		data_send[2] = (uint8_t) startAddress;
		SPI_CS_ClrVal(NULL );
		SPIW_BlockedSend(3, data_send);

		/* read back */
		for (index = 0; index < numByteWrite; index++) {
			SPIW_BlockedSendReceive(1, &read_back, &read_back);
			if (read_back != writeBuf[index]) {
				SPI_CS_SetVal(NULL );
				retry = TRUE;
				numRetry--;
				break;
			} else {
				retry = FALSE;
			}
		}
		SPI_CS_SetVal(NULL );
	} while (retry && numRetry > 0);

	if (numRetry == 0 || retry == TRUE) {
		return SROM_ERR_VALIDATE_FAIL;
	}

	return SROM_ERR_OK;
}
