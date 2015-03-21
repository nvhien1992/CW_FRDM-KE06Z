/*
 * adc_ll.h
 *
 *  Created on: Mar 9, 2015
 *      Author: nvhie_000
 */

#ifndef ADC_LL_H_
#define ADC_LL_H_

#include "PE_Types.h"
#include "PE_Error.h"

typedef struct {
	LDD_TDeviceData* (*Init)(LDD_TUserData *usr_data);
	LDD_TError (*CreateSampleGroup)(LDD_TDeviceData *dev_data,
			LDD_ADC_TSample *sample_group, uint8_t sample_count);
	LDD_TError (*StartSingleMeasurement)(LDD_TDeviceData *dev_data);
	void (*Main)(LDD_TDeviceData *dev_data);
	LDD_TError (*GetMeasuredValues)(LDD_TDeviceData *dev_data,
			LDD_TData *out_buff);
} adc_t;

bool adc_init(adc_t *adc_module);

bool adc_mesurement(uint8_t *channel_table, uint8_t sample_count,
		uint16_t *out_buffer);

#endif /* ADC_LL_H_ */
