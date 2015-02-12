/*
 * sensor.c
 *
 *  Created on: Feb 11, 2015
 *      Author: nvhie_000
 */
#include "global.h"
#include "sensor.h"

#define DEBUG_EN 1
#define NOTIFY_EN 1
#include "debug.h"

#define V_REF 3300 //mV
#define TIMER_PERIOD (10) //ms
#define SENSOR_SAMPLING_PERIOD (500 / TIMER_PERIOD) //sampling every 500ms (timer_period = 10ms)
static uint8_t sampling_time_count = 0;

uint8_t get_sensor_id(temp_sensor_t *a_temp_sensor) {
	return a_temp_sensor->dev_id;
}

float get_sensor_value(temp_sensor_t *a_temp_sensor) {
	return a_temp_sensor->temp_value;
}

void temp_sensor_callback_timer_isr(temp_sensor_t *a_temp_sensor) {
	sampling_time_count = sampling_time_count + 1;

	if (sampling_time_count == SENSOR_SAMPLING_PERIOD) {
		sampling_time_count = 0;
		if (a_temp_sensor->StartMeasurement(a_temp_sensor->arg) == 0) {
			uint16_t adc_value = 0;
			a_temp_sensor->ADCPolling(a_temp_sensor->arg);
			a_temp_sensor->GetADCValue(a_temp_sensor->arg, &adc_value);
//			float v_adc = adc_value*V_REF/4096; //12bits ADC.
//			a_temp_sensor->temp_value = 100*v_adc/1000; //T = 100*V;
			a_temp_sensor->temp_value = (float) adc_value * 100.0 / 4096.0;
			if (_lwevent_set(&adc_lwevent,
					(_mqx_uint) ADC_EVT_BIT_MASK) != MQX_OK) {
				NOTIFY("Event Set failed\n");
			}
		}
	} //end if()
}

