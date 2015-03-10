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
#define VE_REF 2500 //mV
#define TIMER_PERIOD (10) //ms
#define SENSOR_SAMPLING_PERIOD (1 * 1000 / TIMER_PERIOD) //sampling every 1s (timer_period = 10ms)
#define SEND_SENSOR_VALUE_PERIOD (60 * 1000 / SENSOR_SAMPLING_PERIOD) //send value every 1min
static uint16_t sampling_time_count = 0;
static uint16_t send_value_time_count = 0;
static uint16_t sum_sensor_value = 0;
static uint8_t avg_sampling_time = 0;

uint8_t get_sensor_id(sensor_t *a_sensor) {
	return a_sensor->dev_id;
}

uint16_t get_sensor_value(sensor_t *a_sensor) {
	return a_sensor->sensor_value;
}

void sensor_callback_timer_isr(sensor_t *ve_ref, sensor_t *a_sensor) {
	sampling_time_count = sampling_time_count + 1;

	if (sampling_time_count == SENSOR_SAMPLING_PERIOD) {
		sampling_time_count = 0;
		send_value_time_count++;
		uint16_t ve_adc, vsen_adc;
		if (!adc_mesurement(&ve_ref->channel_index, 1, &ve_adc)) {
			sum_sensor_value = 0;
			avg_sampling_time = 0;
			send_value_time_count = 0;
			return;
		}
		if (adc_mesurement(&a_sensor->channel_index, 1, &vsen_adc)) {
			float v_sen = (float) vsen_adc * (float) VE_REF / (float) ve_adc;
			sum_sensor_value += v_sen * 100; //t = 100*v;
			avg_sampling_time++;
		}
		if (send_value_time_count == SEND_SENSOR_VALUE_PERIOD) {
			send_value_time_count = 0;
			a_sensor->sensor_value = sum_sensor_value / avg_sampling_time;
			_mqx_uint msg = (_mqx_uint) (((uint32_t) a_sensor->dev_id << 16)
					| a_sensor->sensor_value);
			_lwmsgq_send((pointer) ctrl_msg_queue, &msg, 0);
		}
	} //end if()
}

