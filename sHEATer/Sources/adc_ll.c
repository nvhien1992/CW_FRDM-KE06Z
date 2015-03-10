/*
 * adc_ll.c
 *
 *  Created on: Mar 9, 2015
 *      Author: nvhie_000
 */
#include "adc_ll.h"

static LDD_TDeviceData *adc_dev = NULL;

void adc_init(void) {
	adc_dev = ADC_SS_Init(NULL);
}

bool adc_mesurement(uint8_t *channel_table, uint8_t sample_count,
		uint16_t *out_buffer) {
	if (!adc_dev) {
		return false;
	}
	LDD_ADC_TSample sample_group[sample_count];
	uint8_t index = 0;
	for(index = 0; index < sample_count; index++) {
		sample_group[index].ChannelIdx = channel_table[index];
	}
	if (ADC_SS_CreateSampleGroup(adc_dev, sample_group, sample_count) != 0) {
		return false;
	}
	if (ADC_SS_StartSingleMeasurement(adc_dev) != 0) {
		return false;
	}
	ADC_SS_Main(adc_dev);
	ADC_SS_GetMeasuredValues(adc_dev, out_buffer);

	return true;
}
