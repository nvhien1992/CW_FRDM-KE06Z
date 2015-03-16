/*
 * adc_ll.h
 *
 *  Created on: Mar 9, 2015
 *      Author: nvhie_000
 */

#ifndef ADC_LL_H_
#define ADC_LL_H_

#include <stdint.h>
#include <stdbool.h>

void adc_init(void);

bool adc_mesurement(uint8_t *channel_table, uint8_t sample_count, uint16_t *out_buffer);

#endif /* ADC_LL_H_ */
