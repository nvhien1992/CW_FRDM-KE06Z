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
#include "adc_ll.h"

typedef struct {
	uint8_t channel_index;
	uint8_t dev_id;
	uint16_t sensor_value;
} sensor_t;

uint8_t get_sensor_id(sensor_t *a_sensor);
uint16_t get_sensor_value(sensor_t *a_sensor);
void sensor_callback_timer_isr(sensor_t *ve_ref, sensor_t *a_sensor, void *dest_queue);

#endif /* SENSOR_H_ */
