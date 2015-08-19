/*
 * DS1307.c
 *
 *  Created on: Aug 2, 2015
 *      Author: nvhie
 */
#include <stdio.h>
#include <cstring>
#include "DS1307.h"

/* chip address */
#define RTC_ADDR	(0x68) //DS1307 address
#define RTC_TIME_FIELD (7) //00H=SEC...MIN...HOUR...DAY...DATE...MONTH...06H=YEAR
/* address map */
#define SEC_REG		(0x00) //second register
#define MIN_REG		(0x01) //minute register
#define HOUR_REG	(0x02) //hour register
#define DAY_REG		(0x03) //day register
#define DATE_REG	(0x04) //date register
#define MONTH_REG	(0x05) //month register
#define YEAR_REG	(0x06) //year register
/* hour modes */
#define HOUR_BITS_MASK	(0x3F) //only support 24h mode (bit6=0)
/* wr/rd bit */
//#define WRITE	(0xFE)
//#define READ	(0x01)

static uint8_t dec2bcd(uint8_t dec_num);
static uint8_t bcd2dec(uint8_t bcd_num);

static uint8_t dec2bcd(uint8_t dec_num) {
	return ((dec_num / 10 * 16) + (dec_num % 10));
}

static uint8_t bcd2dec(uint8_t bcd_num) {
	return ((bcd_num / 16 * 10) + (bcd_num % 16));
}

bool RTC_reinit(void) {
	I2CW_Reinit();

	return TRUE ;
}

bool RTC_set_time(RTC_time_t time_type, uint8_t *time) {
	uint8_t data_send[1 + RTC_TIME_FIELD];
	uint8_t index = 0;

	data_send[1] = dec2bcd(*time);
	switch (time_type) {
	case RTC_TIME:
		data_send[0] = SEC_REG;
		time[2] &= HOUR_BITS_MASK;
		for (index = 0; index < RTC_TIME_FIELD; index++) {
			data_send[index + 1] = dec2bcd(time[index]);
		}
		break;
	case SEC:
		data_send[0] = SEC_REG;
		break;
	case MIN:
		data_send[0] = MIN_REG;
		break;
	case HOUR:
		data_send[0] = HOUR_REG;
		data_send[1] = dec2bcd((*time) & HOUR_BITS_MASK);
		break;
	case DAY:
		data_send[0] = DAY_REG;
		break;
	case DATE:
		data_send[0] = DATE_REG;
		break;
	case MONTH:
		data_send[0] = MONTH_REG;
		break;
	case YEAR:
		data_send[0] = YEAR_REG;
		break;
	default:
		break;
	}

	I2CW_WaitAndAcquire();
	/* set slave addr */
	I2CW_SetSlaveAddress(addr_7bits, RTC_ADDR);
	/* write value into all register */
	if (time_type == RTC_TIME) {
		if (I2CW_BlockedMasterSend(1 + RTC_TIME_FIELD, data_send,
				TRUE ) != I2CW_ERR_OK) {
			I2CW_Release();
			return FALSE ;
		}
	}
	/* write value into a register */
	else {
		if (I2CW_BlockedMasterSend(2, data_send, TRUE ) != I2CW_ERR_OK) {
			I2CW_Release();
			return FALSE ;
		}
	}
	I2CW_Release();

	return TRUE ;
}

bool RTC_get_time(RTC_time_t time_type, uint8_t *time) {
	uint8_t rtc_reg = 0;

	switch (time_type) {
	case RTC_TIME:
	case SEC:
		rtc_reg = SEC_REG;
		break;
	case MIN:
		rtc_reg = MIN_REG;
		break;
	case HOUR:
		rtc_reg = HOUR_REG;
		break;
	case DAY:
		rtc_reg = DAY_REG;
		break;
	case DATE:
		rtc_reg = DATE_REG;
		break;
	case MONTH:
		rtc_reg = MONTH_REG;
		break;
	case YEAR:
		rtc_reg = YEAR_REG;
		break;
	default:
		break;
	}

	I2CW_WaitAndAcquire();
	/* set slave addr */
	I2CW_SetSlaveAddress(addr_7bits, RTC_ADDR);
	/* set internal pointer of RTC chip */
	if (I2CW_BlockedMasterSend(1, &rtc_reg, TRUE ) != I2CW_ERR_OK) {
		I2CW_Release();
		return FALSE ;
	}
	/* get time */
	if (time_type == RTC_TIME) {
		if (I2CW_BlockedMasterReceive(RTC_TIME_FIELD, time) != I2CW_ERR_OK) {
			I2CW_Release();
			return FALSE ;
		}
	} else {
		if (I2CW_BlockedMasterReceive(1, time) != I2CW_ERR_OK) {
			I2CW_Release();
			return FALSE ;
		}
	}
	I2CW_Release();

	if (time_type == RTC_TIME) {
		uint8_t indx = 0;
		for (indx = 0; indx < RTC_TIME_FIELD; indx++) {
			time[indx] = bcd2dec(time[indx]);
		}
	} else {
		*time = bcd2dec(*time);
	}

	return TRUE ;
}
