/*
 * sensor.h
 *
 *  Created on: Feb 11, 2015
 *      Author: nvhie_000
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint16_t (*StartMeasurement)(void *arg);
	void (*ADCPolling)(void *arg);
	uint16_t (*GetADCValue)(void *arg, void* out_value);
	void *arg;
	uint8_t dev_id;
	float temp_value;
} temp_sensor_t;

uint8_t get_sensor_id(temp_sensor_t *a_temp_sensor);
float get_sensor_value(temp_sensor_t *a_temp_sensor);
void temp_sensor_callback_timer_isr(temp_sensor_t *a_temp_sensor);

#endif /* SENSOR_H_ */
