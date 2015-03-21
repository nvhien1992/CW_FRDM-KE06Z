/*
 * adc_ll.c
 *
 *  Created on: Mar 9, 2015
 *      Author: nvhie_000
 */
#include "adc_ll.h"

static LDD_TDeviceData *adc_dev = NULL;
static adc_t *adc;

bool adc_init(adc_t *adc_module) {
	if (!adc_module) {
		return FALSE;
	}
	adc = adc_module;
	adc_dev = adc->Init(NULL);
	
	return TRUE;
}

bool adc_mesurement(uint8_t *channel_table, uint8_t sample_count,
		uint16_t *out_buffer) {
	if (!adc_dev) {
		return FALSE;
	}
	LDD_ADC_TSample sample_group[sample_count];
	uint8_t index = 0;
	for (index = 0; index < sample_count; index++) {
		sample_group[index].ChannelIdx = channel_table[index];
	}
	if (adc->CreateSampleGroup(adc_dev, sample_group, sample_count) != ERR_OK) {
		return FALSE;
	}
	if (adc->StartSingleMeasurement(adc_dev) != ERR_OK) {
		return FALSE;
	}
	adc->Main(adc_dev);
	adc->GetMeasuredValues(adc_dev, out_buffer);

	return TRUE;
}
