/*
 * sensor.c
 *
 *  Created on: Feb 11, 2015
 *      Author: nvhie_000
 */
#include <math.h>
#include "mqxlite.h"
#include "sensor.h"

#define DEBUG_EN 1
#define NOTIFY_EN 1
#include "debug.h"

#define VE_REF 2500 //mV external ref voltage
#define TIMER_PERIOD (10) //ms
#define SENSOR_SAMPLING_PERIOD (200 / TIMER_PERIOD) //sampling every 200ms (timer_period = 10ms)
#define SEND_SENSOR_VALUE_PERIOD (2 * 1000 / SENSOR_SAMPLING_PERIOD) //send value every 2s
static uint16_t sampling_time_count = 0;
static uint16_t send_value_time_count = 0;
static float sum_sensor_value = 0.0;
static uint8_t avg_sampling_count = 0;

uint8_t get_sensor_id(sensor_t *a_sensor) {
	return a_sensor->dev_id;
}

uint16_t get_sensor_value(sensor_t *a_sensor) {
	return a_sensor->sensor_value;
}

void sensor_callback_timer_isr(sensor_t *ve_ref, sensor_t *a_sensor, void *dest_queue) {
	sampling_time_count = sampling_time_count + 1;

	if (sampling_time_count == SENSOR_SAMPLING_PERIOD) {
		sampling_time_count = 0;
		send_value_time_count++;
		uint16_t out_buff[2];
		uint8_t channel_table[2];
		channel_table[0] = ve_ref->channel_index;
		channel_table[1] = a_sensor->channel_index;
		if (!adc_mesurement(channel_table, 2, out_buff)) {
			sum_sensor_value = 0.0;
			avg_sampling_count = 0;
			send_value_time_count = 0;
			return;
		}

		/* external ref voltage */
		float v_sen = (float) out_buff[1] * (float) VE_REF / (float) out_buff[0];
		sum_sensor_value += v_sen * 100.0; //t = 100*v;
		avg_sampling_count++;
		if (send_value_time_count == SEND_SENSOR_VALUE_PERIOD) {
			send_value_time_count = 0;
			sum_sensor_value = sum_sensor_value / (float) avg_sampling_count;
			a_sensor->sensor_value =
					(uint16_t) (round(sum_sensor_value * 10.0));
			_mqx_uint msg = (_mqx_uint) (((uint32_t) a_sensor->dev_id << 16)
					| a_sensor->sensor_value);
			_lwmsgq_send((pointer) dest_queue, &msg, 0);
			sum_sensor_value = 0.0;
			avg_sampling_count = 0;
		}
	} //end if()
}

