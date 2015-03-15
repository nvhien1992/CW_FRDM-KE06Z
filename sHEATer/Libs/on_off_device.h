/*
 * on_off_device.h
 *
 *  Created on: Feb 12, 2015
 *      Author: nvhie_000
 */

#ifndef ON_OFF_DEVICE_H_
#define ON_OFF_DEVICE_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
	ON, OFF, BLINK,
} on_off_status_t;

typedef struct {
	void (*SetPin)(void *arg);
	void (*ClearPin)(void *arg);
	void *arg;
	uint16_t time_cycle_count;
	uint16_t period_in_ms;
	on_off_status_t current_status;
	bool is_on_in_blink;
} on_off_dev_t;

void turn_on(on_off_dev_t *on_off_dev);
void turn_off(on_off_dev_t *on_off_dev);
void toggle(on_off_dev_t *on_off_dev);
void blink(on_off_dev_t *on_off_dev, uint8_t freq_in_hz);
void on_off_blink_callback_timer_isr(void);

#endif /* ON_OFF_DEVICE_H_ */
